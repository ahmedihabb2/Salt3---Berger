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
	PROG_MODE mode = pGUI->getGUIMode();
		
	switch (mode)	//Add a function for each mode in next phases
	{
	case MODE_INTR:
		Restaurant_Modes(1);
		break;
	case MODE_STEP:
		Restaurant_Modes(2);
		break;
	case MODE_SLNT:
		Restaurant_Modes(3);
		break;
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
//by Ahmed
//Serving VIP orders after wating time excess VIP_WT to any cook in Break or Rest
void Restaurant::CheckUrgentOrders(int CurrentTimeStep)
{
	Order**vipordersarr;
	int size;
	vipordersarr = QVIP_Order.toArray(size);
	for (int i = 0; i < size;i++)
	{
		if (CurrentTimeStep - vipordersarr[i]->getArrTime() > VIP_WT)
		{

			vipordersarr[i]->setUrgent(true);
			QUrgentOrders.enqueue(&vipordersarr[i]);
			UrgentOredersNum++;
		}
	}
}
bool Restaurant::GetCooksFor_Urgent_VIP(int CurrentTimeStep)
{
	Order** UrgOrder;
	Cook* urgentcook;
	float cookprio;
	bool found = false;
	if (QUrgentOrders.peekFront(UrgOrder))
	{
		if (VcooksQ.isEmpty() && GcooksQ.isEmpty() && NcooksQ.isEmpty())
		{
			if (CooksInBreak.peekFront(urgentcook, cookprio))        //When waiting time exceeds VIP_WT, we check for
			{                                                        // is there is Cooks in break queue
				CooksInBreak.dequeue(urgentcook, cookprio);
				if (urgentcook->GetType() == TYPE_VIP)               //return the cook to it's suitable list
					VcooksQ.enqueue(urgentcook);
				if (urgentcook->GetType() == TYPE_VGAN)
					GcooksQ.enqueue(urgentcook);
				if (urgentcook->GetType() == TYPE_NRM)
					NcooksQ.enqueue(urgentcook);
				found = true;
			}                                                       //if there is no cooks in break we check for
			else if (CooksInRest.peekFront(urgentcook))             //the cooks in rest but they work with half speed
			{
				CooksInRest.dequeue(urgentcook);
				urgentcook->setSpeed(urgentcook->getSpeed() / 2);
				urgentcook->Give_Urg(true);
				if (urgentcook->GetType() == TYPE_VIP)
					VcooksQ.enqueue(urgentcook);
				if (urgentcook->GetType() == TYPE_VGAN)
					GcooksQ.enqueue(urgentcook);
				if (urgentcook->GetType() == TYPE_NRM)
					NcooksQ.enqueue(urgentcook);
				found = true;
			}
			
		}
		else
			found = true;
	}
	return found;
}
void Restaurant::Serve_Urgent_VIP(int CurrentTimeStep)
{
	bool flag;
	CheckUrgentOrders(CurrentTimeStep);
	flag=GetCooksFor_Urgent_VIP(CurrentTimeStep);
	Order** Urgorder;
	if (flag)
	{
		while (QUrgentOrders.peekFront(Urgorder))
		{
			Cook* Bcook;
			if (VcooksQ.dequeue(Bcook))           //check if there is available VIP cook
			{
				Bcook->setnumofOrderdServed(Bcook->getnumofOrderdServed() + 1);
				(*Urgorder)->setServTime(CurrentTimeStep); //set serving time with current time step 

				int ST = ceil(float((*Urgorder)->getOrderSize()) / Bcook->getSpeed()); //calculate the surving time
				(*Urgorder)->setServInt(ST);
				(*Urgorder)->setWaitTime();
				(*Urgorder)->setFinishTime();
				(*Urgorder)->setStatus(SRV);
				float priority = ((*Urgorder)->getFinishTime());             //set the priority of serving queue with the inverted finished time
				InServing.enqueue(*Urgorder, priority);
				busyCooksQ.enqueue(Bcook, priority);                       //enque the cook in priority busy cook queue
				QUrgentOrders.dequeue(Urgorder);
				Bcook->assign_Order((*Urgorder)->GetID());

				Vserved++;
			}
			else if (NcooksQ.dequeue(Bcook))                              //check if there is available Normal cook when there is no VIP
			{
				Bcook->setnumofOrderdServed(Bcook->getnumofOrderdServed() + 1);
				(*Urgorder)->setServTime(CurrentTimeStep);                       //set serving time with current time step 

				int ST = ceil(float((*Urgorder)->getOrderSize()) / Bcook->getSpeed()); //calculate the surving time
				(*Urgorder)->setServInt(ST);
				(*Urgorder)->setWaitTime();
				(*Urgorder)->setFinishTime();
				(*Urgorder)->setStatus(SRV);
				float priority = ((*Urgorder)->getFinishTime());             //set the priority of serving queue with the inverted finished time
				InServing.enqueue(*Urgorder, priority);
				busyCooksQ.enqueue(Bcook, priority);                       //enque the cook in priority busy cook queue
				QUrgentOrders.dequeue(Urgorder);
				Bcook->assign_Order((*Urgorder)->GetID());
				Vserved++;
			}
			else if (GcooksQ.dequeue(Bcook))                              //check if there is available Vegan cook when there is no VIP&&Normal
			{
				Bcook->setnumofOrderdServed(Bcook->getnumofOrderdServed() + 1);
				(*Urgorder)->setServTime(CurrentTimeStep); //set serving time with current time step 

				int ST = ceil(float((*Urgorder)->getOrderSize()) / Bcook->getSpeed()); //calculate the surving time
				(*Urgorder)->setServInt(ST);
				(*Urgorder)->setWaitTime();
				(*Urgorder)->setFinishTime();
				(*Urgorder)->setStatus(SRV);
				float priority = ((*Urgorder)->getFinishTime());             //set the priority of serving queue with the inverted finished time
				InServing.enqueue(*Urgorder, priority);
				busyCooksQ.enqueue(Bcook, priority);                       //enque the cook in priority busy cook queue
				QUrgentOrders.dequeue(Urgorder);
				Bcook->assign_Order((*Urgorder)->GetID());
				Vserved++;
			}
			else
			{
				return;                                         //there is no more available cooks in this timestep 
			}
		}
	}
}
//by raghad
void Restaurant::serve_VIP_orders(int CurrentTimeStep)
{
	Order* proOrder;
	float prio;
	while (QVIP_Order.peekFront(proOrder,prio))  //get orders from VIP waiting Queue to be serve
	{
		if (!proOrder->isUrgent())
		{
			Cook* Bcook;
			if (VcooksQ.dequeue(Bcook))           //check if there is available VIP cook
			{
				Bcook->setnumofOrderdServed(Bcook->getnumofOrderdServed() + 1);

				int ST = ceil(float(proOrder->getOrderSize()) / Bcook->getSpeed()); //calculate the surving time
				
				proOrder->setServInt(ST);
				proOrder->Serve(CurrentTimeStep);
				float priority = (proOrder->getFinishTime());             //set the priority of serving queue with the inverted finished time
				InServing.enqueue(proOrder, priority);
				busyCooksQ.enqueue(Bcook, priority);                       //enque the cook in priority busy cook queue
				QVIP_Order.dequeue(proOrder, prio);
				Bcook->assign_Order(proOrder->GetID());
				Vserved++;
			}
			else if (NcooksQ.dequeue(Bcook))                              //check if there is available Normal cook when there is no VIP
			{
				Bcook->setnumofOrderdServed(Bcook->getnumofOrderdServed() + 1);

				int ST = ceil(float(proOrder->getOrderSize()) / Bcook->getSpeed()); //calculate the surving time
				proOrder->setServInt(ST);
				proOrder->Serve(CurrentTimeStep);
				float priority = (proOrder->getFinishTime());             //set the priority of serving queue with the inverted finished time
				InServing.enqueue(proOrder, priority);
				busyCooksQ.enqueue(Bcook, priority);                       //enque the cook in priority busy cook queue
				QVIP_Order.dequeue(proOrder, prio);
				Bcook->assign_Order(proOrder->GetID());
				Vserved++;
			}
			else if (GcooksQ.dequeue(Bcook))                              //check if there is available Vegan cook when there is no VIP&&Normal
			{
				Bcook->setnumofOrderdServed(Bcook->getnumofOrderdServed() + 1);

				int ST = ceil(float(proOrder->getOrderSize()) / Bcook->getSpeed()); //calculate the surving time
				proOrder->setServInt(ST);
				proOrder->Serve(CurrentTimeStep);
				float priority = (proOrder->getFinishTime());             //set the priority of serving queue with the inverted finished time
				InServing.enqueue(proOrder, priority);
				busyCooksQ.enqueue(Bcook, priority);                       //enque the cook in priority busy cook queue
				QVIP_Order.dequeue(proOrder, prio);
				Bcook->assign_Order(proOrder->GetID());
				Vserved++;
			}
			else
			{
				return;                                         //there is no more available cooks in this timestep 
			}
		}
		else
		{
			QVIP_Order.dequeue(proOrder, prio);
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
			Bcook->setnumofOrderdServed(Bcook->getnumofOrderdServed() + 1);

			int ST = ceil(float(proOrder->getOrderSize() )/ Bcook->getSpeed()); //calculate the surving time
			proOrder->setServInt(ST);
			proOrder->Serve(CurrentTimeStep);
			float priority =  (proOrder->getFinishTime());             //set the priority of serving queue with the inverted finished time
			InServing.enqueue(proOrder, priority);
			busyCooksQ.enqueue(Bcook, priority);                       //enque the cook in priority busy cook queue
			Qvegan_Order.dequeue(proOrder);
			Bcook->assign_Order(proOrder->GetID());
			Gserved++;
		}
		else
		{
			return;                                         //there is no more available cooks in this timestep 
		}
	}
}


////////////////////By Donia\\\\\\\\\\\\\\\\\\\\\\


bool Restaurant::Health_Emergency(int curr_ts)
{
	Cook* temp;
	float pri_temp;
	Order* tempOrd;
	int no_dishes_left;
	float priority;
	if (busyCooksQ.peekFront(temp, pri_temp) && InServing.peekFront(tempOrd, pri_temp)&&temp->Is_injured()==false)
	{
		no_dishes_left = tempOrd->getOrderSize() - (curr_ts - tempOrd->getServTime()) * temp->getSpeed();
		temp->f_speed = (float(temp->getSpeed()) / 2);
		temp->setSpeed(temp->getSpeed() / 2);
		
		int ST = ceil(float(no_dishes_left) / (temp->getSpeed())); //calculate the surving time
		tempOrd->setServInt(ST);
		
		tempOrd->setFinishTime();
		priority = tempOrd->getFinishTime();
		busyCooksQ.dequeue(temp, pri_temp);
		busyCooksQ.enqueue(temp, priority);
		InServing.dequeue(tempOrd, pri_temp);
		InServing.enqueue(tempOrd, priority);
		temp->set_RstTime(tempOrd->getFinishTime());//set the finish of rest time
		temp->injure(true);//make the flag on
		return true;
	}
	else
		return false;


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
			Bcook->setnumofOrderdServed(Bcook->getnumofOrderdServed() + 1);

			int ST = ceil(float(proOrder->getOrderSize()) / Bcook->getSpeed()); //calculate the surving time
			proOrder->setServInt(ST);
			proOrder->Serve(CurrentTimeStep);
			float priority =  (proOrder->getFinishTime());             //set the priority of serving queue with the inverted finished time
			InServing.enqueue(proOrder, priority);
			busyCooksQ.enqueue(Bcook, priority);                       //enque the cook in priority busy cook queue
			LNormal_Order.DeleteFirst(proOrder);
			Bcook->assign_Order(proOrder->GetID());
			Nserved++;
		}
		else if (VcooksQ.dequeue(Bcook))           //check if there is available VIP cook when there is no Normal
		{
			Bcook->setnumofOrderdServed(Bcook->getnumofOrderdServed() + 1);

			int ST = ceil(float(proOrder->getOrderSize() )/ Bcook->getSpeed()); //calculate the surving time
			proOrder->setServInt(ST);
			proOrder->Serve(CurrentTimeStep);
			float priority =  (proOrder->getFinishTime());             //set the priority of serving queue with the inverted finished time
			InServing.enqueue(proOrder, priority);
			busyCooksQ.enqueue(Bcook, priority);                       //enque the cook in priority busy cook queue
			LNormal_Order.DeleteFirst(proOrder);
			Bcook->assign_Order(proOrder->GetID());
			Nserved++;
		}
		else
		{
			return;                                                   //there is no more available cooks in this timestep 
		}


	}

}
//by raghad
void Restaurant::getfrombusyCookQ(int CurrentTimeStep)
{
	Cook* Acook;
	float priority;
	while (busyCooksQ.peekFront(Acook, priority))
	{
		if ((priority) <= CurrentTimeStep && Acook->getnumofOrderdServed()==Acook->getNumOrdBbreak())     //the cook servesed number of orders it should take break
		{
			if (Acook->Is_injured ()== true && Acook->Has_Urg() == true)
			{
				Acook->injure(false);         ///if he was injured and was assigned to an urgent cook
				Acook->Give_Urg(false);    ////so its speed is still the half until he has his break
				Acook->setSpeed(Acook->f_speed* 2);
			}
			busyCooksQ.dequeue(Acook, priority);
			float F=(Acook->getBreakDur()+ CurrentTimeStep);
			CooksInBreak.enqueue(Acook,  F);
		}
		else if ((priority) <= CurrentTimeStep && Acook->Is_injured() == true)
		{
			busyCooksQ.dequeue(Acook, priority);
			CooksInRest.enqueue(Acook);
			
		}
		else if (( priority) <= CurrentTimeStep)                      //Finish time equal the current time step
		{
			busyCooksQ.dequeue(Acook, priority);
			if (Acook->GetType() == TYPE_VIP)
				VcooksQ.enqueue(Acook);
			if (Acook->GetType() == TYPE_VGAN)
				GcooksQ.enqueue(Acook);
			if (Acook->GetType() == TYPE_NRM)
				NcooksQ.enqueue(Acook);
		}
		else
		{
			return;                                                     //there is no more finished cooks
		}
	}


}
//by raghad
void Restaurant::getfromBreakCookQ(int CurrentTimeStep)
{
	Cook* Acook;
	float priority;
	while (CooksInBreak.peekFront(Acook, priority))
	{
		if ((priority) <= CurrentTimeStep)               //If there is a cooks finished it's break
		{
		
			CooksInBreak.dequeue(Acook, priority);
			if (Acook->GetType() == TYPE_VIP)
				VcooksQ.enqueue(Acook);
			if (Acook->GetType() == TYPE_VGAN)
				GcooksQ.enqueue(Acook);
			if (Acook->GetType() == TYPE_NRM)
				NcooksQ.enqueue(Acook);

		}
		else
		{
			return;                                         //there is no more cooks finished it's break
		}

	}



}
void Restaurant::getfromInServingQ(int CurrentTimeStep)
{
	Order* proOrder;
	float prio;
	while (InServing.peekFront(proOrder, prio))
	{
		if (prio > CurrentTimeStep)
			return;                        //there is no more finished orders

		InServing.dequeue(proOrder, prio);
		proOrder->setStatus(DONE);
		FinishedList.enqueue(proOrder);
	}



}
void Restaurant::getfromRestCookQ(int CurrentTimeStep)
{
	Cook* Rcook;
	while (CooksInRest.peekFront(Rcook))
	{
		
		if ((Rcook->get_rstTime()) <= CurrentTimeStep&&Rcook->Has_Urg()==false) //check if there is a cooks finished his rest time
		{
		
				CooksInRest.dequeue(Rcook);
				Rcook->injure(false);
				Rcook->setSpeed(Rcook->f_speed * 2);
				switch (Rcook->GetType())
				{
				case TYPE_VIP:
					VcooksQ.enqueue(Rcook);
				case TYPE_VGAN:
					GcooksQ.enqueue(Rcook);
				case TYPE_NRM:
					NcooksQ.enqueue(Rcook);
				default:
					break;
				}
			
		
		}
		else
		{
			return; //there is no more cooks finished their rest 
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
	pGUI->PrintMessage("Enter input file name, the files are named ""test"" followed by number of test case ,i.e. test1.txt");
	IPfilename = pGUI->GetString();

	ifstream InFile(IPfilename);
	
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
			newNCook->setSpeed((Ncookspeed_min+rand() % Ncookspeed_max));
			newNCook->setNumOrdBbreak(numOrdersBbreak);
			newNCook->setBreakDur(Nbreak_min+rand() % Nbreak_max);
			newNCook->set_RstPrd(RstPrd);
			
			newNCook->setnumofOrderdServed(0);

			NcooksQ.enqueue(newNCook);
		}

		for (int i = 0; i < numGcooks; i++)
		{

			Cook* newGCook = new Cook;

			newGCook->setID(arrCIDs[i + numNcooks + 1]);
			newGCook->setType(TYPE_VGAN);
			srand((unsigned)time(0));
			newGCook->setSpeed(Gcookspeed_min+	rand() % Gcookspeed_max);
			newGCook->setNumOrdBbreak(numOrdersBbreak);
			newGCook->setBreakDur(Gbreak_min+rand() % Gbreak_max );
			newGCook->set_RstPrd(RstPrd);
			
			newGCook->setnumofOrderdServed(0);
			GcooksQ.enqueue(newGCook);
		}

		for (int i = 0; i < numVcooks; i++)
		{
			Cook* newVCook = new Cook;

			newVCook->setID(arrCIDs[i + numNcooks + numGcooks + 1]);
			newVCook->setType(TYPE_VIP);
			srand((int)time(NULL));
			newVCook->setSpeed(Vcookspeed_min+rand() % Vcookspeed_max );
			newVCook->setNumOrdBbreak(numOrdersBbreak);
			newVCook->setBreakDur(Gbreak_min + rand() %Gbreak_max );
			newVCook->set_RstPrd(RstPrd);
			
			newVCook->setnumofOrderdServed(0);
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
void Restaurant::Restaurant_Modes(int Mode)
{
	bool injured;
	float R;
	if (Mode == 1)
	{
		pGUI->PrintMessage("Welcome To Our Restaurant .... Interactive Mode, Click To Continue");
		pGUI->waitForClick();
		fileLoading();
		int CurrentTimeStep = 1;
		while (!EventsQueue.isEmpty() || !InServing.isEmpty() || !QVIP_Order.isEmpty() || !Qvegan_Order.isEmpty() || !LNormal_Order.isEmpty())
		{
			char timestep[100];
			itoa(CurrentTimeStep, timestep, 10);
			pGUI->PrintMessage(timestep, 1);
			ExecuteEvents(CurrentTimeStep);
			getfromBreakCookQ(CurrentTimeStep);
			getfrombusyCookQ(CurrentTimeStep);
			//donia
			srand((int)time(0));
			R= static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			if (R <= InjProp)
			{
				injured = Health_Emergency(CurrentTimeStep);
			}
			getfromRestCookQ(CurrentTimeStep);
			///end donya
			Serve_Urgent_VIP(CurrentTimeStep);
			Executepromotion(CurrentTimeStep);
			serve_VIP_orders(CurrentTimeStep);
			serve_Vegan_orders(CurrentTimeStep);
			serve_Normal_orders(CurrentTimeStep);
			getfromInServingQ(CurrentTimeStep);
			FillDrawingList();
			/*busyCooksQ.peekFront(Busy, prio);
			cout << Busy->GetID() <<"   "<< Busy->get_order() << "   ";
			while (InServing.peekFront(serv, prio) && serv->getFinishTime() == CurrentTimeStep)
				InServing.dequeue(serv, prio);
			cout << serv->GetID()<<endl;
			*/
			//Printing Cooks and Orders Information
			pGUI->PrintMessage("Wating Orders ->  Normal : " + to_string(NWaitNum) + " Vegan :" + to_string(GWaitNum) + " VIP : " + to_string(VWaitNum), 2);
			pGUI->PrintMessage("Available Cooks - >  Normal : " + to_string(NCookNum) + " Vegan :" + to_string(GCookNum) + " VIP :" + to_string(VCookNum), 3);
			pGUI->PrintMessage("Total Served Orders Till Now-> Normal : " + to_string(Nserved) + " Vegan :" + to_string(Gserved) + " VIP :" + to_string(Vserved), 5);
			pGUI->UpdateInterface();
			pGUI->waitForClick();
			CurrentTimeStep++;
			pGUI->ResetDrawingList();
		}
		outputFileLoading();
	}
	else if (Mode == 2)
	{
		pGUI->PrintMessage("Welcome To Our Restaurant .... Step-by-Step Mode, Click To Continue");
		pGUI->waitForClick();
		fileLoading();
		int CurrentTimeStep = 1;
		while (!EventsQueue.isEmpty() || !InServing.isEmpty()||!QVIP_Order.isEmpty()|| !Qvegan_Order.isEmpty() ||!LNormal_Order.isEmpty())
		{

			char timestep[100];
			itoa(CurrentTimeStep, timestep, 10);
			pGUI->PrintMessage(timestep, 1);
			ExecuteEvents(CurrentTimeStep);
			getfromBreakCookQ(CurrentTimeStep);
			getfrombusyCookQ(CurrentTimeStep);
			//donia
			srand((int)time(0));
			R = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			if (R <= InjProp)
			{
				injured = Health_Emergency(CurrentTimeStep);
			}
			getfromRestCookQ(CurrentTimeStep);
			///end donya
			Serve_Urgent_VIP(CurrentTimeStep);
			Executepromotion(CurrentTimeStep);
			serve_VIP_orders(CurrentTimeStep);
			serve_Vegan_orders(CurrentTimeStep);
			serve_Normal_orders(CurrentTimeStep);
			getfromInServingQ(CurrentTimeStep);
			FillDrawingList();
			//Printing Cooks and Orders Information
			pGUI->PrintMessage("Wating Orders ->  Normal : " + to_string(NWaitNum) + " Vegan :" + to_string(GWaitNum) + " VIP : " + to_string(VWaitNum), 2);
			pGUI->PrintMessage("Available Cooks - >  Normal : " + to_string(NCookNum) + " Vegan :" + to_string(GCookNum) + " VIP :" + to_string(VCookNum), 3);
			pGUI->PrintMessage("Total Served Orders Till Now-> Normal : " + to_string(Nserved) + " Vegan :" + to_string(Gserved) + " VIP :" + to_string(Vserved), 5);
			pGUI->UpdateInterface();
			Sleep(1000);
			CurrentTimeStep++;
			pGUI->ResetDrawingList();
		}
		outputFileLoading();
	}
	else if (Mode == 3)
	{
		pGUI->PrintMessage("Welcome To Our Restaurant .... Silent Mode, Click To Continue");
		pGUI->waitForClick();
		fileLoading();
		int CurrentTimeStep = 1;
		while (!EventsQueue.isEmpty() || !InServing.isEmpty() || !QVIP_Order.isEmpty() || !Qvegan_Order.isEmpty() || !LNormal_Order.isEmpty())
		{
			
			ExecuteEvents(CurrentTimeStep);
			getfromBreakCookQ(CurrentTimeStep);
			getfrombusyCookQ(CurrentTimeStep);
			//donia
			srand((int)time(0));
			R = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			if (R <= InjProp)
			{
				injured = Health_Emergency(CurrentTimeStep);
			}
			getfromRestCookQ(CurrentTimeStep);
			///end donya
			Serve_Urgent_VIP(CurrentTimeStep);
			Executepromotion(CurrentTimeStep);
			serve_VIP_orders(CurrentTimeStep);
			serve_Vegan_orders(CurrentTimeStep);
			serve_Normal_orders(CurrentTimeStep);
			getfromInServingQ(CurrentTimeStep);
			//Printing Cooks and Orders Information
			CurrentTimeStep++;
			
		}

		outputFileLoading();
	}
	pGUI->PrintMessage("End of Simulation....Click to Exit");
	pGUI->waitForClick();
}


void Restaurant::outputFileLoading()
{
	pGUI->PrintMessage("Enter output file name");
	OPfilename = pGUI->GetString();

	ofstream OutFile(OPfilename);

	if (OutFile.is_open())
	{
		OutFile << "FT  ID  AT  WT  ST" << endl;

		Order *orderr;
		while (FinishedList.dequeue(orderr))
		{
			OutFile << orderr->getFinishTime() << "   "
				<< orderr->GetID() << "   "
				<< orderr->getArrTime() << "   "
				<< orderr->getWaitTime() << "   "
				<< orderr->getServInt() << endl;
		}
	}

}



///Appendix

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