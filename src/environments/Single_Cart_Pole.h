
#ifndef SINGLE_CART_POLE_H
#define SINGLE_CART_POLE_H

#include"../parameters.h"
#include"Reinforcement_Environment.h"
#include"../agents/Reinforcement_Agent.h"
#include"random/State_of_Art_Random.h"
#include"stdlib.h"
#include"stdio.h"
#include "Discretizer.h"

#define FORCE_MAG 10.0
   
class Single_Cart_Pole : public Reinforcement_Environment
{
	public:

		Single_Cart_Pole(Random *random);
		~Single_Cart_Pole();
		
		//All Reinforcement Problems have the observartion variable, although it is not declared here!
		//double* observation;		
		//int number_of_observation_vars;
		//int number_of_action_vars;
		//int trial;
		
		//Reinforcement Problem API
		void start(int &number_of_observation_vars, int &number_of_action_vars);
		float step(ParameterType *action);
		float restart();
		void print();
	
		//auxiliary functions
        void cart_pole(float force, float *x, float *x_dot, float *theta, float *theta_dot);

		Random *random;
		float x, x_dot,theta,theta_dot;
};

#endif
