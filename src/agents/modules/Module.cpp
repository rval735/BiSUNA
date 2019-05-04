
#include"Module.h"
#include "ModuleDebug.h"

Random *Module::random = NULL;

// the dna_allocated_length is for both n and c, therefore its value is equivalent to the longest length
Module::Module(int number_of_inputs, int number_of_outputs, neuron *n, connection *c, int dna_allocated_length)
{
	this->n = n;
	this->c = c;
	this->allocated_space = dna_allocated_length;

	//if random was not set, return error
	if (random == NULL) {
		printf("Static Random variable not set!!!!\nPlayer take 5 damage =P!\n");
		exit(1);
	}

	//default max neuron id
	max_neuron_id = 0;
	number_of_neurons = 0;
	number_of_connections = 0;

	//find the max neuron id
	for (int i = 0; n[i].id >= 0; ++i) {
		if (n[i].id > max_neuron_id) {
			max_neuron_id = n[i].id;
		}
        
		number_of_neurons++;
	}
	
	//find the maximum of connections
	for (int i = 0; c[i].from_neuron_id >= 0; ++i) {
		number_of_connections++;
	}

	//printf("allocated space %d neurons %d connections %d\n",allocated_space, number_of_neurons, number_of_connections);

	if (allocated_space <= max_neuron_id + 1 || allocated_space < number_of_connections) {
		printf("Starting allocated dna's length was incorrect\n");
		printf("Pay more attention next time, kid\n");
		exit(1);
	}
	
	//states
	neuron_state = (ParameterType *)calloc(allocated_space, sizeof(ParameterType));
	previous_neuron_state = (ParameterType *)calloc(allocated_space, sizeof(ParameterType));
	internal_neuron_state = (ParameterType *)calloc(allocated_space, sizeof(ParameterType));
	neuron_excitation = (ParameterType *)calloc(allocated_space, sizeof(ParameterType));
	is_fired = (bool *)calloc(allocated_space, sizeof(bool));
	connection_state = (ConnectionTypes *)calloc(allocated_space, sizeof(ConnectionTypes));
	previous_connection_state = (ConnectionTypes *)calloc(allocated_space, sizeof(ConnectionTypes));
	
	//ready to use information
	primer_list = (int *)malloc(sizeof(int) * allocated_space);
	
	updatePrimerList();
	
	//set input and output neurons
	createFixedInterfaceNeurons(number_of_inputs, number_of_outputs);
}

// the dna_allocated_length is for both n and c, therefore its value is equivalent to the longest length
Module::Module(int number_of_inputs, int number_of_outputs, int suggested_allocation_length)
{
	this->allocated_space = suggested_allocation_length;
	
	this->n = (neuron *)malloc(allocated_space * sizeof(neuron));
	this->c = (connection *)malloc(allocated_space * sizeof(connection));

	//set the both DNA arrays as empty
	n[0].id = -1;
	c[0].from_neuron_id = -1;

	//default max neuron id
	max_neuron_id = -1;
	number_of_neurons = 0;
	number_of_connections = 0;
	
	//states
	neuron_state = (ParameterType *)calloc(allocated_space, sizeof(ParameterType));
	previous_neuron_state = (ParameterType *)calloc(allocated_space, sizeof(ParameterType));
	internal_neuron_state = (ParameterType *)calloc(allocated_space, sizeof(ParameterType));
	neuron_excitation = (ParameterType *)calloc(allocated_space, sizeof(ParameterType));
	is_fired = (bool *)calloc(allocated_space, sizeof(bool));
	connection_state = (ConnectionTypes *)calloc(allocated_space, sizeof(ConnectionTypes));
	previous_connection_state = (ConnectionTypes *)calloc(allocated_space, sizeof(ConnectionTypes));
	
	//ready to use information
	primer_list = (int *)malloc(sizeof(int) * allocated_space);
	
	//create the Primer list
	//not all Control Neurons activate, the Primers are the ones that always activate
	number_of_primers = 0;
	//set the end of the primer list
	primer_list[number_of_primers] = -1;
	
	//set input and output neurons
	createFixedInterfaceNeurons(number_of_inputs, number_of_outputs);
}

Module::~Module()
{
    free(n);
    free(c);
    free(neuron_state);
    free(previous_neuron_state);
    free(internal_neuron_state);
    free(neuron_excitation);
    free(is_fired);
    free(connection_state);
    free(previous_connection_state);
    free(primer_list);
}

void Module::saveDNA(const char *filename)
{
	writeDNA(filename, &n, number_of_neurons, &c, number_of_connections);
}

void Module::loadDNA(const char *filename)
{
	readDNA(filename, &n, number_of_neurons, &c, number_of_connections);
	
	//find the max neuron id
	max_neuron_id = -1;
	number_of_neurons = 0;
	for (int i = 0 ; n[i].id >= 0; ++i) {
		if (n[i].id > max_neuron_id) {
			max_neuron_id = n[i].id;
		}
        
		number_of_neurons++;
	}
	
	//find the maximum number of connections
	number_of_connections = 0;
	for (int i = 0; c[i].from_neuron_id >= 0; ++i) {
		number_of_connections++;
	}
	
	updatePrimerList();
}

void Module::clone(Module *brother)
{
	//check the allocated space
	if (allocated_space < brother->allocated_space) {
		reallocEverything(brother->allocated_space);	
	}

	max_neuron_id = brother->max_neuron_id;
	number_of_neurons = brother->number_of_neurons;
	number_of_connections = brother->number_of_connections;
	number_of_primers = brother->number_of_primers;

	//clone the DNA
	memcpy(this->n, brother->n, brother->allocated_space * sizeof(neuron));
	memcpy(this->c, brother->c, brother->allocated_space * sizeof(connection));
	
	//clone related information
	memcpy(this->primer_list, brother->primer_list, brother->allocated_space * sizeof(int));
	
	//erase all information from previous runs
	//that still remains on the network 
	clearMemory();
}

///////////////////////////////////////////////////////////////
//	Weight mutation:
//		- modify weight 
//		(for now the neuromodulation connections are ignored)
///////////////////////////////////////////////////////////////
void Module::weightMutation()
{
	float roulette = random->uniform(0.0, 1.0);
	if (roulette < CHANCE_OF_WEIGHT_MUTATION) {
		for (int i = 0; c[i].from_neuron_id >= 0; ++i) {
            //only modify connection without neuromodulation
			if (c[i].neuro_modulation < 0) {
                ParameterType cw = c[i].weight;
#ifdef CONTINUOUS_PARAM
                ParameterType variance = WEIGHT_MUTATION_CHANGE_PERCENTAGE * cw;
                ParameterType perturbation = random->uniform(-variance, variance);
                c[i].weight += perturbation;
#else
                ParameterType varianceP = cw << WEIGHT_MUTATION_CHANGE_PERCENTAGE;
                ParameterType varianceN = cw >> WEIGHT_MUTATION_CHANGE_PERCENTAGE;
                ParameterType perturbation = random->uniform(varianceN, varianceP);
                c[i].weight = perturbation | cw;
#endif
                //printf("variance %f pert %f\n",variance, perturbation);
                if (c[i].weight > MAXIMUM_WEIGHT  || isinf(c[i].weight)) {
                    c[i].weight = MAXIMUM_WEIGHT;
                }

#ifdef CONTINUOUS_PARAM
                if (c[i].weight < -MAXIMUM_WEIGHT) {
                    c[i].weight = -MAXIMUM_WEIGHT;
                }
#endif
			}
		}
	}
}

///////////////////////////////////////////////////////////////
//	There are 3 types of structural mutation:
//		- topologic
//			-add/remove neuron
//			-add/remove connection 
//			!!!!!!!Important: after removing a connection, a non-primer might become a primer and vice-versa!!!!
//			Threfore, always do a updatePrimerList() after structural Mutation
///////////////////////////////////////////////////////////////
void Module::structuralMutation()
{
	double mutation_chance[] = MUTATION_PROBABILITIES;

	double roulette = random->uniform(0.0, 1.0); 

	int mutation_type;

	double sum = mutation_chance[0];
	
    if (roulette < sum) {
		mutation_type = 1;
	}
	else {
		sum += mutation_chance[1];
		
        if (roulette < sum) {
			mutation_type = 2;
		}
		else {
			sum += mutation_chance[2];
			
            if (roulette < sum) {
				mutation_type = 3;
			}
			else {
				mutation_type = 4;
			}
		}
	}

	switch(mutation_type) {
		//add neuron
		case 1: {
			//find an available index
			int new_index = number_of_neurons;
			number_of_neurons++;
			
			//find an available id
			int new_id = smallestFreeId();
			
			//printf("add neuron id %d\n",new_id);
			
			//update max_neuron_id
			if (max_neuron_id < new_id) {
				max_neuron_id = new_id;	
			}

			//check the allocated space
			if (allocated_space <= number_of_neurons + 1) {
				reallocEverything();
			}

			//chance of being a Control Neuron
			if (random->uniform(0.0,1.0) < CHANCE_OF_CONTROL_NEURON) {
				//create a Control Neuron
				n[new_index].type = CONTROL;
			}
			else {
				//create a random neuron
                int nont = static_cast<int>(NUMBER_OF_NEURON_TYPES);
                int val = random->uniform(0, nont - 1);
				n[new_index].type = static_cast<NeuronTypes>(val);
			}

			//create a random neuron
			n[new_index].id = new_id;
			n[new_index].firing_rate = randomFiringRateLevel(random);
			n[number_of_neurons].id = -1;

			//if it is a Control Neuron then it must be a Primer (there are no connections to this neuron now)
			if (n[new_index].type == CONTROL) {
				primer_list[number_of_primers] = new_id;
				number_of_primers++;
				primer_list[number_of_primers] = -1;
			}

			connectNewNeuronToNetwork(new_id);
		}
		break;
		//remove neuron
		case 2: {
			int delete_index = random->uniform(0, number_of_neurons - 1);
			int delete_id = n[delete_index].id;
			
			//printf("remove neuron id %d\n",delete_id);

			if (delete_id == -1) {
				//printf("No neuron to delete\n");
				return;
			}
			
			//dont remove input and output neurons
			if (n[delete_index].type == INPUT_IDENTITY ||
					n[delete_index].type == OUTPUT_IDENTITY) {
				return;
			}

			//remove from primer_list
			if (n[delete_index].type == CONTROL) {
				for (int i = 0; primer_list[i] >= 0; ++i) {
					if (primer_list[i] == delete_id) {
						primer_list[i] = primer_list[number_of_primers - 1];
						number_of_primers--;
						primer_list[number_of_primers] = -1;
					}
				}		
			}

			//deleting
			n[delete_index].id = n[number_of_neurons - 1].id;
			n[delete_index].type = n[number_of_neurons - 1].type;
			n[delete_index].firing_rate = n[number_of_neurons - 1].firing_rate;
			previous_neuron_state[delete_index] = previous_neuron_state[number_of_neurons - 1];
			internal_neuron_state[delete_index] = internal_neuron_state[number_of_neurons - 1];
			
			number_of_neurons--;
			n[number_of_neurons].id = -1;

			bool found = false;
			//remove all connections to this or from this neuron
			do {
				found = false;
                
				for (int i = 0; c[i].from_neuron_id >= 0 && found == false; ++i) {
					if (c[i].from_neuron_id == delete_id || c[i].to_neuron_id == delete_id || c[i].neuro_modulation == delete_id) {
						//printf("remove connection to neuron\n");
						removeConnection(i);
						found = true;
					}
				}

				//printf("number of connections %d\n",number_of_connections);
			} while (found == true);
			//printf("left\n");
		}
		break;
		//add connection
		case 3: {
			//printf("add connection\n");
			if (allocated_space <= number_of_connections + 1) {
				reallocEverything();
			}

			//if there is no neurons, return
			if (n[0].id == -1) {
				//printf("no neurons\n");
				return;	
			}

			//create a random connection
			int random_index = random->uniform(0, number_of_neurons - 1);
			c[number_of_connections].from_neuron_id = n[random_index].id;
			
			random_index = random->uniform(0, number_of_neurons - 1);
			c[number_of_connections].to_neuron_id = n[random_index].id;

            ParameterType result;
            
			if (random->uniform(0.0, 1.0) < CHANCE_OF_NEUROMODULATION) {
				//int random_index= random->uniform(0,1);
				int random_index = random->uniform(0, number_of_neurons - 1);
				//with modulation
				c[number_of_connections].neuro_modulation = n[random_index].id;
                result = 1;
			}
			else {
				int random_index = random->uniform(0, 1);
				c[number_of_connections].neuro_modulation = -1;
				if (random_index == 0) {
					result = 1;
				}
				else {
                    result = -1;
				}
			}

            c[number_of_connections].weight = result;

			number_of_connections++;
			c[number_of_connections].from_neuron_id = -1;
		}
		break;
		//remove connection
		case 4: {
            if (number_of_connections == 0) {
                return;
            }
            
            int delete_index = random->uniform(0, number_of_connections - 1);

			//printf("remove connection\n");
			removeConnection(delete_index);
		}
		break;
	}
}

//
// States: activated/deactivated, excited/inhibited, called/uncalled
//
// Sequence of activations: 
// 	
// 	example: [condition] type of neuron
//
//	**Control neurons has an activation bias of +1
//
// - Primers (Control Neurons (CN) that does not receive input from CN)
//
// - [excited and deactivated] CN  
//
// - [excited] Input Neurons
//
// - [excited and deactivated and called] Neurons
//
void Module::process(ParameterType *input, ParameterType *output)
{
	this->input = input;
	this->output = output;
	
	///////////////////////////// Excited Input Neurons //////////////////////////
	
	processInputNeurons();

	///////////////////////////// Primers //////////////////////////

	processPrimers();
	
	///////////////////////////// Excited Control Neurons //////////////////////////
		
	processControlNeurons();
	
	///////////////////////////// Excited & Called Neurons //////////////////////////
	
	processRemainingNeurons();
	
	///////////////////////////// Cleaning Phase //////////////////////////

	//printFiredStates();
	//printInternalStates();
		
	//printInformationFlowGraph("graph.dot");

	//Clean Information:
	//
	//fire - fire clean the fire information
	//state - change neuron_state to previous neuron state
	//excitation - clear excitation
	
	for (int i = 0; i < number_of_neurons; ++i) {
		is_fired[i] = false;
		
        if (isnan(previous_neuron_state[i]) || isinf(previous_neuron_state[i])) {
			printf("previous neuron state is inf or nan %d\n", i);
		}
		
        if (isnan(neuron_state[i]) || isinf(neuron_state[i])) {
			printf("neuron state is inf or nan %d\n", i);
		}
        
        ParameterType ns = neuron_state[i];
		previous_neuron_state[i] = ns;
		neuron_state[i] = 0.0;
		neuron_excitation[i] = 0.0;
	}
	
	for (int i = 0; i < number_of_connections; ++i) {
		previous_connection_state[i] = connection_state[i];
		connection_state[i] = Recurrent;
	}
}

//clear all memory information that still remains on the neurons
void Module::clearMemory()
{
	for (int i = 0; i < number_of_neurons; ++i) {
		is_fired[i] = false;
		previous_neuron_state[i] = 0.0;
		neuron_state[i] = 0.0;
		internal_neuron_state[i] = 0.0;
		neuron_excitation[i] = 0.0;
	}	
	
	for (int i = 0; i < number_of_connections; ++i) {
		previous_connection_state[i] = Recurrent;
		connection_state[i] = Recurrent;
	}
}

void Module::processInputNeurons()
{
    std::stack<int> active_neurons;
    
    //execute all Input Neurons that are excited (they are all not activated yet so no check is needed)
    for (int i = 0; n[i].id >= 0; ++i) {
        if (n[i].type == INPUT_IDENTITY) {
            execute(i);
            active_neurons.push(i);
        }
    }
    
    //return if no excited input neuron was found
    if (active_neurons.empty()) {
        return;
    }
    
    //set all Input Neurons as already activated and fire them
    while (!active_neurons.empty()) {
        //int id= active_neuron;
        int index = active_neurons.top();
        
        //remove the top of the stack
        active_neurons.pop();
        
        //mark as fired
        is_fired[index] = true;
    }
}

void Module::processPrimers() {
    //execute all Primers
    for (int i = 0; primer_list[i] != -1; ++i) {
        int id = primer_list[i];
        int index = neuronIdToDNAIndex(id);
        execute(index);
    }
    
    //set all Primer as already activated and fire them
    for (int i = 0; primer_list[i] != -1; ++i) {
        int id = primer_list[i];
        int index = neuronIdToDNAIndex(id);
        //mark as fired
        is_fired[index] = true;
    }
    
    //Excite/Inhibit other neurons
    for (int i = 0; primer_list[i] != -1; ++i) {
        int control_id = primer_list[i];
        int control_index = neuronIdToDNAIndex(control_id);
        
        //check the connections from this neuron
        for (int j = 0; c[j].from_neuron_id >= 0; ++j) {
            //skip if it is not a connection from this Neuron
            if (c[j].from_neuron_id != control_id) {
                continue;
            }
            
            int destination = c[j].to_neuron_id;
            int destination_index = neuronIdToDNAIndex(destination);
            int modulator = c[j].neuro_modulation;
            ParameterType ns = neuron_state[control_index];
            ParameterType cw = c[j].weight;
            //no neuro modulation
            if (modulator < 0.0) {
#ifdef CONTINUOUS_PARAM
                neuron_excitation[destination_index] += cw * ns;
#else
//                ParameterType ne = neuron_excitation[destination_index];
//                neuron_excitation[destination_index] = xnor(cw, ns) ^ ne;
//                neuron_excitation[destination_index] += cw * ns;
                neuron_excitation[destination_index] |= cw & ns;
#endif
            }
            //with neuro modulation
            else {
                int modulator_index = neuronIdToDNAIndex(modulator);
                
                ParameterType modulator_input = 0;
                ParameterType neM = neuron_excitation[modulator_index];
                //check if it is excited
#ifdef CONTINUOUS_PARAM
                if (neM >= EXCITATION_THRESHOLD) {
#else
                if (neM < EXCITATION_THRESHOLD) {
#endif
                    //modulator already activated
                    if (is_fired[modulator_index] == true) {
                        modulator_input = neuron_state[modulator_index];
                    }
                    //modulator not already activated, use the state from the previous iteration
                    else {
                        modulator_input = previous_neuron_state[modulator_index];
                    }
                }
                else {
                    modulator_input = 0.0;
                }
                
#ifdef CONTINUOUS_PARAM
                neuron_excitation[destination_index] += modulator_input * ns;
#else
                ParameterType neD = neuron_excitation[destination_index];
//                neuron_excitation[destination_index] = xnor(modulator_input, ns) | neD;
                neuron_excitation[destination_index] = (modulator_input & ns) | neD;
#endif
            }
        }
    }
}

void Module::processControlNeurons()
{
	//the loop only stops when:
	//	-no Control Neuron that is excited and was not activated is found 
	for (;;)
	{
		std::stack<int> active_neurons;
		std::stack<int> active_neurons_copy;
	
		//execute all Control Neurons that are excited and not activated
		for (int i = 0; n[i].id >= 0; ++i) {
            NeuronTypes type = n[i].type;
            ParameterType ne = neuron_excitation[i];
            bool fired = is_fired[i];
#ifdef CONTINUOUS_PARAM
            if (type == CONTROL && ne >= EXCITATION_THRESHOLD && fired == false) {
#else
            if (type == CONTROL && ne < EXCITATION_THRESHOLD && fired == false) {
#endif
				execute(i);
				active_neurons.push(i);
			}
		}
		
		//stop loop if no excited and not activated control neuron was found
		if (active_neurons.empty()) {
			return;
		}

		//set all Control Neurons as already activated and fire them
		while (!active_neurons.empty()) {
			//int id= active_neuron;
			int index = active_neurons.top();
			
			//remove the top of the stack & add this in the copy of this stack
			active_neurons.pop();
			active_neurons_copy.push(index);
			
			//mark as fired
			is_fired[index] = true;
		}
	
		//Excite/Inhibit other neurons 
		while (!active_neurons_copy.empty()) {
			int control_index = active_neurons_copy.top();
			int control_id = n[control_index].id;
			//pop
			active_neurons_copy.pop();
			
			//check the connections from this neuron
			for (int j = 0; c[j].from_neuron_id >= 0; ++j) {
				//skip if it is not a connection from this Neuron
				if (c[j].from_neuron_id != control_id) {
					continue;
				}

				int destination = c[j].to_neuron_id;

				int destination_index = neuronIdToDNAIndex(destination);
                ParameterType cw = c[j].weight;
                ParameterType ns = neuron_state[control_index];

				//no neuro modulation
				if (c[j].neuro_modulation < 0.0) {
#ifdef CONTINUOUS_PARAM
                    neuron_excitation[destination_index] += cw * ns;
#else
//                    neuron_excitation[destination_index] += cw * ns;
                    neuron_excitation[destination_index] |= cw & ns;
//                    ParameterType ne = neuron_excitation[destination_index];
//                    neuron_excitation[destination_index] = ~(~ns << 1) | ne;
#endif
				}
				//with neuro modulation
				else {
					int modulator = c[j].neuro_modulation;
					int modulator_index = neuronIdToDNAIndex(modulator);
					ParameterType modulator_input = 0;
					
					//check if it is excited
                    ParameterType neM = neuron_excitation[modulator_index];
#ifdef CONTINUOUS_PARAM
					if (neM >= EXCITATION_THRESHOLD) {
#else
                    if (neM < EXCITATION_THRESHOLD) {
#endif
						//modulator already activated
						if (is_fired[modulator_index] == true) {
							modulator_input = neuron_state[modulator_index];
						}
						//modulator not already activated, use the state from the previous iteration
						else {
							modulator_input = previous_neuron_state[modulator_index];
						}
					}
#ifdef CONTINUOUS_PARAM
                    neuron_excitation[destination_index] += modulator_input * ns;
#else
                    ParameterType neD = neuron_excitation[destination_index];
//                    neuron_excitation[destination_index] = (ns ^ modulator_input) | ne;
//                    neuron_excitation[destination_index] = xnor(modulator_input, ns) | neD;
                    neuron_excitation[destination_index] = (modulator_input & ns) | neD;
#endif
				}
			}
		}
	}
}

void Module::processRemainingNeurons()
{
	std::stack<int> output_neurons_stack;

	std::list<int> remaining_neurons_list;

	//build the list of remaining neurons
	for (int i = 0; n[i].id >= 0; ++i) {
        NeuronTypes type = n[i].type;
        ParameterType ne = neuron_excitation[i];
        bool fired = is_fired[i];
#ifdef CONTINUOUS_PARAM
        if (type != CONTROL && ne >= EXCITATION_THRESHOLD && fired == false) {
#else
        if (type != CONTROL && ne < EXCITATION_THRESHOLD && fired == false) {
#endif
			remaining_neurons_list.push_back(i);
		}
		else if (n[i].type == OUTPUT_IDENTITY) {
            output_neurons_stack.push(i);
        }
	}

	//the loop only stops when:
	//	-no Remaining Neuron, that was excited and not activated, is found 
	for (;;) {
		std::stack<int> active_neurons;
	
		std::list<int>::iterator it;

		//printf("list of firing neurons\n");
		for (it = remaining_neurons_list.begin(); it != remaining_neurons_list.end(); ++it)
		{	
			int index= *it;
			ParameterType output = execute(index, true);
#ifdef CONTINUOUS_PARAM
            if (output > REMAINING_NEURON_THRESHOLD || output < -REMAINING_NEURON_THRESHOLD) {
                active_neurons.push(index);
            }
#else
            // TODO!! Check how this value can be adapted to Binary
            if (output >= REMAINING_NEURON_THRESHOLD) {
                active_neurons.push(index);
            }
#endif
		}
		//printf("end list of firing neurons\n\n");
		
		//stop loop if no excited and not activated control neuron was found
		if (active_neurons.empty()) {
			break;
		}
	
		//set current active Neurons as already activated and fire them
		while (!active_neurons.empty()) {
			int index = active_neurons.top();
			
			//remove the top of the stack
			active_neurons.pop();
			
			//mark as fired
			is_fired[index] = true;

			//remove activated neuron from the list of remaining neurons
			remaining_neurons_list.remove(index);	
		}
    }
	//printf("LLLLLLLLLLLLLLLLLL\n");
	
	//activate unactivated Output Neurons 
	// - In other words, output neurons should be activated anyway
	while (!output_neurons_stack.empty()) {
		int index = output_neurons_stack.top();

		output_neurons_stack.pop();

		if (is_fired[index] == false) {
			//NEXT LINE, MARKED FOR EXCLUDING (exclude the internal_neuron_state[index]+= part only)
			//internal_neuron_state[index]+= execute(index,true);
			execute(index,true);
		
			is_fired[index] = true;
		}
	}
}

// Basically, do:
//
//	w_i - weights
//	I_i - inputs
//
//	f( \sum w_i * I_i )  
//	
//	(Neuron0)---I0---w0--
//	         	    |
//	(Neuron1)---I1---w1---(This Neuron)-----
//	                    |
//	(Neuron2)---I2---w2--
//
//	Ignore Input I_i if:
//		-Neuron_i is a control neuron	
//	
//	IMPORTANT: this function returns this neuron's firing signal
//		   if the neuron is going to fire or not this signal is left undecided
//
ParameterType Module::execute(int neuron_index, bool ignore_if_only_recurrent)
{
	std::stack<connection_type> connections_used;
    ParameterType sum = 0;
	int id = n[neuron_index].id;
    bool only_recurrent_inputs = true;
	
	//check the connections to this neuron
	for (int j = 0; c[j].from_neuron_id >= 0; ++j) {
		//skip if it is not a connection to this Neuron
		if (c[j].to_neuron_id != id) {
			continue;
		}

		int source = c[j].from_neuron_id;

		int source_index = neuronIdToDNAIndex(source);

		//add this input multiplied by the weight of the connection
		//IGNORE if it comes from a Control Neuron 
		if (n[source_index].type != CONTROL) {
			//no neuro modulation
			if (c[j].neuro_modulation < 0) {
                
                ParameterType ns = neuron_state[source_index];
                ParameterType cw = c[j].weight;

				//check if they were already activated
				if (is_fired[source_index] == true) {
#ifdef CONTINUOUS_PARAM
                    sum += cw * ns;
#else
                    sum |= cw & ns;
// Shift right one with the value of CW
// ~(~x << n)
// https://www.quora.com/In-C-is-it-possible-to-perform-a-bitwise-left-shift-that-introduces-trailing-1s-rather-than-trailing-0s
//                        sum = ~(~sum << 1);
//                        sum = xnor(sum, ns);
#endif
					
					only_recurrent_inputs = false;
					
					connection_type cnn;
					cnn.index = j;
					cnn.cType = FeedForward;

					//marking the recurrent connection as used to be processed later on
					//because we still do not know if this neuron will fire 
					connections_used.push(cnn);
					
					if (isnan(sum) || isinf(sum)) {
#ifdef CONTINUOUS_PARAM
                        printf("fired %f %f\n", c[j].weight, neuron_state[source_index]);
#else
						printf("fired %i %i\n", c[j].weight, neuron_state[source_index]);
#endif
					}
				}
				//recurrent connection
				//case it was not activated yet, use the state from the previous iteration
				else {
                    ParameterType pn = previous_neuron_state[source_index];
                    ParameterType cw = c[j].weight;
                    
					if (previous_connection_state[j] != FeedForward) {
#ifdef CONTINUOUS_PARAM
                        sum += cw * pn;
#else
                        sum |= cw & pn;
                            // Shift right one with the value of CW
                            // ~(~x << n)
//                            sum = ~(~sum << 1);
//                            sum = xnor(sum, pn);
#endif

						connection_type cnn;
						cnn.index = j;
						cnn.cType = Recurrent;

						//marking the recurrent connection as used to be processed later on
						//because we still do not know if this neuron will fire 
						connections_used.push(cnn);
						
						if (isnan(sum) || isinf(sum)) {
#ifdef CONTINUOUS_PARAM
                            printf("not fired %f %f\n", c[j].weight, previous_neuron_state[source_index]);
#else
                            printf("not fired %i %is\n", c[j].weight, previous_neuron_state[source_index]);
#endif
						}
					}
				}
				
				if (isnan(sum) || isinf(sum)) {
					printf("no neuromodulation \n");
				}
			}
			//with neuron modulation	
			else {
				///////////////////////////////////////////////////////////////
				//	
				//	Neuron Modulation
				//
				//
				//	----(Modulator Neuron)----
				//				 |
				//	----(Source Neuron)---------------(This Neuron)---------			 
				//
				///////////////////////////////////////////////////////////////
				
				/////////////// modulator neuron /////////////

				int modulator = c[j].neuro_modulation;
				int modulator_index = neuronIdToDNAIndex(modulator);
				
				ParameterType modulator_input = 0;
				ParameterType source_input = 0;
                ParameterType ne = neuron_excitation[modulator_index];
				//check if it is excited
#ifdef CONTINUOUS_PARAM
                if (ne >= EXCITATION_THRESHOLD) {
#else
                if (ne < EXCITATION_THRESHOLD) {
#endif
					//modulator already activated
					if (is_fired[modulator_index] == true) {
						modulator_input = neuron_state[modulator_index];
					}
					//modulator not already activated, use the state from the previous iteration
					else {
						modulator_input = previous_neuron_state[modulator_index];
					}
				}	
				else {
					modulator_input = 0;
				}
				
				/////////////// source neuron ////////////

				//source already activated
				if (is_fired[source_index] == true) {
					source_input = neuron_state[source_index];
					connection_type cnn;
					cnn.index = j;
					cnn.cType = FeedForward;

					//marking the recurrent connection as used to be processed later on
					//because we still do not know if this neuron will fire 
					connections_used.push(cnn);
					
					if (isnan(source_input) || isinf(source_input)) {
#ifdef CONTINUOUS_PARAM
                        printf("fired %f\n",neuron_state[source_index]);
#else
                        printf("fired %i\n",neuron_state[source_index]);
#endif
					}
				}
				//recurrent connection
				//source not already activated, use the state from the previous iteration
				else {
					if (previous_connection_state[source_index] != FeedForward) {
						source_input = previous_neuron_state[source_index];
						connection_type cnn;
						cnn.index = j;
						cnn.cType = Recurrent;

						//marking the recurrent connection as used to be processed later on
						//because we still do not know if this neuron will fire 
						connections_used.push(cnn);
					
						if (isnan(source_input) || isinf(source_input)) {
#ifdef CONTINUOUS_PARAM
                            printf("not fired f %f\n", previous_neuron_state[source_index]);
#else
                            printf("Previous not fired %i\n", previous_neuron_state[source_index]);
#endif
						}
					}
						
					if (isnan(sum) || isinf(sum)) {
						printf("not fired nnn\n");
					}
				}

#ifdef CONTINUOUS_PARAM
                sum += modulator_input * source_input;
#else
                sum |= modulator_input & source_input;
                // TODO!! Check how this behaves
//                sum = (modulator_input ^ source_input) | sum;
#endif
                
				if (isnan(sum) || isinf(sum)) {
#ifdef CONTINUOUS_PARAM
                    printf("neuromodulation modulator %f source %f\n", modulator_input, source_input);
#else
                    printf("neuromodulation modulator %i source %i\n", modulator_input, source_input);
#endif
				}
			}
		}	
	}

	//if it is an Input Neuron
	//sum the input
	if (n[neuron_index].type == INPUT_IDENTITY) {
        int input_index = n[neuron_index].interface_index;
        ParameterType inputVal = input[input_index];
#ifdef CONTINUOUS_PARAM
        sum += inputVal;
#else
        sum += inputVal;
//        sum |= inputVal;
#endif
	}
	
	if (isnan(sum) || isinf(sum)) {
		printf("sum is nan, id %d\n",id);
        ModuleDebug::printGraph(this, "problem.dot");
		exit(1);
	}
	
#ifdef CONTINUOUS_PARAM
    if (sum < -MAX_NEURON_OUTPUT || sum > MAX_NEURON_OUTPUT) {
        sum = MAX_NEURON_OUTPUT;
        //printf("neuron_state!! nan, id %d sum %f internal_neuron_state %f\n",id, sum, internal_neuron_state[neuron_index]);
    }
#else
    if (sum > MAX_NEURON_OUTPUT) {
        sum = MAX_NEURON_OUTPUT;
    }
#endif
    
    ParameterType neuron_output = activationFunction(n[neuron_index].type, sum, random);
	
	//In the case that the ignore_if_only_recurrent flag is set,
	//the neuron does not activate if it only has recurrent input
	//this is useful for normal "remaining neurons" (neurons that are not control, not input and not output neurons)
	if (only_recurrent_inputs == true && ignore_if_only_recurrent == true) {
		neuron_output = 0;
	}

	if (isnan(neuron_output) || isinf(neuron_output)) {
#ifdef CONTINUOUS_PARAM
        printf("neuron_state!! nan, id %d sum %f neuron output %f internal_neuron_state %f\n", id, sum, neuron_output, internal_neuron_state[neuron_index]);
#else
        printf("neuron_state!! nan, id %d sum %i neuron output %i internal_neuron_state %i\n", id, sum, neuron_output, internal_neuron_state[neuron_index]);
#endif
	}
    
	//if it is an Output Neuron
	//set the output to the neuron_output
	if (n[neuron_index].type == OUTPUT_IDENTITY) {
		int output_index = n[neuron_index].interface_index;
		output[output_index] = neuron_output;
	}

	if (only_recurrent_inputs == true && ignore_if_only_recurrent == true) {
		return 0;
	}
	else {
		//mark all feedback connections
		for (; !connections_used.empty(); ) {
			connection_type cnn = connections_used.top();
			connections_used.pop();
			connection_state[cnn.index] = cnn.cType;
		}
	}
    
    FiringRate fr = n[neuron_index].firing_rate;
    ParameterType in = internal_neuron_state[neuron_index];
    ParameterType beta = betaFromFR(fr);
	
#ifdef CONTINUOUS_PARAM
    //use Windrow Hoff for slower neurons
    //printf("before beta %f, internal_neuron_state %f neuron_state %f\n",beta, internal_neuron_state[neuron_index],neuron_state[neuron_index]);
    internal_neuron_state[neuron_index] = in + beta * (neuron_output - in);
#else
//    internal_neuron_state[neuron_index] = in + ((neuron_output - in) >> fr);
    internal_neuron_state[neuron_index] = (in ^ (beta & xnor(neuron_output, in)));
//    internal_neuron_state[neuron_index] = in + beta * (neuron_output - in);
#endif

	neuron_state[neuron_index] = internal_neuron_state[neuron_index];
	
	if (isnan(neuron_state[neuron_index]) || isinf(neuron_state[neuron_index])) {
#ifdef CONTINUOUS_PARAM
        printf("neuron_state!! nan, id %d beta %f neuron output %f internal_neuron_state %f\n", id, beta, neuron_output, internal_neuron_state[neuron_index]);
#else
        printf("neuron_state!! nan, id %d beta %i neuron output %i internal_neuron_state %i\n", id, fr, neuron_output, internal_neuron_state[neuron_index]);
#endif
	}

	//printf("beta %f, internal_neuron_state %f neuron_state %f\n",beta, internal_neuron_state[neuron_index],neuron_state[neuron_index]);

	//printf("execute ");
	//printNeuronType(n[neuron_index].type);	
	//printf("%d: %f\n", n[neuron_index].id, neuron_state[neuron_index]);
	
	return neuron_output;
}

void Module::removeConnection(int index)
{
	if (c[index].from_neuron_id == -1) {
		printf("No connections to delete\n");
		return;
	}

	//moving
	c[index].from_neuron_id = c[number_of_connections - 1].from_neuron_id;
	c[index].to_neuron_id = c[number_of_connections - 1].to_neuron_id;
	c[index].neuro_modulation = c[number_of_connections - 1].neuro_modulation;
	c[index].weight = c[number_of_connections - 1].weight;

	//deleting and decreasing the size
	number_of_connections--;
	c[number_of_connections].from_neuron_id = -1;
}

void Module::setRandom(Random *random)
{
	Module::random = random;
}

void Module::reallocEverything()
{
	//double the previous allocated space
    reallocEverything(2 * allocated_space);
}

void Module::reallocEverything(int given_allocated_space)
{
	//double the previous allocated space
	int previous_space = allocated_space;
	allocated_space = given_allocated_space;

	//realloc everything
	n = (neuron *)realloc(n, allocated_space * sizeof(neuron));
	c = (connection *)realloc(c, allocated_space * sizeof(connection));
	neuron_state = (ParameterType *)realloc(neuron_state, allocated_space * sizeof(ParameterType));
	previous_neuron_state = (ParameterType *)realloc(previous_neuron_state, allocated_space * sizeof(ParameterType));
	internal_neuron_state = (ParameterType *)realloc(internal_neuron_state, allocated_space * sizeof(ParameterType));
	neuron_excitation = (ParameterType *)realloc(neuron_excitation, allocated_space * sizeof(ParameterType));
	is_fired = (bool *)realloc(is_fired, allocated_space * sizeof(bool));
	primer_list = (int *)realloc(primer_list, allocated_space * sizeof(int));
	connection_state = (ConnectionTypes *)realloc(connection_state, allocated_space * sizeof(ConnectionTypes));
	previous_connection_state = (ConnectionTypes *)realloc(previous_connection_state, allocated_space * sizeof(ConnectionTypes));

	//clearing data for new allocated space
	for (int i = previous_space; i < allocated_space; ++i) {
		n[i].id = -1;
		c[i].from_neuron_id = -1;
		neuron_state[i] = 0;
		previous_neuron_state[i] = 0;
		internal_neuron_state[i] = 0;
		neuron_excitation[i] = 0;
		is_fired[i] = 0;
		primer_list[i] = -1;
	}
}
	
void Module::createFixedInterfaceNeurons(int number_of_inputs, int number_of_outputs)
{
	for (int i = 0; i < number_of_inputs; ++i) {
		//find an available index
		int new_index = number_of_neurons;
		number_of_neurons++;

		//find an available id
		int new_id = smallestFreeId();
		
		//update max_neuron_id
		if (max_neuron_id < new_id) {
			max_neuron_id = new_id;	
		}

		//check the allocated space
		if (allocated_space <= number_of_neurons + 1) {
			reallocEverything();
		}

		//create a Input Neuron with the type passed
        n[new_index].id = new_id;
        n[new_index].firing_rate = LEVEL1;//randomFiringRateLevel(random);
        n[new_index].type = INPUT_IDENTITY;
		n[new_index].interface_index = i;
		n[number_of_neurons].id = -1;
	}
	
	for (int i = 0; i < number_of_outputs; ++i) {
		//find an available index
		int new_index = number_of_neurons;
		number_of_neurons++;

		//find an available id
		int new_id = smallestFreeId();
		
		//update max_neuron_id
		if (max_neuron_id < new_id) {
			max_neuron_id = new_id;	
		}

		//check the allocated space
		if (allocated_space <= number_of_neurons + 1) {
			reallocEverything();
		}	

		//create a Input Neuron with the type passed
        n[new_index].id = new_id;
        n[new_index].firing_rate = LEVEL1; //randomFiringRateLevel(random);
        n[new_index].type = OUTPUT_IDENTITY;
		n[new_index].interface_index = i;
		n[number_of_neurons].id = -1;
	}	
}

//add connection
void Module::addConnection(int from_neuron_id, int to_neuron_id, int neuro_modulation, ParameterType weight)
{
	if (allocated_space <= number_of_connections + 1) {
		reallocEverything();
	}

	//if there is no neurons, return
	if (n[0].id == -1) {
		//printf("no neurons\n");
		return;	
	}			

	//create the specified connection
	c[number_of_connections].from_neuron_id = from_neuron_id;
	c[number_of_connections].to_neuron_id = to_neuron_id;
	c[number_of_connections].neuro_modulation = neuro_modulation;
	c[number_of_connections].weight = weight;
	
    number_of_connections++;
	c[number_of_connections].from_neuron_id = -1;
}

void Module::connectNewNeuronToNetwork(int new_neuron_id)
{
	if (allocated_space <= number_of_connections + 2) {
		reallocEverything();
	}
	
	// From the New Neuron
	//create a random connection 
	c[number_of_connections].from_neuron_id = new_neuron_id;
	
	int random_index = random->uniform(0, number_of_neurons - 1);
	c[number_of_connections].to_neuron_id = n[random_index].id;
    
    ParameterType weight = 1;
	
	if (random->uniform(0.0, 1.0) < CHANCE_OF_NEUROMODULATION) {
		//int random_index= random->uniform(0,1);
		int random_index2 = random->uniform(0, number_of_neurons - 1);
		//with modulation
		c[number_of_connections].neuro_modulation = n[random_index2].id;
	}
	else {
        c[number_of_connections].neuro_modulation = -1;
        int random_index2 = random->uniform(0, 1);
		if (random_index2 != 0) {
            weight = -1;
		}
	}

    c[number_of_connections].weight = weight;
    
	/**************** Second Connection ***************/
    number_of_connections++;
    c[number_of_connections].from_neuron_id = -1;
    
	// To the New Neuron
	//create a random connection
	random_index = random->uniform(0, number_of_neurons - 1);
	c[number_of_connections].from_neuron_id = n[random_index].id;
	
	c[number_of_connections].to_neuron_id = new_neuron_id;
    
    weight = 1;
	
	if (random->uniform(0.0, 1.0) < CHANCE_OF_NEUROMODULATION) {
		//int random_index= random->uniform(0,1);
		int random_index2 = random->uniform(0, number_of_neurons - 1);
		//with modulation
		c[number_of_connections].neuro_modulation = n[random_index2].id;
	}
	else {
        c[number_of_connections].neuro_modulation = -1;
		int random_index2 = random->uniform(0, 1);
		if (random_index2 != 0) {
			weight = -1;
		}
	}

    c[number_of_connections].weight = weight;
    
	number_of_connections++;
	c[number_of_connections].from_neuron_id = -1;
}

void Module::updatePrimerList()
{
	//create the Primer list
	//not all Control Neurons activate, the Primers are the ones that always activate
	number_of_primers = 0;
	for (int i = 0; n[i].id >= 0; ++i)
	{
		//skip if it is NOT a Control Neuron
		if (n[i].type != CONTROL) {
			continue;
		}

		int cn_id = n[i].id;
		bool is_primer = true;

		//check if this Control Neuron (CN) do not receive connections from other CNs
		for (int j = 0; c[j].from_neuron_id >= 0 && is_primer == true; ++j) {
            
            int source = c[j].from_neuron_id;
			//skip if it is not a connection to this CN
			//or if the connection comes from this neuron (ignore recurrent connections from this neuron)
			if (c[j].to_neuron_id != cn_id || source == cn_id) {
				continue;
			}

			int dna_index = neuronIdToDNAIndex(source);
			if (n[dna_index].type == CONTROL) {
				is_primer = false;
			}	
		}

		//if this CN is a primer, insert it in the primer list
		if (is_primer) {
			primer_list[number_of_primers] = cn_id;
			number_of_primers++;
		}
	}
	
	//set the end of the primer list
	primer_list[number_of_primers] = -1;
}

int Module::neuronIdToDNAIndex(int id)
{
	for (int i = 0; n[i].id >= 0 ; ++i) {
		if (n[i].id == id) {
			return i;
		}
	}

	printf("ERROR: No neuron with %d id found\n",id);
	exit(1);
	return -1;
}

// this function returns an id number that is free to use
// 
// it is useful to avoid even sparse dna with even increasing values of id numbers 
int Module::smallestFreeId()
{
	//check if there is the id "i" in the current dna
	bool id_found;
	
	int i;	
	for (i = 0; n[i].id >= 0; ++i) {
		id_found = false;

		for (int j = 0; n[j].id >= 0 && id_found == false; ++j) {
			if (n[j].id == i) {
				id_found = true;
			}
		}
	
		if (id_found == false) {
			return i;
		}
	}

	return i;
}
