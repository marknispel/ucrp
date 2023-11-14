/******************************************************************************/
/*  UDP Command Response Protocol Server                                      */
/*  Copyright © 2023 Mark Nispel                                              */
/*  All rights reserved                                                       */
/*  Unauthorized use, distribution or duplication is                          */
/*  strictly prohibited without written authorization.                        */
/******************************************************************************/
/**
   @file GLEventLog.h
   @author Mark Nispel
   @date Oct 31, 2023
   @version 1.0
   @brief FILE NOTES:
   This file contains the definitions for the global event log class.
*/
/******************************************************************************/
#ifndef gl_event_log_h
#define gl_event_log_h

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/
#include <string>
#include <memory>
#include <vector>
#include <mutex>

#include "GLConfigureDomains.h"
#include "GLConfigureSystemModules.h"
#include "GLTypedefs.h"

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/
namespace MDN
{

typedef enum
{
   GLEV_TIMESTAMP = 0,
   GLEV_MODULE_NAME,
   GLEV_EVENT_STR,
   GLEV_EVENT_LEVEL_STR
} GLEVEventFieldTypes;

typedef enum
{
      GLEV_EVENT_LEVEL_1 = 1,
      GLEV_EVENT_LEVEL_2,

      GL_EVENT_LEVEL_DEFAULT = GLEV_EVENT_LEVEL_1,
      GLER_EVENT_LEVEL_MAXIMUM = GLEV_EVENT_LEVEL_2,
} GLEVEventLevels;

}

/******************************************************************************/
/*                            C O N S T A N T S                               */
/******************************************************************************/
namespace MDN
{

const bool GLEVRemoteLoggingEnabled = false;

const uint32_t GLEVMaximumFieldStringWidth = 100;
const uint32_t GLEVTimestampMaximumLengthChars = 30;
const uint32_t GLEVModuleNameMaximumLengthChars = 60;
const uint32_t GLEVEEventMaximumLengthChars = 70;
const uint32_t GLEVEventLevelMaximumLengthChars = 4;
const uint32_t GLEVEventLogGetEntryStringMaxSize = 255;
const uint32_t GLEVLogSize = 200;

const uint16_t GLEVEventLogSize = 200;
}

/******************************************************************************/
/*                         D E C L A R A T I O N S                            */
/******************************************************************************/
namespace MDN
{

class GLResourceMain;
class GLTimeHelper;

/******************************************************************************/
class GLEventLogEntry
{
   public:
      GLEventLogEntry();
      GLEventLogEntry(
            uint64_t ts,
            std::string mn,
            std::string err,
            std::string lvl);
      ~GLEventLogEntry();

      static std::string GetFixedWidthString(
            std::string inString,
            GLEVEventFieldTypes field);

      uint64_t m_TimeStampNs;
      std::string m_ModuleName;
      std::string m_Event;
      std::string m_Level;
};

using GLEVLogType = std::array<GLEventLogEntry, GLEVLogSize>;
using GLEVLogPtrType = std::unique_ptr<GLEVLogType>;

/******************************************************************************/
class GLEventLog
{
   public:
      GLEventLog(GLResourceMain& resource);
      ~GLEventLog();

      bool LogEvent(
            GLCFModuleIds moduleName,
            const char* eventStr,
            GLEVEventLevels level);
      const std::string GetEntryString(GLEventLogEntry& entry);
      void PrintEventLogEntries();
      std::shared_ptr<std::vector<GLEventLogEntry*>> GetTempEventEntries();


   private:
      bool SendEventLogEntryToRemoteLogger(std::string& entryString);
      void IncrementBufferPointer(
            uint32_t& bufferPointer,
            const uint32_t bufferLen);

      GLResourceMain& Resource();
      GLTimeHelper& TimeHelper();
      GLEVLogType& EventLog();

      GLResourceMain& m_ResourceMain;
      GLEVLogPtrType m_EventLog;
      uint32_t m_HeadIndex;
      uint32_t m_TailIndex;
      std::mutex m_Mutex;
};

}

/******************************************************************************/

#endif /* gl_event_log_h */