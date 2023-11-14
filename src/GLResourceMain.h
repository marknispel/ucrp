/******************************************************************************/
/*  UDP Command Response Protocol Server                                      */
/*  Copyright © 2023 Mark Nispel                                              */
/*  All rights reserved                                                       */
/*  Unauthorized use, distribution or duplication is                          */
/*  strictly prohibited without written authorization.                        */
/******************************************************************************/
/**
   @file GLResourceMain.h
   @author Mark Nispel
   @date Oct 31, 2023
   @version 1.0
   @brief FILE NOTES:
   This file contains the definitions for the message functionality
   for the application.
*/
/******************************************************************************/
#ifndef gl_resource_main_h
#define gl_resource_main_h

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/
#include <memory>
#include <array>
#include <condition_variable>
#include <mutex>

#include "GLConfigureDomains.h"
#include "GLConfigureSystemModules.h"

/******************************************************************************/
/*                              D E F I N E S                                 */
/******************************************************************************/
namespace MDN
{

/******************************************************************************/
/*               F O R W A R D  D E C L A R A T I O N S                       */
/******************************************************************************/
class GLErrorLog;
class GLEventLog;
class GLTimeHelper;
class IONetworkControlInterfaceManager;
class PRProtocolDomainManager;

using GLTimeHelperPtr = std::unique_ptr<GLTimeHelper>;
using GLErrorLogPtr = std::unique_ptr<GLErrorLog>;
using GLEventLogPtr = std::unique_ptr<GLEventLog>;
using IONetworkControlInterfaceManagerPtr = std::unique_ptr<IONetworkControlInterfaceManager>;
using ProtocolDomainManagerPtr = std::unique_ptr<PRProtocolDomainManager>;

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/
typedef enum
{
   GLRM_ONE_THREAD_MODE,
   GLRM_TWO_THREADS_MODE,   // Receive on main thread; execute on queue thread
   GLRM_THREE_THREADS_MODE  // Monitor on main thread, receive on socket
                            // thread two, execute on queue thread.
} GLRMThreadModeType;

typedef enum
{
   GLRM_STATE_APP_INACTIVE,
   GLRM_STATE_APP_ACTIVE,
   GLRM_STATE_SHUTTING_DOWN,
} GLRMStateType;

/******************************************************************************/
/*                         D E C L A R A T I O N S                            */
/******************************************************************************/
class GLResourceMain
{
   public:
      GLResourceMain();
      ~GLResourceMain();

      void AppStart();
      void AppStop();
      GLErrorLog& ErrorLog();
      GLEventLog& EventLog();
      GLTimeHelper& TimeHelper();
      IONetworkControlInterfaceManager& InterfaceManager();
      PRProtocolDomainManager& ProtocolManager();

      void PrintCombinedLogEntries();
      GLRMStateType State();
      void EventShutdownRequestReceived();
      bool Active();

      const GLCFDomainIds DomainId();
      const GLCFModuleIds ModuleId();
      const std::string& ModuleName();
      const GLRMThreadModeType AppThreadMode();

   private:

      const GLCFDomainIds m_DomainId;
      const GLCFModuleIds m_ModuleId;
      GLRMStateType m_State;
      GLTimeHelperPtr m_TimeHelper;
      GLErrorLogPtr m_ErrorLog;
      GLEventLogPtr m_EventLog;
      IONetworkControlInterfaceManagerPtr m_IONetworkControlInterfaceMgr;
      ProtocolDomainManagerPtr m_ProtocolManager;

      // C L A S S  C O N S T A N T S
      static const GLRMThreadModeType m_theAppThreadMode;
};

}

/******************************************************************************/

#endif /* gl_resource_main_h */


