# uncomment last parts for test coverage reports
AM_CFLAGS=`pkg-config --libs glib-2.0` `pkg-config --cflags glib-2.0` -g -O0 -Wall # -fprofile-arcs -ftest-coverage
bin_PROGRAMS=main
main_SOURCES=main.c weightedgraph.c vertex.c edge.c

TESTS=$(check_PROGRAMS)                               
check_PROGRAMS=test_vertex
test_vertex_SOURCES=test_vertex.c vertex.c edge.c
