#ifndef __RESTAURANT_H_
#define __RESTAURANT_H_

#include "..\Defs.h"
#include "..\CMUgraphicsLib\CMUgraphics.h"
#include "..\GUI\GUI.h"
#include "..\Generic_DS\Queue.h"
#include "..\Generic_DS\PriorityQueue.h"
#include "..\Events\Event.h"



#include <fstream>

#include "Order.h"

// it is the maestro of the project
class Restaurant  
{	
private:
	GUI *pGUI;
	Queue<Event*> EventsQueue;	//Queue of all events that will be loaded from file

	
	/// ==> 
	//	DEMO-related members. Should be removed in phases 1&2
	Queue<Order*> DEMO_Queue;	//Important: This is just for demo
	/// ==>

		// TODO: Add More Data Members As Needed

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

	//Queue To Array
	Order** pON;
	Order** pOG;
	Order** pOV;

public:
	
	Restaurant();
	~Restaurant();
	
	void ExecuteEvents(int TimeStep);	//executes all events at current timestep
	void RunSimulation();

	void fileLoading();

	void SimpleSimulator();
	void FillDrawingList();

	//
	// TODO: Add More Member Functions As Needed
	//added by raghad
	void AddtoVIPQueue(Order* po);
	void AddtoNOQueue(Order* po);
	void AddtoVEQueue(Order* po);
	void cancellorder(int ID);
	void promoteorder(int ID, double exmoney);

	//// Added by abeer phase2
	//void addToBusyCQ(Cook* pC, Order* pO);
	//void addToInBreakCQ(Cook* pC);


/// ===================    DEMO-related functions. Should be removed in phases 1&2   ================= 

	void Just_A_Demo();	//just to show a demo and should be removed in phase1 1 & 2
	void AddtoDemoQueue(Order* po);	//adds an order to the demo queue

/// ================================================================================================== 



};

#endif