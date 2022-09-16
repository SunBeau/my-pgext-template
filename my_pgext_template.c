/*----------------------------------------------------------------------------------------------------------------------
 *
 * my_pgext_template.c
 *    My PostgreSQL Extension Template
 *
 * Author:  SunBeau
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *    contrib/my_pgext_template/my_pgext_template.c
 *
 *----------------------------------------------------------------------------------------------------------------------
 */

#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "tcop/utility.h"

/* These are always necessary for a bgworker */
#include "miscadmin.h"
#include "postmaster/bgworker.h"
#include "postmaster/interrupt.h"
#include "storage/ipc.h"
#include "storage/latch.h"
#include "storage/lwlock.h"
#include "storage/proc.h"
#include "storage/shmem.h"

#include "utils/wait_event.h"

#include "my_pgext_template.h"

#include <stdlib.h>

// ---------------------------------------------------------------------------------------------------------------------
// guc

static char *my_pgext_template_teststr = NULL;

/**
 * @brief 初始化自定义配置参数
 */
void init_guc(void)
{
  DefineCustomStringVariable("my_pgext_template.teststr",
                  "My PostgreSQL Extension Template Test String.",
                  NULL,
                  &my_pgext_template_teststr,
                  "Asd_12345_@#$",
                  PGC_POSTMASTER,
                  0,
                  NULL, NULL, NULL);
}

// ---------------------------------------------------------------------------------------------------------------------
// Hook

static ProcessUtility_hook_type prev_ProcessUtility = NULL;

static void MyProcessUtility(PlannedStmt *pstmt,
                      const char *queryString,
                      bool readOnlyTree,
                      ProcessUtilityContext context,
                      ParamListInfo params,
                      QueryEnvironment *queryEnv,
                      DestReceiver *dest,
                      QueryCompletion *qc);

static void MyProcessUtility(PlannedStmt *pstmt,
                      const char *queryString,
                      bool readOnlyTree,
                      ProcessUtilityContext context,
                      ParamListInfo params,
                      QueryEnvironment *queryEnv,
                      DestReceiver *dest,
                      QueryCompletion *qc)
{
  elog(NOTICE, "my_pgext_template -- MyProcessUtility");

  if (prev_ProcessUtility)
    (*prev_ProcessUtility)(pstmt, queryString, readOnlyTree, context, params, queryEnv, dest, qc);
  else
    standard_ProcessUtility(pstmt, queryString, readOnlyTree, context, params, queryEnv, dest, qc);
}

/**
 * @brief 初始化钩子函数
 */
void init_hook(void)
{
  prev_ProcessUtility = ProcessUtility_hook;
  ProcessUtility_hook = MyProcessUtility;
}

// ---------------------------------------------------------------------------------------------------------------------
// bgworker

void simple_worker_main(Datum main_arg) pg_attribute_noreturn();
static void do_some_thing(int idx);

static void do_some_thing(int idx)
{
  elog(LOG, "----- simple worker [%d] do_some_thing -----", idx);
}

/**
 * @brief 简单后台进程入口
 *
 * @param main_arg
 */
void simple_worker_main(Datum main_arg)
{
  int arg = DatumGetInt32(main_arg);

  /* Establish signal handlers before unblocking signals. */
  pqsignal(SIGHUP, SignalHandlerForConfigReload);
  pqsignal(SIGTERM, die);

  /* We're now ready to receive signals */
  BackgroundWorkerUnblockSignals();

  /* Connect to our database */
  BackgroundWorkerInitializeConnection("postgres", NULL, 0);

  elog(LOG, "%s initialized arg[%d]", MyBgworkerEntry->bgw_name, arg);

  /* Main loop: do this until SIGTERM is received and processed by ProcessInterrupts. */
  for (;;)
  {
    long timeout = 5000L; /* 超时(毫秒) */

    /*
      * Background workers mustn't call usleep() or any direct equivalent:
      * instead, they may wait on their process latch, which sleeps as
      * necessary, but is awakened if postmaster dies.  That way the
      * background process goes away immediately in an emergency.
      */
    (void) WaitLatch(MyLatch,
              WL_LATCH_SET | WL_TIMEOUT | WL_EXIT_ON_PM_DEATH,
              timeout, PG_WAIT_EXTENSION);

    ResetLatch(MyLatch);

    CHECK_FOR_INTERRUPTS();

    /*
      * In case of a SIGHUP, just reload the configuration.
      */
    if (ConfigReloadPending)
    {
      ConfigReloadPending = false;
      ProcessConfigFile(PGC_SIGHUP);
    }

    do_some_thing(arg);
  }

  /* Not reachable */
}

/**
 * @brief 初始化后台进程
 */
void init_bgworker(void)
{
  BackgroundWorker worker;
  memset(&worker, 0, sizeof(worker));

  /* set up common data for all our workers */
  worker.bgw_flags        = BGWORKER_SHMEM_ACCESS | BGWORKER_BACKEND_DATABASE_CONNECTION;
  worker.bgw_start_time   = BgWorkerStart_RecoveryFinished;
  worker.bgw_restart_time = BGW_NEVER_RESTART;
  worker.bgw_notify_pid   = 0;

  sprintf(worker.bgw_library_name,  "my_pgext_template");   /* 库文件名 */
  sprintf(worker.bgw_function_name, "simple_worker_main");  /* 进程入口函数名 */

  /*
    * Now fill in worker-specific data, and do the actual registrations.
    */
  for (int i = 1; i <= 3; i++)
  {
    worker.bgw_main_arg = Int32GetDatum(i);

    snprintf(worker.bgw_name, BGW_MAXLEN, "simple worker [%d]", i);
    snprintf(worker.bgw_type, BGW_MAXLEN, "my_pgext_template");

    RegisterBackgroundWorker(&worker);
  }
}

// ---------------------------------------------------------------------------------------------------------------------
// 可以通过 sql 调用的函数

PG_FUNCTION_INFO_V1(my_hello_world);

Datum my_hello_world(PG_FUNCTION_ARGS)
{
  PG_RETURN_TEXT_P(cstring_to_text("Hello World !!!"));
}