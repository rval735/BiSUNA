
#ifndef UNIFIED_NEURAL_MODEL_H
#define UNIFIED_NEURAL_MODEL_H

#include"Reinforcement_Agent.h"
#include"stdlib.h"
#include"stdio.h"
#include"time.h"
#include"useful/useful_utils.h"
#include"random/State_of_Art_Random.h"
#include"modules/Module.h"
#include"../parameters.h"
#include"Novelty_Map.h"

typedef struct _nmap_cell
{
	Module *module;
	float fitness;
} nmap_cell;


class Unified_Neural_Model : public Reinforcement_Agent
{
	public:

		Unified_Neural_Model(Random* random);
		~Unified_Neural_Model();

		//All Reinforcement Agents have the following commented variables, although it is not declared here!
		//float* action;
		//int number_of_observation_vars;
		//int number_of_action_vars;
		
		Random *random;

		Module ***subpopulation;
		Module ***tmp_subpopulation;
		float **fitness;
		float **tmp_fitness;
		int testing_individual;
		int testing_subpop;
		int best_index;
		int selected_individuals[NUMBER_OF_SUBPOPULATIONS][SUBPOPULATION_SIZE][2];	
		int generation;
		ParameterType step_counter;

		Novelty_Map* nmap;

		//auxiliary
		void printBest();
		void spectrumDiversityEvolve();
		float subpopulationObjective(Module *module, float fitness, int subpopulation_index);
		void calculateSpectrum(ParameterType *spectrum, int subpopulation_index, int individual_index);
		void endBestEpisode();

		//Implementing the Reinforcement Agent Interface
		void init(int number_of_observation_vars, int number_of_action_vars);
		void step(ParameterType *observation, float reward);
		void print();
		ParameterType stepBestAction(ParameterType *observation);
		void endEpisode(float reward);
		void saveAgent(const char* filename);
		void loadAgent(const char* filename);
		
		//debug
		void printSubpop();

};

#endif
