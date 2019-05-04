
PATH_TO_ZWEIFEL_LIBRARY=../zweifel

CFLAGS= -O3 -Wall
#-Wall -pedantic -ansi
#CFLAGS=-O4

MAIN_REINFORCEMENT_LEARNING=src/main.cpp
MAIN_LIVE=live.cpp
ENVIRONMENTS=src/environments/GymEnv.cpp src/environments/Double_Cart_Pole.cpp src/environments/Mountain_Car.cpp src/environments/Single_Cart_Pole.cpp src/environments/Function_Approximation.cpp src/environments/Multiplexer.cpp
ENVIRONMENTSXTRA=src/environments/gym-uds/gym-uds.grpc.pb.cc src/environments/gym-uds/gym-uds.pb.cc src/environments/gym-uds/gym-uds.cc
AGENTS=src/agents/Unified_Neural_Model.cpp
MODELS=src/agents/modules/Module.cpp src/agents/modules/ModuleDebug.cpp
MAPS=src/agents/Novelty_Map.cpp

LDFLAGS = -L${PATH_TO_ZWEIFEL_LIBRARY}/lib -I${PATH_TO_ZWEIFEL_LIBRARY}/src/ -lzrandom -lzgraph
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++ grpc`

all: main

main:
	g++ -std=c++11 $(CFLAGS) $(MAIN_REINFORCEMENT_LEARNING) $(AGENTS) $(MODELS) $(MAPS) $(ENVIRONMENTS) $(ENVIRONMENTSXTRA) $(LDFLAGS) -o bisuna