PHONY_TARGETS := distclean all configure
.PHONY: $(PHONY_TARGETS)

SHELL := /bin/bash
RM    := rm -rf
CONFIGURE_CMD := ./configure 2> cmake_pass.log

# Automatically select the number of processors.
# You can override that on the command line: "make NPROC=1".
NPROC := 1
ifeq ($(OS),Windows_NT)
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        # Keep one proc for system tasks
        NPROC := $(shell nproc --ignore=1)
    endif
    ifeq ($(UNAME_S),Darwin)
        NPROC := $(shell sysctl -n hw.ncpu)
    endif
endif

all: ./build/Makefile
	$(MAKE) -C build -j$(NPROC)

build/Makefile:
	@ ($(CONFIGURE_CMD))

configure:
	@ ($(CONFIGURE_CMD))

distclean:
	@- $(RM) ./build/

SUBTARGETS := $(filter-out $(PHONY_TARGETS), $(MAKECMDGOALS))
ifneq ($(SUBTARGETS),)
    $(SUBTARGETS): ./build/Makefile
	$(MAKE) -C build -j$(NPROC) $(MAKECMDGOALS)
endif

