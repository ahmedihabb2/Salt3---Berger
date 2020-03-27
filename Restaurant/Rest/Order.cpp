#include "Order.h"

Order::Order(int id, ORD_TYPE r_Type,const int&totalmoney)
{
	ID = (id>0&&id<1000)?id:0;	//1<ID<999
	type = r_Type;
	status = WAIT;
	WaitTime = 0;//initially 0 and will increment each time step
	totalMoney = totalmoney;
	
}

Order::~Order()
{
}

int Order::GetID()
{
	return ID;
}


ORD_TYPE Order::GetType() const
{
	return type;
}


void Order::SetDistance(int d)
{
	Distance = d>0?d:0;
}

int Order::GetDistance() const
{
	return Distance;
}


void Order::setStatus(ORD_STATUS s)
{
	status = s;
}

ORD_STATUS Order::getStatus() const
{
	return status;
}

//////////////////////Donia Additions
void Order::setOrderSize(int size)
{
	orderSize = size;
}

void Order::setPriority()
{
	Priority = float(totalMoney) / (ArrTime * orderSize);

}
int Order::getOrderSize()
{
	return orderSize;
}
int Order::getArrTime()
{
	return ArrTime;
}
int Order::getServTime()
{
	return ServTime;
}
int Order::getServInt()
{
	return srvInt;
}
int Order::getWaitTime()
{
	return WaitTime;
}
int Order::getFinishTime()
{
	return FinishTime;
}

float Order::getPriority()
{
	return  Priority;
}
void Order::Promote(Order& promoted,int& addedmoney)
{
	promoted.type = TYPE_VIP;
	promoted.totalMoney += addedmoney;
}
void Order::setArrTime(const int& arr)
{
	ArrTime = arr;
}

void  Order::setServTime(const int& serv)///as TimeStep
{
	ServTime = serv;
}
void  Order::setServInt(const int& serv)////as time interval
{
	srvInt = serv;/////will depend on the cook
}
void Order::setWaitTime(const int& Wait)
{
	WaitTime = ServTime -ArrTime;
}
void Order::setFinishTime()
{
	FinishTime = ArrTime + srvInt + WaitTime;
}
void Order::Serve(const int& ArrT, const int& servInt, const int& Size)
{
	setArrTime(ArrT);
	
	setFinishTime();


}