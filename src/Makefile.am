AUTOMAKE_OPTIONS=foreign
AM_CPPFLAGS= -I $(top_srcdir)/include -I /usr/include/oracle/11.2/client64/ -L /usr/lib/oracle/11.2/client64/lib/
bin_PROGRAMS=insertOracle

noinst_HEADERS = $(top_srcdir)/src/application/application.h \
	$(top_srcdir)/src/basicFun/basicFun.h \
	$(top_srcdir)/src/createThread/createThread.h \
	$(top_srcdir)/src/md/mdspi.h \
	$(top_srcdir)/src/trader/traderspi.h

insertOracle_SOURCES= $(top_srcdir)/src/application/application.cpp \
	$(top_srcdir)/src/basicFun/basicFun.cpp \
	$(top_srcdir)/src/createThread/createThread.cpp \
	$(top_srcdir)/src/md/mdspi.cpp \
	$(top_srcdir)/src/trader/traderspi.cpp \
	main.cpp

insertOracle_LDADD=-lthostmduserapi -lthosttraderapi -lclntsh -locci -lnnz11
AM_CXXFLAGS = -std=c++11
insertOracle_LDFLAGS=-lpthread
