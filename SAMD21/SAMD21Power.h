/* 
* SAMD21Timers.h
*
* Created: 4/28/2019 9:03:11 PM
* Author: mosta
*/

#include "Arduino.h"
#ifndef __SAMD21TIMERS_H__
#define __SAMD21TIMERS_H__


class SAMD21Timers
{
//variables
public:
	Tc *_hw;
	uint8_t _instance = 0;
	uint8_t _glckNum = 0;
protected:

private:


//functions
public:
	SAMD21Timers(uint8_t timernum,uint8_t glckNum);
	~SAMD21Timers();
		void tcConfigure(int sampleRate);
		bool tcIsSyncing();
		void tcStartCounter();
		void tcReset();
		void tcDisable();
		void setCount(int rate);
		uint16_t getCount();
	
protected:
private:



}; //SAMD21Timers

#endif //__SAMD21TIMERS_H__
