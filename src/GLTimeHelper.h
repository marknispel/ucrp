/******************************************************************************/
/*  UDP Command Response Protocol Server                                      */
/*  Copyright © 2023 Mark Nispel                                              */
/*  All rights reserved                                                       */
/*  Unauthorized use, distribution or duplication is                          */
/*  strictly prohibited without written authorization.                        */
/******************************************************************************/
/**
   @file GLTimeHelper.h
   @author Mark Nispel
   @date Oct 31, 2023
   @version 1.0
   @brief FILE NOTES:
   This file defines the class which manages real time references for the
   application.
*/
/*****************************************************************************/
#ifndef time_helper_h
#define time_helper_h

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/
#include <chrono>
#include <ctime>
#include <stdio.h>
#include <array>

#include "GLConfigureDomains.h"
#include "GLTypedefs.h"

using hrc = std::chrono::high_resolution_clock;

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/
namespace MDN
{

typedef enum
{
   GLTH_TIMESTAMP_MODE_FROM_SYSTEM_START,
   GLTH_TIMESTAMP_MODE_EPOCH_TIME,
} GLTHTimeStampModeType;

typedef enum
{
   GLTH_DAYS = 0,
   GLTH_HOURS,
   GLTH_MINS,
   GLTH_SECS,
   GLTH_MS,
   GLTH_US,
   GLTH_NS
} GLTH_DATETIME_TYPE;

/******************************************************************************/
/*                           D A T A  M O D E L S                             */
/******************************************************************************/
class GLTimespecNs
{
   public:
      GLTimespecNs(uint64_t ns);
      GLTimespecNs(timespec time);
      ~GLTimespecNs();
      uint64_t ConvertTimespecToTimespecNs(timespec tm);
      const uint64_t Nanoseconds();

   private:
      uint64_t m_Nanoseconds;
};

}

/******************************************************************************/
/*                         D E C L A R A T I O N S                            */
/******************************************************************************/
namespace MDN
{

class GLTimeHelper
{
   public:
      GLTimeHelper(GLTHTimeStampModeType mode);
      ~GLTimeHelper();

      std::string ConvertNsIntoTimeStampUs(uint64_t time);
      uint64_t GetTimeInNs();

      // P U B LI C  C L A S S  C O N S T A N T S
      static const uint64_t m_theNanosecondsPerSecond;
      static const uint64_t m_theNanosecondsPerMs;

   private:
      std::string IntToFixedWidthString(int32_t value, GLTH_DATETIME_TYPE type);
      std::string GetCurrentDateTimeString();

       //Linux format
      void GetLinuxSystemTime(timespec* tm);
      GLTimespecNs GetLinuxSystemTimeNs();
      uint64_t GetLinuxTimeElapsedNs();
      uint64_t GetLinuxTimeElapsedSinceEpochNs();
      uint64_t GetLinuxTimeElapsedSinceSystemStartNs();
      void SetLinuxMeasurementStartTime();
      std::string GetTimeStampInUs();

      //std::Chrono format
      hrc::time_point GetSystemTime();


      hrc::time_point m_SystemStartTime;
      hrc::time_point m_MeasurementStartTime;
      GLTimespecNs m_LinuxSystemStartTimeNs;
      GLTimespecNs m_LinuxMeasurementStartTimeNs;
      GLTHTimeStampModeType m_TimeStampMode;

      // C L A S S  C O N S T A N T S
      static const uint32_t m_theMaximumFieldTypeWidth;

};

}

/******************************************************************************/

#endif /* time_helper_h */