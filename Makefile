.PHONY: distclean all configure

SHELL := /bin/bash
RM    := rm -rf
CONFIGURE_CMD := ./configure 2> cmake_pass.log

# Automatically select the number of processors (keep one for system tasks).
# You can override that on the command line: "make NPROC=1".
NPROC := $(shell nproc --ignore=1)

all: ./build/Makefile
	$(MAKE) -C build -j$(NPROC)

build/Makefile:
	@ ($(CONFIGURE_CMD))

configure:
	@ ($(CONFIGURE_CMD))

distclean:
	@- $(RM) ./build/

SUBTARGETS := $(filter-out configure distclean, $(MAKECMDGOALS))
ifneq ($(SUBTARGETS),)
    $(SUBTARGETS): ./build/Makefile
	$(MAKE) -C build -j$(NPROC) $(MAKECMDGOALS)
endif

