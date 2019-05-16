/* 
* LPF.cpp
*
* Created: 4/28/2019 8:26:16 PM
* Author: mosta
*/


#include "LPF.h"

// default constructor
LPF::LPF()
{
	
} //LPF

// default destructor
LPF::~LPF()
{
} //~LPF

void LPF::init(float alphaIn)
{
	alpha = alphaIn;
}
void  LPF::initVal(float val)
{
	filtInt = filtInt;
	
}
void LPF::update(float sampledIn)
{
	filtInt = ((filtInt + alpha*(sampledIn - filtInt)) );
}

float LPF::out()
{
	return filtInt;
}
