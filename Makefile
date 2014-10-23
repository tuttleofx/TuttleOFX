

SHELL := /bin/bash
RM    := rm -rf

# keep one processor for system tasks
NPROC := $(shell nproc --ignore=1)

all: ./build/Makefile
	$(MAKE) -C build -j$(NPROC)

build/Makefile: build
	@ (cd build >/dev/null 2>&1 && cmake .. 2> cmake_pass.log)

build:
	@ (mkdir build)

clean: ./build/Makefile
	$(MAKE) -C build clean 

distclean:
	@- $(RM) ./build/

.PHONY: clean distclean all
