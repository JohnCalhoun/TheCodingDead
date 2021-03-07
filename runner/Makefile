.PHONY: clean

COMPILE_OPTS = -std=c++17 -DBOOST_LOG_DYN_LINK -Wall -Ofast -Iinclude 
LINK_OPTS = -lstdc++ -lboost_program_options -lboost_log-mt -lboost_log_setup-mt -lboost_json -lboost_timer

all : ../bin/run  ../bin/process_output

obj/ :
	mkdir -p obj

../bin/process_output: src/process_output.cpp
	gcc src/process_output.cpp $(COMPILE_OPTS) $(LINK_OPTS) -o $@ 

../bin/run: obj/graph.o obj/vertex.o obj/room.o obj/person.o src/main.cpp obj/position.o 
	gcc src/main.cpp obj/graph.o obj/vertex.o obj/person.o obj/room.o obj/position.o $(COMPILE_OPTS) $(LINK_OPTS) -o $@ 

obj/graph.o: include/graph.hpp src/graph.cpp include/vertex.hpp include/room.hpp include/person.hpp 
	gcc $(COMPILE_OPTS)  -c src/graph.cpp -o obj/graph.o

obj/vertex.o: include/vertex.hpp src/vertex.cpp 
	gcc $(COMPILE_OPTS)  -c src/vertex.cpp -o obj/vertex.o

obj/room.o: include/room.hpp src/room.cpp include/vertex.hpp include/position.hpp
	gcc $(COMPILE_OPTS)  -c src/room.cpp -o obj/room.o 

obj/person.o: include/person.hpp src/person.cpp include/vertex.hpp include/position.hpp
	gcc $(COMPILE_OPTS)  -c src/person.cpp -o obj/person.o 

obj/position.o: include/position.hpp src/position.cpp
	gcc $(COMPILE_OPTS)  -c src/position.cpp -o obj/position.o 
clean:
	rm obj/*.o
