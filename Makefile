CC=g++
CXXFLAGS=-O3 -std=c++20 -Iinclude
EXENAME=bin/guidish_sample

all: build
build: $(EXENAME)
$(EXENAME): clean
	@mkdir -p bin
	$(CC) -o $(EXENAME) src/guidish_sample_usage.cpp $(CXXFLAGS)
clean:
	rm -rf $(EXENAME)
