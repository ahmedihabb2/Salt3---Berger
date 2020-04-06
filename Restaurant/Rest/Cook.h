#pragma once

#include "..\Defs.h"

#pragma once
class Cook
{
	int ID;
	ORD_TYPE type;	//for each order type there is a corresponding type (VIP, Normal, Vegan)
	int speed;		//dishes it can prepare in one clock tick (in one timestep)

	//abeer
	int numOrdersBefBreak; 
	int breakDuration; 

public:
	Cook();
	virtual ~Cook();

	int GetID() const;
	ORD_TYPE GetType() const;
	int getSpeed() const;
	int getNumOrdBbreak() const;
	int getBreakDur() const;

	void setID(int);
	void setType(ORD_TYPE) ;
	void setSpeed(int); //abeer added this
	void setNumOrdBbreak(int num);
	void setBreakDur(int bd);

};
