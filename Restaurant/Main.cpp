//#include "Drawing.h"
#include "Rest\Restaurant.h"
#include "GUI\GUI.h"
#include<Windows.h>
#include<mmsystem.h>

int main()
{
	Restaurant* pRest = new Restaurant;
	pRest->RunSimulation();
	PlaySound(TEXT("Spong.wav"), NULL, SND_SYNC);
	delete pRest;
	return 0;
}
