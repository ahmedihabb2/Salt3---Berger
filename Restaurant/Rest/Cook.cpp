#include "Cook.h"


Cook::Cook()
{
	RstPrd = 0;
}


Cook::~Cook()
{
}


int Cook::GetID() const
{
	return ID;
}


ORD_TYPE Cook::GetType() const
{
	return type;
}

int Cook::getSpeed() const
{
	return speed;
}

int Cook::getNumOrdBbreak() const
{
	return numOrdersBefBreak;
}

int Cook::getBreakDur() const
{
	return breakDuration;
}

void Cook::setID(int id)
{
	ID = id;
}

void Cook::setType(ORD_TYPE t)
{
	type = t;
}

void Cook::setSpeed(int s)
{
	speed = s;
}

void Cook::setNumOrdBbreak(int num)
{
	numOrdersBefBreak = num;
}

void Cook::setBreakDur(int bd)
{
	breakDuration = bd;
}
/////////////donya\\\\\\\\\\\\\\\\

void Cook::set_RstPrd(int r)
{
	if (r >= 0)
		RstPrd = r;

}

int Cook::get_RstPrd()
{
	return RstPrd;
}

void Cook::assign_Order(int order)
{
	ord_assigned = order;
}
int Cook::get_order()
{
	return ord_assigned;
}