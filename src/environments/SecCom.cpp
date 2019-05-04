#include"SecCom.h"

SecCom::SecCom(Random* random, int evalPerEpisode, bool verbose)
{
	trial = -1;
	this->random = random;
	MAX_STEPS= evalPerEpisode * 100;

	this->evalPerEpisode = evalPerEpisode;
	this->verbose = verbose;
}

SecCom::~SecCom()
{

}

void SecCom::start(int &number_of_observation_vars, int& number_of_action_vars)
{
	number_of_observation_vars= 1;

    this->number_of_observation_vars = number_of_observation_vars;
	observation= (ParameterType *)malloc(number_of_observation_vars * sizeof(ParameterType));

	number_of_action_vars=1;
	this->number_of_action_vars=number_of_action_vars;

	// Initialize state of Car
	restart();
}

float SecCom::step(ParameterType *action)
{
	// initial reward
	if(action==NULL)
	{
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////
	//Equation
	/////////////////////////////////////////////////////////////////////////

	ParameterType result=0;

	//result= observation[0]*1.43 + 1.69;
	ParameterType x= observation[0];
	result= x*x*x/1000 + 0.4*x + 20*sin(x/10) + 20*sin(100*x);
	
	if(verbose)
	{	
		printf("%f %f %f\n",observation[0],action[0],result);
	}

	ParameterType reward= -fabs(action[0] - result);
		
	observation[0]+= 1;
	
	if(observation[0] >= 100)
	{
		restart();
		return reward;
	}

	return reward;
}

void SecCom::print()
{
	//printf("Equation x*1.43 + 1.69, %f velocity %f\n",observation[0]);
}

float SecCom::restart()
{
	trial++;
	evaluation=0;

	observation[0]= -100;
	
	//printf("%f %f \n",observation[0],observation[1]);

	return 0;
}




