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

	string filename; 
	
	/// ==> 
	//	DEMO-related members. Should be removed in phases 1&2
	Queue<Order*> DEMO_Queue;	//Important: This is just for demo
	/// ==>
	
	//Added By Ahmed
	Queue<Cook> NcooksQ;
	Queue<Cook> GcooksQ;
	Queue<Cook> VcooksQ;
	
	//
	// TODO: Add More Data Members As Needed
	//
	// added by raghad
	PriorityQueue<Order*>  QVIP_Order;
	Queue<Order*> QNormal_Order;
	Queue<Order*> Qvegan_Order;

public:
	
	Restaurant();
	~Restaurant();
	
	void ExecuteEvents(int TimeStep);	//executes all events at current timestep
	void RunSimulation();

	void fileLoading();


	void FillDrawingList();

	//
	// TODO: Add More Member Functions As Needed
	//added by raghad
	void AddtoVIPQueue(Order* po);
	void AddtoNOQueue(Order* po);
	void AddtoVEQueue(Order* po);
	void cancellorder(int ID);
	void promoteorder(int ID, double exmoney);

/// ===================    DEMO-related functions. Should be removed in phases 1&2   ================= 

	void Just_A_Demo();	//just to show a demo and should be removed in phase1 1 & 2
	void AddtoDemoQueue(Order* po);	//adds an order to the demo queue

/// ================================================================================================== 



};

#endif