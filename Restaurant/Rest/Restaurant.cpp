#include <cstdlib>
#include <time.h>
#include <iostream>
using namespace std;

#include "Restaurant.h"
#include "..\Events\ArrivalEvent.h"

#include "..\Events\CancellationEvent.h"
#include"..\Events\PromotionEvent.h"

Restaurant::Restaurant() 
{
	pGUI = NULL;
}

void Restaurant::RunSimulation()
{
	pGUI = new GUI;
	PROG_MODE	mode = pGUI->getGUIMode();
		
	switch (mode)	//Add a function for each mode in next phases
	{
	case MODE_INTR:
		break;
	case MODE_STEP:
		break;
	case MODE_SLNT:
		break;
	case MODE_DEMO:
		SimpleSimulator();

	};

}



//////////////////////////////////  Event handling functions   /////////////////////////////

//Executes ALL events that should take place at current timestep
void Restaurant::ExecuteEvents(int CurrentTimeStep)
{
	Event *pE;
	while( EventsQueue.peekFront(pE) )	//as long as there are more events
	{
		if(pE->getEventTime() > CurrentTimeStep )	//no more events at current timestep
			return;

		pE->Execute(this);
		EventsQueue.dequeue(pE);	//remove event from the queue
		delete pE;		//deallocate event object from memory
	}

}


Restaurant::~Restaurant()
{
		if (pGUI)
			delete pGUI;
}

void Restaurant::FillDrawingList()
{
	int size = 0;
	//Converting The  Cooks Q to Array to iterate on it and add cooks to drawing list
	Cook** pC= NcooksQ.toArray(size);
	for (int i = 0; i < size; i++)
	{
		pGUI->AddToDrawingList(pC[i]);
	}
	size = 0;
	pC = GcooksQ.toArray(size);
	for (int i = 0; i < size; i++)
	{
		pGUI->AddToDrawingList(pC[i]);
	}
	size = 0;
	pC = VcooksQ.toArray(size);
	for (int i = 0; i < size; i++)
	{
		pGUI->AddToDrawingList(pC[i]);
	}
	/////////////////////////////////////////
	//end of drawing all cooks types
	//////////////////////////////////////
	//Converting The  Orders Q to Array to iterate on it and add ordrs to drawing list
	size = 0;
	pON= QNormal_Order.toArray(size);
		for (int i = 0; i < size;i++)
		 pGUI->AddToDrawingList(pON[i]);
	
	size = 0;
	pOG = Qvegan_Order.toArray(size);
	for (int i = 0; i < size;i++)
		pGUI->AddToDrawingList(pOG[i]);

	size = 0;
	pOV = QVIP_Order.toArray(size);
	for (int i = 0; i < size;i++)
		pGUI->AddToDrawingList(pOV[i]);


}

void Restaurant::fileLoading() //abeer added this //not complete yet
{
	pGUI->PrintMessage("Enter file name");
	filename = pGUI->GetString();

	ifstream InFile(filename);
	
	if (InFile.is_open())
	{
		int numNcooks, numGcooks, numVcooks, Ncookspeed, Gcookspeed, Vcookspeed;
		int numOrdersBbreak, Nbreak, Gbreak, Vbreak , AutoP , numofevents ;

		InFile >> numNcooks >> numGcooks >> numVcooks >> Ncookspeed >> Gcookspeed >> Vcookspeed;

		InFile >> numOrdersBbreak >> Nbreak >> Gbreak >> Vbreak>> AutoP>> numofevents;

		int numAllcooks = numNcooks + numGcooks + numVcooks;
		int* arrCIDs = new int[numAllcooks];
		for (int i = 1; i <= numAllcooks; i++)
		{
			arrCIDs[i] = i;
		}

		for (int i = 0; i < numNcooks; i++)
		{
			Cook* newNCook = new Cook();
	
			newNCook->setID(arrCIDs[i + 1]);
			newNCook->setType(TYPE_NRM);
			newNCook->setSpeed(Ncookspeed);
			newNCook->setNumOrdBbreak(numOrdersBbreak);
			newNCook->setBreakDur(Nbreak);

			NcooksQ.enqueue(newNCook);
		}

		for (int i = 0; i < numGcooks; i++)
		{

			Cook* newGCook = new Cook;

			newGCook->setID(arrCIDs[i + numNcooks + 1]);
			newGCook->setType(TYPE_VGAN);
			newGCook->setSpeed(Gcookspeed);
			newGCook->setNumOrdBbreak(numOrdersBbreak);
			newGCook->setBreakDur(Gbreak);

			GcooksQ.enqueue(newGCook);
		}

		for (int i = 0; i < numVcooks; i++)
		{
			Cook* newVCook = new Cook;

			newVCook->setID(arrCIDs[i + numNcooks + numGcooks + 1]);
			newVCook->setType(TYPE_VIP);
			newVCook->setSpeed(Vcookspeed);
			newVCook->setNumOrdBbreak(numOrdersBbreak);
			newVCook->setBreakDur(Vbreak);

			VcooksQ.enqueue(newVCook);
		}

		//added by raghad
		for (int i = 0; i < numofevents; i++)
		{
			char typeofevent;
			InFile >> typeofevent;
			Event* pEv;
			if (typeofevent == 'R')
			{
				char ordertype;
				int Ordertype, arrivaltime, ID, Size;
					double Mony;
				InFile >> ordertype>>arrivaltime>> ID>> Size>> Mony;
				if (ordertype == 'N')
					pEv = new ArrivalEvent(arrivaltime, ID, TYPE_NRM, Size, Mony);
				else if (ordertype == 'G')
					pEv = new ArrivalEvent(arrivaltime, ID, TYPE_VGAN, Size, Mony);
				else if (ordertype == 'V')
					pEv = new ArrivalEvent(arrivaltime, ID, TYPE_VIP, Size, Mony);

			}
			if (typeofevent == 'P')
			{
				int promotiontime, ID;
				double exmony;
				InFile >> promotiontime >> ID >> exmony;
				pEv = new PromotionEvent(promotiontime,ID,exmony);
			}
			if (typeofevent == 'X')
			{
				int cancellationtime, ID;
				InFile >> cancellationtime >> ID;
				pEv = new CancellationEvent(cancellationtime, ID);

			}

				
			
			EventsQueue.enqueue(pEv);
		}
	}
	
}


//////////////////////////////////////////////////////////////////////////////////////////////
/// ==> 
///  DEMO-related functions. Should be removed in phases 1&2

//Begin of DEMO-related functions

//This is just a demo function for project introductory phase
//It should be removed starting phase 1
void Restaurant::Just_A_Demo()
{

	fileLoading();
	//
	// THIS IS JUST A DEMO FUNCTION
	// IT SHOULD BE REMOVED IN PHASE 1 AND PHASE 2

	int EventCnt;
	Order* pOrd;
	Event* pEv;
	srand(time(NULL));

	pGUI->PrintMessage("Just a Demo. Enter EVENTS Count(next phases should read I/P filename):");
	EventCnt = atoi(pGUI->GetString().c_str());	//get user input as a string then convert to integer

	pGUI->PrintMessage("Generating Events randomly... In next phases, Events should be loaded from a file...CLICK to continue");
	pGUI->waitForClick();

	//Just for sake of demo, generate some cooks and add them to the drawing list
	//In next phases, Cooks info should be loaded from input file
	int C_count = 12;
	Cook* pC = new Cook[C_count];
	int cID = 1;

	/*for (int i = 0; i < C_count; i++)
	{
		cID += (rand() % 15 + 1);
		pC[i].setID(cID);
		pC[i].setType((ORD_TYPE)(rand() % TYPE_CNT));
	}*/


	int EvTime = 0;

	int O_id = 1;

	//Create Random events and fill them into EventsQueue
	//All generated event will be "ArrivalEvents" for the demo
	/*for (int i = 0; i < EventCnt; i++)
	{
		O_id += (rand() % 4 + 1);
		int OType = rand() % TYPE_CNT;	//Randomize order type		
		EvTime += (rand() % 5 + 1);			//Randomize event time
		pEv = new ArrivalEvent(EvTime, O_id, (ORD_TYPE)OType);
		EventsQueue.enqueue(pEv);

	}*/

	// --->   In next phases, no random generation is done
	// --->       EventsQueue should be filled from actual events loaded from input file





	//Now We have filled EventsQueue (randomly)
	int CurrentTimeStep = 1;


	//as long as events queue is not empty yet
	while (!EventsQueue.isEmpty())
	{
		//print current timestep
		char timestep[10];
		itoa(CurrentTimeStep, timestep, 10);
		pGUI->PrintMessage(timestep);


		//The next line may add new orders to the DEMO_Queue
		ExecuteEvents(CurrentTimeStep);	//execute all events at current time step


/////////////////////////////////////////////////////////////////////////////////////////
		/// The next code section should be done through function "FillDrawingList()" once you
		/// decide the appropriate list type for Orders and Cooks

	/*	//Let's add ALL randomly generated Cooks to GUI::DrawingList
		for (int i = 0; i < C_count; i++)
			pGUI->AddToDrawingList(&pC[i]);

		//Let's add ALL randomly generated Ordes to GUI::DrawingList
		int size = 0;
		Order** Demo_Orders_Array = DEMO_Queue.toArray(size);

		for (int i = 0; i < size; i++)
		{
			pOrd = Demo_Orders_Array[i];
			pGUI->AddToDrawingList(pOrd);
		}*/
		FillDrawingList();
		/////////////////////////////////////////////////////////////////////////////////////////

		pGUI->UpdateInterface();
		Sleep(1000);
		CurrentTimeStep++;	//advance timestep
		pGUI->ResetDrawingList();
	}

	delete []pC;


	pGUI->PrintMessage("generation done, click to END program");
	pGUI->waitForClick();

	
}
////////////////
//added by raghad
void Restaurant::AddtoVIPQueue(Order* po)
{
	float priority = po->getPriority();
	QVIP_Order.enqueue(po, priority);

}
void Restaurant::AddtoNOQueue(Order* po)
{
	QNormal_Order.enqueue(po);

}
void Restaurant::AddtoVEQueue(Order* po)
{
	Qvegan_Order.enqueue(po);

}
void Restaurant::cancellorder(int Id)
{
	Node<Order*>* prv = QNormal_Order.getfront();
	if (prv->getItem()->GetID() == Id)
		QNormal_Order.setfront(prv->getNext());
	else {
		Node<Order*>* Head = prv->getNext();
		while (Head)
		{
			if (Head->getItem()->GetID() == Id)
			{

				prv->setNext(Head->getNext());
				delete Head;
				break;
			}
			else
			{
				prv = Head;
				Head = Head->getNext();
			}
		}
	}
}


void Restaurant::promoteorder(int Id, double exmoney)
{
	Node<Order*>* prv = QNormal_Order.getfront();
	if (prv->getItem()->GetID() == Id)
	{
		Order* proOrder;
		QNormal_Order.dequeue(proOrder);
	    proOrder->Promote(exmoney);
		float priority = proOrder->getPriority();
		QVIP_Order.enqueue(proOrder, priority);
		
	}
	else
	{
		Node<Order*>* Head = prv->getNext();
		while (Head)
		{
			if (Head->getItem()->GetID() == Id)
			{


				Node<Order*>* proOrder = Head;
				prv->setNext(Head->getNext());
				proOrder->getItem()->Promote(exmoney);
				float priority = proOrder->getItem()->getPriority();
				QVIP_Order.enqueue(proOrder->getItem(), priority);
			
				//delete Head;
				return;
				
			}
			else
			{
				prv = Head;
				Head = Head->getNext();
			}
		}


		
	}
	return; ///if ID isn't in Qnormal
}

void Restaurant::AddtoDemoQueue(Order *pOrd)
{
	DEMO_Queue.enqueue(pOrd);
}


void Restaurant::SimpleSimulator()
{
	Order *pO;
	pGUI->PrintMessage("Simple Simulator Function is Running .... Click to continue");
	pGUI->waitForClick();
	fileLoading();
	int CurrentTimeStep = 1;
	while (!EventsQueue.isEmpty())
	{

		char timestep[100];
		itoa(CurrentTimeStep, timestep, 10);
		pGUI->PrintMessage(timestep);
		ExecuteEvents(CurrentTimeStep);
		FillDrawingList();
		pGUI->UpdateInterface();
		Sleep(1000);
		CurrentTimeStep++;
		pGUI->ResetDrawingList();
		if (CurrentTimeStep > 1 && !QNormal_Order.isEmpty())
		{
			//just for testing mshhhh saaaaaaaa7
			QNormal_Order.peekFront(pO);
			pO->setStatus(SRV);
		}
		
	}
	pGUI->waitForClick();
}


