
#ifndef SECCOM
#define SECCOM

#include "../parameters.h"
#include "Reinforcement_Environment.h"
#include "random/State_of_Art_Random.h"
#include "stdlib.h"
#include "stdio.h"


class SecCom : public Reinforcement_Environment
{

	public:
		SecCom(Random* random, int evalPerEpisode, bool verbose);
		~SecCom();
		
		int problem_index;
		int solved_counter;

		// Global variables:
		Random* random;
		int evalPerEpisode;
		int evaluation;
		bool verbose;
	
		void start(int &number_of_observation_vars, int& number_of_action_vars);
		float step(ParameterType *action);
		float restart();
		void print();
};

#endif
