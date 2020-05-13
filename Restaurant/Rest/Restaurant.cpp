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
		Interactive();
	case MODE_STEP:
		break;
	case MODE_SLNT:
		break;
	//case MODE_DEMO:
		//SimpleSimulator();   //Simulator Function Just For Testing

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
//by Raghad

//promot Normer orders automatically promoted to VIP after it excess (Autop) time step in waiting List  
void Restaurant::Executepromotion(int CurrentTimeStep)
{

	Order* proOrder;
	while (LNormal_Order.peek(proOrder))                        //as long as there is more waiting Orders
	{
		if ((CurrentTimeStep - proOrder->getArrTime()) < AutoP) //no more event can be promoted in this time step
			return;

		LNormal_Order.DeleteFirst(proOrder);                   //remove order from Normal List
		double money = 0;
		proOrder->Promote(money);
		float priority = proOrder->getPriority();
		QVIP_Order.enqueue(proOrder, priority);                //add it to VIP queue
	}
	

}
//by raghad
void Restaurant::serve_VIP_orders(int CurrentTimeStep)
{
	Order* proOrder;
	float prio;
	while (QVIP_Order.peekFront(proOrder,prio))  //get orders from VIP waiting Queue to be serve
	{
		Cook* Bcook;
		if (VcooksQ.dequeue(Bcook))           //check if there is available VIP cook
		{
			
			proOrder->setServTime(CurrentTimeStep); //set serving time with current time step 
			
			int ST = ceil(float(proOrder->getOrderSize()) / Bcook->getSpeed()); //calculate the surving time
			proOrder->setServInt(ST);                                     
			proOrder->setWaitTime();
			proOrder->setFinishTime();
			float priority = 1/(proOrder->getFinishTime());             //set the priority of serving queue with the inverted finished time
			InServing.enqueue(proOrder, priority);
			busyCooksQ.enqueue(Bcook, priority);    //enque the cook in priority busy cook queue
			QVIP_Order.dequeue(proOrder, prio);
		}
		else if (NcooksQ.dequeue(Bcook))                              //check if there is available Normal cook when there is no VIP
		{
			
			proOrder->setServTime(CurrentTimeStep);                       //set serving time with current time step 

			int ST = ceil(float(proOrder->getOrderSize()) / Bcook->getSpeed()); //calculate the surving time
			proOrder->setServInt(ST);
			proOrder->setWaitTime();
			proOrder->setFinishTime();
			float priority = 1 / (proOrder->getFinishTime());             //set the priority of serving queue with the inverted finished time
			InServing.enqueue(proOrder, priority);
			busyCooksQ.enqueue(Bcook, priority);                       //enque the cook in priority busy cook queue
			QVIP_Order.dequeue(proOrder, prio);
		}
		else if (GcooksQ.dequeue(Bcook))                              //check if there is available Vegan cook when there is no VIP&&Normal
		{
			
			proOrder->setServTime(CurrentTimeStep); //set serving time with current time step 

			int ST = ceil(float(proOrder->getOrderSize()) / Bcook->getSpeed()); //calculate the surving time
			proOrder->setServInt(ST);
			proOrder->setWaitTime();
			proOrder->setFinishTime();
			float priority = 1 / (proOrder->getFinishTime());             //set the priority of serving queue with the inverted finished time
			InServing.enqueue(proOrder, priority);
			busyCooksQ.enqueue(Bcook, priority);                       //enque the cook in priority busy cook queue
			QVIP_Order.dequeue(proOrder, prio);

		}
		else
		{
			return;                                         //there is no more available cooks in this timestep 
		}
	}

}
//by raghad
void Restaurant::serve_Vegan_orders(int CurrentTimeStep)
{
	Order* proOrder;

	while (Qvegan_Order.peekFront(proOrder))  //get orders from Vegan waiting Queue to be serve
	{
		Cook* Bcook;
		if (GcooksQ.dequeue(Bcook))                              //check if there is available Vegan cook 
		{

			proOrder->setServTime(CurrentTimeStep); //set serving time with current time step 

			int ST = ceil(float(proOrder->getOrderSize() )/ Bcook->getSpeed()); //calculate the surving time
			proOrder->setServInt(ST);
			proOrder->setWaitTime();
			proOrder->setFinishTime();
			float priority = 1 / (proOrder->getFinishTime());             //set the priority of serving queue with the inverted finished time
			InServing.enqueue(proOrder, priority);
			busyCooksQ.enqueue(Bcook, priority);                       //enque the cook in priority busy cook queue
			Qvegan_Order.dequeue(proOrder);

		}
		else
		{
			return;                                         //there is no more available cooks in this timestep 
		}
	}
}





//by raghad
void Restaurant::serve_Normal_orders(int CurrentTimeStep)
{
	Order* proOrder;
	while (LNormal_Order.peek(proOrder))                   //get orders from Normal list to be serve
	{
		Cook* Bcook;
		if (NcooksQ.dequeue(Bcook))                              //check if there is available Normal cook 
		{

			proOrder->setServTime(CurrentTimeStep);                       //set serving time with current time step 

			int ST = ceil(float(proOrder->getOrderSize() )/ Bcook->getSpeed()); //calculate the surving time
			proOrder->setServInt(ST);
			proOrder->setWaitTime();
			proOrder->setFinishTime();
			float priority = 1 / (proOrder->getFinishTime());             //set the priority of serving queue with the inverted finished time
			InServing.enqueue(proOrder, priority);
			busyCooksQ.enqueue(Bcook, priority);                       //enque the cook in priority busy cook queue
			LNormal_Order.DeleteFirst(proOrder);
		}
		else if (VcooksQ.dequeue(Bcook))           //check if there is available VIP cook when there is no Normal
		{

			proOrder->setServTime(CurrentTimeStep); //set serving time with current time step 

			int ST = ceil(float(proOrder->getOrderSize()) / Bcook->getSpeed()); //calculate the surving time
			proOrder->setServInt(ST);
			proOrder->setWaitTime();
			proOrder->setFinishTime();
			float priority = 1 / (proOrder->getFinishTime());             //set the priority of serving queue with the inverted finished time
			InServing.enqueue(proOrder, priority);
			busyCooksQ.enqueue(Bcook, priority);                       //enque the cook in priority busy cook queue
			LNormal_Order.DeleteFirst(proOrder);
		}
		else
		{
			return;                                                   //there is no more available cooks in this timestep 
		}


	}

}

Restaurant::~Restaurant()
{
		if (pGUI)
			delete pGUI;
}

//Filling GUI with Orders and Cooks of All types

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
	Order** pON= LNormal_Order.toArray(NWaitNum);
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
/////////////////////////////////////////////////////
//Reading inputs from File and filling suitable lists
/////////////////////////////////////////////////////
void Restaurant::fileLoading() 
{
	pGUI->PrintMessage("Enter file name");
	filename = pGUI->GetString();

	ifstream InFile(filename);
	
	if (InFile.is_open())
	{
		int numNcooks, numGcooks, numVcooks, Ncookspeed_min, Ncookspeed_max, Gcookspeed_min, Gcookspeed_max, Vcookspeed_min, Vcookspeed_max;
		int numOrdersBbreak, Nbreak_min, Nbreak_max, Gbreak_min, Gbreak_max, Vbreak_min  , Vbreak_max, numofevents ;
		
		InFile >> numNcooks >> numGcooks >> numVcooks >> Ncookspeed_min>> Ncookspeed_max>> Gcookspeed_min>> Gcookspeed_max>> Vcookspeed_min>> Vcookspeed_max;

		InFile >> numOrdersBbreak >> Nbreak_min>> Nbreak_max>> Gbreak_min>> Gbreak_max>> Vbreak_min>> Vbreak_max >> InjProp >> RstPrd >> AutoP>> VIP_WT>>numofevents;

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
			srand((unsigned)time(0));
			newNCook->setSpeed(Ncookspeed_min+rand() % Ncookspeed_max );
			newNCook->setNumOrdBbreak(numOrdersBbreak);
			newNCook->setBreakDur(Nbreak_min+rand() % Nbreak_max);
			NcooksQ.enqueue(newNCook);
		}

		for (int i = 0; i < numGcooks; i++)
		{

			Cook* newGCook = new Cook;

			newGCook->setID(arrCIDs[i + numNcooks + 1]);
			newGCook->setType(TYPE_VGAN);
			newGCook->setSpeed(rand() % ((Gcookspeed_max - Gcookspeed_min) + 1) + Gcookspeed_min);
			newGCook->setNumOrdBbreak(numOrdersBbreak);
			newGCook->setBreakDur(rand() % ((Gbreak_min - Gbreak_max) + 1));

			GcooksQ.enqueue(newGCook);
		}

		for (int i = 0; i < numVcooks; i++)
		{
			Cook* newVCook = new Cook;

			newVCook->setID(arrCIDs[i + numNcooks + numGcooks + 1]);
			newVCook->setType(TYPE_VIP);
			newVCook->setSpeed(rand() % ((Vcookspeed_min - Vcookspeed_max) + 1));
			newVCook->setNumOrdBbreak(numOrdersBbreak);
			newVCook->setBreakDur(rand() % ((Gbreak_min - Gbreak_max) + 1));

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


void Restaurant::AddtoVIPQueue(Order* po)
{
	float priority = po->getPriority();
	QVIP_Order.enqueue(po, priority);

}
void Restaurant::AddtoNOList(Order* po)
{
	LNormal_Order.InsertEnd(po);

}
void Restaurant::AddtoVEQueue(Order* po)
{
	Qvegan_Order.enqueue(po);

}
void Restaurant::cancellorder(int Id)
{

	Order* del;
	Node<Order*>* prv = LNormal_Order.getHead();
	if (prv&&prv->getItem()->GetID() == Id)
		LNormal_Order.DeleteFirst(del);
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
	Node<Order*>* prv = LNormal_Order.getHead();
	if (prv&&prv->getItem()->GetID() == Id)
	{
		Order* proOrder;
		LNormal_Order.DeleteFirst(proOrder);
	    proOrder->Promote(exmoney);
		float priority = proOrder->getPriority();
		QVIP_Order.enqueue(proOrder, priority);
		
	}
	else if(prv)
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

//Testinf Function

/*void Restaurant::SimpleSimulator()
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
		//Picking one order of each type and adding it to serving list
		if (!LNormal_Order.isEmpty())
		{
			LNormal_Order.DeleteFirst(NpO);
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
		//each 5 timesteps pick one order of each type and add it to finished orders
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
		//Printing Cooks and Orders Information
		pGUI->PrintMessage("Wating Orders ->  Normal : " + to_string(NWaitNum) +" Vegan :"  + to_string(GWaitNum) + " VIP : "+to_string(VWaitNum) , 2);
		pGUI->PrintMessage("Available Cooks - >  Normal : " + to_string(NCookNum) + " Vegan :" + to_string(GCookNum) + " VIP :" + to_string(VCookNum), 3);
		pGUI->UpdateInterface();
		Sleep(1000);
		//pGUI->waitForClick();
		CurrentTimeStep++;
		pGUI->ResetDrawingList();
		
	}
		pGUI->PrintMessage("Simulation Done ... Click To Exit");
		pGUI->waitForClick();
}
*/
//phase2
//void Restaurant::addToBusyCQ(Cook* pC, Order* pO)
//{
//	float priority = pO->getFinishTime();
//	busyCooksQ.enqueue(pC, priority);
//}
//
//void Restaurant::addToInBreakCQ(Cook* pC)
//{
//
//}

void Restaurant::Interactive()
{
	pGUI->PrintMessage("Welcome To Our Restaurant .... Interactive Mode, Click To Continue");
	pGUI->waitForClick();
	fileLoading();
	int CurrentTimeStep = 1;
	while (!EventsQueue.isEmpty() || !InServing.isEmpty())
	{
		char timestep[100];
		itoa(CurrentTimeStep, timestep, 10);
		pGUI->PrintMessage(timestep, 1);
		ExecuteEvents(CurrentTimeStep);
		serve_VIP_orders(CurrentTimeStep);
		serve_Vegan_orders(CurrentTimeStep);
		serve_Normal_orders(CurrentTimeStep);
		FillDrawingList();
		//Printing Cooks and Orders Information
		pGUI->PrintMessage("Wating Orders ->  Normal : " + to_string(NWaitNum) + " Vegan :" + to_string(GWaitNum) + " VIP : " + to_string(VWaitNum), 2);
		pGUI->PrintMessage("Available Cooks - >  Normal : " + to_string(NCookNum) + " Vegan :" + to_string(GCookNum) + " VIP :" + to_string(VCookNum), 3);
		pGUI->UpdateInterface();
		Sleep(1000);
		CurrentTimeStep++;
		pGUI->ResetDrawingList();
	}
}

