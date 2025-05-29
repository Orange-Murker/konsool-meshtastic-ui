PORT ?= /dev/ttyACM0

IDF_PATH ?= $(shell cat .IDF_PATH 2>/dev/null || echo `pwd`/esp-idf)
IDF_TOOLS_PATH ?= $(shell cat .IDF_TOOLS_PATH 2>/dev/null || echo `pwd`/esp-idf-tools)
IDF_BRANCH ?= v5.4
IDF_EXPORT_QUIET ?= 1
IDF_GITHUB_ASSETS ?= dl.espressif.com/github_assets
MAKEFLAGS += --silent

SHELL := /usr/bin/env bash

DEVICE ?= tanmatsu # Default target device

export IDF_TOOLS_PATH
export IDF_GITHUB_ASSETS

# General targets

.PHONY: all
all: build flash

.PHONY: install
install: flash

# Preparation

.PHONY: prepare
prepare: submodules sdk getpypackages

.PHONY: submodules
submodules:
	if [ ! -f .submodules_update_done ]; then \
		echo "Updating submodules"; \
		git submodule update --init --recursive; \
		touch .submodules_update_done; \
	fi

# Get the python packages required by nanopb
.PHONY: getpypackages
getpypackages:
	source "$(IDF_PATH)/export.sh" >/dev/null && pip install protobuf grpcio-tools

.PHONY: sdk
sdk:
	if test -d "$(IDF_PATH)"; then echo -e "ESP-IDF target folder exists!\r\nPlease remove the folder or un-set the environment variable."; exit 1; fi
	if test -d "$(IDF_TOOLS_PATH)"; then echo -e "ESP-IDF tools target folder exists!\r\nPlease remove the folder or un-set the environment variable."; exit 1; fi
	git clone --recursive --branch "$(IDF_BRANCH)" https://github.com/espressif/esp-idf.git "$(IDF_PATH)" --depth=1 --shallow-submodules
	cd "$(IDF_PATH)"; git submodule update --init --recursive
	cd "$(IDF_PATH)"; bash install.sh all

.PHONY: removesdk
removesdk:
	rm -rf "$(IDF_PATH)"
	rm -rf "$(IDF_TOOLS_PATH)"

.PHONY: refreshsdk
refreshsdk: removesdk sdk getpypackages

.PHONY: menuconfig
menuconfig:
	source "$(IDF_PATH)/export.sh" && idf.py menuconfig -DDEVICE=$(DEVICE)

# Cleaning

.PHONY: clean
clean:
	rm -rf "build"

.PHONY: fullclean
fullclean: clean
	rm -rf sdkconfig
	rm -rf sdkconfig.old
	rm -rf sdkconfig.ci
	rm -rf sdkconfig.defaults

.PHONY: distclean
distclean: fullclean
	rm -rf $(IDF_PATH)
	rm -rf $(IDF_TOOLS_PATH)
	rm -rf managed_components
	rm -rf dependencies.lock
	rm -rf .cache

# Check if build environment is set up correctly
.PHONY: checkbuildenv
checkbuildenv:
	if [ -z "$(IDF_PATH)" ]; then echo "IDF_PATH is not set!"; exit 1; fi
	if [ -z "$(IDF_TOOLS_PATH)" ]; then echo "IDF_TOOLS_PATH is not set!"; exit 1; fi

# Building

.PHONY: build
build: checkbuildenv
	source "$(IDF_PATH)/export.sh" >/dev/null && idf.py build -DDEVICE=$(DEVICE)

# Hardware

.PHONY: flash
flash: build
	source "$(IDF_PATH)/export.sh" && \
	idf.py flash -p $(PORT)

.PHONY: flashmonitor
flashmonitor: build
	source "$(IDF_PATH)/export.sh" && \
	idf.py flash -p $(PORT) monitor

.PHONY: erase
erase:
	source "$(IDF_PATH)/export.sh" && idf.py erase-flash -p $(PORT)

.PHONY: monitor
monitor:
	source "$(IDF_PATH)/export.sh" && idf.py monitor -p $(PORT)

.PHONY: openocd
openocd:
	source "$(IDF_PATH)/export.sh" && idf.py openocd

.PHONY: gdb
gdb:
	source "$(IDF_PATH)/export.sh" && idf.py gdb

# Tools

.PHONY: size
size:
	source "$(IDF_PATH)/export.sh" && idf.py size

.PHONY: size-components
size-components:
	source "$(IDF_PATH)/export.sh" && idf.py size-components

.PHONY: partition-table
partition-table:
	source "$(IDF_PATH)/export.sh" && idf.py partition-table

.PHONY: save-defconfig
save-defconfig:
	source "$(IDF_PATH)/export.sh" && idf.py save-defconfig

.PHONY: size-files
size-files:
	source "$(IDF_PATH)/export.sh" && idf.py size-files

# Formatting

.PHONY: format
format:
	find main/ -iname '*.h' -o -iname '*.c' -o -iname '*.cpp' | xargs clang-format -i
