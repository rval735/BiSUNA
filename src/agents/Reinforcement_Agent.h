
#ifndef REINFORCEMENT_AGENT
#define REINFORCEMENT_AGENT

#include "../parameters.h"

class Reinforcement_Agent
{
	public:

		ParameterType *action;
		int number_of_observation_vars;
		int number_of_action_vars;

		virtual void init(int number_of_observation_vars, int number_of_action_vars) = 0;
		virtual void step(ParameterType *observation, float reward) = 0;
		
		//agent uses its best procedure to process the observation,
		//the state-value is returned when present and possible
		virtual ParameterType stepBestAction(ParameterType *observation) = 0;
		virtual void print() = 0;
		virtual void saveAgent(const char* filename) = 0;
		virtual void loadAgent(const char* filename) = 0;


		//a temporary API function
		//this function is not realistic because in the real life knowing when an episode
		//ends is part of the challenge
		virtual void endEpisode(float reward) = 0;
};

#endif
