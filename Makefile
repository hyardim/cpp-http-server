# Makefile for C++ HTTP Server
# Alternative to CMake build system

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread
DEBUG_FLAGS = -std=c++17 -Wall -Wextra -g -O0 -pthread

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Target executable
TARGET = $(BIN_DIR)/http_server

# Default target
all: $(TARGET)

# Create directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Build objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Link executable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ -pthread

# Debug build
debug: CXXFLAGS = $(DEBUG_FLAGS)
debug: $(TARGET)

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Install (requires sudo)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	sudo rm -f /usr/local/bin/http_server

# Run the server
run: $(TARGET)
	./$(TARGET)

# Run on custom port
run-port: $(TARGET)
	@read -p "Enter port number: " port; \
	./$(TARGET) --port $$port

# Test with curl
test: $(TARGET)
	@echo "Starting server in background..."
	@./$(TARGET) --port 8080 &
	@sleep 2
	@echo "Testing endpoints..."
	@curl -s http://localhost:8080/health | head -5
	@curl -s http://localhost:8080/echo?test=value | head -5
	@echo "Stopping server..."
	@pkill -f http_server

# Show help
help:
	@echo "Available targets:"
	@echo "  all        - Build the HTTP server (default)"
	@echo "  debug      - Build with debug symbols"
	@echo "  clean      - Remove build files"
	@echo "  install    - Install to system (requires sudo)"
	@echo "  uninstall  - Remove from system (requires sudo)"
	@echo "  run        - Build and run server on port 8080"
	@echo "  run-port   - Build and run server on custom port"
	@echo "  test       - Build, run, and test server endpoints"
	@echo "  help       - Show this help message"

# Phony targets
.PHONY: all debug clean install uninstall run run-port test help

# Dependencies
$(BUILD_DIR)/main.o: $(INCLUDE_DIR)/http_server.h
$(BUILD_DIR)/http_server.o: $(INCLUDE_DIR)/http_server.h $(INCLUDE_DIR)/http_request.h $(INCLUDE_DIR)/http_response.h $(INCLUDE_DIR)/route_handler.h
$(BUILD_DIR)/http_request.o: $(INCLUDE_DIR)/http_request.h
$(BUILD_DIR)/http_response.o: $(INCLUDE_DIR)/http_response.h
$(BUILD_DIR)/route_handler.o: $(INCLUDE_DIR)/route_handler.h $(INCLUDE_DIR)/http_request.h $(INCLUDE_DIR)/http_response.h 