/******************************************************************************/
/*  UDP Command Response Protocol Server                                      */
/*  Copyright © 2023 Mark Nispel                                              */
/*  All rights reserved                                                       */
/*  Unauthorized use, distribution or duplication is                          */
/*  strictly prohibited without written authorization.                        */
/******************************************************************************/
/**
   @file IONetworkTcpClientHelper.h
   @author Mark Nispel
   @date Nov 1, 2023
   @version 1.0
   @brief FILE NOTES:
   This file contains the definitions for message queue helper which is
   used by every system module in the application to provide a message
   queue.
*/
/******************************************************************************/
#ifndef io_network_udp_helper_h
#define io_network_udp_helper_h

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/
#include <errno.h>
#include <memory>
#include <string>
#include <array>
#include <map>
#include <netinet/in.h>

/******************************************************************************/
/*                           D E F I N E S                                    */
/******************************************************************************/
namespace MDN
{
static const std::string SOCKUDP_TXTEST_LOOPBACK_IP_ADDRESS("127.0.0.1");
static const uint32_t the_IONW_UDP_API_MAX_MESSAGE_LEN = 64;
static const int NO_ERROR = 0;
static const bool SOCKUDP_NEW_PERMANENT_SOCKET = true;
static const bool SOCKUDP_NEW_TEMP_SOCKET = false;

static const int SOCKUDP_NULL_SOCKET_FD = -1;
static const int SOCKUDP_NULL_PORT = 0;

static const int SOCK_RECEIVE_FAILURE = -1;
static const int SOCK_BIND_SUCCESS = 0;
static const int SOCK_BIND_FAIL = -1;
static const int SOCK_CLOSE_SUCCESS = 0;
static const int SOCK_CLOSE_FAIL = -1;
static const int SOCK_SEND_FAIL = -1;
static const int SOCK_INVALID_SOCKET_FD =-1;
}

/******************************************************************************/
/*                        T Y P E D E F S                                     */
/******************************************************************************/
namespace MDN
{

typedef enum
{
   IOUDPH_STATE_INACTIVE,
   IOUDPH_STATE_ACTIVE_WITH_PERMANENT_SOCKET,
} IONetworkUdpHelperState;

typedef struct sockaddr_in SOCKUDP_SOCKET_ADDR;
typedef int32_t SOCKUDP_SOCKET_FD;
typedef struct in_addr SOCKUDP_INET_ADDR;
typedef struct ip_mreq SOCKUDP__REQUEST;
typedef std::array<uint8_t, the_IONW_UDP_API_MAX_MESSAGE_LEN> SOCKUDP_BUFFER_TYPE;
}

/******************************************************************************/
/*                         D E C L A R A T I O N S                            */
/******************************************************************************/
namespace MDN
{

class IONetworkUdpHelperIntf;

class IONetworkUdpHelper
{
   public:
      IONetworkUdpHelper(
         IONetworkUdpHelperIntf& parent,
         int32_t port,
         uint32_t maxMessageLenBytes);
      ~IONetworkUdpHelper();


      bool ReceiveMessage(
         uint8_t* message,
         int32_t& len,
         std::string& sourceIpAddress);
      bool SendResponseMessageToSource(
         uint8_t* message,
         int32_t len);
      bool SendMessageWithTempUnconnectedSocket(
         uint8_t* message,
         int32_t len,
         std::string targetIPAddress,
         int32_t targetPort);
      bool SendUDPMessage(
         uint8_t *message,
         int32_t len,
         std::string targetIPAddr,
         int32_t targetPort);
      bool ShutdownUdpHelper();
      bool ActivateUdpHelper();
      bool Active();
      std::string SourceIp();

   private:
      bool CloseSocket(SOCKUDP_SOCKET_FD extSocketfd);
      bool CloseSocket();
      bool CreatePermanentUdpServerSocket();
      bool NewDatagramSocket(SOCKUDP_SOCKET_FD *socketfd);
      bool BindSocket(
         SOCKUDP_SOCKET_FD * socketfd,
         SOCKUDP_SOCKET_ADDR * socketAddr);
      void SetSocketAddrStruct_INADDR_ANY(
         SOCKUDP_SOCKET_ADDR* socketAddr,
         int32_t port);
      void SetSocketAddrStruct_w_IPADDR(
         SOCKUDP_SOCKET_ADDR * socketAddr,
         std::string ipAddress,
         int32_t port);
      IONetworkUdpHelperState State();
      IONetworkUdpHelperIntf& Parent();

      IONetworkUdpHelperState m_State;
      SOCKUDP_SOCKET_FD m_Sockfd;
      SOCKUDP_SOCKET_ADDR m_SockAddr;
      IONetworkUdpHelperIntf& m_Parent;
      int m_errno;
      std::string m_SourceIpAddress;
      uint32_t m_UdpMaxMsgLenBytes;
      const int32_t m_PortNumber;

      // C L A S S  C O N S T A N T S
      static const int m_theSocketInvalidValue;
      static const int m_thePortInvalidValue;
};

typedef std::unique_ptr<IONetworkUdpHelper> IONetworkUdpHelperPtrType;

}
#endif /* io_network_udp_helper_h */

/******************************************************************************/
