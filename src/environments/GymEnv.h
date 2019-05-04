#ifndef GYMENV_H
#define GYMENV_H

#include "../parameters.h"
#include "Reinforcement_Environment.h"
#include "../agents/Reinforcement_Agent.h"
#include "random/State_of_Art_Random.h"
#include "stdlib.h"
#include "stdio.h"

#include "gym-uds/gym-uds.h"
#include "gym-uds/gym-uds.pb.h"

#include "Discretizer.h"
   
class GymEnv : public Reinforcement_Environment
{
public:
		GymEnv(Random *random, const char *socket);
		~GymEnv();
		
		//Reinforcement Problem API
		void start(int &number_of_observation_vars, int &number_of_action_vars);
		float step(ParameterType *action);
		float restart();
		void print();
	
		Random *random;
        bool didFinish;
private:
        gym_uds::EnvironmentClient envG;
        std::string envID;
        int envIDIdx;
        void copyObservation(gym_uds::observation_t obs);
        void copyObsMoutainCarV0(gym_uds::observation_t obs);
        gym_uds::action_t transformAction(ParameterType *action);
        void copyObsDiscreteTuple(gym_uds::observation_t obs);
        gym_uds::action_t transformActionDiscreteTuple(ParameterType *action);
        void verifyBounds(gym_uds::action_t &act);
        int findEnvIndex(const char **env, const char *name);
};

#endif
