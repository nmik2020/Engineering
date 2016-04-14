////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010-2011, Turtle Rock Studios.  All rights reserved.
//
//	Random.h
//  Description: implementation of functions for random number generation
//
////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <time.h>

#include "Random.h"

void RandomInitialize()
{
	srand ( (unsigned int) time(NULL) );	

}

float RandomGetFloat(float flMin, float flMax)
{
	const float flRange = flMax - flMin;

	const float flRandom = ( ((float)rand()) / RAND_MAX ) * flRange;

	const float flValue = flMin + flRandom;

	return flValue;
}

int RandomGetInt(int nMin, int nMax)
{
	if (nMin == nMax)
		return 0;

	const int nRange = nMax - nMin;

	const int nRandom = rand() % (nRange + 1);	
	
	const int nValue = nMin + nRandom;

	return nValue;
}

