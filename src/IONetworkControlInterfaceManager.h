/******************************************************************************/
/*  UDP Command Response Protocol Server                                      */
/*  Copyright © 2023 Mark Nispel                                              */
/*  All rights reserved                                                       */
/*  Unauthorized use, distribution or duplication is                          */
/*  strictly prohibited without written authorization.                        */
/******************************************************************************/
/**
   @file IONetworkControlMessage.h
   @author Mark Nispel
   @date Nov 5, 2023
   @version 1.0
   @brief FILE NOTES:
   This file contains the definitions for the message functionality
   for the application.
*/
/******************************************************************************/
#ifndef io_network_control_interface_manager_h
#define io_network_control_interface_manager_h

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/
#include <memory>

#include "IONetworkControlMessage.h"
#include "GLConfigureSystemModules.h"
#include "IONetworkUdpHelperIntf.h"

/******************************************************************************/
/*                              D E F I N E S                                 */
/******************************************************************************/
namespace MDN
{

/******************************************************************************/
/*               F O R W A R D  D E C L A R A T I O N S                       */
/******************************************************************************/
class GLResourceMain;
class IOMessageQueueHelper;
class IONetworkUdpHelper;

using UdpHelperPtrType = std::unique_ptr<IONetworkUdpHelper>;

/******************************************************************************/
/*                  T Y P E D E F S  A N D  E N U M S                         */
/******************************************************************************/

typedef enum
{
   IONCIM_STATE_INACTIVE,
   IONCIM_STATE_ACTIVE
} IONetworkControlInterfaceManagerStateType;

/******************************************************************************/
/*                         D E C L A R A T I O N S                            */
/******************************************************************************/
class IONetworkControlInterfaceManager :
   public virtual IONetworkUdpHelperIntf
{
   public:
      IONetworkControlInterfaceManager(GLResourceMain& resource);
      ~IONetworkControlInterfaceManager();

      void StartNetworkControlInterface();
      void StopNetworkControlInterface();
      bool Active();

      const GLCFDomainIds DomainId();
      const GLCFModuleIds ModuleId();
      const std::string& ModuleName();

      // N E T W O R K  U D P  H E L P E R  I N T E R F A C E
      virtual void EventUdpHelperActive() override;
      virtual void EventUdpHelperInactive() override;
      bool SendResponseMessageToSource(uint8_t* message, int32_t len);

      // Tests
      void TestUdpTxWithTempSocket();
      void TestUdpRx();

   private:
      GLResourceMain& Resource();
      IONetworkUdpHelper& UdpHelper();
      void ControlInterfaceReceive();
      void MessageToBeProcessed(std::shared_ptr<IONetworkControlMessage>& msgPtr);

      IONetworkControlInterfaceManagerStateType State();

      const GLCFDomainIds m_DomainId;
      const GLCFModuleIds m_ModuleId;
      IONetworkControlInterfaceManagerStateType m_State;
      GLResourceMain& m_ResourceMain;
      UdpHelperPtrType m_UdpHelperPtr;
      bool m_ReceiveActive;
};

}

/******************************************************************************/

#endif /* io_network_control_interface_manager_h */