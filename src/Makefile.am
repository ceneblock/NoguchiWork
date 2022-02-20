bin_PROGRAMS = noguchi

noguchi_SOURCES = main.cpp 
noguchi_CPPFLAGS = $(APR_CFLAGS) $(APR_UTIL_CFLAGS) -I$(top_srcdir)/include 
noguchi_LDADD = $(APR_LIBS) $(APR_UTIL_LIBS) $(OPENCV4_LIBS) $(top_srcdir)/libs/libjson/libgillJson.la 

