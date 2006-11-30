
noinst_LTLIBRARIES = libtaygeta.la

libtaygeta_la_SOURCES = randgen.cpp r250.cpp registrar.cpp rndlcg.cpp

include_HEADERS = randgen.hpp r250.hpp

EXTRA_DIST = registrar.hpp rndlcg.hpp random.hpp

TESTS = r250tst
check_PROGRAMS = r250tst

r250tst_SOURCES = r250tst.cpp

LDADD = libtaygeta.la

include $(top_srcdir)/config/Makefile.include

