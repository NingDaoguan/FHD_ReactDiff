#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "RNGs.h"

/* A C-program for MT19937: Real number version([0,1)-interval) */
/* (1999/10/28)                                                 */
/*   genrand() generates one pseudorandom real number (double)  */
/* which is uniformly distributed on [0,1)-interval, for each   */
/* call. srandgen(seed) sets initial values to the working area */
/* of 624 words. Before genrand(), srandgen(seed) must be       */
/* called once. (seed is any 32-bit integer.)                   */
/* Integer generator is obtained by modifying two lines.        */
/*   Coded by Takuji Nishimura, considering the suggestions by  */
/* Topher Cooper and Marc Rieffel in July-Aug. 1997.            */

/* This library is free software; you can redistribute it and/or   */
/* modify it under the terms of the GNU Library General Public     */
/* License as published by the Free Software Foundation; either    */
/* version 2 of the License, or (at your option) any later         */
/* version.                                                        */
/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of  */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.            */
/* See the GNU Library General Public License for more details.    */
/* You should have received a copy of the GNU Library General      */
/* Public License along with this library; if not, write to the    */
/* Free Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA   */ 
/* 02111-1307  USA                                                 */

/* Copyright (C) 1997, 1999 Makoto Matsumoto and Takuji Nishimura. */
/* Any feedback is very welcome. For any question, comments,       */
/* see http://www.math.keio.ac.jp/matumoto/emt.html or email       */
/* matumoto@math.keio.ac.jp                                        */

/* REFERENCE                                                       */
/* M. Matsumoto and T. Nishimura,                                  */
/* "Mersenne Twister: A 623-Dimensionally Equidistributed Uniform  */
/* Pseudo-Random Number Generator",                                */
/* ACM Transactions on Modeling and Computer Simulation,           */
/* Vol. 8, No. 1, January 1998, pp 3--30.                          */

/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */   
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

void srandgen (unsigned long seed)
{
/*    int mti; */

    mt[0] = seed & 0xffffffffUL;
    for ( mti=1; mti<N; mti++ ) 
    {
        mt[mti] = (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30L)) + mti); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;       /* for >32 bit machines */
    }

    mti = N;
}

/* A. Donev separated this into a separate routine so we can return integers directly */
unsigned long genrandbase ()
{
    unsigned long y;
    static unsigned long mag01[2]={0x0, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if srandgen() has not been called, */
            srandgen(4357); /* a default initial seed is used   */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];

        mti = 0;
    }
  
    y = mt[mti++];
    y ^= TEMPERING_SHIFT_U(y);
    y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
    y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
    y ^= TEMPERING_SHIFT_L(y);
    
    return(y);
}

void genrand (double* rn) /* Largest returned number should be 1.0-epsilon(1.0) */
{
    unsigned long y;
    y = genrandbase ();
    /* Donev: Original value below was 2.3283064365386963e-10 but now this is same as C++ code */
    *rn = ( (double)y * 2.3283064370807969e-10 ); /* reals: [0,1)-interval */    
}

/* A. Donev added this routine to avoid returning 1.0 in single precision */
void genrand_sp (float* rn) /* Largest returned number should be 1.0f-epsilon(1.0f)*/
{
    unsigned long y;
    y = genrandbase ();
    /* A. Donev: Value taken from C++ code */
    *rn = ( (float)y * 2.32830616e-10f ); /* reals: [0,1)-interval */
}


/* A. Donev addeed this by copying the code from the gsl library's function gsl_rng_uniform_int */
/* Generates a random integer in the range [1,n] */
/* We have had some issues calling this from Fortran and it has to do with unsigned integers it seems or a gfortran bug?!? */
void genrandint (unsigned long int *r, unsigned long int n)
{
  unsigned long int offset = 0;
  unsigned long int range = 0xffffffffUL - offset;
  unsigned long int scale;
  unsigned long int k, rng;

  /*unsigned long int n=*n_input;*/
  
  scale = range / n;
  /*printf("n=%u range=%u scale=%u\n",n,range,scale);*/

  do
    {
      rng = genrandbase();
      k = (rng - offset) / scale;
      /*printf("n=%d  rng=%d r=%d\n", n, rng, k+1);*/
    }
  while (k >= n);

  *r = k+1;
}

/*
** Lower tail quantile for standard normal distribution function.
**
** This function returns an approximation of the inverse cumulative
** standard normal distribution function.  I.e., given P, it returns
** an approximation to the X satisfying P = Pr{Z <= X} where Z is a
** random variable from the standard normal distribution.
**
** The algorithm uses a minimax approximation by rational functions
** and the result has a relative error whose absolute value is less
** than 1.15e-9.
**
** Author:      Peter J. Acklam
** Time-stamp:  2002-06-09 18:45:44 +0200
** E-mail:      jacklam@math.uio.no
** WWW URL:     http://www.math.uio.no/~jacklam
**
** C implementation adapted from Peter's Perl version
*/

double InvNormDist (double p)
{
    static const double a[6] =
    {
	-3.969683028665376e+01,
        2.209460984245205e+02,
	-2.759285104469687e+02,
        1.383577518672690e+02,
	-3.066479806614716e+01,
        2.506628277459239e+00
    };
    static const double b[5] =
    {
	-5.447609879822406e+01,
        1.615858368580409e+02,
	-1.556989798598866e+02,
        6.680131188771972e+01,
	-1.328068155288572e+01
    };
    static const double c[6] =
    {
	-7.784894002430293e-03,
	-3.223964580411365e-01,
	-2.400758277161838e+00,
	-2.549732539343734e+00,
        4.374664141464968e+00,
        2.938163982698783e+00
    };
    static const double d[4] =
    {
	7.784695709041462e-03,
	3.224671290700398e-01,
	2.445134137142996e+00,
	3.754408661907416e+00
    };

    static const double lo = 0.02425;
    static const double hi = 0.97575;

    double x;

    if (p <= 0 || p >= 1)
    {
        printf("InvNormDist(): p MUST be in (0,1)");
        exit(-1);
    }
    /*
    ** Coefficients in rational approximations.
    */
    if (p < lo)
    {
        /*
        ** Rational approximation for lower region.
        */
        double q = sqrt(-2*log(p));

        x = (((((c[0]*q+c[1])*q+c[2])*q+c[3])*q+c[4])*q+c[5]) /
            ((((d[0]*q+d[1])*q+d[2])*q+d[3])*q+1);
    }
    else if (p > hi)
    {
        /*
        ** Rational approximation for upper region.
        */
        double q = sqrt(-2*log(1-p));

        x = -(((((c[0]*q+c[1])*q+c[2])*q+c[3])*q+c[4])*q+c[5]) /
            ((((d[0]*q+d[1])*q+d[2])*q+d[3])*q+1);
    }
    else
    {
        /*
        ** Rational approximation for central region.
        */
        double q = p - 0.5;
        double r = q*q;

        x = (((((a[0]*r+a[1])*r+a[2])*r+a[3])*r+a[4])*r+a[5])*q /
            (((((b[0]*r+b[1])*r+b[2])*r+b[3])*r+b[4])*r+1);
    }

    return x;
}

void genrandn(double* result)
{
    double val;
    /*
    ** Get a random number in (0,1) -- the generator gives one in [0,1);
    */
    do
    {
        genrand(&val);
    }
    while (val == 0);

    *result = InvNormDist(val);
}

void hg_genrandn(double* result, void* rng)
{
    double val;
    /*
    ** Get a random number in (0,1) -- the generator gives one in [0,1);
    */
    do
    {
      hg_genrand(&val, rng);
    }
    while (val == 0);

    *result = InvNormDist(val);
}

#ifdef FORTRAN_BOXLIB
// BoxLib/Src/F_BaseLib/bl_random_c.cpp provides hg_genrand
#else
void hg_genrand (double* rn, void* rng)
{
  genrand(rn);
}
void hg_genrand_sp (float* rn, void* rng)
{
  genrand_sp(rn);
}
#endif

