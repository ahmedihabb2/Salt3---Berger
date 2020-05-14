#pragma once

#include "..\Defs.h"
#include"Order.h"
#pragma once
class Cook
{
	int ID;
	ORD_TYPE type;	//for each order type there is a corresponding type (VIP, Normal, Vegan)
	int speed;		//dishes it can prepare in one clock tick (in one timestep)
	//Donya
	int RstPrd;
	//abeer
	int numOrdersBefBreak; 
	int breakDuration; 
	int ord_assigned;
public:
	Cook();
	virtual ~Cook();
	
	int GetID() const;
	ORD_TYPE GetType() const;

	//abeer
	int getSpeed() const;
	int getNumOrdBbreak() const;
	int getBreakDur() const;

	//////////////////////////////////////////////////

	void setID(int);
	void setType(ORD_TYPE) ;


	//abeer
	void setSpeed(int); 
	void setNumOrdBbreak(int num);
	void setBreakDur(int bd);
	//Donia
	int get_RstPrd();
	void set_RstPrd(int r);
	void assign_Order(int id);
	int get_order();
};
