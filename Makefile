COMPILE_OPTS = -std=c++17 -DBOOST_LOG_DYN_LINK -Wall -Ofast -Iinclude
LINK_OPTS = -lstdc++ -lboost_program_options -lboost_log-mt -lboost_log_setup-mt -lboost_json

all : run 
run: obj/graph.o obj/vertex.o obj/room.o obj/person.o
	gcc main.cpp obj/graph.o obj/vertex.o obj/person.o obj/room.o $(COMPILE_OPTS) $(LINK_OPTS) -o $@ 

obj/graph.o: include/graph.hpp src/graph.cpp include/vertex.hpp include/room.hpp include/person.hpp
	gcc $(COMPILE_OPTS) -g -c src/graph.cpp -o obj/graph.o

obj/vertex.o: include/vertex.hpp src/vertex.cpp 
	gcc $(COMPILE_OPTS) -g -c src/vertex.cpp -o obj/vertex.o

obj/room.o: include/room.hpp src/room.cpp include/vertex.hpp
	gcc $(COMPILE_OPTS) -g -c src/room.cpp -o obj/room.o 

obj/person.o: include/person.hpp src/person.cpp include/vertex.hpp
	gcc $(COMPILE_OPTS) -g -c src/person.cpp -o obj/person.o 

