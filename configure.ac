#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([noguchi], [0.0.2], [ceneblock@wustl.edu])
AC_CONFIG_SRCDIR([src/main.cpp])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_HEADERS([config.h])

#AC_CONFIG_TESTDIR([tests])
AC_CONFIG_FILES([
                 Makefile 
                 src/Makefile 
                 libs/Makefile
                 libs/libjson/Makefile
                 ]
                )
#Muk up the CFLAGS variable
: ${CXXFLAGS="-g"}

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
LT_INIT
LT_LANG([C++])

AC_LANG([C++])
AC_CONFIG_MACRO_DIRS([m4])

# Checks for libraries.
AX_CXX_COMPILE_STDCXX_17([ext],[mandatory])

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([vector])

#Should be in POSIX fstat(3p)
AC_CHECK_HEADERS([sys/stat.h])

AC_CHECK_HEADERS([rapidjson/document.h], [], [have_rapidjson=no])
if test "x${have_rapidjson}" = xno; then
  AC_MSG_ERROR([
      -----------------------------------------------
       Unable to find rapidjson dev on this system.
       Check with your distribution on how to install
       rapidjson.

       Source code for rapidjson can be found at:
           https://github.com/Tencent/rapidjson
      -----------------------------------------------
      ])
fi

AC_CHECK_HEADERS([opencv2/imgproc.hpp], [], [have_opencv=no])
if test "x${have_opencv}" = xno; then
  AC_MSG_ERROR([
      -----------------------------------------------
       Unable to find opencv dev on this system.
       Check with your distribution on how to install
       opencv.

       Source code for opencv can be found at:
        https://github.com/opencv/opencv/tree/4.5.5
      -----------------------------------------------
      ])
fi

# Checks for typedefs, structures, and compiler characteristics.


# Checks for library functions.
PKG_CHECK_MODULES([GTEST], [gtest_main])
PKG_CHECK_MODULES([OPENCV4], [opencv4])

AC_OUTPUT

cat << EOF
-------------------------------------------------

${PACKAGE_NAME} Version ${PACKAGE_VERSION}

Prefix: '${prefix}'.
Compiler: '${CXX} ${CXXFLAGS} ${CPPFLAGS}'
Libraries: '${LIBS} ${GTEST_LIBS} ${OPENCV4_LIBS}'

Package features:

  Now type 'make @<:@<target>@:>@'
    where the optional <target> is:
      all                - build all binaries
      install            - install everything
      check              - runs the tests
--------------------------------------------------
EOF
