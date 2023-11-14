/******************************************************************************/
/*  UDP Command Response Protocol Server                                      */
/*  Copyright © 2023 Mark Nispel                                              */
/*  All rights reserved                                                       */
/*  Unauthorized use, distribution or duplication is                          */
/*  strictly prohibited without written authorization.                        */
/******************************************************************************/
/**
   @file GLConfigureDomains.h
   @author Mark Nispel
   @date Oct 31, 2023
   @version 1.0
   @brief FILE NOTES:
   This file defines the class which contains the primary
   resources used by the application.
*/
/******************************************************************************/
#ifndef gl_configure_system_modules_h
#define gl_configure_system_modules_h

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/
#include <map>
#include <vector>
#include <string>

/******************************************************************************/
/*                         D E C L A R A T I O N S                            */
/******************************************************************************/
namespace MDN
{

// E X P E C T E D  S Y S T E M  M O D U L E S
typedef enum
{
   GLCF_FIRST_MODULE_ID = 0,

   GLCF_GL_RESOURCE_MAIN_ID = GLCF_FIRST_MODULE_ID,
   GLCF_GL_IO_NETWORK_CONTROL_INTERFACE_MANAGER_ID,
   GLCF_PR_PROTOCOL_DOMAIN_MANAGER_ID,

   GLCF_NUMBER_OF_SYSTEM_MODULES,
   GLCF_NON_SYSTEM_MODULE_ID,
   GLCF_ALL_SYSTEM_MODULES_ID = 0xFFFF

} GLCFModuleIds;

static const std::map<GLCFModuleIds, std::string> m_theModuleNameMap
{
   {GLCF_GL_RESOURCE_MAIN_ID, "RESOURCE MAIN"},
   {GLCF_GL_IO_NETWORK_CONTROL_INTERFACE_MANAGER_ID,
      "GLCF_GL_IO_NETWORK_CONTROL_INTERFACE_MANAGER"},
   {GLCF_PR_PROTOCOL_DOMAIN_MANAGER_ID, "PROTOCOL DOMAIN MANAGER"},

   {GLCF_ALL_SYSTEM_MODULES_ID, "ALL SYSTEM MODULES"},
   {GLCF_NON_SYSTEM_MODULE_ID, "NON SYSTEM MODULE"},
};
static const std::string GLCFUnknownModuleName = "UNKNOWN MODULE";

}

#endif /* gl_configure_system_modules_h */