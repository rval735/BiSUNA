
#include"Dummy.h"

Dummy::Dummy(Reinforcement_Environment* env)
{
	//random= new State_of_Art_Random(time(NULL));
	this->env= env;
}

Dummy::~Dummy()
{
	free(action);
}
		
void Dummy::saveAgent(const char* filename)
{
	printf("saveAgent() not implemented\n");

}

void Dummy::loadAgent(const char* filename)
{
	printf("loadAgent() not implemented\n");
}

void Dummy::init(int number_of_observation_vars, int number_of_action_vars)
{
	this->number_of_observation_vars= number_of_observation_vars;
	this->number_of_action_vars= number_of_action_vars;

	action = (ParameterType *)malloc(number_of_action_vars * sizeof(ParameterType));
}

void Dummy::step(ParameterType *observation, ParameterType reward)
{
	int i;

	//print environment's information
	env->print();

	//print observation and reward
	printArray(observation, number_of_observation_vars);
    
#ifdef CONTINUOUS_PARAM
	printf("Reward: %f\n",reward);
#else
    printf("Reward: %i\n",reward);
#endif
    
	//just ignore reward and observation 
	//and give a random action! :P
	for(i=0;i<number_of_action_vars;++i)
	{
		printf("Action %d: ",i);
#ifdef CONTINUOUS_PARAM
        scanf("%f",&(action[i]));
#else
        scanf("%i",&(action[i]));
#endif

	}
	printArray(action,number_of_action_vars);
}
		
void Dummy::endEpisode()
{
	printf("Episode finished!!!!!!!!!!!\nTime for a beer\n");
}

void Dummy::print()
{

}

ParameterType Dummy::stepBestAction(ParameterType *observation)
{
	return 0;
}
