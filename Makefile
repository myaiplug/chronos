# Chronos Spectral Weaver - 7-Band Parametric EQ Engine
# Professional Audio DSP Library

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -march=native -ffast-math
INCLUDES = -I./include
LDFLAGS = -lm

# Directories
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests
EXAMPLE_DIR = examples
BUILD_DIR = build

# Targets
TEST_TARGET = $(BUILD_DIR)/test_spectral_weaver
DEMO_TARGET = $(BUILD_DIR)/demo_spectral_weaver

# Source files
TEST_SRC = $(TEST_DIR)/test_spectral_weaver.cpp
DEMO_SRC = $(EXAMPLE_DIR)/demo_spectral_weaver.cpp

.PHONY: all test demo clean help build_only

all: test demo

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Build tests
test: $(BUILD_DIR) $(TEST_TARGET)
	@echo "Running tests..."
	@./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC)
	@echo "Building tests..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(TEST_SRC) -o $(TEST_TARGET) $(LDFLAGS)

# Build demo
demo: $(BUILD_DIR) $(DEMO_TARGET)
	@echo "Running demo..."
	@./$(DEMO_TARGET)

$(DEMO_TARGET): $(DEMO_SRC)
	@echo "Building demo..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEMO_SRC) -o $(DEMO_TARGET) $(LDFLAGS)

# Build both without running
build_only: $(BUILD_DIR) $(TEST_TARGET) $(DEMO_TARGET)
	@echo "Build complete!"

# Clean build artifacts
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)

# Help
help:
	@echo "Chronos Spectral Weaver - Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all        - Build and run tests and demo (default)"
	@echo "  test       - Build and run test suite"
	@echo "  demo       - Build and run demo application"
	@echo "  build_only - Build tests and demo without running"
	@echo "  clean      - Remove build artifacts"
	@echo "  help       - Display this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make            # Build and run everything"
	@echo "  make test       # Run only tests"
	@echo "  make demo       # Run only demo"
	@echo "  make build_only # Just compile, don't run"
