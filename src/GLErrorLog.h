/******************************************************************************/
/*  UDP Command Response Protocol Server                                      */
/*  Copyright © 2023 Mark Nispel                                              */
/*  All rights reserved                                                       */
/*  Unauthorized use, distribution or duplication is                          */
/*  strictly prohibited without written authorization.                        */
/******************************************************************************/
/**
   @file GLErrorLog.h
   @author Mark Nispel
   @date Oct 31, 2023
   @version 1.0
   @brief FILE NOTES:
   This file contains the definitions for the global router class
   for the application.
*/
/******************************************************************************/
#ifndef gl_error_log_h
#define gl_error_log_h

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

namespace MDN
{
/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/
typedef enum
{
   GLEL_TIMESTAMP = 0,
   GLEL_MODULE_NAME,
   GLEL_ERROR_STR,
   GLEL_ERROR_LEVEL_STR
} GLEL_FIELD_TYPE;

typedef enum
{
      GLEL_ERROR_LEVEL_1 = 1,
      GLEL_ERROR_LEVEL_2,

      GL_ERROR_LEVEL_DEFAULT = GLEL_ERROR_LEVEL_1,
      GLEL_ERROR_LEVEL_MAXIMUM = GLEL_ERROR_LEVEL_2,
} GLELErrorLevels;

/******************************************************************************/
/*                            C O N S T A N T S                               */
/******************************************************************************/

const bool GLELRemoteLoggingEnabled = false;

const uint32_t GLELMaximumFieldStringWidth = 50;
const uint32_t GLELTimestampMaximumLengthChars = 30;
const uint32_t GLELModuleNameMaximumLengthChars = 60;
const uint32_t GLELErrorMaximumLengthChars = 40;
const uint32_t GLELErrorLevelMaximumLengthChars = 4;
const uint32_t GLErrorLogGetEntryStringMaxSize = 255;
const uint32_t GLELLogSize = 100;

const uint16_t GLELErrorLogSize = 100;

/******************************************************************************/
/*                         D E C L A R A T I O N S                            */
/******************************************************************************/

class GLResourceMain;
class GLTimeHelper;

class GLErrorLogEntry
{
   public:
      GLErrorLogEntry();
      GLErrorLogEntry(
            uint64_t ts,
            std::string mn,
            std::string err,
            std::string lvl);
      ~GLErrorLogEntry();

      static std::string GetFixedWidthString(
            std::string inString,
            GLEL_FIELD_TYPE field);

      uint64_t m_TimeStampNs;
      std::string m_ModuleName;
      std::string m_Error;
      std::string m_Level;
};

using GLELLogType = std::array<GLErrorLogEntry, GLELLogSize>;
using GLELLogPtrType = std::unique_ptr<GLELLogType>;

/******************************************************************************/
class GLErrorLog
{
   public:
      GLErrorLog(GLResourceMain& resource);
      ~GLErrorLog();

      bool LogError(
            GLCFModuleIds moduleName,
            const char* error,
            GLELErrorLevels level);
      const std::string GetEntryString(GLErrorLogEntry& entry);
      void PrintErrorLogEntries();
      std::shared_ptr<std::vector<GLErrorLogEntry*>> GetTempErrorEntries();

   private:
      void IncrementBufferPointer(
            uint32_t& bufferPointer,
            const uint32_t bufferLen);
      bool SendLogErrorEntryToRemoteLogger(std::string& entryString);

      GLResourceMain& Resource();
      GLTimeHelper& TimeHelper();
      GLELLogType& ErrorLog();

      GLResourceMain& m_ResourceMain;
      GLELLogPtrType m_ErrorLog;
      uint32_t m_HeadIndex;
      uint32_t m_TailIndex;
      std::mutex m_ErrorMutex;
};

}
/******************************************************************************/

#endif /* gl_router_h */
