CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -MMD -MP -Iinclude

# Headers en include/, implementaciones en src/, main.cpp en la raíz.
TARGET := main
BUILD  := build
SRCS   := main.cpp $(wildcard src/*.cpp)
OBJS   := $(patsubst %.cpp,$(BUILD)/%.o,$(notdir $(SRCS)))
DEPS   := $(OBJS:.o=.d)

vpath %.cpp src

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
  .PHONY: $(RUN_ARGS)
  # Modo silencioso: sin esto make avisa "Nothing to be done" por cada argumento repetido.
  MAKEFLAGS += -s
endif

run: $(TARGET)
	./$(TARGET) $(RUN_ARGS)

clean:
	rm -rf $(BUILD) $(TARGET)

-include $(DEPS)

.PHONY: run clean
