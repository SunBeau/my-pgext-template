# contrib/my_pgext_template/Makefile

MODULE_big = my_pgext_template
OBJS = $(WIN32RES) my_pgext_template.o

PGFILEDESC = "my_pgext_template - My PostgreSQL Extension Template"

PG_CPPFLAGS = -I$(libpq_srcdir)
SHLIB_LINK_INTERNAL = $(libpq)

# 如果不是 Extension 则可以不需要如下两行,
# 也就可以不需要 xxx.sql 和 xxx.control 两个文件
EXTENSION = my_pgext_template
DATA = my_pgext_template--1.0.sql

REGRESS = paths my_pgext_template
REGRESS_OPTS = --dlpath=$(top_builddir)/src/test/regress

EXTRA_CLEAN = sql/paths.sql expected/paths.out

ifdef USE_PGXS
	PG_CONFIG = pg_config
	PGXS := $(shell $(PG_CONFIG) --pgxs)
	include $(PGXS)
else
	SHLIB_PREREQS = submake-libpq
	subdir = contrib/my_pgext_template
	top_builddir = ../..
	include $(top_builddir)/src/Makefile.global
	include $(top_srcdir)/contrib/contrib-global.mk
endif
