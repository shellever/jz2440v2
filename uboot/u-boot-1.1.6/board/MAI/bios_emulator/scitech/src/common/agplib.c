/****************************************************************************
*
*                   SciTech Nucleus Graphics Architecture
*
*               Copyright (C) 1991-1998 SciTech Software, Inc.
*                            All rights reserved.
*
*  ======================================================================
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  |                                                                    |
*  |This copyrighted computer code contains proprietary technology      |
*  |owned by SciTech Software, Inc., located at 505 Wall Street,        |
*  |Chico, CA 95928 USA (http://www.scitechsoft.com).                   |
*  |                                                                    |
*  |The contents of this file are subject to the SciTech Nucleus        |
*  |License; you may *not* use this file or related software except in  |
*  |compliance with the License. You may obtain a copy of the License   |
*  |at http://www.scitechsoft.com/nucleus-license.txt                   |
*  |                                                                    |
*  |Software distributed under the License is distributed on an         |
*  |"AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or      |
*  |implied. See the License for the specific language governing        |
*  |rights and limitations under the License.                           |
*  |                                                                    |
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  ======================================================================
*
* Language:     ANSI C
* Environment:  Any 32-bit protected mode environment
*
* Description:  C module for the Graphics Accelerator Driver API. Uses
*               the SciTech PM library for interfacing with DOS
*               extender specific functions.
*
****************************************************************************/

#include "nucleus/graphics.h"
#include "nucleus/agp.h"

/*---------------------------- Global Variables ---------------------------*/

#ifndef DEBUG_AGP_DRIVER
static AGP_exports  _AGP_exports;
static int          loaded = false;
static PE_MODULE    *hModBPD = NULL;

static N_imports _N_imports = {
    sizeof(N_imports),
    _OS_delay,
    };

static AGP_imports _AGP_imports = {
    sizeof(AGP_imports),
    };
#endif

#include "pmimp.h"

/*----------------------------- Implementation ----------------------------*/

#define DLL_NAME        "agp.bpd"

#ifndef DEBUG_AGP_DRIVER
/****************************************************************************
REMARKS:
Fatal error handler for non-exported GA_exports.
****************************************************************************/
static void _AGP_fatalErrorHandler(void)
{
    PM_fatalError("Unsupported AGP export function called! Please upgrade your copy of AGP!\n");
}

/****************************************************************************
PARAMETERS:
shared  - True to load the driver into shared memory.

REMARKS:
Loads the Nucleus binary portable DLL into memory and initilises it.
****************************************************************************/
static ibool LoadDriver(void)
{
    AGP_initLibrary_t   AGP_initLibrary;
    AGP_exports         *agpExp;
    char                filename[PM_MAX_PATH];
    char                bpdpath[PM_MAX_PATH];
    int                 i,max;
    ulong               *p;

    /* Check if we have already loaded the driver */
    if (loaded)
	return true;
    PM_init();

    /* Open the BPD file */
    if (!PM_findBPD(DLL_NAME,bpdpath))
	return false;
    strcpy(filename,bpdpath);
    strcat(filename,DLL_NAME);
    if ((hModBPD = PE_loadLibrary(filename,false)) == NULL)
	return false;
    if ((AGP_initLibrary = (AGP_initLibrary_t)PE_getProcAddress(hModBPD,"_AGP_initLibrary")) == NULL)
	return false;
    bpdpath[strlen(bpdpath)-1] = 0;
    if (strcmp(bpdpath,PM_getNucleusPath()) == 0)
	strcpy(bpdpath,PM_getNucleusConfigPath());
    else {
	PM_backslash(bpdpath);
	strcat(bpdpath,"config");
	}
    if ((agpExp = AGP_initLibrary(bpdpath,filename,GA_getSystemPMImports(),&_N_imports,&_AGP_imports)) == NULL)
	PM_fatalError("AGP_initLibrary failed!\n");
    _AGP_exports.dwSize = sizeof(_AGP_exports);
    max = sizeof(_AGP_exports)/sizeof(AGP_initLibrary_t);
    for (i = 0,p = (ulong*)&_AGP_exports; i < max; i++)
	*p++ = (ulong)_AGP_fatalErrorHandler;
    memcpy(&_AGP_exports,agpExp,MIN(sizeof(_AGP_exports),agpExp->dwSize));
    loaded = true;
    return true;
}

/* The following are stub entry points that the application calls to
 * initialise the Nucleus loader library, and we use this to load our
 * driver DLL from disk and initialise the library using it.
 */

/* {secret} */
int NAPI AGP_status(void)
{
    if (!loaded)
	return nDriverNotFound;
    return _AGP_exports.AGP_status();
}

/* {secret} */
const char * NAPI AGP_errorMsg(
    N_int32 status)
{
    if (!loaded)
	return "Unable to load Nucleus device driver!";
    return _AGP_exports.AGP_errorMsg(status);
}

/* {secret} */
AGP_devCtx * NAPI AGP_loadDriver(N_int32 deviceIndex)
{
    if (!LoadDriver())
	return NULL;
    return _AGP_exports.AGP_loadDriver(deviceIndex);
}

/* {secret} */
void NAPI AGP_unloadDriver(
    AGP_devCtx *dc)
{
    if (loaded)
	_AGP_exports.AGP_unloadDriver(dc);
}

/* {secret} */
void NAPI AGP_getGlobalOptions(
    AGP_globalOptions *options)
{
    if (LoadDriver())
	_AGP_exports.AGP_getGlobalOptions(options);
}

/* {secret} */
void NAPI AGP_setGlobalOptions(
    AGP_globalOptions *options)
{
    if (LoadDriver())
	_AGP_exports.AGP_setGlobalOptions(options);
}

/* {secret} */
void NAPI AGP_saveGlobalOptions(
    AGP_globalOptions *options)
{
    if (loaded)
	_AGP_exports.AGP_saveGlobalOptions(options);
}
#endif

/* {secret} */
void NAPI _OS_delay8253(N_uint32 microSeconds);

/****************************************************************************
REMARKS:
This function delays for the specified number of microseconds
****************************************************************************/
void NAPI _OS_delay(
    N_uint32 microSeconds)
{
    static ibool    inited = false;
    static ibool    haveRDTSC;
    LZTimerObject   tm;

    if (!inited) {
#ifndef __WIN32_VXD__
	/* This has been causing problems in VxD's for some reason, so for now */
	/* we avoid using it. */
	if (_GA_haveCPUID() && (_GA_getCPUIDFeatures() & CPU_HaveRDTSC) != 0) {
	    ZTimerInit();
	    haveRDTSC = true;
	    }
	else
#endif
	    haveRDTSC = false;
	inited = true;
	}
    if (haveRDTSC) {
	LZTimerOnExt(&tm);
	while (LZTimerLapExt(&tm) < microSeconds)
	    ;
	LZTimerOnExt(&tm);
	}
    else
	_OS_delay8253(microSeconds);
}
