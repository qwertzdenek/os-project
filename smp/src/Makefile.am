AUTOMAKE_OPTIONS = subdir-objects
SUBDIRS = sched

bin_PROGRAMS = scheduler
scheduler_SOURCES = main.cpp core/smp.cpp
scheduler_LDADD = sched/libsched_round_robin.a \
                  sched/libsched_fair.a

AM_CPPFLAGS = -pedantic -std=c++11
AM_LDFLAGS = -static -static-libgcc -static-libstdc++

