#ifndef PARAMETERS
#define PARAMETERS

// -----------  Unified Neural Model ------------- //

// Comment line below to make SUNA values discrete (binary).
// When it is continuous, it will behave as expected from the
// original publication, whereas commented, it will evolve
// binary neural networks only
#define CONTINUOUS_PARAM

#ifdef CONTINUOUS_PARAM
    using ParameterType = float;
    #define MAXIMUM_WEIGHT 2147483647.0
    #define WEIGHT_MUTATION_CHANGE_PERCENTAGE 1.0 //10 = 1000% change, 1 = 100% change possible
    #define EXCITATION_THRESHOLD 0.0    //minimum excitation necessary to activate the neuron
    #define MAX_NEURON_OUTPUT 1000000
    #define REMAINING_NEURON_THRESHOLD 0.001
    #define HALF_WEIGHT 0
    #define MID_WEIGHT 0
#else
    using ParameterType = unsigned short int;
    #define MAXIMUM_WEIGHT 65535
    #define WEIGHT_MUTATION_CHANGE_PERCENTAGE 1
    #define EXCITATION_THRESHOLD 256    //minimum excitation necessary to deactivate the neuron
// This constant considers the number of bits an excitation neuron must have in order
// to trigger its actions.
    #define EXCITATION_THRESHOLD_BITS 4
//    #define EXCITATION_THRESHOLD_BITS 6
    #define HALF_WEIGHT 255
    #define MID_WEIGHT 32767
    #define MAX_NEURON_OUTPUT 65535
    #define REMAINING_NEURON_THRESHOLD 15
#endif

#define EXTREME_NEGATIVE_REWARD -1000000
#define NUMBER_OF_INITIAL_MUTATIONS 200
#define NUMBER_OF_STEP_MUTATIONS 5
#define INITIAL_ALLOCATION_LENGTH 100000
#define SUBPOPULATION_SIZE 100

#define EPISODES_PER_INDIVIDUAL 1.0

//subpopulations
enum {
    FITNESS_BASED,
    NEURON_EFFICIENT,
    CONNECTION_EFFICIENT,
    NEURON_RICH,
    CONNECTION_RICH,
    NUMBER_OF_SUBPOPULATIONS
};

#define MAIN_SUBPOP FITNESS_BASED

//spectrum diversity
#define SPECTRUM_SIZE 6
#define NOVELTY_MAP_SIZE 20
#define NUMBER_OF_SUBPOPULATIONS 1
//#define NORMALIZED_SPECTRUM_DIVERSITY
//1-add neuron, 2-del neuron, 3-add connection, 4-del connection
//#define MUTATION_PROBABILITIES {0.25, 0.05, 0.6, 0.1}
#define MUTATION_PROBABILITIES {0.01, 0.01, 0.49, 0.49}
#define CHANCE_OF_NEUROMODULATION 0.1
#define CHANCE_OF_CONTROL_NEURON 0.2
#define CHANCE_OF_WEIGHT_MUTATION 0.5

//#define MUTATION_PROBABILITIES {0.55, 0.1, 0.20, 0.15}
//#define CHANCE_OF_NEUROMODULATION 0.2
//#define CHANCE_OF_CONTROL_NEURON 0.5
//#define CHANCE_OF_WEIGHT_MUTATION 0.7

//  ----------  Environments  ---------- //

//general features

//#define SET_NORMALIZED_INPUT
//#define SET_NORMALIZED_OUTPUT
//stop and exit the process if the max steps are reached in one trial, it means that the algorithm reached the best solution
//#define TERMINATE_IF_MAX_STEPS_REACHED		

////////Reinforcement Learning///////

//Mountain Car
#define MAX_MOUNTAIN_CAR_STEPS 1000
//#define	NOISY_MOUNTAIN_CAR
#define CONTINUOUS_MOUNTAIN_CAR	//if defined only one continuous output, otherwise 3 outputs

//#define CHANGING_MCAR_MAX_VELOCITY
#define MODIFIED_MCAR_MAX_VELOCITY 0.04
//#define MCAR_TRIALS_TO_CHANGE 10000
//#define MCAR_TRIALS_TO_CHANGE 100
#define MCAR_TRIALS_TO_CHANGE 1

#define MCAR_MIN_POSITION -1.2
#define MCAR_MAX_POSITION 0.6
#define MCAR_MAX_VELOCITY 0.07            // the negative of this in the minimum velocity
#define MCAR_GOAL_POSITION 0.6

//Double Pole Balancing
#define MAX_DOUBLE_POLE_STEPS 100000
#define NON_MARKOV_DOUBLE_POLE

//Function Approximation
#define	SEQUENTIAL_FUNCTION_APPROXIMATION
//#define	SUPERVISED_FUNCTION_APPROXIMATION
//#define NUMBER_OF_FUNCTION_APPROXIMATION 2
//#define	MULTIPLE_RANDOM_FUNCTION_APPROXIMATION

//Single Pole Balancing
//#define MAX_SINGLE_POLE_STEPS 10000
#define MAX_SINGLE_POLE_STEPS 10000
#define RANDOM_START false

#endif
