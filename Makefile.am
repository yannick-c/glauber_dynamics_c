### GENERAL FLAGS
AM_CFLAGS=-I./include -I$(PCG_DIR)/include -I$(PCG_DIR)/extras -I$(WEIGHTEDGRAPH_DIR)/include `pkg-config --libs glib-2.0` `pkg-config --cflags glib-2.0` -g -O0 -Wall # -fprofile-arcs -ftest-coverage
ACLOCAL_AMFLAGS = '-I m4'
AM_LDFLAGS=-lm # include math with -lm

### START WEIGHTEDGRAPHS LIB
lib_LIBRARIES=lib/weightedgraph/libweightedgraph.a
lib_weightedgraph_libweightedgraph_a_SOURCES=$(WEIGHTEDGRAPH_DIR)/src/weightedgraph.c $(WEIGHTEDGRAPH_DIR)/src/vertex.c $(WEIGHTEDGRAPH_DIR)/src/edge.c
### END WEIGHTEDGRAPHS LIB

### START LOCAL SRC
bin_PROGRAMS=glauber_dynamics
glauber_dynamics_SOURCES=./src/glauber_dynamics.c ./src/update_rules.c $(PCG_DIR)/extras/entropy.c
glauber_dynamics_LDADD=$(PCG_DIR)/src/libpcg_random.a $(WEIGHTEDGRAPH_DIR)/libweightedgraph.a
### END LOCAL SRC

### START TEST
TESTS=$(check_PROGRAMS)                               
check_PROGRAMS=test/test_update_rules lib/weightedgraph/test/test_vertex lib/weightedgraph/test/test_weightedgraph

test_test_update_rules_SOURCES=test/test_update_rules.c src/update_rules.c
test_test_update_rules_LDADD=$(WEIGHTEDGRAPH_DIR)/libweightedgraph.a

lib_weightedgraph_test_test_vertex_SOURCES=$(WEIGHTEDGRAPH_DIR)/test/test_vertex.c $(WEIGHTEDGRAPH_DIR)/src/vertex.c $(WEIGHTEDGRAPH_DIR)/src/edge.c

lib_weightedgraph_test_test_weightedgraph_SOURCES=$(WEIGHTEDGRAPH_DIR)/test/test_weightedgraph.c
lib_weightedgraph_test_test_weightedgraph_LDADD=$(WEIGHTEDGRAPH_DIR)/libweightedgraph.a
### END TEST

### LOCAL DIRS
PCG_DIR=lib/pcg-c
WEIGHTEDGRAPH_DIR=lib/weightedgraph
