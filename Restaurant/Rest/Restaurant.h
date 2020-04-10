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

	//Added By Abeer //Brought as data members here by Ahmed
	Queue<Cook*> NcooksQ;
	Queue<Cook*> GcooksQ;
	Queue<Cook*> VcooksQ;
	
	PriorityQueue<Cook*> busyCooksQ;
	PriorityQueue<Cook*> inBreakCooksQ;

	//
	//
	// added by raghad
	PriorityQueue<Order*>  QVIP_Order;
	Queue<Order*> QNormal_Order;
	Queue<Order*> Qvegan_Order;
	LinkedList<Order*>InServing;
	Queue<Order*>FinishedList;

	
public:
	
	Restaurant();
	~Restaurant();
	
	void ExecuteEvents(int TimeStep);	//executes all events at current timestep
	void RunSimulation();

	void fileLoading();
	void testprint();
	void SimpleSimulator();
	void FillDrawingList();

	//added by raghad
	void AddtoVIPQueue(Order* po);
	void AddtoNOQueue(Order* po);
	void AddtoVEQueue(Order* po);
	void cancellorder(int ID);
	void promoteorder(int ID, double exmoney);
	//added by ahmed
	int NCookNum , GCookNum , VCookNum;
	int NWaitNum, GWaitNum, VWaitNum;
	int SRVorders;
	
	//// Added by abeer phase2
	//void addToBusyCQ(Cook* pC, Order* pO);
	//void addToInBreakCQ(Cook* pC);

};

#endif