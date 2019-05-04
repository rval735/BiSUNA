/***************************************************
 * State_of_Art Random - Interface to the state of art in randomness
 *
 * The best trade off between speed and randomness.
 * Though might have some bugs. Because this is just an interface 
 * to the algorithm implemented by people researching random generators.
 *
 * Double precision SIMD-oriented Fast Mersenne Twister (dSFMT) 
 *
 * described in:
 *
 * Mutsuo Saito and Makoto Matsumoto, "SIMD-oriented Fast Mersenne Twister: a 128-bit Pseudorandom Number Generator", Monte Carlo and Quasi-Monte Carlo Methods 2006, Springer, 2008, pp. 607 -- 622. DOI:10.1007/978-3-540-74496-2_36
 *
 * **************************************************/

#ifndef STATE_OF_ART_RANDOM_H
#define STATE_OF_ART_RANDOM_H


#include"Random.h"
#include<stdlib.h>
#include"dSFMT/dSFMT.h"

class State_of_Art_Random: public Random
{
	private:
	

	public:
		
		State_of_Art_Random(unsigned int seed);
		~State_of_Art_Random();

		int uniform(int min, int max);
		double uniform(double min, double max);
		//return random result uniformly distributed between [0,1)
		double uniform();	
	
};

#endif
