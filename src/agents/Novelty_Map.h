
#ifndef NOVELTY_MAP
#define NOVELTY_MAP

#include"stdlib.h"
#include"stdio.h"
#include"math.h"
#include"../parameters.h"

typedef struct _cell
{
	ParameterType *weight;
	int frequency;
	void *pointer;	//used to store something
} cell;

class Novelty_Map
{
	public:
		Novelty_Map(int population_size, int number_of_inputs);
		~Novelty_Map();
	
		cell *map;
		int population_size;
		int number_of_inputs;

		int current_population_size;
		ParameterType min_distance;
		int worst_individual;

		//auxiliary functions
		void insertIndividual(int individual_index, ParameterType *weight);
		void updateMinDistance();
		ParameterType minDistance(int individual);
		ParameterType minDistance(ParameterType *input_array);
		ParameterType minDistance(ParameterType *input_array, int &closest_individual);
		ParameterType quadraticDist(int individual_a, ParameterType *input_array);
		ParameterType quadraticDist(int individual_a, int individual_b);
        ParameterType hamming_distance(ParameterType x, ParameterType y);
		
        void print();
		void printToFile(const char *filename);
		
		//API
		int input(ParameterType *input_array);
		int inputNeutral(ParameterType *input_array);
};

#endif
