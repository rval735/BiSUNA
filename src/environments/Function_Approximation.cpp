#include"Function_Approximation.h"

Function_Approximation::Function_Approximation(Random *random, int evaluations_per_episode, bool verbose)
{
	trial = -1;
	this->random = random;
#ifdef SEQUENTIAL_FUNCTION_APPROXIMATION
	MAX_STEPS = evaluations_per_episode*100;
#else
	MAX_STEPS = evaluations_per_episode;
#endif

	this->evaluations_per_episode = evaluations_per_episode;
	this->verbose = verbose;

	//force a random selection between the possible functionn, see restart()
#ifdef MULTIPLE_RANDOM_FUNCTION_APPROXIMATION
	solved_counter = NUMBER_OF_FUNCTION_APPROXIMATION;
	problem_index = (int)random->uniform(0, NUMBER_OF_FUNCTION_APPROXIMATION - 1);
#endif
}

Function_Approximation::~Function_Approximation()
{
}

void Function_Approximation::start(int &number_of_observation_vars, int &number_of_action_vars)
{
#ifdef SUPERVISED_FUNCTION_APPROXIMATION
	number_of_observation_vars = 2;
#else
	number_of_observation_vars = 1;
#endif
    
	this->number_of_observation_vars = number_of_observation_vars;
	observation = (ParameterType *)malloc(number_of_observation_vars * sizeof(ParameterType));

	number_of_action_vars = 1;
	this->number_of_action_vars = number_of_action_vars;

	// Initialize state of Car
	restart();
#ifdef SUPERVISED_FUNCTION_APPROXIMATION
	observation[1] = 0;
#endif
}

float Function_Approximation::step(ParameterType *action)
{
	// initial reward
	if (action == NULL)
	{
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////
	//Equation
	/////////////////////////////////////////////////////////////////////////

	float result = 0;

#ifdef CONTINUOUS_PARAM
    float act = action[0];
#else
    float act = transformFromPT(action[0], -1000, 1000);
#endif
    
#ifdef MULTIPLE_RANDOM_FUNCTION_APPROXIMATION
	switch(problem_index)
	{
		case 0:
		{
			result = observation[0] * 1.43 + 1.69;
		}
		break;
		case 1:
		{
			ParameterType x = observation[0];
			result = x * x * x / 1000 + 0.4 * x + 20 * sin(x / 10) + 20 * sin(100 * x);
			//result = observation[0]*1.43 + 1.69;
			//result = -result;
		}
		break;
	}
#else
	//result = observation[0]*1.43 + 1.69;
#ifdef CONTINUOUS_PARAM
	float x = observation[0];
#else
    float x = observation[0] - 100;
#endif
	result = x * x  * x / 1000 + 0.4 * x + 20 * sin(x / 10) + 20 * sin(100 * x);
#endif

	if (verbose)
	{
#ifdef CONTINUOUS_PARAM
        printf ("%f %f %f\n", observation[0], action[0], result);
#else
        printf ("%i %i %f\n", observation[0], action[0], result);
#endif
	}

	float reward = -fabs(act - result);

#ifdef	SUPERVISED_FUNCTION_APPROXIMATION
	/*if (reward > 0.2 * action[0] || reward < -0.2 * action[0])
	{
		observation[1] = 1;
	}
	else
	{
		observation[1] = 0;
	}
	*/
	//observation[1] = result;
	observation[1] = (ParameterType)problem_index;
#endif
	//printf ("reward %f\n",reward);
		
#ifdef	SEQUENTIAL_FUNCTION_APPROXIMATION
	observation[0] += 1;
	
	if (observation[0] >= 100)
	{
		restart();
		return reward;
	}
#else
	observation[0] = obsValue(random->uniform(-100.0,100.0));
#endif


	return reward;
}

void Function_Approximation::print()
{
	//printf ("Equation x*1.43 + 1.69, %f velocity %f\n",observation[0]);
}

float Function_Approximation::restart()
{
	trial++;
	evaluation = 0;

#ifdef SEQUENTIAL_FUNCTION_APPROXIMATION
	observation[0] = obsValue(-100);
#else
	observation[0] = obsValue(random->uniform(-100.0, 100.0));
#endif
	
	//printf ("%f %f \n",observation[0],observation[1]);
		
#ifdef MULTIPLE_RANDOM_FUNCTION_APPROXIMATION
	solved_counter++;
	if (solved_counter >= NUMBER_OF_FUNCTION_APPROXIMATION)
	{
		//since all problems were already tested, start again from a different initial problem
		problem_index = (int) random->uniform(0, NUMBER_OF_FUNCTION_APPROXIMATION - 1);
		solved_counter = 0;
	}
	else
	{
		//go to a different problem
		problem_index++;

		if (problem_index >= NUMBER_OF_FUNCTION_APPROXIMATION)
		{
			problem_index = 0;
		}
	}
	
	observation[1] = (ParameterType)problem_index;
#endif

	return 0;
}

ParameterType Function_Approximation::obsValue(float val) {
#ifdef CONTINUOUS_PARAM
    return val;
#else
    return val + 100;
#endif
}
