/******************************************************************************/
/*  UDP Command Response Protocol Server                                      */
/*  Copyright © 2023 Mark Nispel                                              */
/*  All rights reserved                                                       */
/*  Unauthorized use, distribution or duplication is                          */
/*  strictly prohibited without written authorization.                        */
/******************************************************************************/
/**
   @file IONetworkUdpHelper.cpp
   @author Mark Nispel
   @date Nov 1, 2023
   @version 1.0
   @brief FILE NOTES:
   This file contains the implementation for the message source helper which
   is used by every system module in the application which are driven by
   the message routing system.
*/
/******************************************************************************/
/*       I N C L U D E S                                                      */
/******************************************************************************/
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <array>

#include "GLConfigureSystemModules.h"
#include "GLResourceMain.h"
#include "IONetworkControlMessage.h"
#include "IONetworkUdpHelperIntf.h"
#include "IONetworkUdpHelper.h"

using namespace MDN;

/******************************************************************************/
/*                        C O N S T A N T S                                   */
/******************************************************************************/
// System Module Constants
const int IONetworkUdpHelper::m_theSocketInvalidValue = -1;
const int IONetworkUdpHelper::m_thePortInvalidValue = -1;

/******************************************************************************/
/*       I M P L E M E N T A T I O N                                          */
/******************************************************************************/
IONetworkUdpHelper::IONetworkUdpHelper(
   IONetworkUdpHelperIntf& parent,
   int32_t port,
   uint32_t maxMessageLenBytes)
   :
   m_Parent(parent),
   m_State(IOUDPH_STATE_INACTIVE),
   m_Sockfd(m_theSocketInvalidValue),
   m_errno(NO_ERROR),
   m_UdpMaxMsgLenBytes((size_t)maxMessageLenBytes),
   m_PortNumber(port)
{
   m_SourceIpAddress.clear();
}

/******************************************************************************/
IONetworkUdpHelper::~IONetworkUdpHelper()
{

}

/******************************************************************************/
IONetworkUdpHelperIntf& IONetworkUdpHelper::Parent()
{
   return(m_Parent);
}

/******************************************************************************/
IONetworkUdpHelperState IONetworkUdpHelper::State()
{
   return(m_State);
}

/******************************************************************************/
bool IONetworkUdpHelper::Active()
{
   bool active = false;

   if (State() == IOUDPH_STATE_ACTIVE_WITH_PERMANENT_SOCKET)
   {
      active = true;
   }

   return (active);
}

/******************************************************************************/
bool IONetworkUdpHelper::ActivateUdpHelper()
{
   bool success = false;

   success = CreatePermanentUdpServerSocket();
   if(success)
   {
      m_State = IOUDPH_STATE_ACTIVE_WITH_PERMANENT_SOCKET;
      Parent().EventUdpHelperActive();
   };

   return (success);
}

/******************************************************************************/
std::string IONetworkUdpHelper::SourceIp()
{
   return m_SourceIpAddress;
}

/******************************************************************************/
bool IONetworkUdpHelper::ShutdownUdpHelper()
{
   m_State = IOUDPH_STATE_INACTIVE;
   Parent().EventUdpHelperInactive();

   return (true);
}

/******************************************************************************/
bool IONetworkUdpHelper::CreatePermanentUdpServerSocket()
{
   SOCKUDP_SOCKET_ADDR udpSockAddr;
   int32_t bindresult;
   int32_t createresult;
   bool success;

   success = false;
   m_errno = NO_ERROR;
   m_Sockfd = m_theSocketInvalidValue;

   createresult = NewDatagramSocket(&m_Sockfd);

   if (createresult)
   {
      SetSocketAddrStruct_INADDR_ANY(&m_SockAddr, m_PortNumber);

      bindresult = BindSocket(&m_Sockfd, &m_SockAddr);
      if (bindresult)
      {
         success = true;
      }
      else
      {
         CloseSocket(m_Sockfd);
         success = false;
      }
   }
   else
   {
      m_errno = errno;
      success = false;
   }
   return(success);
}

/**********************************************************/
bool IONetworkUdpHelper::NewDatagramSocket(SOCKUDP_SOCKET_FD *socketfd)
{
   bool success;

   success = FALSE;
   m_errno = NO_ERROR;

   *socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
   if (*socketfd != m_theSocketInvalidValue)
   {
      success = true;
   }
   else
   {
      success = false;
      m_errno = errno;
   }

   return(success);
}

/**********************************************************/
bool IONetworkUdpHelper::ReceiveMessage(
        uint8_t* message,
        int32_t& len,
        std::string& sourceIpAddress)
{
   bool success;
   sockaddr src_addr;
   socklen_t src_addr_len = sizeof(struct sockaddr);
   char ip[INET6_ADDRSTRLEN] = {0};

   success = FALSE;

   if (m_Sockfd != m_theSocketInvalidValue)
   {
      len = static_cast<int32_t>(recvfrom(
         m_Sockfd,
         message,
         MDN::m_theIoNwControlMessageFixedLengthBytes,
         0,
         &src_addr,
         &src_addr_len));

      if (len != SOCK_RECEIVE_FAILURE)
      {
         success = TRUE;

         if (src_addr.sa_family == AF_INET)
         {
            // get the ip address of the source of this message
            sockaddr_in *src_addr_in = reinterpret_cast<sockaddr_in*>(&src_addr);
            m_SourceIpAddress = std::string(
                  inet_ntop(AF_INET, &src_addr_in->sin_addr, ip, INET6_ADDRSTRLEN));
            sourceIpAddress = m_SourceIpAddress;
         }
      }
      else
      {
         success = FALSE;
      }
   }

   return(success);
}

/**********************************************************/
bool IONetworkUdpHelper::SendMessageWithTempUnconnectedSocket(
      uint8_t* message,
      int32_t len,
      std::string targetIPAddress,
      int32_t targetPort)
{
   SOCKUDP_SOCKET_FD tempTxSockFd;
   SOCKUDP_SOCKET_ADDR tempTxSockAddr;
   SOCKUDP_SOCKET_ADDR targetAddr;

   int32_t sendresult;
   bool success;

   sendresult = SOCK_SEND_FAIL;
   success = false;
   m_errno = NO_ERROR;

   if (NewDatagramSocket(&tempTxSockFd))
   {
      SetSocketAddrStruct_INADDR_ANY(&tempTxSockAddr, SOCKUDP_NULL_PORT);
      if (BindSocket(&tempTxSockFd, &tempTxSockAddr))
      {
         SetSocketAddrStruct_w_IPADDR(&targetAddr, targetIPAddress, targetPort);
         sendresult = sendto(
            tempTxSockFd,
            message,
            len,
            0,
            (struct sockaddr*)&targetAddr,
            sizeof(targetAddr)
            );

         if (sendresult != SOCK_SEND_FAIL)
         {
            success = TRUE;
         }
         else
         {
            success = FALSE;
            m_errno = errno;
         }
      }
      else
      {
         success = false;
      }
      CloseSocket(tempTxSockFd);
   }
   else
   {
      m_errno = errno;
      success = false;
   }

   return(success);
}

/**********************************************************/
bool IONetworkUdpHelper::BindSocket(
   SOCKUDP_SOCKET_FD * socketfd,
   SOCKUDP_SOCKET_ADDR * socketAddr)
{
   bool success = false;;
   int bindresult = SOCK_BIND_FAIL;
   m_errno = NO_ERROR;

   bindresult =
      bind(*socketfd,(struct sockaddr*)socketAddr, sizeof(*socketAddr));

   if (bindresult == SOCK_BIND_SUCCESS)
   {
      success = TRUE;
   }
   else
   {
      success = FALSE;
      m_errno = errno;
   }

   return(success);
}

/**********************************************************/
bool IONetworkUdpHelper::SendUDPMessage(
   uint8_t *message,
   int32_t len,
   std::string targetIPAddr,
   int32_t targetPort)
{
   bool success;
   int32_t sendresult;
   SOCKUDP_SOCKET_ADDR targetAddr;

   success = FALSE;
   sendresult = SOCK_SEND_FAIL;
   SetSocketAddrStruct_w_IPADDR(&targetAddr, targetIPAddr, targetPort);
   m_errno = NO_ERROR;

   sendresult = sendto(
         m_Sockfd,
         message,
         len,
         0,
         (struct sockaddr*)&targetAddr,
         sizeof(targetAddr)
         );

   if (sendresult != SOCK_SEND_FAIL)
   {
      success = TRUE;
   }
   else
   {
      success = FALSE;
      m_errno = errno;
   }

   return(success);
}

/**********************************************************/
bool IONetworkUdpHelper::CloseSocket(SOCKUDP_SOCKET_FD extSocketfd)
{
   bool success;
   int32_t closeresult;

   success = false;
   closeresult = SOCK_CLOSE_FAIL;
   m_errno = NO_ERROR;

   closeresult = close(extSocketfd);
   if (closeresult == SOCK_CLOSE_SUCCESS)
   {
      success = true;
   }
   else
   {
      success = false;
      m_errno = errno;
   }

   return(success);
}

/**********************************************************/
bool IONetworkUdpHelper::CloseSocket()
{
   bool success = false;

   if(CloseSocket(m_Sockfd))
   {
      m_Sockfd = m_theSocketInvalidValue;
      success = true;
   }

   return(success);
}

/**********************************************************/
void IONetworkUdpHelper::SetSocketAddrStruct_INADDR_ANY(
      SOCKUDP_SOCKET_ADDR* socketAddr,
      int32_t port)
{
   socketAddr->sin_family = AF_INET;
   socketAddr->sin_addr.s_addr = htonl(INADDR_ANY);
   socketAddr->sin_port = htons(port);
}

/**********************************************************/
void IONetworkUdpHelper::SetSocketAddrStruct_w_IPADDR(
      SOCKUDP_SOCKET_ADDR * socketAddr,
      std::string ipAddress,
      int32_t port)
{
   socketAddr->sin_family = AF_INET;
   socketAddr->sin_addr.s_addr = inet_addr(ipAddress.c_str());;
   socketAddr->sin_port = htons(port);
}
/******************************************************************************/
