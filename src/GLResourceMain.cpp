/******************************************************************************/
/*  UDP Command Response Protocol Server                                      */
/*  Copyright © 2023 Mark Nispel                                              */
/*  All rights reserved                                                       */
/*  Unauthorized use, distribution or duplication is                          */
/*  strictly prohibited without written authorization.                        */
/******************************************************************************/
/**
   @file GLResourceMain.cpp
   @author Mark Nispel
   @date Oct 31, 2023
   @version 1.0
   @brief FILE NOTES:
   This file contains the implementation for the message queue helper which
   is used by every system module in the application which are driven by
   the message routing system.
*/
/******************************************************************************/
/*       I N C L U D E S                                                      */
/******************************************************************************/
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include "limits.h"

#include "GLTimeHelper.h"
#include "GLErrorLog.h"
#include "GLEventLog.h"
#include "IONetworkControlInterfaceManager.h"
#include "PRProtocolDomainManager.h"
#include "GLResourceMain.h"

using namespace MDN;

/******************************************************************************/
/*                        C O N S T A N T S                                   */
/******************************************************************************/
const GLRMThreadModeType
   GLResourceMain::m_theAppThreadMode = GLRM_ONE_THREAD_MODE;

/******************************************************************************/
/*       I M P L E M E N T A T I O N                                          */
/******************************************************************************/
GLResourceMain::GLResourceMain()
   :
   m_DomainId(GLCF_GLOBAL_DOMAIN_ID),
   m_ModuleId(GLCF_GL_RESOURCE_MAIN_ID),
   m_State(GLRM_STATE_APP_INACTIVE),
   m_TimeHelper(std::make_unique<GLTimeHelper>(GLTH_TIMESTAMP_MODE_FROM_SYSTEM_START)),
   m_ErrorLog(std::make_unique<GLErrorLog>(*this)),
   m_EventLog(std::make_unique<GLEventLog>(*this)),
   m_IONetworkControlInterfaceMgr(std::make_unique<IONetworkControlInterfaceManager>(*this)),
   m_ProtocolManager(std::make_unique<PRProtocolDomainManager>(*this))
{
   EventLog().LogEvent(
      MDN::GLCF_GL_RESOURCE_MAIN_ID,
      "GLResourceMain:: Constructor.",
      MDN::GLEV_EVENT_LEVEL_1);
}

/******************************************************************************/
GLResourceMain::~GLResourceMain()
{
}

/******************************************************************************/
const GLCFDomainIds GLResourceMain::DomainId()
{
   return m_DomainId;
}

/******************************************************************************/
const GLCFModuleIds GLResourceMain::ModuleId()
{
   return m_ModuleId;
}

/******************************************************************************/
const std::string& GLResourceMain::ModuleName()
{
   return m_theModuleNameMap.at(ModuleId());
}

/******************************************************************************/
GLRMStateType GLResourceMain::State()
{
   return m_State;
}

/******************************************************************************/
bool GLResourceMain::Active()
{
   return (m_State == GLRM_STATE_APP_ACTIVE ? true : false);
}

/******************************************************************************/
void GLResourceMain::AppStart()
{
   EventLog().LogEvent(
         GLCF_GL_RESOURCE_MAIN_ID,
         "GLResourceMain::AppStart().",
         GLEV_EVENT_LEVEL_1);

   ProtocolManager().ActivateProtocolManager();
   InterfaceManager().StartNetworkControlInterface();

   m_State = GLRM_STATE_APP_ACTIVE;
}

/******************************************************************************/
void GLResourceMain::AppStop()
{
   if (State() == GLRM_STATE_APP_ACTIVE || State() == GLRM_STATE_SHUTTING_DOWN)
   {
      EventLog().LogEvent(
            GLCF_GL_RESOURCE_MAIN_ID,
            "GLResourceMain::AppStop().",
            GLEV_EVENT_LEVEL_1);

      // The InterfaceManager() is stopped during processing of the SHUTDOWN message.
      ProtocolManager().DeactivateProtocolManager();

      m_State = GLRM_STATE_APP_INACTIVE;

      EventLog().LogEvent(
            GLCF_GL_RESOURCE_MAIN_ID,
            "GLResourceMain::AppStop() Completed.",
            GLEV_EVENT_LEVEL_1);
   }
}

/******************************************************************************/
GLTimeHelper& GLResourceMain::TimeHelper()
{
   return *m_TimeHelper;
}

/******************************************************************************/
GLErrorLog& GLResourceMain::ErrorLog()
{
   return *m_ErrorLog;
}

/******************************************************************************/
GLEventLog& GLResourceMain::EventLog()
{
   return *m_EventLog;
}

/******************************************************************************/
IONetworkControlInterfaceManager& GLResourceMain::InterfaceManager()
{
   return *m_IONetworkControlInterfaceMgr;
}

/******************************************************************************/
PRProtocolDomainManager& GLResourceMain::ProtocolManager()
{
   return *m_ProtocolManager;
}

/******************************************************************************/
const GLRMThreadModeType GLResourceMain::AppThreadMode()
{
   return m_theAppThreadMode;
}

/******************************************************************************/
void GLResourceMain::EventShutdownRequestReceived()
{
   m_State = GLRM_STATE_SHUTTING_DOWN;
}

/******************************************************************************/
void GLResourceMain::PrintCombinedLogEntries()
{
#ifdef nodef
   std::shared_ptr<std::vector<GLTraceLogEntry*>> traceEntries =
         TraceLog().GetTempTraceEntries();
   auto traceIter = traceEntries->begin();

   std::shared_ptr<std::vector<GLEventLogEntry*>> eventEntries =
         EventLog().GetTempEventEntries();
   auto eventIter = eventEntries->begin();

   std::shared_ptr<std::vector<GLErrorLogEntry*>> errorEntries =
         ErrorLog().GetTempErrorEntries();
   auto errorIter = errorEntries->begin();

   while (traceIter != traceEntries->end() ||
         eventIter != eventEntries->end() ||
         errorIter != errorEntries->end())
   {
      uint64_t traceEntryTs = ULLONG_MAX;
      uint64_t eventEntryTs = ULLONG_MAX;
      uint64_t errorEntryTs = ULLONG_MAX;

      if (traceIter != traceEntries->end())
      {
         traceEntryTs = (*traceIter)->m_TimeStampNs;
      }
      if (eventIter != eventEntries->end())
      {
         eventEntryTs = (*eventIter)->m_TimeStampNs;
      }
      if (errorIter != errorEntries->end())
      {
         errorEntryTs = (*errorIter)->m_TimeStampNs;
      }

      if (traceIter != traceEntries->end() &&
         traceEntryTs < eventEntryTs &&
         traceEntryTs < errorEntryTs)
      {
         printf("%s\n", TraceLog().GetEntryString(**traceIter).c_str());
         traceIter++;
      }
      else if (eventIter != eventEntries->end() &&
         eventEntryTs < traceEntryTs &&
         eventEntryTs < errorEntryTs)
      {
         printf("%s\n", EventLog().GetEntryString(**eventIter).c_str());
         eventIter++;
      }
      else if (errorIter != errorEntries->end() &&
         errorEntryTs < traceEntryTs &&
         errorEntryTs < eventEntryTs)
      {
         printf("%s\n", ErrorLog().GetEntryString(**errorIter).c_str());
         errorIter++;
      }
      else
      {
         printf ("EventRequestForMergedLogsReceived: ERROR!!\n");
         break;
      }
   }
#endif
}
/******************************************************************************/

