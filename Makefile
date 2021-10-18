
.PHONY: generate debug release build

FLAVOR := debug

all: help

help:
	@echo "debug   - build a debug version"
	@echo "release - build a release version"
	@echo "test    - run tests"

generate:
	@cmake -B $(FLAVOR) .

set-debug:
	@$(eval FLAVOR := debug)

set-release:
	@$(eval FLAVOR := release)

debug: set-debug generate build

release: set-release generate build

test: release
	$(FLAVOR)/clist-test

build:
	$(MAKE) -C $(FLAVOR)

clean:
	$(MAKE) -C $(FLAVOR) clean