#include "Cook.h"


Cook::Cook()
{
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


int Cook::getnumofOrderdServed() const
{
	return numofOrderdServed;
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

void Cook::setnumofOrderdServed(int num)
{
	numofOrderdServed = num;
}