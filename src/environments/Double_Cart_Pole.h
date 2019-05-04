
#ifndef DOUBLE_CART_POLE_H
#define DOUBLE_CART_POLE_H

#include "../parameters.h"
#include "Reinforcement_Environment.h"
#include "../agents/Reinforcement_Agent.h"
#include "random/State_of_Art_Random.h"
#include "stdlib.h"
#include "stdio.h"
#include "Discretizer.h"
  	
#define FORCE_MAG 10.0
	
#define MUP 0.000002
#define FORCE_MAG 10.0
#define TAU 0.01		  //seconds between state updates 
#define GRAVITY -9.8
#define MASSCART 1.0
#define LENGTH_1 0.5		  
#define MASSPOLE_1 0.1
#define LENGTH_2 0.05
#define MASSPOLE_2 0.01
#define thirty_six_degrees 0.628329

class Double_Cart_Pole : public Reinforcement_Environment
{
	public:

		Double_Cart_Pole(Random *random);
		~Double_Cart_Pole();
		
		//All Reinforcement Problems have the observartion variable, although it is not declared here!
		//double* observation;		
		//int number_of_observation_vars;
		//int number_of_action_vars;
		//int trial;
		
		Random *random;
		float state[6];
		bool  normalized_observation;
		bool  normalized_action;
		
		//Reinforcement Problem API
		void start(int &number_of_observation_vars, int &number_of_action_vars);
		float step(ParameterType *action);
		float restart();
		void print();
		bool set(int feature);
	
		//auxiliary functions
        void double_cart_pole(float action, float *st, float *derivs);
        void rk4(float f, float y[], float dydx[], float yout[]);
        void copyToObservation(float *obs);
};

#endif
