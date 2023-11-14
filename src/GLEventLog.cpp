/******************************************************************************/
/*  UDP Command Response Protocol Server                                      */
/*  Copyright © 2023 Mark Nispel                                              */
/*  All rights reserved                                                       */
/*  Unauthorized use, distribution or duplication is                          */
/*  strictly prohibited without written authorization.                        */
/******************************************************************************/
/**
   @file GLEventLog.cpp
   @author Mark Nispel
   @date Oct 31, 2023
   @version 1.0
   @brief FILE NOTES:
   This file contains the implementation for the event log class.
*/
/******************************************************************************/
/*       I N C L U D E S                                                      */
/******************************************************************************/
#include <string>
#include <chrono>
#include <array>
#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdio.h>

#include "GLResourceMain.h"
#include "GLTimeHelper.h"
#include "GLEventLog.h"

/******************************************************************************/
/*       D E C L A R A T I O N S                                              */
/******************************************************************************/
using namespace MDN;

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

/******************************************************************************/
/*                          D A T A  M O D E L S                              */
/******************************************************************************/
GLEventLogEntry::GLEventLogEntry()
{
}

/******************************************************************************/
GLEventLogEntry::GLEventLogEntry(
      uint64_t ts,
      std::string mn,
      std::string event,
      std::string lvl)
   :
   m_TimeStampNs(ts),
   m_ModuleName(mn),
   m_Event(event),
   m_Level(lvl)
{
}

/******************************************************************************/
GLEventLogEntry::~GLEventLogEntry()
{

}

/******************************************************************************/
std::string GLEventLogEntry::GetFixedWidthString(
      std::string inString,
      GLEVEventFieldTypes field)
{
   std::array<char, GLEVMaximumFieldStringWidth> buffer;
   uint32_t fixedwidth;

   buffer.fill(0x00);
   switch(field)
   {
      case GLEV_TIMESTAMP:
         fixedwidth = GLEVTimestampMaximumLengthChars;
         break;

      case GLEV_MODULE_NAME:
         fixedwidth = GLEVModuleNameMaximumLengthChars;
         break;

      case GLEV_EVENT_STR:
         fixedwidth = GLEVEEventMaximumLengthChars;
         break;

      case GLEV_EVENT_LEVEL_STR:
         fixedwidth = GLEVEventLevelMaximumLengthChars;
         break;

      default:
         fixedwidth = 1;
         break;
   }

   std::stringstream ss;
   ss << std::left << std::setw(fixedwidth - 1) << inString;

   return ss.str();
}

/******************************************************************************/
/*       I M P L E M E N T A T I O N                                          */
/******************************************************************************/
GLEventLog::GLEventLog(GLResourceMain& resource)
   :
   m_ResourceMain(resource),
   m_EventLog(std::make_unique<GLEVLogType>()),
   m_HeadIndex(0),
   m_TailIndex(0)
{
}

/******************************************************************************/
GLEventLog::~GLEventLog()
{
}

/******************************************************************************/
GLResourceMain& GLEventLog::Resource()
{
   return m_ResourceMain;
}

/******************************************************************************/
GLTimeHelper& GLEventLog::TimeHelper()
{
   return Resource().TimeHelper();
}

/******************************************************************************/
GLEVLogType& GLEventLog::EventLog()
{
   return *m_EventLog;
}

/******************************************************************************/
bool GLEventLog::LogEvent(
      GLCFModuleIds moduleId,
      const char* eventStr,
      GLEVEventLevels level)
{
   std::lock_guard<std::mutex> lock(m_Mutex);

   bool success = false;
   uint64_t tsns = Resource().TimeHelper().GetTimeInNs();


   std::string moduleName =
         GLEventLogEntry::GetFixedWidthString(
               m_theModuleNameMap.at(moduleId),
               GLEV_MODULE_NAME);

   std::string eventText =
         GLEventLogEntry::GetFixedWidthString(
               std::string(eventStr),
               GLEV_EVENT_STR);

   std::string eventLevel =
         GLEventLogEntry::GetFixedWidthString(
               std::to_string(level),
               GLEV_EVENT_LEVEL_STR);

   GLEventLogEntry newEntry(tsns, moduleName, eventText, eventLevel);

   EventLog().at(m_HeadIndex) = newEntry;

   IncrementBufferPointer(m_HeadIndex, GLEVEventLogSize);
   if (m_HeadIndex == m_TailIndex)
   {  // Tail should point at the oldest available message
      IncrementBufferPointer(m_TailIndex, GLEVEventLogSize);
   }

   return success;
}

/******************************************************************************/
void GLEventLog::PrintEventLogEntries()
{
   std::lock_guard<std::mutex> lock(m_Mutex);

   uint32_t tailIndex = m_TailIndex;

   printf("\n\n***** EVENT LOG *****\n");
   while (tailIndex != m_HeadIndex)
   {
      std::string entryString = GetEntryString(EventLog()[tailIndex]);
      printf("%s\n", entryString.c_str());
      if (GLEVRemoteLoggingEnabled)
      {
         SendEventLogEntryToRemoteLogger(entryString);
      }
      IncrementBufferPointer(tailIndex, GLEVEventLogSize);
   }
   printf("\n\n\n");
}

/******************************************************************************/
std::shared_ptr<std::vector<GLEventLogEntry*>> GLEventLog::GetTempEventEntries()
{
   std::lock_guard<std::mutex> lock(m_Mutex);

   // This is temporary data.  The ownership of the shared_ptr will be passed
   // to the calling function when this method returns.  The calling object
   // is to release it when it is done with the data.
   auto eventEntries = std::make_shared<std::vector<GLEventLogEntry*>>();

   uint32_t tailIndex = m_TailIndex;
   while (tailIndex != m_HeadIndex)
   {
      eventEntries->push_back(&(EventLog()[tailIndex]));
      IncrementBufferPointer(tailIndex, GLEVEventLogSize);
   }

   return eventEntries;
}

/******************************************************************************/
const std::string GLEventLog::GetEntryString(GLEventLogEntry& entry)
{
   const std::string ts = TimeHelper().ConvertNsIntoTimeStampUs(entry.m_TimeStampNs);
   std::string entryString =
         GLEventLogEntry::GetFixedWidthString(ts, GLEV_TIMESTAMP) +
         GLEventLogEntry::GetFixedWidthString(entry.m_ModuleName, GLEV_MODULE_NAME) +
         "EVNT: " +
         GLEventLogEntry::GetFixedWidthString(entry.m_Event, GLEV_EVENT_STR);

   return entryString;
}

/******************************************************************************/
void GLEventLog::IncrementBufferPointer(
      uint32_t& bufferPointer,
      const uint32_t bufferLen)
{
   if (bufferPointer < bufferLen - 1)
   {
      bufferPointer++;
   }
   else
   {
      bufferPointer = 0;
   }
}

/******************************************************************************/
bool GLEventLog::SendEventLogEntryToRemoteLogger(std::string& entryString)
{
   bool success = false;

#ifdef nodef
   std::array<uint8_t, IONW_SOCKET_FIXED_MESSAGE_LEN>& buffer =
         TcpHelper().BufferOut();
   buffer.fill(0x00);

   if (entryString.size() < IONW_SOCKET_FIXED_MESSAGE_LEN - 2)
   {
      buffer[0] = entryString.size() + 1;
      std::strcpy((char*)&buffer[1], entryString.c_str());
      success = TcpHelper().SendToServer(buffer);

      if (!success)
      {
         printf("ERROR: SendTraceLogEntryToRemoteLogger() send failed.\n");
      }
   }
   else
   {
      printf("ERROR: SendTraceLogEntryToRemoteLogger -> buffer too small.");
   }

#endif

   return success;
}

/******************************************************************************/