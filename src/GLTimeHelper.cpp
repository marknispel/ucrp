/******************************************************************************/
/*  UDP Command Response Protocol Server                                      */
/*  Copyright © 2023 Mark Nispel                                              */
/*  All rights reserved                                                       */
/*  Unauthorized use, distribution or duplication is                          */
/*  strictly prohibited without written authorization.                        */
/******************************************************************************/
/**
   @file GLTimeHelper.cpp
   @author Mark Nispel
   @date Oct 31, 2023
   @version 1.0
   @brief FILE NOTES:
   This file contains the implementation for the time helper object which manager
   time references for the application.
*/
/******************************************************************************/
/*       I N C L U D E S                                                      */
/******************************************************************************/
#include "stdio.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <time.h>
#include <string>

#include "GLTimeHelper.h"

using namespace MDN;

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

/******************************************************************************/
/*                            C O N S T A N T S                               */
/******************************************************************************/
const uint32_t GLTimeHelper::m_theMaximumFieldTypeWidth = 10;
const uint64_t GLTimeHelper::m_theNanosecondsPerSecond = 1000000000;
const uint64_t GLTimeHelper::m_theNanosecondsPerMs =
      GLTimeHelper::m_theNanosecondsPerSecond / 1000;

/******************************************************************************/
/*                           D A T A  M O D E L S                             */
/******************************************************************************/
GLTimespecNs::GLTimespecNs(uint64_t timeInNs)
   :
   m_Nanoseconds(timeInNs)
{
}

/******************************************************************************/
GLTimespecNs::GLTimespecNs(timespec tm)
   :
   m_Nanoseconds(ConvertTimespecToTimespecNs(tm))
{
}

/******************************************************************************/
GLTimespecNs::~GLTimespecNs()
{
}

/******************************************************************************/
uint64_t GLTimespecNs::ConvertTimespecToTimespecNs(timespec tm)
{
   return
         static_cast<uint64_t>(tm.tv_sec * GLTimeHelper::m_theNanosecondsPerSecond)
         + static_cast<uint64_t>(tm.tv_nsec);
}

/******************************************************************************/
const uint64_t GLTimespecNs::Nanoseconds()
{
   return m_Nanoseconds;
}

/******************************************************************************/
/*                      I M P L E M E N T A T I O N                           */
/******************************************************************************/
GLTimeHelper::GLTimeHelper(GLTHTimeStampModeType mode)
   :
   m_SystemStartTime(GetSystemTime()),
   m_MeasurementStartTime(m_SystemStartTime),
   m_LinuxSystemStartTimeNs(GetLinuxSystemTimeNs()),
   m_LinuxMeasurementStartTimeNs(m_LinuxSystemStartTimeNs),
   m_TimeStampMode(mode)
{
}

/******************************************************************************/
GLTimeHelper::~GLTimeHelper()
{
}

/******************************************************************************/
/*                  O T H E R  F U N C T I O N S                              */
/******************************************************************************/
uint64_t GLTimeHelper::GetTimeInNs()
{
   uint64_t timeNs = 0;

   switch (m_TimeStampMode)
   {
      case GLTH_TIMESTAMP_MODE_EPOCH_TIME:
         timeNs = GetLinuxTimeElapsedSinceEpochNs();
         break;

      case GLTH_TIMESTAMP_MODE_FROM_SYSTEM_START:
      default:
         timeNs = GetLinuxTimeElapsedSinceSystemStartNs();
         break;
   }

   return timeNs;
}

/******************************************************************************/
std::string GLTimeHelper::GetTimeStampInUs()
{
   return ConvertNsIntoTimeStampUs(GetTimeInNs());
}

/******************************************************************************/
std::string GLTimeHelper::ConvertNsIntoTimeStampUs(uint64_t time)
{
   const uint64_t NANOSECONDS_IN_A_DAY = 86400000000000;
   const uint64_t NANOSECONDS_IN_AN_HOUR = 3600000000000;
   const uint64_t NANOSECONDS_IN_A_MIN = 60000000000;
   const uint64_t NANOSECONDS_IN_A_SEC = 1000000000;
   const uint64_t NANOSECONDS_IN_A_MS = 1000000;
   const uint64_t NANOSECONDS_IN_A_US = 1000;

   int32_t Days = time / NANOSECONDS_IN_A_DAY;
   time = time - Days * NANOSECONDS_IN_A_DAY;

   int32_t Hours = time / NANOSECONDS_IN_AN_HOUR;
   time = time - Hours * NANOSECONDS_IN_AN_HOUR;

   int32_t Mins = time / NANOSECONDS_IN_A_MIN;
   time = time - Mins * NANOSECONDS_IN_A_MIN;

   int32_t Secs = time / NANOSECONDS_IN_A_SEC;
   time = time - Secs * NANOSECONDS_IN_A_SEC;

   int32_t Msecs = time / NANOSECONDS_IN_A_MS;
   time = time - Msecs * NANOSECONDS_IN_A_MS;

   int32_t Usecs = time / NANOSECONDS_IN_A_US;
   time = time - Usecs * NANOSECONDS_IN_A_US;

   //int64_t Nsecs = time;

   // 27 char + "/0" timestamp
   std::string timeString =
         IntToFixedWidthString(Days, GLTH_DAYS) + ":" +
         IntToFixedWidthString(Hours, GLTH_HOURS) + ":" +
         IntToFixedWidthString(Mins, GLTH_MINS) + ":" +
         IntToFixedWidthString(Secs, GLTH_SECS) + "::" +
         IntToFixedWidthString(Msecs, GLTH_MS) + "::" +
         IntToFixedWidthString(Usecs, GLTH_US) + " us";

   return timeString;
}

/******************************************************************************/
std::string GLTimeHelper::IntToFixedWidthString(int32_t value, GLTH_DATETIME_TYPE type)
{
   char buffer[GLTimeHelper::m_theMaximumFieldTypeWidth + 1];
   int32_t fixedwidth;
   const int32_t maxchars = sizeof(buffer);

   switch (type)
   {
      case GLTH_DAYS:
         fixedwidth = 5;
         break;
      case GLTH_HOURS:
         fixedwidth = 2;
         break;
      case GLTH_MINS:
         fixedwidth = 2;
         break;
      case GLTH_SECS:
         fixedwidth = 2;
         break;
      case GLTH_MS:
         fixedwidth = 3;
         break;
      case GLTH_US:
         fixedwidth = 3;
         break;
      case GLTH_NS:
      default:
         fixedwidth = 3;
         break;
   }

   snprintf(buffer, maxchars, "%.*d", fixedwidth, value);
   std::string s = std::string(buffer);

   return s;
}

/******************************************************************************/
/*             S T D : : C H R O N O  F U N C T I O N S                       */
/******************************************************************************/
hrc::time_point GLTimeHelper::GetSystemTime()
{
   hrc::time_point tp = std::chrono::high_resolution_clock::now();

   return tp;
}

/******************************************************************************/
std::string GLTimeHelper::GetCurrentDateTimeString()
{
   std::stringstream timeString;
/*
   TO BE IMPLEMENTED - Problem with cross-platform
   std::string format = "UTC: %Y-%m-%d %H:%M:%S";

    std::time_t tt = std::chrono::system_clock::to_time_t(GetSystemTime());
    std::tm tm = *std::gmtime(&tt); //GMT (UTC)

    timeString << std::put_time( &tm, format.c_str() );
 */
    return timeString.str();
}

/******************************************************************************/
/*                  L I N U X  F U N C T I O N S                              */
/******************************************************************************/
void GLTimeHelper::GetLinuxSystemTime(struct timespec* tm)
{
   // Note: LINUX / UNIX function
   clock_gettime(CLOCK_REALTIME, tm);
}

/******************************************************************************/
GLTimespecNs GLTimeHelper::GetLinuxSystemTimeNs()
{
   struct timespec tm;

   GetLinuxSystemTime(&tm);
   GLTimespecNs timespecNs(tm);

   return timespecNs;
}

/******************************************************************************/
uint64_t GLTimeHelper::GetLinuxTimeElapsedNs()
{
   uint64_t elapsedTime =
      GetLinuxSystemTimeNs().Nanoseconds() -
      m_LinuxMeasurementStartTimeNs.Nanoseconds();

   return elapsedTime;
}

/******************************************************************************/
uint64_t GLTimeHelper::GetLinuxTimeElapsedSinceEpochNs()
{
   // Assume system time is already set to epoch time
   return GetLinuxSystemTimeNs().Nanoseconds();
}

/******************************************************************************/
uint64_t GLTimeHelper::GetLinuxTimeElapsedSinceSystemStartNs()
{
   uint64_t elapsedTime =
         GetLinuxSystemTimeNs().Nanoseconds() -
         m_LinuxSystemStartTimeNs.Nanoseconds();

   return elapsedTime;
}

/******************************************************************************/
void GLTimeHelper::SetLinuxMeasurementStartTime()
{
   m_LinuxMeasurementStartTimeNs = GetLinuxSystemTimeNs();
}

#ifdef nodef
/******************************************************************************/
std::string GLTimeHelper::GetTimeMsSinceEpochString()
{
   std::stringstream timeString;
   std::string format = "UTC: %Y-%m-%d %H:%M:%S";

   uint64_t millisec_since_epoch =
         duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

   std::tm tm = *std::gmtime(&tt); //GMT (UTC)

   timeString << std::put_time( &tm, format.c_str() );

   return timeString.str();
}

/******************************************************************************/
std::string GLTimeHelper::GetTimePointDateString()
{
   std::stringstream timeString;
   std::string format = "UTC: %Y-%m-%d %H:%M:%S";

    std::time_t tt = std::chrono::system_clock::to_time_t(GetHiResSystemTime());
    std::tm tm = *std::gmtime(&tt); //GMT (UTC)

    timeString << std::put_time( &tm, format.c_str() );

    return timeString.str();
}

/******************************************************************************/
void GLTimeHelper::GetDateTimeString(std::string& dateTimeString)
{
   std::time_t time = GetLoResSystemTime();
   dateTimeString = std::string(std::ctime(&time));
}

/******************************************************************************/
void GLTimeHelper::SetRealTimeReferenceHiResTime()
{
   m_TimeStartHiRes = GetHiResSystemTime();
}

/******************************************************************************/

#endif