# ------------------------------------------------------------------------------
# Project
# ------------------------------------------------------------------------------

CXX 				= g++
FLAGS 			= -g -Wall -std=c++11
BUILD_DIR 	= build
SRC_DIR 		= src
INCLUDE			= -I include/ -I libs/
EX_DIR			= examples
TEST_DIR		= test

SRC	= $(wildcard $(SRC_DIR)/*.cpp)

.PHONY: clean build-dir run run-unit-test

all: build 

build: build-dir build-example build-test

build-dir:
	@mkdir -p $(BUILD_DIR)

# general obj files compilation
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@echo "Compiling: $< -> $@"
	@$(CXX) $(FLAGS) $(INCLUDE) -c $< -o $@

clean:
	-@rm -rvf $(BUILD_DIR)/*

# ------------------------------------------------------------------------------
# Build and Run example
# ------------------------------------------------------------------------------

EX_TARGET		= example
EX_SRC			= ${SRC} $(wildcard $(EX_DIR)/*.cpp)
EX_OBJ 			= $(EX_SRC:%.cpp=$(BUILD_DIR)/%.o) 

build-example: $(BUILD_DIR)/$(EX_TARGET)
	@echo "DONE: Compiled '${EX_TARGET}' successsfully"

$(BUILD_DIR)/$(EX_TARGET): $(EX_OBJ)
	$(CXX) $(FLAGS) $^ -o $@
	
run: 
	@./$(BUILD_DIR)/$(EX_TARGET)

# ------------------------------------------------------------------------------
# Test
# ------------------------------------------------------------------------------
TEST_TARGET	= test-runner # Can't be 'test' (same name as folder)
TEST_MAIN 	= ${TEST_DIR}/main.cpp
TEST_SRC 		= ${SRC} ${TEST_MAIN} $(wildcard test/**/*.cpp)
TEST_OBJ 		= $(TEST_SRC:%.cpp=$(BUILD_DIR)/%.o)

build-test: $(BUILD_DIR)/$(TEST_TARGET)
	@echo "DONE: Compiled '${TEST_TARGET}' successsfully"

$(BUILD_DIR)/$(TEST_TARGET): ${TEST_OBJ}
	$(CXX) $(FLAGS) $^ -o $@

run-test: run-unit-test

run-unit-test:
	@./$(BUILD_DIR)/$(TEST_TARGET) [unit]

