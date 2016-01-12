include $(top_srcdir)/build/autotools/Makefile.am.silent

NULL =

SUBDIRS = build clutter
DIST_SUBDIRS = clutter build

# XXX - this is a massive hack to make autoreconf honour the ACLOCAL_FLAGS
# that jhbuild sets while still retaining build/autotools as the authoritative
# source for m4 macros
ACLOCAL_AMFLAGS = -I build/autotools ${ACLOCAL_FLAGS}

EXTRA_DIST = \
	ChangeLog.pre-git-import	\
	README.in			\
	README				\
	$(NULL)

CLEANFILES = $(pcfiles)

DISTCLEANFILES =

DISTCHECK_CONFIGURE_FLAGS = --enable-maintainer-flags

# .release requires .changelog rules
include $(top_srcdir)/build/autotools/Makefile.am.changelog
include $(top_srcdir)/build/autotools/Makefile.am.release
