# uncomment last parts for test coverage reports
AM_CFLAGS=-I../include -I../lib/pcg-c/include -I../lib/pcg-c/extras -I../lib/weightedgraph/include `pkg-config --libs glib-2.0` `pkg-config --cflags glib-2.0` -g -O0 -Wall # -fprofile-arcs -ftest-coverage
AM_LDFLAGS=-lm # include math with -lm
bin_PROGRAMS=glauber_dynamics
glauber_dynamics_SOURCES=glauber_dynamics.c update_rules.c ../lib/pcg-c/extras/entropy.c
glauber_dynamics_LDADD=../lib/pcg-c/src/libpcg_random.a  ../lib/weightedgraph/src/libweightedgraph.la
