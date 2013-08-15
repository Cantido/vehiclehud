#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([vehiclehud], [1.0], [robert.c.richter@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
AC_PROG_CC_STDC
AC_LANG(C)

AC_CHECK_HEADERS([fcntl.h stdlib.h termios.h unistd.h])

AC_TYPE_SSIZE_T
AC_CHECK_FUNCS([strtol])


AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
