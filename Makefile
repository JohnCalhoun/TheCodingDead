COMPILE_OPTS = -std=c++17 -DBOOST_LOG_DYN_LINK
LINK_OPTS = -lstdc++ -lboost_program_options -lboost_log-mt -lboost_log_setup-mt

all : run 

run: $(shell find src -type f)
	gcc ./src/main.cpp $(COMPILE_OPTS) $(LINK_OPTS) -o $@ -Ofast
