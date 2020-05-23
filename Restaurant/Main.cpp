//#include "Drawing.h"
#include "Rest\Restaurant.h"
#include "GUI\GUI.h"
#include<Windows.h>
#include<mmsystem.h>

int main()
{
	PlaySound(TEXT("Spong2.wav"), NULL, SND_SYNC);
	Restaurant* pRest = new Restaurant;
	pRest->RunSimulation();
	delete pRest;
	return 0;
}
