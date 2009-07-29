#include "misc.h"

char doorSide = 0;
bool doorOpen = true;

Timer hornTimer = TIMER_OFF;
int hornDuration = 0;


void InitMiscellaneous()
{
//	setDoorSide(0);
	setDoorState(true);
	
	hornTimer = TIMER_OFF;
	hornDuration = 0;
}

void DestroyMiscellaneous(){
}


void RunMiscellaneous()
{
	if(gOpts[KLAXONINDICATOR].v != -1 && hornTimer != TIMER_OFF && tOn(hornTimer, gState.Time, hornDuration))
		gPanel[gOpts[KLAXONINDICATOR].v] = 0;

	if(gOpts[TRAVELMETERMODE].v == -1)
		return;

	float div = 1;
	if(gOpts[TRAVELMETERMODE].v == 1)
 		div = 1.609;
	
	if(gOpts[TRAVELMETER100].v != -1)
		gPanel[gOpts[TRAVELMETER100].v] = (int)(gState.Location / div / 100000);
	if(gOpts[TRAVELMETER10].v != -1)
		gPanel[gOpts[TRAVELMETER10].v] = (int)(gState.Location / div / 10000);
	if(gOpts[TRAVELMETER1].v != -1)
		gPanel[gOpts[TRAVELMETER1].v] = (int)(gState.Location / div / 1000);
	if(gOpts[TRAVELMETER01].v != -1)
		gPanel[gOpts[TRAVELMETER01].v] = (int)(gState.Location / div / 100);
	if(gOpts[TRAVELMETER001].v != -1)
		gPanel[gOpts[TRAVELMETER001].v] = (int)(gState.Location / div / 10);
}

void setDoorIndicators()
{
	if(gOpts[DOORINDICATOR].v == -1)
		return;
		
	if(doorSide != 0 && doorOpen)
	{
		if(doorSide == -1 || doorSide == 2)
			gPanel[gOpts[DOORINDICATOR].v] = 1;
		if((doorSide == 1 || doorSide == 2) && gOpts[DOORINDICATOR].next != NULL)
			gPanel[gOpts[DOORINDICATOR].next->v] = 1;
	}
	else
	{
		gPanel[gOpts[DOORINDICATOR].v] = 0;
		if(gOpts[DOORINDICATOR].next != NULL)
			gPanel[gOpts[DOORINDICATOR].next->v] = 0;
	}
}

void setDoorState(bool open)
{
	doorOpen = open;
	setDoorIndicators();
}

void setDoorSide(char side)
{
	if(side < -1 || side > 2)
		return;
	doorSide = side;
	if(gState.Speed == 0)
		setDoorIndicators();
}


void hornSounded(int hornType)
{
	if(gOpts[KLAXONINDICATOR].v == -1 || gOpts[KLAXONINDICATOR].next == NULL)
		return;
		
	if(hornType == ATS_HORN_PRIMARY)
	{
		ResetTimer(hornTimer);
		hornDuration = gOpts[KLAXONINDICATOR].next->v;
		gPanel[gOpts[KLAXONINDICATOR].v] = 1;
	}
	else if(hornType == ATS_HORN_SECONDARY && gOpts[KLAXONINDICATOR].next->next != NULL)
	{
		ResetTimer(hornTimer);
		hornDuration = gOpts[KLAXONINDICATOR].next->next->v;
		gPanel[gOpts[KLAXONINDICATOR].v] = 1;
	}
}

void miscKeyPressed(int key)
{
	PLUGIN_OPTION *p = &gOpts[CUSTOMINDICATORS];
	while(p != NULL)
	{
		if(p->next == NULL)
			break;
		if(key == p->v)
			gPanel[p->next->v] = (int)!gPanel[p->next->v];
		p = p->next->next;
	}
}

