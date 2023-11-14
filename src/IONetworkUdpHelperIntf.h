/******************************************************************************/
/*  UDP Command Response Protocol Server                                      */
/*  Copyright © 2023 Mark Nispel                                              */
/*  All rights reserved                                                       */
/*  Unauthorized use, distribution or duplication is                          */
/*  strictly prohibited without written authorization.                        */
/******************************************************************************/
/**
   @file IONetworkUdpHelperIntf.h
   @author Mark Nispel
   @date Feb 7, 2022
   @version 1.0
   @brief FILE NOTES:
   This file contains the definitions for the global router class
   for the application.
*/
/******************************************************************************/
#ifndef io_network_helper_intf_h
#define io_network_helper_intf_h

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/
#include <memory>

/******************************************************************************/
/*                         D E C L A R A T I O N S                            */
/******************************************************************************/
namespace MDN
{
class IONetworkUdpHelperIntf
{
   // Methods required to interact with the IONetworkUdpHelper
   // included as a member object of a system module object.

   public:
      // These methods are called on the system object by the helper.
      virtual void EventUdpHelperActive() = 0;
      virtual void EventUdpHelperInactive() = 0;

      virtual ~IONetworkUdpHelperIntf() {};
};

}

/******************************************************************************/

#endif /* io_network_helper_intf_h */