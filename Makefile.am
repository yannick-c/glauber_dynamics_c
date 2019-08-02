AM_CFLAGS=`pkg-config --cflags glib-2.0` -g -O3 -Wall
bin_PROGRAMS=main
main_SOURCES=main.c weightedgraph.c vertex.c edge.c

TESTS=$(check_PROGRAMS)                               
check_PROGRAMS=test_vertex
test_vertex_source=test_vertex.c vertex.c edge.c
