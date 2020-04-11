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
	//Converting The  Cooks Q to Array to iterate on it and add cooks to drawing list
	Cook** pC= NcooksQ.toArray(NCookNum);
	for (int i = 0; i < NCookNum; i++)
	{
		pGUI->AddToDrawingList(pC[i]);
	}
	pC = GcooksQ.toArray(GCookNum);
	for (int i = 0; i < GCookNum; i++)
	{
		pGUI->AddToDrawingList(pC[i]);
	}
	pC = VcooksQ.toArray(VCookNum);
	for (int i = 0; i < VCookNum; i++)
	{
		pGUI->AddToDrawingList(pC[i]);
	}
	/////////////////////////////////////////
	//end of drawing all cooks types
	//////////////////////////////////////
	//Converting The  Orders Q to Array to iterate on it and add ordrs to drawing list
	Order** pON= QNormal_Order.toArray(NWaitNum);
		for (int i = 0; i < NWaitNum;i++)
		 pGUI->AddToDrawingList(pON[i]);

	Order** pOG = Qvegan_Order.toArray(GWaitNum);
	for (int i = 0; i < GWaitNum ;i++)
		pGUI->AddToDrawingList(pOG[i]);

	Order** pOV = QVIP_Order.toArray(VWaitNum);
	for (int i = 0; i < VWaitNum;i++)
		pGUI->AddToDrawingList(pOV[i]);

	int size = 0;
	Order** SO = InServing.toArray(SRVorders);
		for (int i = 0; i < SRVorders;i++)
			pGUI->AddToDrawingList(SO[i]);
		size = 0;
	Order** FD = FinishedList.toArray(size);
		for (int i = 0; i < size;i++)
			pGUI->AddToDrawingList(FD[i]);


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
	if (prv&&prv->getItem()->GetID() == Id)
		QNormal_Order.setfront(prv->getNext());
	else if(prv) 
	{
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


void Restaurant::SimpleSimulator()
{
	Order *NpO,*GpO,*VpO,*pO;
	float pr;
	
	pGUI->PrintMessage("Simple Simulator Function is Running .... Click to continue");
	pGUI->waitForClick();
	fileLoading();
	int CurrentTimeStep = 1;
	while (!EventsQueue.isEmpty() || !InServing.isEmpty())
	{

		char timestep[100];
		itoa(CurrentTimeStep, timestep, 10);
		pGUI->PrintMessage(timestep,1);
		ExecuteEvents(CurrentTimeStep);
		if (!QNormal_Order.isEmpty())
		{
			QNormal_Order.dequeue(NpO);
			NpO->setStatus(SRV);
			InServing.InsertEnd(NpO);
		}
		if (!Qvegan_Order.isEmpty())
		{
			Qvegan_Order.dequeue(GpO);
			GpO->setStatus(SRV);
			InServing.InsertEnd(GpO);
		}
		if (!QVIP_Order.isEmpty())
		{
			QVIP_Order.dequeue(VpO, pr);
			VpO->setStatus(SRV);
			InServing.InsertEnd(VpO);
		}
		if (CurrentTimeStep % 5 == 0)
		{
			Node<Order*>* temp = InServing.getHead();
			while (temp)
			{
				if (temp->getItem()->GetType() == TYPE_NRM )
				{
					temp->getItem()->setStatus(DONE);
					FinishedList.enqueue(temp->getItem());
					InServing.DeleteNode(temp->getItem());
					break;
				}
				temp = temp->getNext();
			}
			temp = InServing.getHead();
			while (temp)
			{
				if (temp->getItem()->GetType() == TYPE_VGAN)
				{
					temp->getItem()->setStatus(DONE);
					FinishedList.enqueue(temp->getItem());
					InServing.DeleteNode(temp->getItem());
					break;
				}
				temp = temp->getNext();
			}
			temp = InServing.getHead();
			while (temp)
			{
				if (temp->getItem()->GetType() == TYPE_VIP)
				{
					temp->getItem()->setStatus(DONE);
					FinishedList.enqueue(temp->getItem());
					InServing.DeleteNode(temp->getItem());
					break;
				}
				temp = temp->getNext();
			}
		}
		FillDrawingList();
		pGUI->PrintMessage("Wating Orders ->  Normal : " + to_string(NWaitNum) +" Vegan :"  + to_string(GWaitNum) + " VIP : "+to_string(VWaitNum) , 2);
		pGUI->PrintMessage("Available Cooks - >  Normal : " + to_string(NCookNum) + " Vegan :" + to_string(GCookNum) + " VIP :" + to_string(VCookNum), 3);
		pGUI->UpdateInterface();
		//Sleep(1000);
		pGUI->waitForClick();
		CurrentTimeStep++;
		pGUI->ResetDrawingList();
		
	}
		pGUI->PrintMessage("Simulation Done ... Click To Exit");
		pGUI->waitForClick();
}



