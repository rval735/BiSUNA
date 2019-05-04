
#ifndef MULTIPLEXER
#define MULTIPLEXER

#include"../parameters.h"
#include"Reinforcement_Environment.h"
#include"../agents/Reinforcement_Agent.h"
#include"random/State_of_Art_Random.h"
#include"stdlib.h"
#include"stdio.h"
#include"math.h"


class Multiplexer : public Reinforcement_Environment
{

	public:
		Multiplexer(int address_bits, int data_bits, Random* random);
		~Multiplexer();
		
		//Inherited variables
		//ParameterType* observation;
		//int number_of_observation_vars;
		//int number_of_action_vars;
		//int trial;
		//int MAX_STEPS;
		
		Random *random;
		int current_combination;
		int all_combinations;
		char **input_data;
		int address_bits;
		int data_bits;
		bool normalized_observation;
		bool normalized_action;

		void shuffle();
		ParameterType getCorrectOutput();
	
		//API
		void start(int &number_of_observation_vars, int &number_of_action_vars);
		float step(ParameterType *action);
		float restart();
		void print();
		bool set(int feature);
};

#endif
