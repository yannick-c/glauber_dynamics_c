# uncomment last parts for test coverage reports
AM_CFLAGS=-Ipcg-c/include -Ipcg-c/extras `pkg-config --libs glib-2.0` `pkg-config --cflags glib-2.0` -g -O0 -Wall # -fprofile-arcs -ftest-coverage
AM_LDFLAGS= -Lpcg-c/src/libpcg_random -lm # include math with -lm
bin_PROGRAMS=glauber_dynamics
glauber_dynamics_SOURCES=glauber_dynamics.c weightedgraph.c vertex.c edge.c update_rules.c pcg-c/extras/entropy.c
glauber_dynamics_LDADD=./pcg-c/src/libpcg_random.a

TESTS=$(check_PROGRAMS)                               
check_PROGRAMS=test_vertex test_weightedgraph
test_vertex_SOURCES=test_vertex.c vertex.c edge.c
test_weightedgraph_SOURCES=test_weightedgraph.c weightedgraph.c vertex.c edge.c
