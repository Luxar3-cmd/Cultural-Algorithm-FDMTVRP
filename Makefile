CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -MMD -MP

TARGET := main
BUILD  := build
SRCS   := $(wildcard *.cpp)
OBJS   := $(SRCS:%.cpp=$(BUILD)/%.o)
DEPS   := $(OBJS:.o=.d)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD)/%.o: %.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $@

# `make run <args...>`: todo lo que sigue a "run" se pasa al binario.
# Los argumentos se declaran como targets vacíos para que make no intente construirlos.
ifeq (run,$(firstword $(MAKECMDGOALS)))
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(RUN_ARGS):;@:)
endif

run: $(TARGET)
	./$(TARGET) $(RUN_ARGS)

clean:
	rm -rf $(BUILD) $(TARGET)

-include $(DEPS)

.PHONY: run clean
