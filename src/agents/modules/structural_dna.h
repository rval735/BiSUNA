
#ifndef STRUCTURAL_DNA
#define STRUCTURAL_DNA

#include "stdio.h"
#include "random/State_of_Art_Random.h"
#include "../../parameters.h"

//all neurons not activated by a control neuron are inactive by default
#define DEFAULT_INACTIVE

//structure and weight cycles are inexistent, all the evolution happens on only a structure cycle and imprecise weights
#define NO_WEIGHT_STRUCTURE_CYCLE

//neuron types
//
//Important:
//Input and Output neurons are special neurons, which can not be randomly generated and therefore
//are after the NUMBER_OF_NEURON_TYPES definition
//enum{IDENTITY, SIGMOID, THRESHOLD, RANDOM, CONTROL, SIN, NUMBER_OF_NEURON_TYPES, INPUT_IDENTITY, OUTPUT_IDENTITY };
enum NeuronTypes: int {
    IDENTITY,
    ACTIVATION,
    THRESHOLD,
    RANDOM,
    CONTROL,
    NUMBER_OF_NEURON_TYPES,
///////////////////////////////////
    INPUT_IDENTITY,
    OUTPUT_IDENTITY,
};

enum ConnectionTypes {
    Recurrent,
    FeedForward,
    NumberConnTypes
};

struct connection_type {
    int index;
    ConnectionTypes cType;
};

//starting levels of firing rate 
enum FiringRate : int {
    LEVEL1 = 1,
    LEVEL7 = 7,
    LEVEL49 = 49,
    NUMBER_OF_FIRING_RATE_LEVELS = 3
};

typedef struct _neuron {
	int id;
	FiringRate firing_rate;
	NeuronTypes type;
	int interface_index;	//only used by input and output neurons to find the respective variable inside the input/output vector
} neuron;

typedef struct _connection {
	int from_neuron_id;
	int to_neuron_id;
    ParameterType weight;
	int neuro_modulation;	//-1 for inactive, for >0 it is active and represents the id of the neuron whose response is used as weight 
} connection;

inline void writeDNA(const char *filename, neuron **n, int n_size, connection **c, int c_size) {
	FILE *fp = fopen(filename, "w");

	if (fp == NULL) {
		printf("Could no open file to write\n");
		exit(1);
	}

	fwrite((const void *)&n_size, sizeof(n_size), 1, fp);
	fwrite((const void *)&c_size, sizeof(c_size), 1, fp);
	fwrite((const void *)*n, sizeof(neuron), n_size+1, fp);
	fwrite((const void *)*c, sizeof(connection), c_size+1, fp);
	//fwrite((unsigned char *)n, sizeof(neuron), n_size, fp);
	//fwrite((unsigned char *)c, sizeof(connection), c_size, fp);
	
	//printf("saving n_size %d %d\n",n_size,sizeof(neuron));
	//printf("saving c_size %d %d\n",c_size,sizeof(connection));
	
	fclose(fp);
}

inline void readDNA(const char *filename, neuron **n, int &n_size, connection **c, int &c_size) {
	FILE *fp = fopen(filename, "r");
	
	if (fp == NULL) {
		printf("Could no open file to read\n");
		exit(1);
	}

	int size;

	fread((void *)&size, sizeof(size), 1, fp);
	n_size = size;
	fread((void *)&size, sizeof(size), 1, fp);
	c_size = size;
	fread((void *)*n, sizeof(neuron), n_size + 1, fp);
	fread((void *)*c, sizeof(connection), c_size + 1, fp);

	//printf("n_size %d\n",n_size);
	//printf("c_size %d\n",c_size);

	fclose(fp);
}

inline FiringRate randomFiringRateLevel(Random *random)
{
	int level = random->uniform(0, NUMBER_OF_FIRING_RATE_LEVELS - 1);

	switch(level) {
		case 0: {
			return LEVEL1;
		}
		break;
		case 1: {
			return LEVEL7;
		}
		break;
		case 2: {
			return LEVEL49;
		}
		break;
	}

	printf("ERROR: random firing rate is generated with incorrect value of %d\n",level);
	exit(1);
	return NUMBER_OF_FIRING_RATE_LEVELS;
}

inline ParameterType activationFunction(int neuron_type, ParameterType weighted_input, Random *random)
{
    switch(neuron_type)
    {
        case RANDOM: {
#ifdef CONTINUOUS_PARAM
            return random->uniform(-1.0, 1.0);;
#else
            return random->uniform(0, MAX_NEURON_OUTPUT);
#endif
        }
            break;
        case THRESHOLD:
        case CONTROL: {
#ifdef CONTINUOUS_PARAM
            return weighted_input >= 0 ? 1.0 : -1.0;
#else
            int val = __builtin_popcount(weighted_input);
            return val >= EXCITATION_THRESHOLD_BITS ? HALF_WEIGHT : 0;
#endif
        }
            break;
        case IDENTITY:
        case INPUT_IDENTITY:
        case OUTPUT_IDENTITY: {
            return weighted_input;
        }
            break;
        case ACTIVATION: {
#ifdef CONTINUOUS_PARAM
            return tanhf(weighted_input);
#else
//            return weighted_input > HALF_WEIGHT ? weighted_input : 0;
            return __builtin_popcount(weighted_input);
#endif
        }
            break;
    }
    
    printf("ERROR: activationFunction() with an incorrect value of neuron type %d\n",neuron_type);
    exit(1);
    return 0;
}

inline void fprintNeuronType(FILE *fp, int neuron_type)
{
	switch(neuron_type) {
		case IDENTITY: {
			fprintf(fp,"Identity");
			return;
		}
		break;
        case ACTIVATION: {
            fprintf(fp,"Activation");
            return;
        }
        break;
        case THRESHOLD: {
			fprintf(fp,"Threshold");
			return;
		}
		break;
		case RANDOM:	{
			fprintf(fp,"Random");
			return;
		}
		break;
		case CONTROL: {
			fprintf(fp,"Control");
			return;
		}
		break;
		case INPUT_IDENTITY: {
			fprintf(fp,"Input Identity");
			return;
		}
		break;
		case OUTPUT_IDENTITY: {
			fprintf(fp,"Output Identity");
			return;
		}
		break;
	}
	
	printf("ERROR: fprintfNeuronType() function with an incorrect value of neuron type %d\n", neuron_type);
	exit(1);
	return;
}

inline void printNeuronType(int neuron_type)
{
	switch(neuron_type) {
		case IDENTITY: {
			printf("Identity");
			return;
		}
		break;
        case ACTIVATION: {
            printf("Activation");
            return;
        }
        break;
        case THRESHOLD: {
			printf("Threshold");
			return;
		}
		break;
		case RANDOM: {
			printf("Random");
			return;
		}
		break;
		case CONTROL: {
			printf("Control");
			return;
		}
		break;
		case INPUT_IDENTITY: {
			printf("Input Identity");
			return;
		}
		break;
		case OUTPUT_IDENTITY: {
			printf("Output Identity");
			return;
		}
		break;
	}
	
	printf("ERROR: printNeuronType() function with an incorrect value of neuron type %d\n",neuron_type);
	exit(1);
	return;
}

inline ParameterType betaFromFR(FiringRate fr)
{
#ifdef CONTINUOUS_PARAM
    float frD = static_cast<float>(fr);
    float partial = 1 / frD;
    return partial;
#else
    switch(fr) {
        case LEVEL1: {
            return MAXIMUM_WEIGHT;
        }
            break;
        case LEVEL7: {
            return 2047;
        }
            break;
        case LEVEL49: {
            return 63;
        }
            break;
        default: {
            return 0;
        }
    }
#endif
}

inline ParameterType xnor(ParameterType x, ParameterType y)
{
#ifdef CONTINUOUS_PARAM
    return 0;
#else
    return ~(x ^ y);
#endif
}


#endif
