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

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD) $(TARGET)

-include $(DEPS)

.PHONY: run clean
