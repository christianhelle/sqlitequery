# Makefile - Build automation for SQLiteQueryAnalyzer

BUILD_DIR ?= build
INSTALL_PREFIX ?= ./linux
CMAKE ?= cmake
CPACK ?= cpack
MACDEPLOYQT ?= macdeployqt
NPROC ?= $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 1)
TEST_EXE ?= $(BUILD_DIR)/tests/SQLiteQueryTests
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
CONFIGURE_FLAGS = -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
DEFAULT_TARGETS = build test
INSTALL_TARGETS = build
PACKAGE_TARGETS = build
PACKAGE_CMD = $(MACDEPLOYQT) $(BUILD_DIR)/SQLiteQueryAnalyzer.app -dmg -appstore-compliant
else
CONFIGURE_FLAGS = -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$(INSTALL_PREFIX)
DEFAULT_TARGETS = cmake-install test
INSTALL_TARGETS = cmake-install
PACKAGE_TARGETS = cmake-install
PACKAGE_GENERATORS = 7Z ZIP TBZ2 TGZ TXZ TZ DEB RPM
endif

.PHONY: all configure build cmake-install install test package clean

all: $(DEFAULT_TARGETS)

configure:
	$(CMAKE) $(CONFIGURE_FLAGS)

build: configure
	$(CMAKE) --build $(BUILD_DIR) --config Release --parallel $(NPROC)

cmake-install: build
	$(CMAKE) --install $(BUILD_DIR)

test: build
	@if [ -x "$(TEST_EXE)" ]; then \
		"$(TEST_EXE)"; \
	else \
		echo "Test executable not found at: $(TEST_EXE)"; exit 1; \
	fi

install: $(INSTALL_TARGETS)
ifeq ($(UNAME_S),Darwin)
	@echo "Nothing extra to install on macOS; run 'make package' to create a DMG"
else
	mkdir -p "$(HOME)/.local/bin"
	rm -rf "$(HOME)/.local/opt/sqlitequery"
	mkdir -p "$(HOME)/.local/opt/sqlitequery"
	cp -rf "$(INSTALL_PREFIX)"/* "$(HOME)/.local/opt/sqlitequery"
	rm -f "$(HOME)/.local/bin/sqlitequery"
	ln -sf "$(HOME)/.local/opt/sqlitequery/bin/SQLiteQueryAnalyzer" "$(HOME)/.local/bin/sqlitequery"
	@echo "Installed to ~/.local/bin/sqlitequery"
endif

package: $(PACKAGE_TARGETS)
ifeq ($(UNAME_S),Darwin)
	$(PACKAGE_CMD)
	@echo "Package creation complete"
else
	@FAILED=0; for GEN in $(PACKAGE_GENERATORS); do \
		echo "Creating $$GEN package..."; \
		if $(CPACK) -G $$GEN --config "$(BUILD_DIR)/CPackConfig.cmake"; then \
			echo "Created $$GEN package"; \
		else \
			echo "Failed to create $$GEN package"; \
			FAILED=1; \
		fi; \
	done; \
	if [ "$$FAILED" = "1" ]; then echo "One or more packages failed"; exit 1; fi

	@echo "Snap packages: build them directly with snapcraft (Docker image recommended)"
	@echo "Package creation complete"
endif

clean:
	rm -rf $(BUILD_DIR)
