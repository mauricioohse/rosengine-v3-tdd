# Common variables
CXX_WINDOWS = g++
CXX_WEB = emcc
CXXFLAGS = -Wall -MD -MP
INCLUDES = -I./include/SDL2 -I./src 

# Source files - now includes all cpp files in src and subdirectories
SOURCES = $(shell find src -name '*.cpp')
BUILD_DIR = bin
WEB_DIR = web

# Output targets
DEBUG_DIR = $(BUILD_DIR)/debug
RELEASE_DIR = $(BUILD_DIR)/release
DEBUG_TARGET = $(DEBUG_DIR)/game.exe
RELEASE_TARGET = $(RELEASE_DIR)/game.exe
WEB_TARGET = $(WEB_DIR)/index.html

# Object files
OBJECTS_DEBUG = $(SOURCES:src/%.cpp=$(DEBUG_DIR)/%.o)
OBJECTS_RELEASE = $(SOURCES:src/%.cpp=$(RELEASE_DIR)/%.o)

# Create necessary directories for object files
$(shell mkdir -p $(DEBUG_DIR)/core $(DEBUG_DIR)/game $(RELEASE_DIR)/core $(RELEASE_DIR)/game)

# DLL files to copy (using wildcard to get all DLLs)
DLLS = $(wildcard dll/*.dll)

# Debug-specific
DEBUG_FLAGS = -g -DDEBUG
DEBUG_LIBS = -L./lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

# Release-specific (with static linking)
RELEASE_FLAGS = -O2 -DNDEBUG
RELEASE_LIBS = -L./lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lwinmm -lusp10 -lgdi32 \
    -static -static-libgcc -static-libstdc++ \
    -lole32 -loleaut32 -limm32 -lversion -lsetupapi -lcfgmgr32 -lrpcrt4 \
    -mwindows

# Web-specific
# Optimization level 3 and link-time optimization for better performance
WEB_FLAGS = -O3 -flto \
    -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 \
    -s SDL2_IMAGE_FORMATS='["png"]' \
    -s SDL2_MIXER_FORMATS='["wav","mp3"]' \
    --preload-file assets \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s INITIAL_MEMORY=67108864 \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
    -s EXPORTED_FUNCTIONS='["_main"]' \
    -s ASSERTIONS=1 \
    -s GL_DEBUG=1 \
    --shell-file web/shell.html

# Debug build rules
$(DEBUG_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX_WINDOWS) $(CXXFLAGS) $(DEBUG_FLAGS) $(INCLUDES) -c $< -o $@

# Include dependency files
-include $(OBJECTS_DEBUG:.o=.d)

$(DEBUG_TARGET): $(OBJECTS_DEBUG)
	$(CXX_WINDOWS) $(OBJECTS_DEBUG) $(DEBUG_FLAGS) $(DEBUG_LIBS) -o $(DEBUG_TARGET)

debug: $(DEBUG_TARGET) copy_dlls_debug copy_assets_debug
	@echo "Debug build complete: $(DEBUG_TARGET)"

# Release build rules
$(RELEASE_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX_WINDOWS) $(CXXFLAGS) $(RELEASE_FLAGS) $(INCLUDES) -c $< -o $@

$(RELEASE_TARGET): $(OBJECTS_RELEASE)
	$(CXX_WINDOWS) $(OBJECTS_RELEASE) $(RELEASE_FLAGS) $(RELEASE_LIBS) -o $(RELEASE_TARGET)

release: $(RELEASE_TARGET) copy_assets_release
	@echo "Release build complete: $(RELEASE_TARGET)"

# Web build
web: $(WEB_TARGET)

$(WEB_TARGET): $(SOURCES) web/shell.html
	@mkdir -p $(WEB_DIR)
	@echo "Building for web..."
	@echo "Sources: $(SOURCES)"
	@echo "Output: $(WEB_TARGET)"
	$(CXX_WEB) $(SOURCES) $(INCLUDES) $(CXXFLAGS) $(WEB_FLAGS) -o $(WEB_TARGET) -v
	@echo "Web build complete: $(WEB_TARGET)"

# Utility targets
copy_dlls_debug:
	@echo "Copying DLLs to debug directory..."
	@for dll in $(DLLS); do \
		cp $$dll $(DEBUG_DIR)/; \
	done

copy_assets_debug:
	@echo "Copying assets to debug directory..."
	@rm -rf $(DEBUG_DIR)/assets
	@cp -r assets $(DEBUG_DIR)/

copy_assets_release:
	@echo "Copying assets to release directory..."
	@rm -rf $(RELEASE_DIR)/assets
	@cp -r assets $(RELEASE_DIR)/

clean:
	rm -rf $(DEBUG_DIR)/* $(RELEASE_DIR)/* web/*.js web/*.wasm web/*.data

.PHONY: debug release web clean copy_dlls_debug copy_assets_debug copy_assets_release

# Default target
help:
	@echo "Please specify a target: debug, release, or web"
	@echo "Usage:"
	@echo "  make debug   - Build debug version with DLLs"
	@echo "  make release - Build release version (standalone)"
	@echo "  make web     - Build web version"
	@echo "  make clean   - Clean all builds"

.DEFAULT_GOAL := help