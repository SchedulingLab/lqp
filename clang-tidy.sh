#!/bin/sh

clang-tidy -p build include/lqp/*.h library/*.cc
