.PHONY: all configure build clean
# Attention: This Makefile is intended for Windows usage only.

# Variables (do not change these)
BUILD_DIR = build
TOOLKITS_FOLDER := $(CURDIR)/lib
CONFIGURATION_TYPES = Debug;Release;MinSizeRel;RelWithDebInfo

# Variables (adjust these if needed)
GENERATOR = "Visual Studio 17 2022"
PROJECT_NAME = "SolarSysCG"
TOOLKIT_SUBDIRS = $(TOOLKITS_FOLDER)/glut;$(TOOLKITS_FOLDER)/glew;$(TOOLKITS_FOLDER)/devil

# The default target: run configuration and build
all: configure build run

dev: build run

# Configure the project using CMake
configure:
	@echo "Configuring the project in $(BUILD_DIR)..."
	-@mkdir $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. -DTOOLKITS_FOLDER=$(TOOLKITS_FOLDER) -DCMAKE_CONFIGURATION_TYPES="$(CONFIGURATION_TYPES)" -G $(GENERATOR) -A Win32

# Build the project
build:
	@echo "Building the project in $(BUILD_DIR)..."
	@cd $(BUILD_DIR) && cmake --build . --config Release
	@cd $(BUILD_DIR) && cmake --build . --config Debug

# Run the project
run:
	@echo "Running the project..."
	@cmd /C "set PATH=$(subst /,\,$(TOOLKIT_SUBDIRS));%PATH% && cd $(BUILD_DIR)\Release && $(PROJECT_NAME).exe"

# Debug the project
debug:
	@echo "Debugging the project..."
	-@cd $(BUILD_DIR) && ctest -C Debug
	@cmd /C "set PATH=$(subst /,\,$(TOOLKIT_SUBDIRS));%PATH% && cd $(BUILD_DIR)\Debug && $(PROJECT_NAME).exe"

# Clean up the build directory
clean:
	@echo "Cleaning up..."
	-@rmdir /S /Q $(BUILD_DIR)