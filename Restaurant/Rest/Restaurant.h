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
	float InjProp;

	//Queue for each type of Cooks
	Queue<Cook*> NcooksQ;
	Queue<Cook*> GcooksQ;
	Queue<Cook*> VcooksQ;

	//Priority Queue For Busy Cooks and queues for inBreak cooks
	PriorityQueue<Cook*> busyCooksQ;

	//Queue<Cook*> NCooksInBreakQ;
	//Queue<Cook*> GCooksInBreakQ;
	//Queue<Cook*> VCooksInBreakQ;
	PriorityQueue<Cook*> CooksInBreak;
	Queue<Cook*>CooksInRest;

	//Priority Queue for vip orders depending on Priority Equation
	PriorityQueue<Order*>  QVIP_Order;
	//
	LinkedList<Order*> LNormal_Order;
	Queue<Order*> Qvegan_Order;
	Queue<Order**> QUrgentOrders;
	//We made it LinkedList only in this phase to be able to take order of each type
	//and put it in finished list as SimpleSimulator function requires
	//In General we choosed Priority Queue for this
	//LinkedList<Order*>InServing;    //////////////NMSA7HAAA
	PriorityQueue<Order*> InServing;
	Queue<Order*>FinishedList;
public:
	Restaurant();
	~Restaurant();
	void ExecuteEvents(int TimeStep);	//executes all events at current timestep
	void RunSimulation();
	void fileLoading();
	void SimpleSimulator();
	void Interactive();
	void FillDrawingList();
	void AddtoVIPQueue(Order* po);
	void AddtoNOList(Order* po);
	void AddtoVEQueue(Order* po);
	void cancellorder(int ID);
	void promoteorder(int ID, double exmoney);
	void Executepromotion(int CurrentTimeStep);
	void serve_VIP_orders(int CurrentTimeStep);
	void serve_Normal_orders(int CurrentTimeStep);
	void serve_Vegan_orders(int CurrentTimeStep);
	void getfrombusyCookQ(int CurrentTimeStep);
	void getfromBreakCookQ(int CurrentTimeStep);
	void CheckUrgentOrders(int CurrentTimeStep);
	void GetCooksFor_Urgent_VIP(int CurrentTimeStep);
	void Serve_Urgent_VIP(int CurrentTimeStep);
	int NCookNum , GCookNum , VCookNum;
	int NWaitNum, GWaitNum, VWaitNum;
	int SRVorders, AutoP, RstPrd, VIP_WT;
	int UrgentOredersNum=0;
	bool Health_Emergency();
	//// Added by abeer phase2
	//void addToBusyCQ(Cook* pC, Order* pO);
	//void addToInBreakCQ(Cook* pC);


};

#endif