// -*- c++ -*-
//
//  File:         random.hpp
//
//  Description:  C++ implementation of the random number generator class.
//                eCGA version 1.0 implemented Prime Modulus Multiplicative 
//                Linear Congruential Genertator (PMMLCG), based on the paper 
//                by Park and Miller published in the Communications of the
//                ACM, October 1988.
//                ECGA version 1.1 uses Mersenne Twister by M. Matsumoto and 
//                T. Nishimura. See 
//                http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html 
//                for more information on Mersenne Twister                               *
//
//  Authors:       Fernando Lobo, Kumara Sastry
//
//  Date:         June/1999
//
//  Modified to be compliant with gcc 3.4 and changed the
//  pseudo-random number generator to Mersenne twister by Kumara Sastry
//
//  Date:        March/2006


#ifndef _random_hpp
#define _random_hpp

//#define SIMPLE_SPRNG

//#include "sprng.h"

#define Nmt 624
#define Mmt 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

static unsigned long mt[Nmt]; /* the array for the state vector  */
static int mti=Nmt+1; /* mti==N+1 means mt[N] is not initialized */

const int UNSET = -1;

class randomG
{ 
  private:
    unsigned long Seed;
    unsigned long genrand_int32();
    long genrand_int31();
    double genrand_real1();
    double genrand_real2();
    double genrand_real3(void);
    double genrand_res53(void);

  protected:
  //    int *Stream;
  public:
    randomG(){randomize(UNSET);}
    ~randomG() {}
    void init_Mersenne();
    unsigned long makeSeed();
    void randomize( int seed);
    void randomize( double seed );
  //    void randomize (unsigned long seed);
    double uniform01();
    double uniform( double a, double b );
    int    uniform( int a, int b );
    double exponential( double mu );
    bool   bernoulli( double p );
    bool   flip( double p ) { return bernoulli(p); }
    long   geometric( double p );
    double normal01();
    double normal( double mean, double variance );
};


#endif
