CXX = g++
CXXFLAGS = -std=c++23 -Wall -g  # Add the -g flag here

msplot_demo: msplot_demo.cpp msplot.hpp
	$(CXX) $(CXXFLAGS) msplot_demo.cpp -o msplot_demo

clean:
	rm -f msplot_demo

run: msplot_demo
	./msplot_demo
