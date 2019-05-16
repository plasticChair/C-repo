/* 
* SAMD21Timers.cpp
*
* Created: 4/28/2019 9:03:11 PM
* Author: mosta
*/

// 16bit mode only

#include "SAMD21Timers.h"

// default constructor
SAMD21Timers::SAMD21Timers(uint8_t timernum, uint8_t glckNum, uint8_t source = 6 )
{
	
	_instance = timernum-3;
	_glckNum = glckNum;
	_source = source;

} //SAMD21Timers

// default destructor
SAMD21Timers::~SAMD21Timers()
{
} //~SAMD21Timers

 void SAMD21Timers::configure(int sampleRate, bool IntEnable)
 {
	 //                               3                                           4                            5
	 uint32_t inst_gclk_id[] = {GCLK_CLKCTRL_ID_TCC2_TC3, GCLK_CLKCTRL_ID_TC4_TC5, GCLK_CLKCTRL_ID_TC4_TC5};
	 IRQn_Type inst_irq_id[] = {TC3_IRQn                      , TC4_IRQn                    ,TC5_IRQn};
	 Tc *inst_tc[] = TC_INSTS;
	 
	 _hw = inst_tc[_instance];
	 
	 NVIC_DisableIRQ(inst_irq_id[_instance]);
	 NVIC_ClearPendingIRQ(inst_irq_id[_instance]);
	 
	 NVIC_SetPriority(inst_irq_id[_instance], 0);
	 
	 /* OSC8M Internal 8MHz Oscillator */
	 SYSCTRL->OSC8M.bit.PRESC = SYSCTRL_OSC8M_PRESC_0;
	 SYSCTRL->OSC8M.bit.ONDEMAND = SYSCTRL_OSC8M_ONDEMAND;
	 SYSCTRL->OSC8M.bit.RUNSTDBY = SYSCTRL_OSC8M_RUNSTDBY;

	 /* Enable OSC8M */
	 SYSCTRL->OSC8M.reg |= SYSCTRL_OSC8M_ENABLE;

	 // Gen Clock setup
	GCLK->GENDIV.reg  = (uint32_t) (GCLK_GENDIV_ID(_glckNum) | GCLK_GENDIV_DIV(1));
	while (GCLK->STATUS.bit.SYNCBUSY);
	
//   GCLK_GENCTRL_SRC_XOSC_Val       0x0ul  /**< \brief (GCLK_GENCTRL) XOSC oscillator output */
 //  GCLK_GENCTRL_SRC_GCLKIN_Val     0x1ul  /**< \brief (GCLK_GENCTRL) Generator input pad */
 //  GCLK_GENCTRL_SRC_GCLKGEN1_Val   0x2ul  /**< \brief (GCLK_GENCTRL) Generic clock generator 1 output */
 //  GCLK_GENCTRL_SRC_OSCULP32K_Val  0x3ul  /**< \brief (GCLK_GENCTRL) OSCULP32K oscillator output */
 //  GCLK_GENCTRL_SRC_OSC32K_Val     0x4ul  /**< \brief (GCLK_GENCTRL) OSC32K oscillator output */
 //  GCLK_GENCTRL_SRC_XOSC32K_Val    0x5ul  /**< \brief (GCLK_GENCTRL) XOSC32K oscillator output */
 //  GCLK_GENCTRL_SRC_OSC8M_Val      0x6ul  /**< \brief (GCLK_GENCTRL) OSC8M oscillator output */
 //  GCLK_GENCTRL_SRC_DFLL48M_Val    0x7ul  /**< \brief (GCLK_GENCTRL) DFLL48M output */
 //  GCLK_GENCTRL_SRC_FDPLL_Val      0x8ul  /**< \brief (GCLK_GENCTRL) FDPLL output */
 
	GCLK->GENCTRL.reg = (uint32_t) (GCLK_GENCTRL_RUNSTDBY | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC(_source) | GCLK_GENCTRL_ID(_glckNum));
	while (GCLK->STATUS.bit.SYNCBUSY);
 
	GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(_glckNum) |  inst_gclk_id[_instance]) ;  
	while (GCLK->STATUS.bit.SYNCBUSY);
	 

	 reset(); //reset _hw

	 // Set Timer counter Mode to 16 bits
	 _hw->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
	 // Set _hw mode as match frequency
	 _hw->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MPWM;
	 //set prescaler and enable _hw
	 _hw->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_ENABLE;
	 //set _hw timer counter based off of the system clock and the user defined sample rate or waveform
	 if (IntEnable) _hw->COUNT16.CC[0].reg = (uint16_t) ((8000000 / 2) / sampleRate -1);
	 while (isSyncing());
	 
	 // Configure interrupt request
	 NVIC_DisableIRQ(inst_irq_id[_instance]);
	 NVIC_ClearPendingIRQ(inst_irq_id[_instance]);
	 NVIC_SetPriority(inst_irq_id[_instance], 0);
	 if (IntEnable)  NVIC_EnableIRQ(inst_irq_id[_instance]);

	 // Enable the _hw interrupt request
	 _hw->COUNT16.INTENSET.bit.MC0 = 1;
	 while (isSyncing()); //wait until _hw is done syncing
 }

 //Function that is used to check if _hw is done syncing
 //returns true when it is done syncing
 bool SAMD21Timers::isSyncing()
 {
	 return _hw->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
 }

 //This function enables _hw and waits for it to be ready
 void SAMD21Timers::startCounter()
 {
	 _hw->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE; //set the CTRLA register
	 while (isSyncing()); //wait until snyc'd
 }

 //Reset _hw
 void SAMD21Timers::reset()
 {
	 _hw->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
	 while (isSyncing());
	 while (_hw->COUNT16.CTRLA.bit.SWRST);
 }

 //disable _hw
 void SAMD21Timers::disable()
 {
	 _hw->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
	 while (isSyncing());
 }
 
 void SAMD21Timers::setCount(int rate)
 {
	 _hw->COUNT16.CC[0].reg = (uint16_t) ((F_CPU / 2) / rate -1);
	 while (isSyncing());
 }
 
 uint16_t SAMD21Timers::getCount()
 {
	 return _hw->COUNT16.COUNT.bit.COUNT; 
 }
 
void  SAMD21Timers::restart()
 {
	 _hw->COUNT16.COUNT.bit.COUNT = (uint16_t)(0);
	 while (isSyncing());
 }