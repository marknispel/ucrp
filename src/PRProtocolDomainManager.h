/******************************************************************************/
/*  UDP Command Response Protocol Server                                      */
/*  Copyright © 2023 Mark Nispel                                              */
/*  All rights reserved                                                       */
/*  Unauthorized use, distribution or duplication is                          */
/*  strictly prohibited without written authorization.                        */
/******************************************************************************/
/**
   @file PRProtocolDomainManager.h
   @author Mark Nispel
   @date Nov 7, 2023
   @version 1.0
   @brief FILE NOTES:
   This file contains the definitions for the message functionality
   for the application.
*/
/******************************************************************************/
#ifndef pr_protocol_domain_manager_h
#define pr_protocol_domain_manager_h

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/
#include <string>
#include <memory>

#include "GLConfigureDomains.h"
#include "GLConfigureSystemModules.h"
#include "IONetworkControlMessage.h"

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

using MsgQueueHelperPtrType = std::unique_ptr<IOMessageQueueHelper>;
using UdpHelperPtrType = std::unique_ptr<IONetworkUdpHelper>;

/******************************************************************************/
/*                  T Y P E D E F S  A N D  E N U M S                         */
/******************************************************************************/
typedef enum
{
   PRDM_STATE_INACTIVE,
   PRDM_STATE_ACTIVE
} PrProtocolDomainManagerStateType;

/******************************************************************************/
/*                         D E C L A R A T I O N S                            */
/******************************************************************************/

class GLResourceMain;

class PRProtocolDomainManager
{
   public:
      PRProtocolDomainManager(GLResourceMain& resource);
      ~PRProtocolDomainManager();

      bool Active();

      const GLCFDomainIds DomainId();
      const GLCFModuleIds ModuleId();
      const std::string& ModuleName();

      void ActivateProtocolManager();
      void DeactivateProtocolManager();
      void ProcessMessage(
         std::shared_ptr<IONetworkControlMessage>& msgPtr);
      void ProcessMessageStateActive(
         std::shared_ptr<IONetworkControlMessage>& msgPtr);

      bool EventPingMsgRcvdStateActive(std::shared_ptr<IONetworkControlMessage>& msgPtr);
      bool EventRqstIntfMsgRcvdStateActive(std::shared_ptr<IONetworkControlMessage>& msgPtr);

   private:
      bool SendResponseMessage(
         IONetworkControlMsgIds msgId,
         std::array<uint8_t, m_theIoNwControlMessageFixedLengthBytes>& response,
         uint8_t msglen);
      GLResourceMain& Resource();
      PrProtocolDomainManagerStateType State();

      const GLCFDomainIds m_DomainId;
      const GLCFModuleIds m_ModuleId;
      PrProtocolDomainManagerStateType m_State;
      GLResourceMain& m_ResourceMain;
};

}

/******************************************************************************/

#endif /* pr_protocol_domain_manager_h */