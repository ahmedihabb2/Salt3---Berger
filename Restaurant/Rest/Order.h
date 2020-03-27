#ifndef __ORDER_H_
#define __ORDER_H_

#include "..\Defs.h"

class Order
{

protected:
	int ID;         //Each order has a unique ID (from 1 --> 999 )
	ORD_TYPE type;		//order type: Normal, vegan, VIP
	ORD_STATUS status;	//waiting, in-service, done
	int Distance;	//The distance (in meters) between the order location and the resturant 

	double totalMoney;	//Total order money

	int ArrTime, ServTime, FinishTime;	//arrival, service start, and finish times

	//
	// TODO: Add More Data Members As Needed
	//
	int orderSize;
	float Priority;
	int WaitTime;
	int srvInt;

public:
	Order(int ID, ORD_TYPE r_Type,  const int& totalmoney);
	virtual ~Order();

	int GetID();


	ORD_TYPE GetType() const;

	void SetDistance(int d);
	int GetDistance() const;

	void setStatus(ORD_STATUS s);
	ORD_STATUS getStatus() const;

	//
	// TODO: Add More Member Functions As Needed
	//Donia Addition
	void setPriority();
	void setOrderSize(int size);
	float getPriority();
	int getOrderSize();
	int getArrTime();
	int getServTime();
	int getServInt();
	int getWaitTime();
	int getFinishTime();
	void Promote(Order& promoted, int& addedmoney);
	void setArrTime(const int& arr);

	void setServTime(const int& serv);
	void setServInt(const int& serv);
	void setFinishTime();
	void setWaitTime(const int&);
	void Serve(const int& ArrT, const int& Servtime, const int& Size);//
};

#endif