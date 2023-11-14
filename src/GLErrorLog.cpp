/******************************************************************************/
/*  UDP Command Response Protocol Server                                      */
/*  Copyright © 2023 Mark Nispel                                              */
/*  All rights reserved                                                       */
/*  Unauthorized use, distribution or duplication is                          */
/*  strictly prohibited without written authorization.                        */
/******************************************************************************/
/**
   @file GLErrorLog.cpp
   @author Mark Nispel
   @date Oct 31, 2023
   @version 1.0
   @brief FILE NOTES:
   This file contains the implementation for the message router which distributes
   messages to the various system modules in the application.
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

#include "GLResourceMain.h"
#include "GLTimeHelper.h"
#include "GLErrorLog.h"

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
GLErrorLogEntry::GLErrorLogEntry()
{
}

/******************************************************************************/
GLErrorLogEntry::GLErrorLogEntry(
   uint64_t ts,
   std::string mn,
   std::string err,
   std::string lvl)
   :
   m_TimeStampNs(ts),
   m_ModuleName(mn),
   m_Error(err),
   m_Level(lvl)
{
}

/******************************************************************************/
GLErrorLogEntry::~GLErrorLogEntry()
{

}

/******************************************************************************/
std::string GLErrorLogEntry::GetFixedWidthString(
      std::string inString,
      GLEL_FIELD_TYPE field)
{
   std::array<char, GLELMaximumFieldStringWidth> buffer;
   uint32_t fixedwidth;

   buffer.fill(0x00);
   switch(field)
   {
      case GLEL_TIMESTAMP:
         fixedwidth = GLELTimestampMaximumLengthChars;
         break;

      case GLEL_MODULE_NAME:
         fixedwidth = GLELModuleNameMaximumLengthChars;
         break;

      case GLEL_ERROR_STR:
         fixedwidth = GLELErrorMaximumLengthChars;
         break;

      case GLEL_ERROR_LEVEL_STR:
         fixedwidth = GLELErrorLevelMaximumLengthChars;
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
GLErrorLog::GLErrorLog(GLResourceMain& resource)
   :
   m_ResourceMain(resource),
   m_ErrorLog(std::make_unique<GLELLogType>()),
   m_HeadIndex(0),
   m_TailIndex(0)
{
}

/******************************************************************************/
GLErrorLog::~GLErrorLog()
{
}

/******************************************************************************/
GLResourceMain& GLErrorLog::Resource()
{
   return m_ResourceMain;
}

/******************************************************************************/
GLTimeHelper& GLErrorLog::TimeHelper()
{
   return Resource().TimeHelper();
}

/******************************************************************************/
GLELLogType& GLErrorLog::ErrorLog()
{
   return *m_ErrorLog;
}

/******************************************************************************/
bool GLErrorLog::LogError(
      GLCFModuleIds moduleId,
      const char* error,
      GLELErrorLevels level)
{
   std::lock_guard<std::mutex> lock(m_ErrorMutex);

   bool success = false;

   uint64_t tsns = Resource().TimeHelper().GetTimeInNs();
   std::string moduleName =
         GLErrorLogEntry::GetFixedWidthString(
               m_theModuleNameMap.at(moduleId),
               GLEL_MODULE_NAME);
   std::string errorText =
         GLErrorLogEntry::GetFixedWidthString(
               std::string(error),
               GLEL_ERROR_STR);
   std::string errorLevel =
         GLErrorLogEntry::GetFixedWidthString(
               std::to_string(level),
               GLEL_ERROR_LEVEL_STR);

   GLErrorLogEntry newEntry(tsns, moduleName, errorText, errorLevel);

   ErrorLog().at(m_HeadIndex) = newEntry;

   IncrementBufferPointer(m_HeadIndex, GLELErrorLogSize);
   if (m_HeadIndex == m_TailIndex)
   {  // Tail should point at the oldest available message
      IncrementBufferPointer(m_TailIndex, GLELErrorLogSize);
   }

   return success;
}

/******************************************************************************/
void GLErrorLog::PrintErrorLogEntries()
{
   std::lock_guard<std::mutex> lock(m_ErrorMutex);

   uint32_t tailIndex = m_TailIndex;

   printf("\n\n***** ERROR LOG *****\n");
   while (tailIndex != m_HeadIndex)
   {
      std::string entryString = GetEntryString(ErrorLog()[tailIndex]);
      printf("%s\n", entryString.c_str());
      if (GLELRemoteLoggingEnabled)
      {
         SendLogErrorEntryToRemoteLogger(entryString);
      }
      IncrementBufferPointer(tailIndex, GLELErrorLogSize);
   }
   printf("\n\n\n");
}

/******************************************************************************/
std::shared_ptr<std::vector<GLErrorLogEntry*>> GLErrorLog::GetTempErrorEntries()
{
   std::lock_guard<std::mutex> lock(m_ErrorMutex);

   // This is temporary data.  The ownership of the shared_ptr will be passed
   // to the calling function when this method returns.  The calling object
   // is to release it when it is done with the data.
   auto traceEntries = std::make_shared<std::vector<GLErrorLogEntry*>>();

   uint32_t tailIndex = m_TailIndex;
   while (tailIndex != m_HeadIndex)
   {
      traceEntries->push_back(&(ErrorLog()[tailIndex]));
      IncrementBufferPointer(tailIndex, GLELErrorLogSize);
   }

   return traceEntries;
}

/******************************************************************************/
const std::string GLErrorLog::GetEntryString(GLErrorLogEntry& entry)
{
   const std::string ts = TimeHelper().ConvertNsIntoTimeStampUs(entry.m_TimeStampNs);
   std::string entryString =
         GLErrorLogEntry::GetFixedWidthString(ts, GLEL_TIMESTAMP) +
         GLErrorLogEntry::GetFixedWidthString(entry.m_ModuleName, GLEL_MODULE_NAME) +
         GLErrorLogEntry::GetFixedWidthString(entry.m_Error, GLEL_ERROR_STR) +
         GLErrorLogEntry::GetFixedWidthString(entry.m_Level, GLEL_ERROR_LEVEL_STR);

   return entryString;
}

/******************************************************************************/
void GLErrorLog::IncrementBufferPointer(
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
bool GLErrorLog::SendLogErrorEntryToRemoteLogger(std::string& entryString)
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
