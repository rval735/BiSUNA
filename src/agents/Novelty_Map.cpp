#include"Novelty_Map.h"

Novelty_Map::Novelty_Map(int population_size, int number_of_inputs)
{
	this->population_size = population_size;
	this->number_of_inputs = number_of_inputs;
	
	map = (cell *)malloc(sizeof(cell) * population_size);
	
	for (int i = 0; i < population_size; ++i) {
		map[i].weight = (ParameterType *)calloc(number_of_inputs, sizeof(ParameterType));
	}

	current_population_size = 0;
}

Novelty_Map::~Novelty_Map()
{
	//freeing memory
	for (int i = 0; i < population_size; ++i) {
		free(map[i].weight);
	}
    
	free(map);
}

void Novelty_Map::print()
{
	for (int i = 0; i < current_population_size; ++i) {
		for(int j = 0; j < number_of_inputs; ++j) {
#ifdef CONTINUOUS_PARAM
            printf("%f ", map[i].weight[j]);
#else
            printf("%i ", map[i].weight[j]);
#endif
		}
        
		printf("\n");
	}
}

void Novelty_Map::printToFile(const char *filename)
{
	FILE *fp = fopen(filename, "w");

	for(int i = 0; i < current_population_size; ++i) {
		for(int j = 0; j < number_of_inputs; ++j) {
#ifdef CONTINUOUS_PARAM
            fprintf(fp, "%f ", map[i].weight[j]);
#else
            fprintf(fp, "%i ", map[i].weight[j]);
#endif
		}
        
		fprintf(fp,"\n");
	}
    
    fclose(fp);
}

//return the index of the closest cell to the input_array
int Novelty_Map::input(ParameterType *input_array)
{
	int inserted_index;

	//check if the population is not full
	if(current_population_size < population_size) {
		inserted_index = current_population_size;
		insertIndividual(inserted_index, input_array);
		map[inserted_index].pointer = NULL;
		current_population_size++;
        
		if(current_population_size == population_size) {
			updateMinDistance();
		}

		return inserted_index;
	}
	//if the population is full, check for the uniqueness of the input
	//in the case the input is "unique enough", insert it
	else {
		int closest_individual;
		ParameterType current_input_min_distance = minDistance(input_array, closest_individual);
		if(min_distance < current_input_min_distance) {
			insertIndividual(worst_individual, input_array);
			updateMinDistance();
			//printf("removing worst %d \n",worst_individual);
			return worst_individual;
		}
		else {
			return closest_individual;
		}
	}
		
//    printf("ERROR: should have returned previously\n");
//    return -1;
}

//return the index of the closest cell to the input_array
//IMPORTANT: inputNeutral() does not modify anything inside the Novelty map, i.e., does not add or update it
int Novelty_Map::inputNeutral(ParameterType *input_array)
{
	int closest_individual;
	minDistance(input_array, closest_individual);

	return closest_individual;
}

void Novelty_Map::insertIndividual(int individual_index, ParameterType *weight)
{
	for(int i = 0; i < number_of_inputs; ++i) {
		map[individual_index].weight[i] = weight[i];
	}
    
	map[individual_index].frequency = 0;
}

//update the values of both:
//	-min_distance	- minimal distance of the worst individual to another individual in the population
//	-worst_individual - individual with smaller min_distance
void Novelty_Map::updateMinDistance()
{
	worst_individual = 0;
	min_distance = minDistance(worst_individual);
	for (int i = 1; i < current_population_size; ++i) {
		if (min_distance > minDistance(i)) {
			min_distance = minDistance(i);
			worst_individual = i;
		}
	}
}

//calculate the minimal distance to the individual
ParameterType Novelty_Map::minDistance(int individual)
{
	ParameterType dist;
    
	if (individual != 0) {
		dist = quadraticDist(individual, 0);
	}
	else {
		dist = quadraticDist(individual, 1);
	}

	for (int i = 1; i < current_population_size; ++i)
	{
		if (i != individual && dist > quadraticDist(individual, i)) {
			dist = quadraticDist(individual, i);
		}
	}

	return dist;
}

//calculate the minimal distance to the input
ParameterType Novelty_Map::minDistance(ParameterType *input_array, int &closest_individual)
{
	ParameterType dist = quadraticDist(0, input_array);
	closest_individual = 0;

	for (int i = 1; i < current_population_size; ++i) {
        ParameterType qd = quadraticDist(i, input_array);
		if(dist > qd) {
			dist = qd;
			closest_individual = i;
		}
	}

	return dist;
}

ParameterType Novelty_Map::quadraticDist(int individual_a, ParameterType *input_array)
{
	ParameterType dist = 0.0;
    
#ifdef CONTINUOUS_PARAM
	for (int i = 0; i < number_of_inputs; ++i) {
		dist += (map[individual_a].weight[i] - input_array[i]) *
			(map[individual_a].weight[i] - input_array[i]);
	}
#else
    for (int i = 0; i < number_of_inputs; ++i) {
        ParameterType w = map[individual_a].weight[i];
        ParameterType ia = input_array[i];
        dist += hamming_distance(w, ia);
    }
#endif

	return dist;
}

ParameterType Novelty_Map::quadraticDist(int individual_a, int individual_b)
{
	ParameterType dist = 0.0;
#ifdef CONTINUOUS_PARAM
	for (int i = 0; i < number_of_inputs; ++i) {
		dist += (map[individual_a].weight[i] - map[individual_b].weight[i]) *
			(map[individual_a].weight[i] - map[individual_b].weight[i]);
	}
#else
    for (int i = 0; i < number_of_inputs; ++i) {
        ParameterType ia = map[individual_a].weight[i];
        ParameterType ib = map[individual_b].weight[i];
        dist += hamming_distance(ia, ib);
    }
#endif
    
    return dist;
}

ParameterType Novelty_Map::hamming_distance(ParameterType x, ParameterType y)
{
#ifdef CONTINUOUS_PARAM
    return 0;
#else
    return __builtin_popcount(x ^ y);
#endif
}

