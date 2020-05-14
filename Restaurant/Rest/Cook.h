#pragma once

#include "..\Defs.h"

#pragma once
class Cook
{
	int ID;
	ORD_TYPE type;	//for each order type there is a corresponding type (VIP, Normal, Vegan)
	int speed;		//dishes it can prepare in one clock tick (in one timestep)
	//donya
	int ord_assigned;
	//abeer
	int RstPrd;
	int numOrdersBefBreak; 
	int breakDuration; 
	//raghad
	int numofOrderdServed;

public:
	Cook();
	virtual ~Cook();

	int GetID() const;
	ORD_TYPE GetType() const;

	//abeer
	int getSpeed() const;
	int getNumOrdBbreak() const;
	int getBreakDur() const;
	int getnumofOrderdServed() const;
	//////////////////////////////////////////////////

	void setID(int);
	void setType(ORD_TYPE) ;

	//abeer
	void setSpeed(int); 
	void setNumOrdBbreak(int num);
	void setBreakDur(int bd);
	void setnumofOrderdServed(int num);
	//donya
	void set_RstPrd(int r);

	int get_RstPrd();
	void assign_Order(int order);
	int get_order();


};
