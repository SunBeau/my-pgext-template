/*----------------------------------------------------------------------------------------------------------------------
 *
 * main.c
 *    插件入口
 *
 * Author:  SunBeau
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *    contrib/my_pgext_template/main.c
 *
 *----------------------------------------------------------------------------------------------------------------------
 */

#include "postgres.h"
#include "fmgr.h"

#include "my_pgext_template.h"

/* 插件必备 */
PG_MODULE_MAGIC;

extern void _PG_init(void);
extern void _PG_fini(void);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief 加载扩展时调用此接口
 *
 * 此接口中的操作如果需要全局生效，
 * 那么需要将此扩展配置到 shared_preload_libraries 或 session_preload_libraries 列表中.
 */
void _PG_init(void)
{
  init_guc();
  init_hook();
  init_bgworker();
}

/**
 * @brief 经测试, 此接口未被调用
 */
void _PG_fini(void)
{
  ;
}
