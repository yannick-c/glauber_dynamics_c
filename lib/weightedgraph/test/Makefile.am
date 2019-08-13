# uncomment last parts for test coverage reports
AM_CFLAGS=-I../include `pkg-config --libs glib-2.0` `pkg-config --cflags glib-2.0` -g -O0 -Wall # -fprofile-arcs -ftest-coverage
AM_LDFLAGS=-lm # include math with -lm

TESTS=$(check_PROGRAMS)                               
check_PROGRAMS=test_vertex test_weightedgraph
test_vertex_SOURCES=test_vertex.c vertex.c edge.c
test_weightedgraph_SOURCES=test_weightedgraph.c weightedgraph.c vertex.c edge.c
