COMPILE_OPTS = -lstdc++ -std=c++17 

all : run 

run: $(shell find src -type f)
	gcc ./src/main.cpp $(COMPILE_OPTS) -o $@
