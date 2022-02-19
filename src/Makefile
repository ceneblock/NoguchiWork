all: noguchi

clean:
	-rm noguchi

noguchi: noguchi.cpp
	$(CXX) -g `pkg-config --libs opencv4` $? -o $@

.PHONY:
	all clean
