CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -g
GTEST_DIR = /opt/homebrew/Cellar/googletest/1.15.2
GTEST_INCLUDE = -I$(GTEST_DIR)/include
GTEST_LIBS = -L$(GTEST_DIR)/lib -lgtest -lgtest_main -pthread

TARGET = bin/svg_test
SRCS = svg_test.cpp

.PHONY: all clean run create_bin

all: create_bin $(TARGET)

create_bin:
	mkdir -p bin

$(TARGET): $(SRCS) | create_bin
	$(CXX) $(CXXFLAGS) $(GTEST_INCLUDE) $^ -o $@ $(GTEST_LIBS)

clean:
	rm -rf $(TARGET)

run: $(TARGET)
	pushd bin && ./svg_test && popd
