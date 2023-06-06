/*==========================================================================*
 *    Copyright(c) 2021, Vertiv Tech Co., Ltd.
 *                     ALL RIGHTS RESERVED
 *
 *  PRODUCT  : ACU(Advanced Controller Unit)
 *
 *  FILENAME : main.c
 *  CREATOR  : Mao Fuhua(Frank)         DATE: 2004-11-17 14:55
 *  VERSION  : V1.00
 *  PURPOSE  : the main proc of ACU, init sys and init app services
 *
 *
 *  HISTORY  :
 *
 *==========================================================================*/

#include <signal.h>
#include <setjmp.h>
#include "stdsys.h"
#include "public.h"

#include "cfg_model.h"
#include "../config_mgmt/cfg_mgmt.h"	// for loading the site config info
#include "../equip_mon/equip_mon.h"
#include "../time_sync/time_sync.h"
#include "main.h"

#include "sig_handler.h"


// the global variable to store the solution information.
SITE_INFO	g_SiteInfo;  /* to load config info */
// current config base path
char		g_szACUConfigDir[MAX_FILE_PATH];
extern		DWORD g_dwSWVersion;	// defined in data_exachange.c


SERVICE_MANAGER		g_ServiceManager;



// s_nSystemRunningState
#define SYS_IS_INITIALIZING 0
#define SYS_IS_RUNNING		1
#define SYS_IS_EXITING		2

//s_nSystemExitingCode: Moved to err_code.h maofuhua,2005-02-25

static int s_nSystemRunningState = SYS_IS_INITIALIZING;
static int s_nSystemExitingCode  = 0;

static jmp_buf s_JumpBufFaultError;

//do NOT change the interval, or will cause low effective. 
#define MAIN_SLEEP_INTERVAL			3000	// 3 sec. 
#define THREAD_MANAGER_INIT_TICKS   ((60*1000)/MAIN_SLEEP_INTERVAL+1)
#define MAIN_PROCESS_INIT_TICKS		\
		((2*60*1000)/RUN_THREAD_MANAGER_HEARTBEAT_INTERVAL+1)	// 2min.
static int s_nMainProcessTicks   = MAIN_PROCESS_INIT_TICKS*2;
static int s_nThreadManagerTicks = THREAD_MANAGER_INIT_TICKS;

#ifdef _DEBUG_EQUIP_TASK // defined in Makefile.
BOOL	Site_CreateQueryEquipTask(IN SITE_INFO *pSite);
BOOL	Site_StopQueryEquipTask(IN SITE_INFO *pSite);
#endif	// ifdef _DEBUG_EQUIP_TASK

static void Main_ExitAbnormally(int nRunningState);

#define _ACU_SPLASH_AS_PROCESS	1 //splash run as an individual process. 

#ifndef _CODE_FOR_MINI
//Added for G3 project, by Lin.Tao.Thomas, 2013-4
#define _G3_SPLASH_IN_QT		1 //splash run in QT process
#endif

#ifdef _ACU_SPLASH_AS_PROCESS
static BOOL	Main_StopSplash(char *pszStopSplash, int nWaitSec)
{
	char	szCmdLine[MAX_FILE_PATH];

	snprintf(szCmdLine, sizeof(szCmdLine), "%s/bin/%s %d",
		getenv(ENV_ACU_ROOT_DIR), pszStopSplash, nWaitSec);
	//system(szCmdLine);
	_SYSTEM(szCmdLine);

	return TRUE;
}

#else
// acu starting splash define. maofuhua, 2005-3-6
static 	APP_SERVICE s_AcuSplash = DEF_APP_SERVICE("Splash",		"lib_acu_splash.so",		
		SERVICE_OF_MISC,	FALSE,  SERVICE_RUN_AS_TASK);
#endif

//
//NOTE: Any module that uses DXI must be initialized after EquipMonitoring_Init!
static SYS_INIT_EXIT_MODULE	s_SysInitExitModule[] =
{
	// splash.	
#ifndef _G3_SPLASH_IN_QT    //added for G3 project, by Lin.Tao.Thomas, 2013-4
#ifndef _ACU_SPLASH_AS_PROCESS
#ifdef _HAS_LCD_UI
	DEF_INIT_EXIT_MODULE("Splash",				FALSE,	ERR_OK,		
						Service_Start,				&s_AcuSplash,	NULL,
						NULL,						NULL,			NULL),
#endif
#endif
#endif

	DEF_INIT_EXIT_MODULE("Data Management Interface",FALSE,	ERR_INIT_DATA_MGMT,		
						DAT_IniMngHisData,			NULL,			NULL,
						DAT_ClearResource,			NULL,			NULL),
	DEF_INIT_EXIT_MODULE("Solution Configuration",	TRUE,	ERR_LOADING_CONFIG,
						EquipMonitoring_Init,		&g_SiteInfo,	NULL, 
						EquipMonitoring_Unload,		&g_SiteInfo,	NULL),
	DEF_INIT_EXIT_MODULE("User Manager",			FALSE,	ERR_INIT_USER_MGR,
						InitUserManage,				NULL,			NULL,
						DestroyUserManager,			NULL,			NULL),
	DEF_INIT_EXIT_MODULE("Data Exchange Interface",	TRUE,	ERR_INIT_DXI,	
						InitDataExchangeModule,		NULL,			NULL,
						DestroyDataExchangeModule,	NULL,			NULL),
	DEF_INIT_EXIT_MODULE("Equipment Manager",		TRUE,	ERR_CREATING_EQUIPMENT_MANAGER,
						EquipMonitoring_Start,		&g_SiteInfo,	NULL,
						EquipMonitoring_Stop,		&g_SiteInfo,	NULL),

	// splash must be stopped before starting services.
#ifndef _G3_SPLASH_IN_QT    //added for G3 project, by Lin.Tao.Thomas, 2013-4
#ifdef _ACU_SPLASH_AS_PROCESS
#ifndef _CODE_FOR_MINI
	DEF_INIT_EXIT_MODULE("Splash",				FALSE,	ERR_OK,		
						Main_StopSplash,			"stopsplash",	2/*sec*/,
						NULL,						NULL,			NULL),	
#endif
#else
#ifdef _HAS_LCD_UI
	DEF_INIT_EXIT_MODULE("Splash",				FALSE,	ERR_OK,		
						Service_Stop,				&s_AcuSplash,	500,
						NULL,						NULL,			NULL),	
#endif
#endif
#endif

	DEF_INIT_EXIT_MODULE("Service Manager",			TRUE,	ERR_CREATING_SERVICE,
						ServiceManager_Start,		&g_ServiceManager,	NULL,
						ServiceManager_Stop,		&g_ServiceManager,	10000/*10s*/),
	DEF_INIT_EXIT_MODULE("Time sync",				FALSE,	ERR_INIT_TIME_SYNC,	
						InitTimeSync,				NULL,			NULL,
						DestroyTimeSync,			NULL,			NULL),
#ifdef _DEBUG_EQUIP_TASK // defined in Makefile.
	DEF_INIT_EXIT_MODULE("Data Debugger",			FALSE,	0,
						Site_CreateQueryEquipTask,	&g_SiteInfo,	NULL,
						Site_StopQueryEquipTask,	&g_SiteInfo,	NULL),
#endif //_DEBUG_EQUIP_TASK

};


/*==========================================================================*
 * FUNCTION : Main_InitModules
 * PURPOSE  : init the modules by normal sequence.
 * CALLS    : 
 * CALLED BY: Main_Init
 * ARGUMENTS:   void : 
 * RETURN   : static int : if any mandatory module fails on init, 
 *                         return its errcode, or return OK.
 * COMMENTS : 
 * CREATOR  : Mao Fuhua(Frank)         DATE: 2004-12-03 11:13
 *==========================================================================*/
static int Main_InitModules(void)
{
	int						i;
	BOOL					bResult;
	SYS_INIT_EXIT_MODULE	*pModule;
	int						nResult = ERR_MAIN_OK;

	AppLogOut(SYS_MAIN, APP_LOG_UNUSED, "Initializing system modules...\n");

	for (i = 1; i <= ITEM_OF(s_SysInitExitModule); i++)
	{
		pModule = &s_SysInitExitModule[i-1];

		if (pModule->pfnInit != NULL)
		{
			AppLogOut(SYS_MAIN, APP_LOG_UNUSED, "%d: Initializing %s...\n",
				i, pModule->pszModuleName);

			bResult = pModule->pfnInit(
				pModule->pInitArgs[0],
				pModule->pInitArgs[1]);

			AppLogOut(SYS_MAIN, 
				bResult ? APP_LOG_UNUSED : 
					pModule->bMandatoryModule ? APP_LOG_ERROR : APP_LOG_WARNING, 
				"%d: %s is %s successfully initialized.\n",
				i, pModule->pszModuleName,
				bResult ? "now" : "NOT");

			if (!bResult && pModule->bMandatoryModule)
			{
				nResult = pModule->dwErrorCode;
				break;
			}
		}

		//delete for G3_OPT [loader], by Lin.Tao.Thomas, 2013-4
		/*if(i == 1)
		{
			Sleep(2000);
		}*/
	}

	AppLogOut(SYS_MAIN, (nResult == ERR_MAIN_OK) ? APP_LOG_UNUSED : APP_LOG_ERROR,
		"System modules are %s successully initialized.\n",
		(nResult == ERR_MAIN_OK) ? "now" : "NOT");

	return nResult;
}


/*==========================================================================*
 * FUNCTION : Main_DestroyModules
 * PURPOSE  : Destroy the module by reverse sequence.
 * CALLS    : 
 * CALLED BY: 
 * ARGUMENTS:   void : 
 * RETURN   : static int : always OK
 * COMMENTS : 
 * CREATOR  : Mao Fuhua(Frank)         DATE: 2004-12-03 11:15
 *==========================================================================*/
static int Main_DestroyModules(void)
{
	int						i;
	SYS_INIT_EXIT_MODULE	*pModule;

	AppLogOut(SYS_MAIN, APP_LOG_UNUSED, "Destroying system modules...\n");

	// call destroy function by reverse sequence
	for (i = ITEM_OF(s_SysInitExitModule); i > 0; i--)
	{
		pModule = &s_SysInitExitModule[i-1];

		if (pModule->pfnExit != NULL)
		{
			AppLogOut(SYS_MAIN, APP_LOG_UNUSED, "%d: Destroying %s...\n",
				i, pModule->pszModuleName);

			pModule->pfnExit(
				pModule->pExitArgs[0],
				pModule->pExitArgs[1]);

			AppLogOut(SYS_MAIN, APP_LOG_UNUSED, "%d: %s is successfully destroyed.\n",
				i, pModule->pszModuleName);
		}
	}

	AppLogOut(SYS_MAIN, APP_LOG_UNUSED, "System modules are destroyed.\n");

	return ERR_MAIN_OK;
}


/*==========================================================================*
 * FUNCTION : Main_RunThreadEventHandler
 * PURPOSE  : 
 * CALLS    : 
 * CALLED BY: 
 * ARGUMENTS: IN DWORD      dwThreadEvent  : //	the thread event. below.
 *            IN HANDLE     hThread        : //	The thread id
 *            IN const char *pszThreadName : 
 * RETURN   : static DWORD : 
 * COMMENTS : 
 * CREATOR  : Mao Fuhua(Frank)         DATE: 2004-11-17 20:18
 *==========================================================================*/
static DWORD Main_RunThreadEventHandler(
	IN DWORD	dwThreadEvent,
	IN HANDLE	hThread,
	IN const char *pszThreadName)
{
	UNUSED(pszThreadName);

	WatchDog_Feed();

	if (THREAD_EVENT_NO_RESPONSE == dwThreadEvent)
	{
		printf("[Main_RunThreadEventHandler] -- Thread %s(%d) is no response,write log\n",pszThreadName,hThread);

	
		AppLogOut( SYS_MAIN, APP_LOG_ERROR,
			"Thread %s(%p) is no response, it will be killed, "
			"and the system will be restarted!\n",
			pszThreadName, hThread);
			
		printf("[Main_RunThreadEventHandler] -- after sleep~~~~\n");

		s_nSystemRunningState = SYS_IS_EXITING;
		s_nSystemExitingCode  = SYS_EXIT_BY_THREAD;

		//continue this thread, but we need quit system
		//return THREAD_CONTINUE_THIS;
	}

	else if (THREAD_EVENT_CANNOT_BE_STOPPED == dwThreadEvent)
	{
		// this happends on exiting.
		AppLogOut( SYS_MAIN, APP_LOG_WARNING,
			"Thread %s(%p) can NOT be stopped while exiting, it will be canceled.\n",
			pszThreadName, hThread);
		//continue this thread, but we need quit system
		//return THREAD_CONTINUE_THIS;
	}

	// check the main process running status.  if the main process has
	// no response in 2 minutes, exit the system.
	s_nMainProcessTicks--;
	if (s_nMainProcessTicks < 0)
	{
		AppLogOut( SYS_MAIN, APP_LOG_ERROR,
			"PANIC: Main process is no response, the system will exit and "
			"restart!\n");

		Main_ExitAbnormally(SYS_EXIT_BY_THREAD);	// will not return.
	}

	s_nThreadManagerTicks = THREAD_MANAGER_INIT_TICKS;

	return THREAD_CONTINUE_RUN;
}


static void Main_HandleFaultSignals(IN int nSig, IN siginfo_t *pSigInfo, 
							   IN void *pSigVal);

/*==========================================================================*
 * FUNCTION : Main_HandleFaultSignals
 * PURPOSE  : 
 * CALLS    : 
 * CALLED BY: 
 * ARGUMENTS: IN int        nSig      : 
 *            IN siginfo_t  *pSigInfo : 
 *            IN void       *pSigVal  : 
 * RETURN   : static void : 
 * COMMENTS : 
 * CREATOR  : Mao Fuhua(Frank)         DATE: 2004-11-20 19:50
 *==========================================================================*/
static void Main_HandleFaultSignals(IN int nSig, IN siginfo_t *pSigInfo, 
							   IN void *pSigVal)
{
	UNUSED(pSigVal);

	AppLogOut(SYS_MAIN, APP_LOG_ERROR, 
		"PANIC: System recv fault sig(signo:%d,code=%d) at addr:%p. exit.\n",
		nSig, pSigInfo->si_code, pSigInfo->si_addr);

	//G3_OPT [backtrace], David contribute to this part. 2013-06
#ifdef _DEBUG_BACKTRACE
#define number_of_entries 30
	void *array[number_of_entries];	// array for the stack entries
	size_t array_size;		// size of the array for stack entries
	int j;
	char **strings;			// string pointers to hold the stack entry output

	// get void*'s for all entries on the stack, convert them to symbols, and output
	array_size = backtrace(array, number_of_entries);
	strings = backtrace_symbols(array, array_size);
	AppLogOut(SYS_MAIN, APP_LOG_ERROR, "======= Backtrace: =========\n");
	if (strings != NULL) 
	{
		for (j = 0; j < array_size; j++)
		{
			AppLogOut(SYS_MAIN, APP_LOG_ERROR, "%s.", strings[j]);
		}

		free(strings);				// allocated by backtrace_symbols
	} 

	// print out memory map by copying /proc/self/maps to the output log
	AppLogOut(SYS_MAIN, APP_LOG_ERROR, "======= Memory map: ========\n");
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen("/proc/self/maps", "r");	// open the kernel file containing the map
	if (fp != NULL) {
		while ((read = getline(&line, &len, fp)) != -1) 
		{
			AppLogOut(SYS_MAIN, APP_LOG_ERROR, "%s\n", line);
		}

		free(line);		// line is allocated in getline and needs to be freed
		fclose( fp ); 
	}
#endif

	Main_ExitAbnormally(SYS_EXIT_BY_FAULT);	// no return.
}


/*==========================================================================*
 * FUNCTION : Main_HandleStopSignals
 * PURPOSE  : 
 * CALLS    : 
 * CALLED BY: 
 * ARGUMENTS:   void : 
 * RETURN   : static void : 
 * COMMENTS : 
 * CREATOR  : Mao Fuhua(Frank)         DATE: 2004-11-18 13:32
 *==========================================================================*/
void Main_HandleStopSignals(IN int nSig)
{
	static int		nKillCount = 0;
	static time_t	tmLastSignaled = 0;
	time_t			tmNow = time(NULL);

	if (tmNow - tmLastSignaled > 10)	// 10 seconds
	{
		nKillCount = 0;
	}

	tmLastSignaled = tmNow;

	nKillCount++;

	switch (nKillCount)
	{
	case 1:
#define SYS_PREQUIT_MSG	"Received a quit command, please send "\
			"command again to stop system.\n"
		AppLogOut(SYS_MAIN, APP_LOG_UNUSED, "%s", SYS_PREQUIT_MSG);
		printf("%s", SYS_PREQUIT_MSG);

		break;

	case 2:
#define SYS_INTR_MSG	"Received two quit commands, system is now quiting...\n"
		AppLogOut(SYS_MAIN, APP_LOG_WARNING, "%s", SYS_INTR_MSG);

		printf("%s", SYS_INTR_MSG);
		s_nSystemRunningState = SYS_IS_EXITING;
		s_nSystemExitingCode  = SYS_EXIT_BY_USER;

		if (nSig == SIGUSR2)
		{
			s_nSystemExitingCode  = SYS_EXIT_BY_REBOOT_REQ;
			AppLogOut(SYS_MAIN, APP_LOG_WARNING, 
				"And the System will reboot after quiting for received reboot cmd.\n");
		}
		else
		{
			Main_AckStopCmd();
		}

		break;

	default:		
		printf( "System is quiting, please wait... \n" );
	}
}


/*==========================================================================*
 * FUNCTION : Main_HandleSwapSignal
 * PURPOSE  : swap to PowerKit for SIGUSR1
 * CALLS    : 
 * CALLED BY: 
 * ARGUMENTS: IN int  nSig : 
 * RETURN   : static void : 
 * COMMENTS : 
 * CREATOR  : Mao Fuhua(Frank)         DATE: 2005-03-17 19:17
 *==========================================================================*/
static void Main_HandleSwapSignal(IN int nSig)
{
	UNUSED(nSig);

	AppLogOut(SYS_MAIN, APP_LOG_INFO, 
		"System received the signal of swapping to maintenance service. exiting.\n");

	s_nSystemRunningState = SYS_IS_EXITING;
	s_nSystemExitingCode  = SYS_EXIT_BY_SWAP;
}



static void Main_HandleChildSignal(IN int nSig)
{
	UNUSED(nSig);
//	TRACE("Handle Child Signal.\n");
}



/*==========================================================================*
 * FUNCTION : Main_InstallSigHandler
 * PURPOSE  : 
 * CALLS    : 
 * CALLED BY: 
 * ARGUMENTS: BOOL  bInstall : 
 * RETURN   : static BOOL : 
 * COMMENTS : 
 * CREATOR  : Mao Fuhua(Frank)         DATE: 2004-12-06 10:37
 *==========================================================================*/
static BOOL Main_InstallSigHandler(BOOL bInstall)
{
	UNUSED(bInstall);

	Sig_InstallHandlers(TRUE, // ignore all signal at first.
		Main_HandleStopSignals,  SIGTERM, SIGQUIT, SIGUSR2, 0,			// quit
#ifndef _DEBUG
		Main_HandleFaultSignals, SIGILL,  SIGFPE,  SIGSEGV, SIGBUS, 
								 SIGABRT, 0,	// error
#endif //_DEBUG
		Main_HandleChildSignal,	 SIGCHLD, 0,	//Added by Fan Xiansheng, 2008-12-09
		Main_HandleSwapSignal,	 SIGUSR1, 0,							// PowerKit
		NULL);

	return TRUE;
}


/*==========================================================================*
 * FUNCTION : Main_CheckRootDir
 * PURPOSE  : 
 * CALLS    : 
 * CALLED BY: 
 * ARGUMENTS: char  *pszExecName : 
 * RETURN   : static void : 
 * COMMENTS : 
 * CREATOR  : Mao Fuhua(Frank)         DATE: 2004-11-22 15:16
 *==========================================================================*/
static void Main_CheckRootDir(char *pszExecName)
{
    char *pszConfigEnv = getenv(ENV_ACU_ROOT_DIR);

	if (pszConfigEnv == NULL)
	{
		static char	szEnvRootDir[MAX_FILE_PATH];	// must be static.
		char	szRootDir[MAX_FILE_PATH], *pExecPath;

		pExecPath = strrchr(pszExecName, '/');
		if (pExecPath != NULL)
		{
			// add one extra room for '\0'
			size_t		len = (pExecPath-pszExecName)+1;

			// copy the dir in the exec file. 
			strncpyz(szRootDir, pszExecName, (int)MIN(len, sizeof(szRootDir)));
		}
		else
		{
			strcpy(szRootDir, ".");
		}

		printf("Please set the environment variable '%s' of ACU root dir.!\n"
			"The default dir \"%s\" will be used.\n",
			ENV_ACU_ROOT_DIR,
			szRootDir);

		snprintf(szEnvRootDir, sizeof(szEnvRootDir), "%s=%s",
			ENV_ACU_ROOT_DIR, szRootDir);
	
		putenv(szEnvRootDir);	// set back the env.

		TRACEX("ENV is: %s. got env is %s\n",
			szEnvRootDir, getenv(ENV_ACU_ROOT_DIR));

		ASSERT(strcmp(szRootDir, getenv(ENV_ACU_ROOT_DIR)) == 0);
	}
}


/*==========================================================================*
 * FUNCTION : Main_Init
 * PURPOSE  : init services and everythings
 * CALLS    : 
 * CALLED BY: 
 * ARGUMENTS: IN MAIN_ARGS  *THIS : 
 * RETURN   : static int : ERR_OK for OK
 * COMMENTS : 
 * CREATOR  : Mao Fuhua(Frank)         DATE: 2004-11-17 20:22
 *==========================================================================*/
static int Main_Init(IN MAIN_ARGS *THIS)
{
	int		nResult;

#ifdef _DEBUG
	int		nBlock, nId;
	size_t  ulBytes;

	//added for memory opt
	size_t  ulRew;
	size_t  ulDel;
#endif //_DEBUG

	UNUSED(THIS);

	//s_nSystemRunningState = SYS_IS_INITIALIZING;

	//added for memory opt
	if (!Pool_Manager_Initial())
	{
		AppLogOut(SYS_MAIN, APP_LOG_ERROR, "Pool init failed!\n");
	}

	// 1. starting msg.
	AppLogOut(SYS_MAIN, APP_LOG_MILESTONE, "+---------------------------------------+\n");
	AppLogOut(SYS_MAIN, APP_LOG_MILESTONE, "***The application now is starting......\n");
	AppLogOut(SYS_MAIN, APP_LOG_MILESTONE, "+---------------------------------------+\n");

	// 2. init thread mamager
	AppLogOut(SYS_MAIN, APP_LOG_UNUSED, "Initializing run thread manager...\n");
	RunThread_ManagerInit(Main_RunThreadEventHandler);	// must be at 1st


	// 3. init sys modules.
	nResult = Main_InitModules();


	//updated for memory opt
//#ifdef _DEBUG
//	nBlock = MEM_GET_INFO(&nId, &ulBytes);
//	AppLogOut("MAIN", APP_LOG_UNUSED, "Total allocated %d mem blocks, %u bytes, "
//		"%d time NEW/RENEW calls.\n",
//		nBlock, ulBytes, nId);
//#endif //_DEBUG

#ifdef _DEBUG
	nBlock = MEM_GET_INFO(&nId, &ulBytes, &ulRew, &ulDel);
	AppLogOut("MAIN", APP_LOG_UNUSED, "Total allocated %d mem blocks, %u bytes, "
		"%d time NEW/RENEW calls.\n",
		nBlock, ulBytes, nId);

	AppLogOut("MAIN", APP_LOG_UNUSED, "More info: Del in Renew: %d, Del in DELETE: %d",
		 ulRew, ulDel);
#endif //_DEBUG


	//added for memory opt
	Pool_Manager_Release();

	return nResult;
}


/*==========================================================================*
 * FUNCTION : Main_Run
 * PURPOSE  : 
 * CALLS    : 
 * CALLED BY: 
 * ARGUMENTS: OUT MAIN_ARGS  *THIS : 
 * RETURN   : static int : 
 * COMMENTS : 
 * CREATOR  : Mao Fuhua(Frank)         DATE: 2004-11-17 21:12
 *==========================================================================*/
static int Main_Run(IN MAIN_ARGS *THIS)
{
	UNUSED(THIS);

	Sleep(1000);	// sleep before log running msg

	// the program will receive a quit signal while system is starting.
	if (s_nSystemRunningState != SYS_IS_INITIALIZING)
	{
		return s_nSystemExitingCode;
	}

	s_nSystemRunningState = SYS_IS_RUNNING;

	AppLogOut(SYS_MAIN, APP_LOG_UNUSED, "+---------------------------------------+\n");	
	AppLogOut(SYS_MAIN, APP_LOG_MILESTONE, "***The application now is running......\n");
	AppLogOut(SYS_MAIN, APP_LOG_UNUSED, "+---------------------------------------+\n");

	// running
	while (s_nSystemRunningState == SYS_IS_RUNNING)
	{
		//WatchDog_Feed();	// do NOT feed dog in main process.

		Sleep(MAIN_SLEEP_INTERVAL);		// sleep 3 second.

		if (!THREAD_IS_RUNNING(g_ServiceManager.hServiceManager))
		{
			AppLogOut(SYS_MAIN, APP_LOG_ERROR,
				"PANIC! The service manager exited abnormally, "
				"the system will quit and restart now.\n");
			s_nSystemRunningState = SYS_IS_EXITING;
			s_nSystemExitingCode  = SYS_EXIT_BY_SERVICE;
		}

		// check the thread manager running status.  if it has
		// no response in 1 minutes, exit the system.
		s_nThreadManagerTicks--;
		if (s_nThreadManagerTicks < 0)
		{
			AppLogOut( SYS_MAIN, APP_LOG_ERROR,
				"PANIC: Thread manager is no response, the system will exit and "
				"restart!\n");

			// do NOT cleanup, exit now.
			Main_ExitAbnormally(SYS_EXIT_BY_THREAD);	// will not return;
		}

		// set the main process running flag.
		s_nMainProcessTicks   = MAIN_PROCESS_INIT_TICKS;

		// check the stop status
		if (Main_CheckStopCmd())	// quit now?
		{
#define SYS_USER_INTR_MSG	"Received user stop command, system is now quiting...\n"
			AppLogOut(SYS_MAIN, APP_LOG_WARNING, "%s", SYS_USER_INTR_MSG);
			printf("%s", SYS_USER_INTR_MSG);

			s_nSystemRunningState = SYS_IS_EXITING;
			s_nSystemExitingCode  = SYS_EXIT_BY_USER;
		}
	}


	return s_nSystemExitingCode;
}


/*==========================================================================*
 * FUNCTION : Main_ExitAbnormally
 * PURPOSE  : 
 * CALLS    : 
 * CALLED BY: 
 * ARGUMENTS: int nRunningState:
 * RETURN   : static void : 
 * COMMENTS : 
 * CREATOR  : Mao Fuhua(Frank)         DATE: 2004-11-20 19:50
 *==========================================================================*/
static void Main_ExitAbnormally(int nRunningState)
{
	WatchDog_Close(FALSE);	// do NOT disable dog, to cause system restart by dog

	longjmp(s_JumpBufFaultError, nRunningState);
}


/*==========================================================================*
 * FUNCTION : Main_Exit
 * PURPOSE  : exit system accoring to the current status.
 * CALLS    : 
 * CALLED BY: 
 * ARGUMENTS: IN OUT MAIN_ARGS  *THIS          : 
 *            IN int            nExitCode : 
 * RETURN   : static int : 
 * COMMENTS : 
 * CREATOR  : Mao Fuhua(Frank)         DATE: 2004-11-17 21:33
 *==========================================================================*/
static int Main_Exit(IN OUT MAIN_ARGS *THIS, IN int nExitCode)
{
	int		nResult;
	UNUSED(THIS);

	//s_nSystemRunningState = SYS_IS_EXITING;	// other value is already sett!
	AppLogOut(SYS_MAIN, APP_LOG_UNUSED, "+---------------------------------------+\n");
	AppLogOut(SYS_MAIN, APP_LOG_MILESTONE, "***The application now is quiting......\n");
	AppLogOut(SYS_MAIN, APP_LOG_UNUSED, "+---------------------------------------+\n");

	WatchDog_Close(TRUE);	// for  stop
	printf("Main_Exit()~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~--1\n");	

	// destroy sys modules
	Main_DestroyModules();
	printf("Main_Exit()~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~--2\n");	

	// stop the  thread manager.
	AppLogOut(SYS_MAIN, APP_LOG_UNUSED, "Destroying run thread manager...\n");
	nResult = RunThread_ManagerExit(10000, TRUE);	// must be at last
	AppLogOut(SYS_MAIN, APP_LOG_UNUSED, 
		"Run thread manager has exited with %d(%08X), %s.\n", 
		nResult, nResult, (nResult == ERR_THREAD_OK) ? "OK" : "FAILURE");
	printf("Main_Exit()~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~--3\n");	

	//Close the system handler mutex.Added by Marco Yang, 20131011
	_CloseSystemMutex();

	AppLogOut(SYS_MAIN, APP_LOG_UNUSED, "+---------------------------------------+\n");
	AppLogOut(SYS_MAIN, APP_LOG_MILESTONE, "|***The application exited %s ***|\n\n\n",
		(nExitCode < SYS_EXIT_BY_ERROR) ? "normally.  " : "abnormally." );
	AppLogOut(SYS_MAIN, APP_LOG_UNUSED, "+---------------------------------------+\n\n\n");
	printf("Main_Exit()~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~--4\n");	

	return nExitCode;
}


/*==========================================================================*
 * FUNCTION : main
 * PURPOSE  : 
 * CALLS    : 
 * CALLED BY: 
 * ARGUMENTS: int   argc    : 
 *            char  *argv[] : 
 * RETURN   : int : 
 * COMMENTS : 
 * CREATOR  : Mao Fuhua(Frank)         DATE: 2004-11-20 19:52
 *==========================================================================*/
int main(int argc, char *argv[])
{
	int			nExitStatus;
	MAIN_ARGS	args;

#ifdef _CODE_FOR_MINI
	printf("The code for mini NCU~~~~~~~~~~~~~~~~~~~~~~~~~\n");
#else
	printf("The code for ESNA NCU~~~~~~~~~~~~~~~~~~~~~~~~~\n");
#endif


	g_dwSWVersion = ACU_APP_VER;	// set new version set the first statement.

	nExitStatus   = (int)WatchDog_Open();

	// 0. init msg handler.
	Main_InstallSigHandler(TRUE);

	if (!nExitStatus)
	{
		AppLogOut(SYS_MAIN, APP_LOG_ERROR,
			"***** ACU starting. Fails on opening the watch-dog.*****\n");
			
		// continue to run.
	}

	// return 0 after init jmp buffer
	if((nExitStatus = setjmp(s_JumpBufFaultError)) != 0)
	{
		AppLogOut(SYS_MAIN, APP_LOG_ERROR,
			"***** Longjumped for fault error. System exiting.*****\n");
		_exit(nExitStatus);	// must call _exit. do NOT call exit.
		return nExitStatus;
	}

	//{{ parse the command line. maofuhua added. 2005-2-27
	if (Main_ProcessCmdLine(argc, argv) == -1)
	{
		// return -1 when show help only.
		WatchDog_Close(TRUE);
		return 0;
	}
	//}}
	
	Main_CheckRootDir(argv[0]);

#ifdef _DEBUG
	TRACEX("ENV is:%s\n", getenv(ENV_ACU_ROOT_DIR));
#endif

	args.pSite = &g_SiteInfo;

	// 1. init
	nExitStatus = Main_Init(&args);

	if (nExitStatus != ERR_MAIN_OK)
	{
		AppLogOut(SYS_MAIN, APP_LOG_ERROR,
			"Init system got error code %d(%x), system exit.\n",
			nExitStatus, nExitStatus);
		nExitStatus = SYS_EXIT_BY_INIT;
	}

	// 2. run
	else 
	{
		nExitStatus = Main_Run(&args);
		printf("Main_Run is return, s_nSystemRunningState = %d,s_nSystemExitingCode=%d\n",s_nSystemRunningState,s_nSystemExitingCode);	
	}

	// 3. cleanup
	Main_Exit(&args, nExitStatus);
	
	printf("Main_Exit is return, s_nSystemRunningState = %d,s_nSystemExitingCode=%d\n",s_nSystemRunningState,s_nSystemExitingCode);
// disable and close watch dog
	//WatchDog_Close((nExitStatus == SYS_EXIT_BY_USER) 
	//	? TRUE		// for user interrupt, stop and disable dog
	//	: FALSE);	// for abnormally stop, do NOT disabel dog to make dog reset system

	return nExitStatus;
}


