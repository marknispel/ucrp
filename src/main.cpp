/******************************************************************************/
/*  UDP Command Response Protocol Server                                      */
/*  Copyright © 2023 Mark Nispel                                              */
/*  All rights reserved                                                       */
/*  Unauthorized use, distribution or duplication is                          */
/*  strictly prohibited without written authorization.                        */
/******************************************************************************/
/**
   @file main.cpp
   @author Mark Nispel
   @date Oct 31, 2023
   @version 1.0
   @brief FILE NOTES:
   This file contains the main method of the application.
*/
/******************************************************************************/
/*       I N C L U D E S                                                      */
/******************************************************************************/
#include <unistd.h>
#include <memory>
#include <iostream>
#include <string>

#include "GLErrorLog.h"
#include "GLEventLog.h"
#include "GLResourceMain.h"
#include "IONetworkControlInterfaceManager.h"
#include "IONetworkControlMessage.h"
#include "IONetworkUdpHelper.h"

/******************************************************************************/
/*       T Y P E D E F S                                                      */
/******************************************************************************/
typedef std::unique_ptr<MDN::GLResourceMain> GLResourceMainPtrType;

/******************************************************************************/
/*       D E C L A R A T I O N S                                              */
/******************************************************************************/

/******************************************************************************/
/*       G L O B A L  V A R S                                                 */
/******************************************************************************/
GLResourceMainPtrType m_GLResourceMainPtr;

/******************************************************************************/
int main()
{
   m_GLResourceMainPtr = std::make_unique<MDN::GLResourceMain>();

   m_GLResourceMainPtr->AppStart();

   if (m_GLResourceMainPtr->AppThreadMode() == MDN::GLRM_THREE_THREADS_MODE)
   {
      // in GLRM_THREE_THREADS_MODE this main thread is the monitor thread and
      // and is free to execute tasks aside from message receive and message
      // execute.
      // In this mode we can shut down the app via a client SHUTDOWN command, or
      // by the the monitor thread calling m_GLResourceMainPtr->AppStop() by
      // exiting this conditional loop here {e.g. due to timeout}.
      int seconds = 0;
      while (m_GLResourceMainPtr->Active())
      {
         // Monitor thread: implement other functionality here.;
         sleep(1);
         if (seconds++ == 5)
            break;
      }
   }
   else
   {
      // in GLRM_ONE_THREAD_MODE OR GLRM_TWO_THREADS_MODE this main thread is
      // the socket thread.  It is blocked by ReceiveMessage in the UdpHelper,
      // until the client sends a SHUTDOWN message. So nothing to do here.
   }

   m_GLResourceMainPtr->AppStop();

   m_GLResourceMainPtr->EventLog().PrintEventLogEntries();
   m_GLResourceMainPtr->ErrorLog().PrintErrorLogEntries();

   return 0;
}

/******************************************************************************/
