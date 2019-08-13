# uncomment last parts for test coverage reports
AM_CFLAGS=-I../include `pkg-config --libs glib-2.0` `pkg-config --cflags glib-2.0` -g -O0 -Wall # -fprofile-arcs -ftest-coverage
AM_LDFLAGS= -lm # include math with -lm
lib_LTLIBRARIES=libweightedgraph.la
libweightedgraph_la_SOURCES=weightedgraph.c vertex.c edge.c
