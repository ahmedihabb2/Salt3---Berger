#ifndef __RESTAURANT_H_
#define __RESTAURANT_H_

#include "..\Defs.h"
#include "..\CMUgraphicsLib\CMUgraphics.h"
#include "..\GUI\GUI.h"
#include "..\Generic_DS\Queue.h"
#include "..\Generic_DS\PriorityQueue.h"
#include "..\Events\Event.h"
#include"..\Generic_DS\LinkedList.h"
#include"..\LinkedList.h"

#include <fstream>

#include "Order.h"

// it is the maestro of the project
class Restaurant  
{	
private:
	GUI *pGUI;
	Queue<Event*> EventsQueue;	//Queue of all events that will be loaded from file

	string filename;

	//Queue for each type of Cooks
	Queue<Cook*> NcooksQ;
	Queue<Cook*> GcooksQ;
	Queue<Cook*> VcooksQ;
	//Priority Queue For Busy Cooks
	PriorityQueue<Cook*> busyCooksQ;
	PriorityQueue<Cook*> inBreakCooksQ;

	//Priority Queue for vip orders depending on Priority Equation
	PriorityQueue<Order*>  QVIP_Order;
	//
	LinkedList<Order*> LNormal_Order;
	Queue<Order*> Qvegan_Order;
	LinkedList<Order*>InServing;
	Queue<Order*>FinishedList;

	
public:
	
	Restaurant();
	~Restaurant();
	
	void ExecuteEvents(int TimeStep);	//executes all events at current timestep
	void RunSimulation();

	void fileLoading();
	void SimpleSimulator();
	void FillDrawingList();

	void AddtoVIPQueue(Order* po);
	void AddtoNOList(Order* po);
	void AddtoVEQueue(Order* po);
	void cancellorder(int ID);
	void promoteorder(int ID, double exmoney);
	int NCookNum , GCookNum , VCookNum;
	int NWaitNum, GWaitNum, VWaitNum;
	int SRVorders;
	
	//// Added by abeer phase2
	//void addToBusyCQ(Cook* pC, Order* pO);
	//void addToInBreakCQ(Cook* pC);

};

#endif