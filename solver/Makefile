CXX	= g++
# FLAGS = -g -Wall -std=c++17
FLAGS = -Wall -O3 -std=c++17
INCLUDE = -I include/ -I lib/
BUILD_DIR = build
SRC_DIR = src

SRC	= $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/**/*.cpp)
OBJ = $(SRC:%.cpp=$(BUILD_DIR)/%.o) 
TARGET = sp-solver

.PHONY: clean build

all: build 

clean:
	-@rm -rvf $(BUILD_DIR)/*
	-@rm -rvf $(TARGET)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@echo "Compiling: $< -> $@"
	@$(CXX) $(FLAGS) $(INCLUDE) -c $< -o $@

$(TARGET): $(OBJ)
	$(CXX) $(FLAGS) $^ -o $@

build: $(TARGET)
	@echo "DONE: Compiled '${TARGET}' successsfully"