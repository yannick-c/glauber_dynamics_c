### GENERAL FLAGS
AM_CFLAGS=-I./include -Ilib/pcg-c/include -Ilib/pcg-c/extras -Ilib/weightedgraph/include ${libglib_CFLAGS} ${libgvc_CFLAGS} -g -O0 -Wall # -fprofile-arcs -ftest-coverage
LDADD=-lm # include math with -lm
AUTOMAKE_OPTIONS = foreign # this allows you to not have README (since we have README.md)

### START WEIGHTEDGRAPHS LIB
lib_LIBRARIES=lib/weightedgraph/libweightedgraph.a
lib_weightedgraph_libweightedgraph_a_SOURCES=lib/weightedgraph/src/weightedgraph.c lib/weightedgraph/src/vertex.c lib/weightedgraph/src/edge.c
### END WEIGHTEDGRAPHS LIB

### START LOCAL SRC
bin_PROGRAMS=glauber_dynamics
glauber_dynamics_SOURCES=./src/glauber_dynamics.c ./src/update_rules.c lib/pcg-c/extras/entropy.c
glauber_dynamics_LDADD=lib/pcg-c/src/libpcg_random.a lib/weightedgraph/libweightedgraph.a ${libglib_LIBS} ${libgvc_LIBS}
### END LOCAL SRC

### START TEST
TESTS=$(check_PROGRAMS)                               
check_PROGRAMS=test/test_update_rules lib/weightedgraph/test/test_vertex lib/weightedgraph/test/test_weightedgraph

test_test_update_rules_SOURCES=test/test_update_rules.c src/update_rules.c
test_test_update_rules_LDADD=lib/weightedgraph/libweightedgraph.a ${libglib_LIBS} ${libgvc_LIBS}

lib_weightedgraph_test_test_vertex_SOURCES=lib/weightedgraph/test/test_vertex.c lib/weightedgraph/src/vertex.c lib/weightedgraph/src/edge.c
lib_weightedgraph_test_test_vertex_LDADD=${libglib_LIBS}

lib_weightedgraph_test_test_weightedgraph_SOURCES=lib/weightedgraph/test/test_weightedgraph.c
lib_weightedgraph_test_test_weightedgraph_LDADD=lib/weightedgraph/libweightedgraph.a ${libglib_LIBS} ${libgvc_LIBS}
### END TEST
