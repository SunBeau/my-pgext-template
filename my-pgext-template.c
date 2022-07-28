/*-------------------------------------------------------------------------
 *
 * my-pgext-template.c
 *    My PostgreSQL Extension Template
 *
 * Author:  SunBeau
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *    contrib/my-pgext-template/my-pgext-template.c
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "tcop/utility.h"

#include <stdlib.h>

/* 扩展必备 */
PG_MODULE_MAGIC;

extern void _PG_init(void);
extern void _PG_fini(void);

static void MyProcessUtility(PlannedStmt *pstmt,
                      const char *queryString,
                      bool readOnlyTree,
                      ProcessUtilityContext context,
                      ParamListInfo params,
                      QueryEnvironment *queryEnv,
                      DestReceiver *dest,
                      QueryCompletion *qc);

static ProcessUtility_hook_type prev_ProcessUtility = NULL;

// ---------------------------------------------------------------------------------------------------------------------

static void MyProcessUtility(PlannedStmt *pstmt,
                      const char *queryString,
                      bool readOnlyTree,
                      ProcessUtilityContext context,
                      ParamListInfo params,
                      QueryEnvironment *queryEnv,
                      DestReceiver *dest,
                      QueryCompletion *qc)
{
  elog(NOTICE, "MyProcessUtility");

  if (prev_ProcessUtility)
    (*prev_ProcessUtility)(pstmt, queryString, readOnlyTree, context, params, queryEnv, dest, qc);
  else
    standard_ProcessUtility(pstmt, queryString, readOnlyTree, context, params, queryEnv, dest, qc);
}

PG_FUNCTION_INFO_V1(my_hello_world);
Datum my_hello_world(PG_FUNCTION_ARGS)
{
  PG_RETURN_TEXT_P(cstring_to_text("Hello World !!!"));
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief 加载扩展时调用此接口
 *
 * 此接口中的操作如果需要全局生效，
 * 那么需要将此扩展配置到 shared_preload_libraries 或 session_preload_libraries 列表中.
 */
void _PG_init(void)
{
  prev_ProcessUtility = ProcessUtility_hook;
  ProcessUtility_hook = MyProcessUtility;
}

/**
 * @brief 经测试, 此接口未被调用
 */
void _PG_fini(void)
{
  ProcessUtility_hook = prev_ProcessUtility;
}
