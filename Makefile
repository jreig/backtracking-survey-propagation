# ------------------------------------------------------------------------------
# Project
# ------------------------------------------------------------------------------

CXX 						= g++
FLAGS 					= -g -Wall -std=c++17
BUILD_DIR 			= build
SRC_DIR 				= src
INCLUDE					= -I include/ -I libs/
EXP_DIR					= experiments
EXP_INST_DIR 		= ${EXP_DIR}/instances
EXP_RESULT_DIR 	= ${EXP_DIR}/result
TEST_DIR				= test

SRC	= $(wildcard $(SRC_DIR)/*.cpp)

.PHONY: clean build-dir run-experiments run-unit-test run-integration-test

all: build 

build: build-dir build-experiments build-test

build-dir:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(EXP_INST_DIR)
	@mkdir -p $(EXP_RESULT_DIR)

# general obj files compilation
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@echo "Compiling: $< -> $@"
	@$(CXX) $(FLAGS) $(INCLUDE) -c $< -o $@

# Separated both clean (src and test) because compilation of test main.cpp
# takes a lot more time than the rest of the code. As this file shouldn't change,
# there is no need to delete its compiled obj file.
# To force a test clean run 'make clean-test'.
# To force clean all run 'make clean-all'
clean: clean-src

clean-all: clean-test clean-src

# ------------------------------------------------------------------------------
# Build and Run experiments
# ------------------------------------------------------------------------------

EXP_TARGET	= experiment
EXP_SRC			= ${SRC} $(wildcard $(EXP_DIR)/*.cpp)
EXP_OBJ 		= $(EXP_SRC:%.cpp=$(BUILD_DIR)/%.o) 

build-experiments: $(BUILD_DIR)/$(EXP_TARGET)
	@echo "DONE: Compiled '${EXP_TARGET}' successsfully"

$(BUILD_DIR)/$(EXP_TARGET): $(EXP_OBJ)
	$(CXX) $(FLAGS) $^ -o $@
	
run-experiments: 
	@./$(BUILD_DIR)/$(EXP_TARGET)

clean-src:
	-@rm -rvf $(BUILD_DIR)/${SRC_DIR}/*
	-@rm -rvf $(BUILD_DIR)/${EXP_DIR}/*
	-@rm -rvf $(BUILD_DIR)/${EXP_TARGET}

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

run-test: run-unit-test run-integration-test

run-integration-test:
	@./${BUILD_DIR}/$(TEST_TARGET) [integration]

run-unit-test:
	@./$(BUILD_DIR)/$(TEST_TARGET) [unit]

run-test-prueba:
	@./$(BUILD_DIR)/$(TEST_TARGET) [prueba]

clean-test:
	-@rm -rvf $(BUILD_DIR)/${TEST_DIR}/*
	-@rm -rvf $(BUILD_DIR)/${TEST_TARGET}