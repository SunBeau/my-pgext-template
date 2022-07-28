# contrib/my-pgext-template/Makefile

MODULE_big = my-pgext-template
OBJS = $(WIN32RES) my-pgext-template.o
PG_CPPFLAGS = -I$(libpq_srcdir)
SHLIB_LINK_INTERNAL = $(libpq)

EXTENSION = my-pgext-template
DATA = my-pgext-template--1.0.sql
PGFILEDESC = "my-pgext-template - My PostgreSQL Extension Template"

REGRESS = paths my-pgext-template
REGRESS_OPTS = --dlpath=$(top_builddir)/src/test/regress
EXTRA_CLEAN = sql/paths.sql expected/paths.out

ifdef USE_PGXS
	PG_CONFIG = pg_config
	PGXS := $(shell $(PG_CONFIG) --pgxs)
	include $(PGXS)
else
	SHLIB_PREREQS = submake-libpq
	subdir = contrib/my-pgext-template
	top_builddir = ../..
	include $(top_builddir)/src/Makefile.global
	include $(top_srcdir)/contrib/contrib-global.mk
endif
