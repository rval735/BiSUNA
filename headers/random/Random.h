
#ifndef RANDOM_H
#define RANDOM_H

#include"math.h"

#define PI 3.14159265359

class Random
{
	public:

		static double spare;
		static bool is_spare_ready;
	
		//return closed closed uniform for integer -> [min, max]
		virtual int uniform(int min, int max) = 0;

		//return close open [min, max)
		virtual double uniform(double min, double max) = 0;

		//return random result uniformly distributed between [0,1)
		virtual double uniform() = 0;

		//generate a gaussian distribution with mean = 0 and variance = 1 (standard normal distribution)
		double gaussian(double mean, double std_dev)
		{
			//Marsaglia polar method
			//Method for generating a gaussian distribution from a random distribution

			if(is_spare_ready)
			{
				is_spare_ready=false;
				return mean + spare*std_dev;
			}
			else
			{
				double u= uniform(-1.0,1.0);
				double v= uniform(-1.0,1.0);

				double s= u*u + v*v;

				while(s == 0.0 || s >= 1.0)
				{
					u= uniform(-1.0,1.0);
					v= uniform(-1.0,1.0);

					s= u*u + v*v;
				}
				
				double mul = sqrt(-2.0*log(s)/s);
				spare = v*mul;
				is_spare_ready = true;
				
				return mean + std_dev*u*mul;
			}
		}
	
};


#endif
