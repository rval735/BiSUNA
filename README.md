# BiSUNA

Train from scratch Binary Neural Networks using neuroevolution as its base technique
(gradient decent free), to then apply such results to reinforcement learning environments
tested in the OpenAI Gym

This project extends the original [SUNA paper](https://github.com/zweifel/Physis-Shard) with binary operations and connections.
It can be compiled on MacOS using Xcode or in Linux using commands from script/RecreateEnvironment.sh,
considering that the last case needs the library [Zweifel](https://github.com/zweifel/zweifel)
to be compiled first.

## Contents

Agents:
 - **S**pectrum-diverse **U**nified **N**euron **E**volution **A**rchitecture (SUNA)
 - Binary SUNA (BiSUNA)
 
In order to compile either environment, use the directive "CONTINUOUS_PARAM" inside
"parameters.h".

Environments (i.e., Problems):
 - Mountain Car
 - Double Cart Pole (with and without velocities)
 - Function Approximation
 - Multiplexer
 - Single Cart Pole
 - OpenAI Gym Interface
 
To run the gRPC server to connect to the OpenAI Gym environment, check the following
project, they work together. If you would like to work with different environments,
(un)comment lines 76 - 82 in main.cpp

## Fist SUNA implementation

This project is an extension on the original [SUNA implementation](https://github.com/zweifel/Physis-Shard)
Follow the link to recreate the original environment.
 
## Install

If you are testing on a Linux environment, follow the commands from the file "script/RecreateEnvironment.sh",
that will ease the process, which can be summarized as follows:

- Get all dependencies
- Install gRPC (Python and C++)
- Compile gym-uds-api project (OpenAI Gym server interface)
- Compile Zwefiel library
- Compile BiSUNA project

## Changing Environments (same as SUNA)

Environment can be changed in main.cpp.
For example commenting out where the Reinforcement_Environment is defined and
uncommenting the line with:
```
Reinforcement_Environment* env= new Double_Cart_Pole(random);
```

If the environment should be terminated when the maximum steps is reached
uncomment the following in parameters.h:
```
#define TERMINATE_IF_MAX_STEPS_REACHED
```
Do not forget to comment it out when surpassing the maximum number of steps is
not a termination condition! For example, montain car does not need it while
double cart pole does.

## Changing Parameters (same as SUNA)

Many parameters of the environment as well as of the agent can be changed by modifying some definitions
in parameters.h

## Running Experiments (same as SUNA)

To run a trial until its maximum number of trials defined in main.cpp, run:
```
./rl
```

To test the best individual, run: (same as SUNA)
```
./rl_live dna_best_individual
```

A series of trials can be run by using the script mean_curve.sh

## Adding Agents or Problems (same as SUNA)

An agent needs to implement the Reinforcement_Agent.h while a problem needs to implement the Reinforcement_Environment.h.
There are simple examples of agents and problems inside respectively the agents/ and environments/ directories.
Most of the examples were built with the general reinforcement learning in mind, however they can be applied to supervised learning as well as unsupervised learning (e.g., consider the reward from the system as an error).

## License

Apache License Version 2.0
