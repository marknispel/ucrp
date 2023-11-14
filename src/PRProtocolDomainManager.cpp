/******************************************************************************/
/*  UDP Command Response Protocol Server                                      */
/*  Copyright © 2023 Mark Nispel                                              */
/*  All rights reserved                                                       */
/*  Unauthorized use, distribution or duplication is                          */
/*  strictly prohibited without written authorization.                        */
/******************************************************************************/
/**
   @file IONetworkControlInterfaceManager.cpp
   @author Mark Nispel
   @date Feb 7, 2022
   @version 1.0
   @brief FILE NOTES:
   This file contains the implementation for the message queue helper which
   is used by every system module in the application which are driven by
   the message routing system.
*/
/******************************************************************************/
/*       I N C L U D E S                                                      */
/******************************************************************************/
#include <iostream>

#include "GLErrorLog.h"
#include "GLEventLog.h"
#include "GLResourceMain.h"
#include "IONetworkControlMessage.h"
#include "IONetworkControlMessages.h"
#include "IONetworkControlInterfaceManager.h"
#include "PRProtocolDomainManager.h"

using namespace MDN;

/******************************************************************************/
/*                  T Y P E D E F S  A N D  E N U M S                         */
/******************************************************************************/

/******************************************************************************/
/*                        C O N S T A N T S                                   */
/******************************************************************************/


/******************************************************************************/
/*                     I M P L E M E N T A T I O N                            */
/******************************************************************************/
PRProtocolDomainManager::PRProtocolDomainManager(GLResourceMain& resource)
   :
   m_DomainId(GLCF_PROTOCOL_DOMAIN_ID),
   m_ModuleId(GLCF_PR_PROTOCOL_DOMAIN_MANAGER_ID),
   m_State(PRDM_STATE_INACTIVE),
   m_ResourceMain(resource)
{

}

/******************************************************************************/
PRProtocolDomainManager::~PRProtocolDomainManager()
{

}

/******************************************************************************/
const GLCFDomainIds PRProtocolDomainManager::DomainId()
{
   return m_DomainId;
}

/******************************************************************************/
const GLCFModuleIds PRProtocolDomainManager::ModuleId()
{
   return m_ModuleId;
}

/******************************************************************************/
const std::string& PRProtocolDomainManager::ModuleName()
{
   return m_theModuleNameMap.at(ModuleId());
}

/******************************************************************************/
PrProtocolDomainManagerStateType PRProtocolDomainManager::State()
{
   return m_State;
}

/******************************************************************************/
void PRProtocolDomainManager::ActivateProtocolManager()
{
   m_State = PRDM_STATE_ACTIVE;
   Resource().EventLog().LogEvent(
      GLCF_GL_RESOURCE_MAIN_ID,
      "PRProtocolDomainManager::ActivateProtocolManager().",
      GLEV_EVENT_LEVEL_1);
}

/******************************************************************************/
void PRProtocolDomainManager::DeactivateProtocolManager()
{
   m_State = PRDM_STATE_INACTIVE;
   Resource().EventLog().LogEvent(
      GLCF_GL_RESOURCE_MAIN_ID,
      "PRProtocolDomainManager::DeactivateProtocolManager().",
      GLEV_EVENT_LEVEL_1);
}

/******************************************************************************/
bool PRProtocolDomainManager::Active()
{
   return (State() == PRDM_STATE_ACTIVE) ? true : false;
}

/******************************************************************************/
GLResourceMain& PRProtocolDomainManager::Resource()
{
   return m_ResourceMain;
}

/******************************************************************************/
void PRProtocolDomainManager::ProcessMessage(
      std::shared_ptr<IONetworkControlMessage>& msgPtr)
{
   //TraceMessage(m_ModuleState, msgPtr);

   if (m_State == PRDM_STATE_ACTIVE)
   {
      switch (m_State)
      {
         case PRDM_STATE_ACTIVE:
            ProcessMessageStateActive(msgPtr);
            break;

         default:
            Resource().ErrorLog().LogError(
                  ModuleId(),
                  "ProcessMessage(): Unknown state.",
                  GLEL_ERROR_LEVEL_1);
      }
   }
}

/******************************************************************************/
void PRProtocolDomainManager::ProcessMessageStateActive(
      std::shared_ptr<IONetworkControlMessage>& msgPtr)
{
   std::string logStr = "PRProtocolDomainManager::ProcessMessageStateActive(): " +
      std::string(IONetworkControlMessage::MessageName(msgPtr->MessageId()));
   Resource().EventLog().LogEvent(
      ModuleId(),
      logStr.c_str(),
      GLEV_EVENT_LEVEL_1);

   switch (msgPtr->MessageId())
   {
      case IONW_CONTROL_MSG_PING_INTERFACE:
         EventPingMsgRcvdStateActive(msgPtr);
         break;

      case IONW_CONTROL_MSG_REQUEST_INTERFACE_CONTROL:
         EventRqstIntfMsgRcvdStateActive(msgPtr);
         break;

      case IONW_CONTROL_MSG_RESTART_SOC:
         break;

      case IONW_CONTROL_MSG_REBOOT_ECU:
         break;

      case IONW_CONTROL_MSG_GET_SOC_SW_VERSION_STRING:
         break;

      case IONW_CONTROL_MSG_GET_MCU_SW_VERSION_STRING:
         break;

      case IONW_CONTROL_MSG_GET_SWITCH_SW_VERSIONS_STRING:
         break;

      case IONW_CONTROL_MSG_GET_SOC_TEMPERATURE:
         break;

      case IONW_CONTROL_MSG_GET_SOC_TEMPERATURE_LIMIT:
         break;

      case IONW_CONTROL_MSG_GET_SOC_VOLTAGE:
         break;

      case IONW_CONTROL_MSG_GET_SOC_LIMIT:
         break;

      case IONW_CONTROL_MSG_SHUTDOWN_INTERFACE:
         Resource().InterfaceManager().StopNetworkControlInterface();
         break;

      default:
         std::string errStr =
               "ProcessMessageStateActive(): Unhandled message. Id = " +
               std::to_string(msgPtr->MessageId());
         Resource().ErrorLog().LogError(
               ModuleId(),
               errStr.c_str(),
               GLEL_ERROR_LEVEL_1);
         break;
   }
}

/******************************************************************************/
bool PRProtocolDomainManager::EventPingMsgRcvdStateActive(std::shared_ptr<IONetworkControlMessage>& msgPtr)
{
   // Any  actions go here.
   // Send response to message IONW_CONTROL_MSG_PING_INTERFACE
   uint8_t msglen = m_theIoNwControlMessageFixedLengthBytes;
   std::array<uint8_t, m_theIoNwControlMessageFixedLengthBytes> message =
      {0x55,0xAA,0x00,0xff,0xaa,0x55,0xff,0x00, // sync pattern
       IONW_CONTROL_MSG_PING_INTERFACE_RSP >> 8,
       IONW_CONTROL_MSG_PING_INTERFACE_RSP & 0x00ff, // command IONW_CONTROL_MSG_PING_INTERFACE_RSP
       14, // number of data bytes
       1,  // message format version
       0x00, 0x00, //msg security number
       0x00, 0x00, // reserved 1
       0x00, 0x00, // msg verification value (CRC)
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // msg data

   bool success = SendResponseMessage(IONW_CONTROL_MSG_PING_INTERFACE_RSP, message, msglen);

   return (success);
}

/******************************************************************************/
bool PRProtocolDomainManager::EventRqstIntfMsgRcvdStateActive(std::shared_ptr<IONetworkControlMessage>& msgPtr)
{
   // Any actions go here
   // Send response to message IONW_CONTROL_MSG_REQUEST_INTERFACE_CONTROL
   uint8_t msglen = m_theIoNwControlMessageFixedLengthBytes;
   std::array<uint8_t, m_theIoNwControlMessageFixedLengthBytes> message =
      {0x55,0xAA,0x00,0xff,0xaa,0x55,0xff,0x00, // sync pattern
       IONW_CONTROL_MSG_REQUEST_INTERFACE_CONTROL_RSP >> 8,
       IONW_CONTROL_MSG_REQUEST_INTERFACE_CONTROL_RSP & 0x00ff, // command IONW_CONTROL_MSG_PING_INTERFACE_RSP
       14, // number of data bytes
       1,  // message format version
       0x00, 0x00, //msg security number
       0x00, 0x00, // reserved 1
       0x00, 0x00, // msg verification value (CRC)
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // msg data

   bool success = SendResponseMessage(IONW_CONTROL_MSG_REQUEST_INTERFACE_CONTROL_RSP, message, msglen);

   return (success);
}

/******************************************************************************/
bool PRProtocolDomainManager::SendResponseMessage(
      IONetworkControlMsgIds msgId,
      std::array<uint8_t, m_theIoNwControlMessageFixedLengthBytes>& response,
      uint8_t msglen)
{
   bool success =
      Resource().InterfaceManager().SendResponseMessageToSource(response.data(), msglen);
   if (success)
   {
      std::string logStr =
         "EventRqstIntfMsgRcvdStateActive(): sending SUCCESS for " +
         IONetworkControlMessage::MessageName(msgId);
      Resource().EventLog().LogEvent(
         ModuleId(),
         logStr.c_str(),
         GLEV_EVENT_LEVEL_1);
   }
   else
   {
      std::string errStr =
         "SendResponseMessage(): Sending response FAIL for " +
         IONetworkControlMessage::MessageName(msgId);
      Resource().ErrorLog().LogError(
         ModuleId(),
         errStr.c_str(),
         GLEL_ERROR_LEVEL_1);
   }

   return (success);
}

/******************************************************************************/