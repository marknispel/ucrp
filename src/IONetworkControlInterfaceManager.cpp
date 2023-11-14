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
   @date Feb 5, 2022
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

#include "GLResourceMain.h"
#include "IONetworkUdpHelper.h"
#include "IONetworkControlMessage.h"
#include "IONetworkControlMessages.h"
#include "GLEventLog.h"
#include "GLErrorLog.h"
#include "PRProtocolDomainManager.h"
#include "IONetworkControlInterfaceManager.h"

using namespace MDN;

/******************************************************************************/
/*                  T Y P E D E F S  A N D  E N U M S                         */
/******************************************************************************/

/******************************************************************************/
/*                        C O N S T A N T S                                   */
/******************************************************************************/
const uint16_t theIoNetworkControlInterfacePort = 49153;

/******************************************************************************/
/*                     I M P L E M E N T A T I O N                            */
/******************************************************************************/
IONetworkControlInterfaceManager::IONetworkControlInterfaceManager(GLResourceMain& resource)
   :
   m_DomainId(GLCF_IO_NETWORK_DOMAIN_ID),
   m_ModuleId(GLCF_GL_IO_NETWORK_CONTROL_INTERFACE_MANAGER_ID),
   m_State(IONCIM_STATE_INACTIVE),
   m_ResourceMain(resource),
   m_ReceiveActive(false),
   m_UdpHelperPtr(std::make_unique<IONetworkUdpHelper>(
      *this,
      theIoNetworkControlInterfacePort,
      MDN::m_theIoNwControlMessageMaximumLengthBytes))
{

}

/******************************************************************************/
IONetworkControlInterfaceManager::~IONetworkControlInterfaceManager()
{

}

/******************************************************************************/
const GLCFDomainIds IONetworkControlInterfaceManager::DomainId()
{
   return m_DomainId;
}

/******************************************************************************/
const GLCFModuleIds IONetworkControlInterfaceManager::ModuleId()
{
   return m_ModuleId;
}

/******************************************************************************/
const std::string& IONetworkControlInterfaceManager::ModuleName()
{
   return m_theModuleNameMap.at(ModuleId());
}

/******************************************************************************/
IONetworkControlInterfaceManagerStateType IONetworkControlInterfaceManager::State()
{
   return m_State;
}

/******************************************************************************/
bool IONetworkControlInterfaceManager::Active()
{
   return (State() == IONCIM_STATE_ACTIVE) ? true : false;
}

/******************************************************************************/
GLResourceMain& IONetworkControlInterfaceManager::Resource()
{
   return m_ResourceMain;
}

/******************************************************************************/
IONetworkUdpHelper& IONetworkControlInterfaceManager::UdpHelper()
{
   return *m_UdpHelperPtr;
}

/******************************************************************************/
void IONetworkControlInterfaceManager::StartNetworkControlInterface()
{
   if(UdpHelper().ActivateUdpHelper())
   {
      Resource().EventLog().LogEvent(
         MDN::GLCF_GL_IO_NETWORK_CONTROL_INTERFACE_MANAGER_ID,
         "StartNetworkControlInterface:: Active.",
         MDN::GLEV_EVENT_LEVEL_1);

      m_ReceiveActive = true;
      ControlInterfaceReceive();
   }
   else
   {
      Resource().ErrorLog().LogError(
         ModuleId(),
         "StartNetworkControlInterface(): Start FAIL.",
         GLEL_ERROR_LEVEL_1);
   }
}

/******************************************************************************/
void IONetworkControlInterfaceManager::StopNetworkControlInterface()
{
   if(UdpHelper().ShutdownUdpHelper())
   {
      m_ReceiveActive = false;

      Resource().EventLog().LogEvent(
            MDN::GLCF_GL_IO_NETWORK_CONTROL_INTERFACE_MANAGER_ID,
            "StopNetworkControlInterface.",
            MDN::GLEV_EVENT_LEVEL_1);
   }
   else
   {
      Resource().ErrorLog().LogError(
         ModuleId(),
         "StopNetworkControlInterface(): Stop FAIL.",
         GLEL_ERROR_LEVEL_1);
   }
}

/******************************************************************************/
void IONetworkControlInterfaceManager::ControlInterfaceReceive()
{
   uint8_t message[m_theIoNwControlMessageFixedLengthBytes + 1];
   int32_t len = 0;
   std::string sourceIpAddress;
   bool success = false;

   if (UdpHelper().Active())
   {
      while (m_ReceiveActive)
      {
         success = UdpHelper().ReceiveMessage(message, len, sourceIpAddress);

         if (success && (len > MDN::SOCK_RECEIVE_FAILURE))
         {

            if (IONetworkControlMessage::ValidateReceivedMessage(message, len))
            {
               std::string eventStr;
               IONetworkControlMsgIds msgId = (IONetworkControlMsgIds)(message[8] << 8 | message[9]);
               std::string msgName = std::string(IONetworkControlMessage::MessageName(msgId));

               IONetworkControlMessagePtr newMsgPtr = std::make_shared<IONetworkControlMessage>(message);

               MessageToBeProcessed(newMsgPtr);
            }
         }
      }
   }
}

/******************************************************************************/
void IONetworkControlInterfaceManager::MessageToBeProcessed(
   std::shared_ptr<IONetworkControlMessage>& msgPtr)
{
   std::string logStr = "Message to be processed: " +
      std::string(IONetworkControlMessage::MessageName(msgPtr->MessageId()));
   Resource().EventLog().LogEvent(
      ModuleId(),
      logStr.c_str(),
      GLEV_EVENT_LEVEL_1);

   switch (msgPtr->MessageId())
   {
      // if necessary you can intercept a message here and act upon
      // the message locally.
      default:
         Resource().ProtocolManager().ProcessMessage(msgPtr);
         break;
   }
}

/******************************************************************************/
/*               N E T W O R K  U D P  I N T F  M E T H O D S                 */
/******************************************************************************/
void IONetworkControlInterfaceManager::EventUdpHelperActive()
{
   // Executed on the main thread as part of StartNetworkControlInterface
   std::string logStr = "EventUdpHelperActive.";
   Resource().EventLog().LogEvent(
      ModuleId(),
      logStr.c_str(),
      GLEV_EVENT_LEVEL_1);
}

/******************************************************************************/
void IONetworkControlInterfaceManager::EventUdpHelperInactive()
{
   // Executed on the main thread as part of StopNetworkControlInterface
   std::string logStr = "EventUdpHelperInactive.";
   Resource().EventLog().LogEvent(
      ModuleId(),
      logStr.c_str(),
      GLEV_EVENT_LEVEL_1);
}

/******************************************************************************/
bool IONetworkControlInterfaceManager::SendResponseMessageToSource(
   uint8_t* message,
   int32_t len)
{
   bool success = UdpHelper().SendMessageWithTempUnconnectedSocket(
      message,
      len,
      UdpHelper().SourceIp(),
      theIoNetworkControlInterfacePort);

   std::string logStr = "SendResponseMessageToSource: ip:"
      + UdpHelper().SourceIp() + ", port: " + std::to_string(theIoNetworkControlInterfacePort);
   Resource().EventLog().LogEvent(
      ModuleId(),
      logStr.c_str(),
      GLEV_EVENT_LEVEL_1);
   return (success);
}

/******************************************************************************/
void IONetworkControlInterfaceManager::TestUdpTxWithTempSocket()
{
   const std::string TX_TARGET_IP_ADDRESS("127.0.0.1");
   const int32_t TX_TARGET_PORT = 49153;

   //uint8_t message[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   //uint8_t msglen = 10;
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
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // msg databool success;

   bool success = UdpHelper().SendMessageWithTempUnconnectedSocket(
                     message.data(),
                     msglen,
                     TX_TARGET_IP_ADDRESS,
                     TX_TARGET_PORT);
   if (success)
   {
      Resource().EventLog().LogEvent(
         MDN::GLCF_GL_IO_NETWORK_CONTROL_INTERFACE_MANAGER_ID,
         "TestUdpTxWithTempSocket(): Send Udp Msg Success.",
         MDN::GLEV_EVENT_LEVEL_1);
   }
   else
   {
      Resource().ErrorLog().LogError(
         MDN::GLCF_GL_IO_NETWORK_CONTROL_INTERFACE_MANAGER_ID,
         "TestUdpTxWithTempSocket(): TX Udp Msg Failure",
         MDN::GLEL_ERROR_LEVEL_1);
   }
}

/******************************************************************************/
void IONetworkControlInterfaceManager::TestUdpRx()
{
   const int32_t RX_TEST_PORT = 49154;
   const int32_t RX_MESSAGE_LEN = MDN::the_IONW_UDP_API_MAX_MESSAGE_LEN;

   bool success;
   uint8_t message[RX_MESSAGE_LEN + 1];
   int32_t len = 0;
   std::string sourceIpAddress;

   if (UdpHelper().Active())
   {
      success = UdpHelper().ReceiveMessage(message, len, sourceIpAddress);

      if (len > MDN::SOCK_RECEIVE_FAILURE)
      {
         std::string eventStr = "TestUdpRx(): Udp message received from " +
            sourceIpAddress;
         Resource().EventLog().LogEvent(
               MDN::GLCF_GL_IO_NETWORK_CONTROL_INTERFACE_MANAGER_ID,
               eventStr.c_str(),
               MDN::GLEV_EVENT_LEVEL_1);

         eventStr.clear();
         eventStr = "TestUdpRx(): msg is: ";
         for (int i = 0; i < len; i++)
         {
            char c = message[i];
            eventStr.append(std::to_string(message[i]));
            eventStr.append(" ");
         }
         Resource().EventLog().LogEvent(
            MDN::GLCF_GL_IO_NETWORK_CONTROL_INTERFACE_MANAGER_ID,
            eventStr.c_str(),
            MDN::GLEV_EVENT_LEVEL_1);

         if (IONetworkControlMessage::ValidateReceivedMessage(message, len))
         {
            eventStr.clear();
            eventStr = "TestUdpRx(): msg validated.";
            Resource().EventLog().LogEvent(
               MDN::GLCF_GL_IO_NETWORK_CONTROL_INTERFACE_MANAGER_ID,
               eventStr.c_str(),
               MDN::GLEV_EVENT_LEVEL_1);

            eventStr.clear();
            IONetworkControlMsgIds msgId = (IONetworkControlMsgIds)(message[8] << 8 | message[9]);
            std::string msgName = std::string(IONetworkControlMessage::MessageName(msgId));

            eventStr = "TestUdpRx(): msg name is " + msgName;
            Resource().EventLog().LogEvent(
               MDN::GLCF_GL_IO_NETWORK_CONTROL_INTERFACE_MANAGER_ID,
               eventStr.c_str(),
               MDN::GLEV_EVENT_LEVEL_1);

            IONetworkControlMessage newMessage(message);
         }
      }
   }
   else
   {
      Resource().ErrorLog().LogError(
         MDN::GLCF_NON_SYSTEM_MODULE_ID,
         "TestUdpRx(): Create Socket Failure",
         MDN::GLEL_ERROR_LEVEL_1);
   }
}
/******************************************************************************/