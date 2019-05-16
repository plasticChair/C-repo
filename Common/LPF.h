/* 
* LPF.h
*
* Created: 4/28/2019 8:26:16 PM
* Author: mosta
*/


#ifndef __LPF_H__
#define __LPF_H__


class LPF
{
//variables
public:
	void init(float alphaIn);
	void initVal(float val);
	void update(float sampledIn);
	float out();
protected:
private:

//functions
public:
	LPF();
	~LPF();
protected:
private:
	LPF( const LPF &c );
	LPF& operator=( const LPF &c );
	float filtInt = 0.0;
	float alpha = 0.0;

}; //LPF

#endif //__LPF_H__
