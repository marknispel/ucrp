/******************************************************************************/
/*  UDP Command Response Protocol Server                                      */
/*  Copyright © 2023 Mark Nispel                                              */
/*  All rights reserved                                                       */
/*  Unauthorized use, distribution or duplication is                          */
/*  strictly prohibited without written authorization.                        */
/******************************************************************************/
/**
   @file IONetworkControlMessage.cpp
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
#include "IONetworkUdpHelper.h"
#include "IONetworkControlMessage.h"

using namespace MDN;

/******************************************************************************/
/*                     I M P L E M E N T A T I O N                            */
/******************************************************************************/
IONetworkControlMessage::IONetworkControlMessage(uint8_t *message)
{
   uint16_t index = m_theSyncPatternSizeBytes;
   m_msg.Header.msgId = message[index++] << 8 | message[index++];
   m_msg.Header.numberOfDataBytes = message[index++];
   m_msg.Header.dataFormatVersion = message[index++];
   m_msg.Header.securityNumber = message[index++] << 8 | message[index++];
   m_msg.Header.headerReserved1 = message[index++] << 8 | message[index++];
   m_msg.Header.msgVerification = message[index++] << 8 | message[index++];

   index = 0;
   while (index < m_theIONwControlMessageDataBytesBlockSizeBytes)
   {
      m_msg.msgData[index] = message[m_theIoNwControlMessageHeaderSizeBytes + index];
      index++;
   }
}

/******************************************************************************/
IONetworkControlMessage::~IONetworkControlMessage()
{
}

/******************************************************************************/
uint16_t IONetworkControlMessage::MessageId()
{
   return m_msg.Header.msgId;
}

/******************************************************************************/
uint8_t IONetworkControlMessage::NumberOfDataBytes()
{
   return m_msg.Header.numberOfDataBytes;
}

// Static Methods
/******************************************************************************/
bool IONetworkControlMessage::ValidateReceivedMessage(uint8_t *msgPtr, int32_t len)
{
   bool valid = false;
   uint16_t index = 0;

   if (  msgPtr[index++] == 0x55 && msgPtr[index++] == 0xAA &&
         msgPtr[index++] == 0x00 && msgPtr[index++] == 0xFF &&
         msgPtr[index++] == 0xAA && msgPtr[index++] == 0x55 &&
         msgPtr[index++] == 0xFF && msgPtr[index++] == 0x00 &&
         len == m_theIoNwControlMessageFixedLengthBytes)
   {
      // MessageType = IONetworkControlMessageType;
      valid = true;
   }

   return (valid);
}

/******************************************************************************/
const std::string& IONetworkControlMessage::MessageName(IONetworkControlMsgIds MsgId)
{
   return m_theIONetworkControlMsgNameMap.at(MsgId);
}

/******************************************************************************/
const std::string& IONetworkControlMessage::MessageName(uint16_t MsgId)
{
   return m_theIONetworkControlMsgNameMap.at((IONetworkControlMsgIds) MsgId);
}

/******************************************************************************/
