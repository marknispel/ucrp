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
#ifndef io_network_control_messages_h
#define io_network_control_messages_h

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/
#include <map>
#include <string>

/******************************************************************************/
/*                              D E F I N E S                                 */
/******************************************************************************/

/******************************************************************************/
/*                  T Y P E D E F S  A N D  E N U M S                         */
/******************************************************************************/
namespace MDN
{
typedef enum
{
   IONW_CONTROL_MSG_FIRST_MESSAGE_ID = 0x00,

   // INBOUND CONTROL COMMANDS
   IONW_CONTROL_MSG_REQUEST_APP_SHUTDOWN = IONW_CONTROL_MSG_FIRST_MESSAGE_ID,
   IONW_CONTROL_MSG_PING_INTERFACE,
   IONW_CONTROL_MSG_REQUEST_INTERFACE_CONTROL,
   IONW_CONTROL_MSG_RESTART_SOC,
   IONW_CONTROL_MSG_REBOOT_ECU,
   IONW_CONTROL_MSG_GET_SOC_SW_VERSION_STRING,
   IONW_CONTROL_MSG_GET_MCU_SW_VERSION_STRING,
   IONW_CONTROL_MSG_GET_SWITCH_SW_VERSIONS_STRING,
   IONW_CONTROL_MSG_GET_SOC_TEMPERATURE,
   IONW_CONTROL_MSG_GET_SOC_TEMPERATURE_LIMIT,
   IONW_CONTROL_MSG_GET_SOC_VOLTAGE,
   IONW_CONTROL_MSG_GET_SOC_LIMIT,


   // OUTBOUND RESPONSES
   IONW_CONTROL_MSG_REQUEST_APP_SHUTDOWN_RSP = 0x8000,
   IONW_CONTROL_MSG_PING_INTERFACE_RSP,
   IONW_CONTROL_MSG_REQUEST_INTERFACE_CONTROL_RSP,
   IONW_CONTROL_MSG_RESTART_SOC_RSP,
   IONW_CONTROL_MSG_REBOOT_ECU_RSP,
   IONW_CONTROL_MSG_GET_SOC_SW_VERSION_STRING_RSP,
   IONW_CONTROL_MSG_GET_MCU_SW_VERSION_STRING_RSP,
   IONW_CONTROL_MSG_GET_SWITCH_SW_VERSIONS_STRING_RSP,
   IONW_CONTROL_MSG_GET_SOC_TEMPERATURE_RSP,
   IONW_CONTROL_MSG_GET_SOC_TEMPERATURE_LIMIT_RSP,
   IONW_CONTROL_MSG_GET_SOC_VOLTAGE_RSP,
   IONW_CONTROL_MSG_GET_SOC_LIMIT_RSP,

   IONW_CONTROL_MSG_SHUTDOWN_INTERFACE = 0xFFFF,

} IONetworkControlMsgIds;

/******************************************************************************/
/*                         D E C L A R A T I O N S                            */
/******************************************************************************/
static const std::map<IONetworkControlMsgIds, std::string> m_theIONetworkControlMsgNameMap
{
   {IONW_CONTROL_MSG_REQUEST_APP_SHUTDOWN, "REQUEST_APP_SHUTDOWN"},
   {IONW_CONTROL_MSG_PING_INTERFACE, "PING_INTERFACE"},
   {IONW_CONTROL_MSG_REQUEST_INTERFACE_CONTROL, "REQUEST_INTERFACE_CONTROL"},
   {IONW_CONTROL_MSG_RESTART_SOC, "RESTART SOC"},
   {IONW_CONTROL_MSG_REBOOT_ECU, "REBOOT ECU"},
   {IONW_CONTROL_MSG_GET_SOC_SW_VERSION_STRING, "GET SOC SW VERSION"},
   {IONW_CONTROL_MSG_GET_MCU_SW_VERSION_STRING, "GET_MCU_SW_VERSION"},
   {IONW_CONTROL_MSG_GET_SWITCH_SW_VERSIONS_STRING, "GET SWITCH SW VERSION"},
   {IONW_CONTROL_MSG_GET_SOC_TEMPERATURE, "GET SOC TEMPERATURE"},
   {IONW_CONTROL_MSG_GET_SOC_TEMPERATURE_LIMIT, "GET SOC TEMPERATURE LIMIT"},
   {IONW_CONTROL_MSG_GET_SOC_VOLTAGE, "GET SOC VOLTAGE"},
   {IONW_CONTROL_MSG_GET_SOC_LIMIT, "GET SOC VOLTAGE LIMIT"},

   {IONW_CONTROL_MSG_REQUEST_APP_SHUTDOWN_RSP, "REQUEST_APP_SHUTDOWN_RSP"},
   {IONW_CONTROL_MSG_PING_INTERFACE_RSP, "PING_INTERFACE_RSP"},
   {IONW_CONTROL_MSG_REQUEST_INTERFACE_CONTROL_RSP, "REQUEST_INTERFACE_CONTROL_RSP"},
   {IONW_CONTROL_MSG_RESTART_SOC_RSP, "RESTART SOC_RSP"},
   {IONW_CONTROL_MSG_REBOOT_ECU_RSP, "REBOOT ECU_RSP"},
   {IONW_CONTROL_MSG_GET_SOC_SW_VERSION_STRING_RSP, "GET SOC SW VERSION_RSP"},
   {IONW_CONTROL_MSG_GET_MCU_SW_VERSION_STRING_RSP, "GET_MCU_SW_VERSION_RSP"},
   {IONW_CONTROL_MSG_GET_SWITCH_SW_VERSIONS_STRING_RSP, "GET SWITCH SW VERSION_RSP"},
   {IONW_CONTROL_MSG_GET_SOC_TEMPERATURE_RSP, "GET SOC TEMPERATURE_RSP"},
   {IONW_CONTROL_MSG_GET_SOC_TEMPERATURE_LIMIT_RSP, "GET SOC TEMPERATURE LIMIT_RSP"},
   {IONW_CONTROL_MSG_GET_SOC_VOLTAGE_RSP, "GET SOC VOLTAGE_RSP"},
   {IONW_CONTROL_MSG_GET_SOC_LIMIT_RSP, "GET SOC VOLTAGE LIMIT_RSP"},

   {IONW_CONTROL_MSG_SHUTDOWN_INTERFACE, "SHUTDOWN NETWORK CONTROL INTERFACE"},
};

static const std::string m_theUnknwnMessageName = "UNKNOWN MESSAGE NAME";
}

/******************************************************************************/

#endif /* io_network_control_messages_h */