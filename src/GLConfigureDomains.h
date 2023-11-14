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
   @date Feb 1, 2022
   @version 1.0
   @brief FILE NOTES:
   This file defines the class which contains the primary
   resources used by the application.
*/
/*****************************************************************************/
#ifndef gl_configure_h
#define gl_configure_h

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/
#include <string>
#include <map>
#include <vector>

#include "GLTypedefs.h"
#include "GLConfigureDomains.h"

/******************************************************************************/
/*                         D E C L A R A T I O N S                            */
/******************************************************************************/
namespace MDN
{

// Expected Domains
typedef enum
{
   GLCF_FIRST_DOMAIN_ID = 0,

   GLCF_GLOBAL_DOMAIN_ID = GLCF_FIRST_DOMAIN_ID,
   GLCF_IO_NETWORK_DOMAIN_ID,
   GLCF_PROTOCOL_DOMAIN_ID,

   GLCF_NUMBER_OF_DOMAINS,
   GLCF_MAX_NUMBER_OF_DOMAINS = 15,
   GLCF_ALL_DOMAINS_ID, // Should be equal to 16

} GLCFDomainIds;

static const std::map<GLCFDomainIds, std::string> m_theDomainNameMap
{
   {GLCF_GLOBAL_DOMAIN_ID, "GL"},
   {GLCF_IO_NETWORK_DOMAIN_ID, "IO"},
   {GLCF_PROTOCOL_DOMAIN_ID, "PR"},

   {GLCF_ALL_DOMAINS_ID, "ALL DOMAINS"},
};

typedef uint8_t GLCFDomainIdType;
typedef std::vector<GLCFDomainIds> GLDomainIdsType;

static const std::string GLCFUnknownDomainName = "UNKNOWN DOMAIN";

#ifdef nodef
typedef uint8_t GLModuleIdType;
typedef uint16_t GLModuleAddrType;
typedef uint16_t GLSystemModuleStateType;
typedef std::map<GLSystemModuleStateType, std::string> GLSystemModuleStateMapType;
#endif
}
/******************************************************************************/

#endif /* gl_configure_h */
