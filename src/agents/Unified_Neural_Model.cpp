
#include "Unified_Neural_Model.h"
#include "modules/ModuleDebug.h"

Unified_Neural_Model::Unified_Neural_Model(Random *random)
{
	this->random = random;

	Module::setRandom(random);
	
	testing_subpop = 0;
	testing_individual = 0;
	generation = 1;
	best_index = 0;

	nmap = new Novelty_Map(NOVELTY_MAP_SIZE, SPECTRUM_SIZE);

	if (NOVELTY_MAP_SIZE >= SUBPOPULATION_SIZE) {
		printf("ERROR: Novelty map's size is bigger or equal to the subpopulation size\n");
		exit(1);
	}
	
	if (NUMBER_OF_SUBPOPULATIONS != 1) {
		printf("ERROR: Number of subpopulations is different than 1\n");
		exit(1);
	}

	step_counter = 0;
}

Unified_Neural_Model::~Unified_Neural_Model()
{
	free(action);
	
	for(int i = 0; i < NUMBER_OF_SUBPOPULATIONS; ++i) {
		for(int j = 0; j < SUBPOPULATION_SIZE; ++j) {
			delete subpopulation[i][j];
		}
        
		delete subpopulation[i];
		free(fitness[i]);
	}
	
	delete subpopulation;
	free(fitness);
}

/*
void Unified_Neural_Model::savePopulation()
{
	printf("Saving population\n");
	
	for(int i= 0; i < NUMBER_OF_SUBPOPULATIONS; ++i )
	{
		for(int j=0;j<SUBPOPULATION_SIZE; ++j)
		{
			subpopulation[i][j]->saveDNA();		
		}
	}
}
*/
		
void Unified_Neural_Model::saveAgent(const char *filename)
{
	//printf("Saving best agent model\n");

	//execute individual
	subpopulation[MAIN_SUBPOP][best_index]->saveDNA(filename);		
    
    unsigned int fnSize = static_cast<unsigned int>(strlen(filename)) + 4 * sizeof(char);
    char fn[fnSize];
    strcpy(fn, filename);
    strcat(fn, ".dot");
    
    ModuleDebug::printGraph(subpopulation[MAIN_SUBPOP][best_index], fn);
}

void Unified_Neural_Model::loadAgent(const char *filename)
{
	//printf("Loading agent model\n");
	
	//execute individual
	subpopulation[MAIN_SUBPOP][best_index]->loadDNA(filename);	

	subpopulation[MAIN_SUBPOP][best_index]->clearMemory();
}

void Unified_Neural_Model::init(int number_of_observation_vars, int number_of_action_vars)
{
	this->number_of_observation_vars = number_of_observation_vars;
	this->number_of_action_vars = number_of_action_vars;

	action = (ParameterType *)calloc(number_of_action_vars, sizeof(ParameterType));

	//random subpopulation	
	subpopulation = new Module **[NUMBER_OF_SUBPOPULATIONS];
	tmp_subpopulation= new Module **[NUMBER_OF_SUBPOPULATIONS];
	fitness = (float **)malloc(sizeof(float *) * NUMBER_OF_SUBPOPULATIONS);
	tmp_fitness = (float **)malloc(sizeof(float *) * NUMBER_OF_SUBPOPULATIONS);
	for (int i = 0; i < NUMBER_OF_SUBPOPULATIONS; ++i) {
		subpopulation[i] = new Module *[SUBPOPULATION_SIZE];
		tmp_subpopulation[i] = new Module *[SUBPOPULATION_SIZE];
		fitness[i] = (float *)malloc(sizeof(float) * SUBPOPULATION_SIZE);
		tmp_fitness[i] = (float *)malloc(sizeof(float) * SUBPOPULATION_SIZE);
		for (int j = 0; j < SUBPOPULATION_SIZE; ++j) {
			subpopulation[i][j] = new Module(number_of_observation_vars, number_of_action_vars, INITIAL_ALLOCATION_LENGTH);
			tmp_subpopulation[i][j] = new Module(number_of_observation_vars, number_of_action_vars, INITIAL_ALLOCATION_LENGTH);

			fitness[i][j] = EXTREME_NEGATIVE_REWARD;

			//starting mutations
			for(int k = 0; k < NUMBER_OF_INITIAL_MUTATIONS; ++k) {
				subpopulation[i][j]->structuralMutation();
			}
            
			subpopulation[i][j]->updatePrimerList();
		}
	}
}

void Unified_Neural_Model::step(ParameterType *observation, float reward)
{
	//execute individual
	subpopulation[testing_subpop][testing_individual]->process(observation, action);		

	//update reward
	tmp_fitness[testing_subpop][testing_individual] += reward;
}
		
void Unified_Neural_Model::endEpisode(float reward)
{
	//update reward
	tmp_fitness[testing_subpop][testing_individual] += reward;

	step_counter++;
	
	//printf("%f\n",tmp_fitness[testing_subpop][testing_individual]);

	//average 
	if (step_counter >= EPISODES_PER_INDIVIDUAL) {
		tmp_fitness[testing_subpop][testing_individual] /= (ParameterType)step_counter;
		step_counter = 0;
	}
	else {
		return;
	}
	
	subpopulation[testing_subpop][testing_individual]->clearMemory();		
	
	//test a new individual from this subpopulation
	testing_individual++;

	//change subpopulation if all individuals from this subpop were already tested
	//evolve if all subpopulations and all individuals were tested (evaluated)
	if (testing_individual >= SUBPOPULATION_SIZE) {
		testing_subpop++;
		if (testing_subpop >= NUMBER_OF_SUBPOPULATIONS) {
			testing_subpop = 0;
			//Evolve
			spectrumDiversityEvolve();
		}
        
		testing_individual = 0;
	}
}

ParameterType Unified_Neural_Model::stepBestAction(ParameterType *observation)
{
	//execute individual
	subpopulation[MAIN_SUBPOP][best_index]->process(observation, action);
	return 0;	
}

void Unified_Neural_Model::endBestEpisode()
{
	subpopulation[MAIN_SUBPOP][best_index]->clearMemory();		
}

void Unified_Neural_Model::spectrumDiversityEvolve()
{
	//update fitness
	unsigned int fcounter = 0;
    float avg_fitness = 0;
    float best_fitness = tmp_fitness[0][0];
	//int previous_best_index= best_index;
	best_index = 0;
	int best_number_of_neurons = subpopulation[0][0]->number_of_neurons;

	//find best individual	
	//printf("\n\n\nprevious best index %d %f\n", previous_best_index, fitness[0][previous_best_index]);
	for (int i = 0; i < NUMBER_OF_SUBPOPULATIONS; ++i) {
		for (int j = 0; j < SUBPOPULATION_SIZE; ++j) {
			fitness[i][j] = tmp_fitness[i][j];

			if (tmp_fitness[i][j] > best_fitness) {
				best_fitness = tmp_fitness[i][j];
				best_index = j;
				best_number_of_neurons = subpopulation[i][j]->number_of_neurons;
			}
			else {
				//if they have the same fitness only update if the new individual have the same or less number of neurons
				if (tmp_fitness[i][j] == best_fitness) {
					if (best_number_of_neurons >= subpopulation[i][j]->number_of_neurons) {
						best_fitness = tmp_fitness[i][j];
						best_index = j;
						best_number_of_neurons = subpopulation[i][j]->number_of_neurons;

					}
				}
			}
		
			//mark this subpopulation as without any individuals inserted
			//selected_individuals[i][j][0]=-1;
			
			//reset the fitness
			tmp_fitness[i][j] = 0.0;
	
			//printf("%d %f\n", j, fitness[i][j]);
			avg_fitness += fitness[i][j];
			fcounter++;
		}
	}
    
	avg_fitness /= fcounter;
    
	//decide the parents
	for(int i = 0; i < NUMBER_OF_SUBPOPULATIONS; ++i)
	{
		for(int j = 0; j < SUBPOPULATION_SIZE; ++j)
		{
			//calculate spectrum
			ParameterType spectrum[SPECTRUM_SIZE];

			calculateSpectrum(spectrum, i, j);

			//printArray(spectrum,SPECTRUM_SIZE);

			//insert in Novelty Map
			int index = nmap->input(spectrum);

			nmap_cell *cell = (nmap_cell *)(nmap->map[index]).pointer;
			float this_fitness = fitness[i][j];
			
			if (cell == NULL) {
				cell = (nmap_cell *)malloc(sizeof(nmap_cell));
				nmap->map[index].pointer = cell;
				cell->module = NULL;
			}	
			
			//check if nothing was inserted
			if (cell->module == NULL) {
				cell->module = subpopulation[i][j];
				cell->fitness = this_fitness;
			}	
			else	{
				if (cell->fitness < this_fitness) {
					cell->module = subpopulation[i][j];
					cell->fitness = this_fitness;
				}
				else	{
					//if they have the same fitness only updtate if the new individual have the same or less number of neurons
					if (cell->fitness == this_fitness) {
						if ((cell->module)->number_of_neurons >= subpopulation[i][j]->number_of_neurons) {
							cell->module = subpopulation[i][j];
							cell->fitness = this_fitness;
						}
					}
				}
			}
		}
	}
		
	nmap_cell *cell = (nmap_cell *)(nmap->map[0]).pointer;
	best_fitness = cell->fitness;
	best_index = 0;

	//copy parents (nmap's cells) to the population
	for (int i = 0; i < NOVELTY_MAP_SIZE; ++i) {
		cell = (nmap_cell *)(nmap->map[i]).pointer;
		
		if (cell->module != NULL) {
			if (cell->fitness > best_fitness) {
				best_fitness = cell->fitness;
				best_index = i;
			}
		}
		
		//printf("%d %p %p %p cell %d %d\n",i, tmp_subpopulation[0][i], cell->module, subpopulation[cell->i][cell->j], cell->i, cell->j);
		//prevent the algorithm from choosing empty cells (those cells appear when there is many cells with the same weight array)
		while(cell->module == NULL) {
			//printf("%d %p is null\n",i, cell->module);
			int roullete = random->uniform(0, NOVELTY_MAP_SIZE - 1);
			cell = (nmap_cell *)(nmap->map[roullete]).pointer;
			//printf("%d %p is null\n",i, cell->module);
		}

		tmp_subpopulation[0][i]->clone(cell->module);

		//tmp_subpopulation[0][i]->clone(subpopulation[cell->i][cell->j]);
	}

// 	verbose
	//printf("%d new index %d fitness %f avg_fitness %f\n", generation, best_index, best_fitness, avg_fitness);

	//reproduce
	for (int i = NOVELTY_MAP_SIZE; i < SUBPOPULATION_SIZE; ++i) {
		int random_individual = random->uniform(0, NOVELTY_MAP_SIZE - 1);
		cell = (nmap_cell *)(nmap->map[random_individual]).pointer;
		
		while (cell->module == NULL) {
			int roullete = random->uniform(0, NOVELTY_MAP_SIZE - 1);
			cell = (nmap_cell *)(nmap->map[roullete]).pointer;
		}

		//tmp_subpopulation[0][i]->clone(subpopulation[0][random_individual]);
		tmp_subpopulation[0][i]->clone(cell->module);

		//int number_of_mutations= random->uniform(1, NUMBER_OF_STEP_MUTATIONS);
		int number_of_mutations = NUMBER_OF_STEP_MUTATIONS;

		//structural mutation
		for (int k = 0; k < number_of_mutations; ++k) {
			tmp_subpopulation[0][i]->structuralMutation();
		}
        
		tmp_subpopulation[0][i]->updatePrimerList();

		//weight mutation
		tmp_subpopulation[0][i]->weightMutation();
	}
	
	//remove all modules inserted in the novelty map
	for (int i = 0; i < NOVELTY_MAP_SIZE; ++i) {	
		cell = (nmap_cell *)(nmap->map[i]).pointer;
		cell->module = NULL;
		cell->fitness = EXTREME_NEGATIVE_REWARD;
	}

	//swap temporary population with the original one
	for (int i = 0; i < NUMBER_OF_SUBPOPULATIONS; ++i) {
		for(int j = 0; j < SUBPOPULATION_SIZE; ++j) {
			Module *swap_individual = subpopulation[i][j];
			subpopulation[i][j] = tmp_subpopulation[i][j];
			tmp_subpopulation[i][j] = swap_individual;
		}
	}

	generation++;
}

// Create the Spectrum of the DNA
//
// | Identity | Sigmoid | Threshold | Random | Control | Slow |
void Unified_Neural_Model::calculateSpectrum(ParameterType *spectrum, int subpopulation_index, int individual_index)
{
	neuron *mn = subpopulation[subpopulation_index][individual_index]->n;

	//clear spectrum
	for (int i = 0; i < SPECTRUM_SIZE; ++i) {
		spectrum[i] = 0;
	}
	
	//execute all Control Neurons that are excited and not activated
	int counter = 0;
    
	for (int i = 0; mn[i].id >= 0; ++i) {
		switch (mn[i].type) {
			case IDENTITY: {
				spectrum[0]++;
			}
			break;
			case THRESHOLD: {
				spectrum[1]++;
			}
			break;
            case RANDOM: {
				spectrum[2]++;
			}
			break;
			case CONTROL: {
				spectrum[3]++;
			}
            break;
            case ACTIVATION: {
                spectrum[4]++;
            }
            break;
            default:
			break;
		}
		
		if (mn[i].firing_rate != LEVEL1) {
			spectrum[5]++;
		}

		counter++;
	}

#ifdef NORMALIZED_SPECTRUM_DIVERSITY
	for (int i = 0; i < SPECTRUM_SIZE; ++i) {
		spectrum[i] /= (ParameterType)counter;
	}
	//printArray(spectrum,SPECTRUM_SIZE);
#endif
}

float Unified_Neural_Model::subpopulationObjective(Module *module, float fitness, int subpopulation_index)
{
	switch(subpopulation_index) {
		case FITNESS_BASED: {
			return fitness;
		}
		break;
		case NEURON_EFFICIENT: {
			return fitness - module->number_of_neurons;
		}
		break;	
		case CONNECTION_EFFICIENT: {
			return fitness - module->number_of_connections;
		}
		break;	
		case NEURON_RICH: {
			return fitness + module->number_of_neurons;
		}
		break;	
		case CONNECTION_RICH: {
			return fitness + module->number_of_connections;
		}
		break;
	}

	printf("ERROR: subpopulation objective with unknown subpopulation index %d\n", subpopulation_index);
	exit(1);
	return 0;
}
	
void Unified_Neural_Model::printSubpop()
{
	printf("Fitness of the subpops\n");
	//for the time being, move the fittest individuals to a temporary subpopulation
	for (int i = 0; i < NUMBER_OF_SUBPOPULATIONS; ++i) {
		int j;
		for (j = 0; j < SUBPOPULATION_SIZE; ++j) {
			float objective = subpopulationObjective(subpopulation[i][j], fitness[i][j], i);
			float main_objective = subpopulationObjective(subpopulation[i][j], fitness[i][j], MAIN_SUBPOP);

			printf("%d:%d current objective %f main objective: %f\n",i,j, objective, main_objective);	
		}
	}
	
	printf("\nFitness of best individuals in each subpop\n");
	//for the time being, move the fittest individuals to a temporary subpopulation
	for(int i = 0; i < NUMBER_OF_SUBPOPULATIONS; ++i) {
		int j;
		//for(j=0;j<SUBPOPULATION_SIZE/2; ++j)
		//for(j=0;j<selected_individuals[i][j][0]>=0; ++j)
		j=0;
		{
			int subpopulation_index = selected_individuals[i][j][0];
			int individual_index = selected_individuals[i][j][1];
	
			float objective = subpopulationObjective(subpopulation[subpopulation_index][individual_index], fitness[subpopulation_index][individual_index], i);

			printf("%d:%d %f   %f\n",i,j, fitness[subpopulation_index][individual_index], objective);	
		}
	}
	
	printf("\nBest individual:\n");
	int subpopulation_index = selected_individuals[MAIN_SUBPOP][best_index][0];
	int individual_index = selected_individuals[MAIN_SUBPOP][best_index][1];
	float objective = subpopulationObjective(subpopulation[subpopulation_index][individual_index], fitness[subpopulation_index][individual_index], MAIN_SUBPOP);
	printf("%d  %d:%d %f\n", generation, MAIN_SUBPOP, best_index, objective);
	ModuleDebug::printGraph(subpopulation[subpopulation_index][individual_index], "best_graph.dot");
}

void Unified_Neural_Model::printBest()
{
	int subpopulation_index = selected_individuals[MAIN_SUBPOP][best_index][0];
	int individual_index = selected_individuals[MAIN_SUBPOP][best_index][1];
	float objective = subpopulationObjective(subpopulation[subpopulation_index][individual_index], fitness[subpopulation_index][individual_index], MAIN_SUBPOP);
	printf("%d %f\n", generation, objective);
}

void Unified_Neural_Model::print()
{
    ModuleDebug::printGraph(subpopulation[MAIN_SUBPOP][best_index], "best_individual.dot");
}
