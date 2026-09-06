# Makefile - Build automation for SQLiteQueryAnalyzer

BUILD_DIR ?= build
INSTALL_PREFIX ?= ./linux
CMAKE ?= cmake
CPACK ?= cpack
MACDEPLOYQT ?= macdeployqt
NPROC ?= $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 1)
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
CONFIGURE_FLAGS = -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
DEFAULT_TARGETS = build
INSTALL_TARGETS = build
PACKAGE_TARGETS = build
PACKAGE_CMD = $(MACDEPLOYQT) $(BUILD_DIR)/SQLiteQueryAnalyzer.app -dmg -appstore-compliant
else
CONFIGURE_FLAGS = -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$(INSTALL_PREFIX)
DEFAULT_TARGETS = cmake-install
INSTALL_TARGETS = cmake-install
PACKAGE_TARGETS = cmake-install
PACKAGE_GENERATORS = 7Z ZIP TBZ2 TGZ TXZ TZ DEB RPM
endif

.PHONY: all configure build cmake-install install package clean

all: $(DEFAULT_TARGETS)

configure:
	$(CMAKE) $(CONFIGURE_FLAGS)

build: configure
	$(CMAKE) --build $(BUILD_DIR) --config Release --parallel $(NPROC)

cmake-install: build
	$(CMAKE) --install $(BUILD_DIR)

install: $(INSTALL_TARGETS)
ifeq ($(UNAME_S),Darwin)
	@echo "Nothing extra to install on macOS; run 'make package' to create a DMG"
else
	mkdir -p $(HOME)/.local/bin
	rm -rf /tmp/sqlitequery
	mkdir -p /tmp/sqlitequery
	cp -rf $(INSTALL_PREFIX)/* /tmp/sqlitequery
	rm -f $(HOME)/.local/bin/sqlitequery
	ln -sf /tmp/sqlitequery/bin/SQLiteQueryAnalyzer $(HOME)/.local/bin/sqlitequery
	@echo "Installed to ~/.local/bin/sqlitequery"
endif

package: $(PACKAGE_TARGETS)
ifeq ($(UNAME_S),Darwin)
	$(PACKAGE_CMD)
	@echo "Package creation complete"
else
	@for GEN in $(PACKAGE_GENERATORS); do \
		echo "Creating $$GEN package..."; \
		$(CPACK) -G $$GEN --config ./$(BUILD_DIR)/CPackConfig.cmake || exit 1; \
	done
	@echo "Snap packages: build them directly with snapcraft (Docker image recommended)"
	@echo "Package creation complete"
endif

clean:
	rm -rf $(BUILD_DIR)
