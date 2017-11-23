#!/bin/bash

set -e

SRC_DIR=platform
TEST_DIR=test-cpp
RUNNER=$TEST_DIR/run-tests

g++ -std=c++11 -g -O0 -o $RUNNER \
  $SRC_DIR/formatNumber.cpp \
  $TEST_DIR/test.cpp \
  $TEST_DIR/formatNumber.cpp \
  $TEST_DIR/list.cpp

if [[ $* == *--leak-check* ]]; then
    valgrind --leak-check=yes $RUNNER
else
    $RUNNER $@
fi
