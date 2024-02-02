// SPDX-License-Identifier: GPL-3.0
// Copyright (c) 2023-2024 Julien Bernard
#ifndef LQP_API_H
#define LQP_API_H

#if defined _WIN32 || defined __CYGWIN__
#  ifdef __GNUC__
#    define LQP_EXPORT __attribute__((dllexport))
#    define LQP_IMPORT __attribute__((dllimport))
#  else
#    define LQP_EXPORT __declspec(dllexport)
#    define LQP_IMPORT __declspec(dllimport)
#  endif
#else
#  if __GNUC__ >= 4
#    define LQP_EXPORT __attribute__((visibility("default")))
#    define LQP_IMPORT __attribute__((visibility("default")))
#  else
#    define LQP_EXPORT
#    define LQP_IMPORT
#  endif
#endif

#ifndef LQP_STATIC
#  ifdef LQP_BUILD
#    define LQP_API LQP_EXPORT
#  else
#    define LQP_API LQP_IMPORT
#  endif
#else
#  define LQP_API
#endif

#endif // LQP_API_H
