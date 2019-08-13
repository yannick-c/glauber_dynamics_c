#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([simulatePPP], [0.9], [yannick.couzinie@uniroma3.it])
AC_CONFIG_SRCDIR([src/glauber_dynamics.c])
AC_CONFIG_HEADERS([config.h])

# Program inits
AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT([])
CFLAGS+=" -std=c11"

# This is recommended by autoreconf -i
AC_CONFIG_MACRO_DIRS([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
AC_CHECK_LIB([m], [ldexp])
AC_CHECK_LIB([pcg_random], [pcg32_srandom_r])
AC_CHECK_LIB([glib-2.0])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h inttypes.h stddef.h stdint.h stdlib.h string.h])
AC_CHECK_HEADERS([unistd.h math.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([pow log ldexp pcg32_srandom_r pcg32_boundedrand_r pcg32_random_r])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
