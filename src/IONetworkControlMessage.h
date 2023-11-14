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
   @date Nov 4, 2023
   @version 1.0
   @brief FILE NOTES:
   This file contains the definitions for the message functionality
   for the application.
*/
/******************************************************************************/
#ifndef io_network_control_msg_h
#define io_network_control_msg_h

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/
#include <memory>

#include "IONetworkControlMessages.h"

/******************************************************************************/
/*                              D E F I N E S                                 */
/******************************************************************************/
namespace MDN
{
   static const uint16_t m_theIoNwControlMessageMaximumLengthBytes = 64;
   static const uint16_t m_theIoNwControlMessageFixedLengthBytes = 32;
   static const uint16_t m_theIoNwControlMessageHeaderSizeBytes = 18;
   static const uint16_t m_theIONwControlMessageDataBytesBlockSizeBytes =
      m_theIoNwControlMessageFixedLengthBytes -
      m_theIoNwControlMessageHeaderSizeBytes;
   static const uint64_t m_theIoNetworkControlMsgHeaderSyncPattern = 0x55AA00FFAA55FF00;
   static const uint16_t m_theSyncPatternSizeBytes = sizeof(m_theIoNetworkControlMsgHeaderSyncPattern);

   typedef struct message_header_struct
   {
      uint64_t msgFixedSyncPattern = m_theIoNetworkControlMsgHeaderSyncPattern;
      uint16_t msgId;
      uint8_t  numberOfDataBytes;
      uint8_t  dataFormatVersion;
      uint16_t securityNumber;
      uint16_t headerReserved1;
      uint16_t msgVerification;
   } IO_NETWORK_CONTROL_MESSAGE_HEADER_TYPE;

   typedef struct message_struct
   {
      IO_NETWORK_CONTROL_MESSAGE_HEADER_TYPE Header;
      // FIRST POC DO FIXED LENGTH COMMAND MESSAGES OF m_theIoNwControlMessageFixedLengthBytes
      uint8_t  msgData[m_theIONwControlMessageDataBytesBlockSizeBytes];
   } IO_NETWORK_CONTROL_MESSAGE_TYPE;

/******************************************************************************/
/*                  T Y P E D E F S  A N D  E N U M S                         */
/******************************************************************************/

/******************************************************************************/
/*                         D E C L A R A T I O N S                            */
/******************************************************************************/

class IONetworkControlMessage
{
   public:
      IONetworkControlMessage(uint8_t *receivedMsgBytes);
      ~IONetworkControlMessage();

      // S T A T I C  M E T H O D S
      static const std::string& MessageName(IONetworkControlMsgIds MsgId);
      static const std::string& MessageName(uint16_t MsgId);
      static bool ValidateReceivedMessage(uint8_t *msgPtr, int32_t len);

      // G E T T E R S  /  S E T T E R S
      uint16_t MessageId();
      uint8_t NumberOfDataBytes();

   private:
      IO_NETWORK_CONTROL_MESSAGE_TYPE m_msg;

};
   typedef std::shared_ptr<IONetworkControlMessage> IONetworkControlMessagePtr;
}

/******************************************************************************/

#endif /* io_network_control_msg_h */