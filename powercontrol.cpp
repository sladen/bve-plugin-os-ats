#include "powercontrol.h"

#define GAP_END_UNDEFINED -1

struct Gap
{
	double loc;
	unsigned int length;
	bool op;
	Gap *next;
};

Gap *gaplist = NULL;
int pickupPoints;
int gappedPickups;
bool reminder;

static bool initialized = false;


void InitPowerControl()
{
	initialized = true;
	gaplist = NULL;
	pickupPoints = 0;
	gappedPickups = 0;
	reminder = false;
	
	PLUGIN_OPTION *ptr = &gOpts[POWERPICKUPPOINTS];
	while(ptr != NULL)
	{
		pickupPoints++;
		ptr = ptr->next;
	}
	
	if(gOpts[POWERINDICATOR].v != -1)
		gPanel[gOpts[POWERINDICATOR].v] = 1;
}

void DestroyPowerControl()
{
	if(!initialized)
		return;
		
	if(gaplist != NULL)
	{
		while(gaplist->next != NULL)
		{
			Gap *ptr = gaplist->next;
			Gap *prev = gaplist;
			while(ptr->next != NULL)
			{
				prev = ptr;
				ptr = ptr->next;
			}
			delete ptr;
			prev->next = NULL;
		}
		delete gaplist;
	}
}


void CreateGap(int length)
{
	PLUGIN_OPTION *pp = &gOpts[POWERPICKUPPOINTS];
	while(pp != NULL)
	{
		Gap *gap = new Gap;
		if(gap == NULL)
			break;
		gap->loc = gState.Location + pp->v;
		gap->length = length;
		gap->op = false;
		gap->next = NULL;
		
		if(gaplist == NULL)
			gaplist = gap;
		else
		{
			Gap *ptr = gaplist;
			while(ptr->next != NULL)
				ptr = ptr->next;
			ptr->next = gap;
		}

		pp = pp->next;
	}
}


void PowerBeacon(ATS_BEACONDATA p)
{
	if(p.Type != BCN_POWERGAP || gOpts[POWERGAPBEHAVIOUR].v == NORMAL || p.Optional == 0 || gState.Speed <= 0)
		return;
	CreateGap(p.Optional);
}


void PowerControl()
{
	if(gOpts[POWERGAPBEHAVIOUR].v == 0 || gState.Speed == 0)
		return;

	Gap *ptr = gaplist;
	Gap *prev = NULL;

	while(ptr != NULL)
	{
		if((gState.Speed > 0 && gState.Location > ptr->loc && ptr->op == false) ||
  		   (gState.Speed > 0 && gState.Location > ptr->loc + ptr->length && ptr->op == true) ||
  		   (gState.Speed < 0 && gState.Location < ptr->loc && ptr->op == true))
		{
			ptr->op = !ptr->op;
			if(gOpts[POWERGAPBEHAVIOUR].v == 2)
			{
				OperatePower(PWR_NEUTRAL, ptr->op);
				if(gOpts[POWERINDICATOR].v != -1)
					gPanel[gOpts[POWERINDICATOR].v] = !ptr->op;
			}
			else
			{
				if(gOpts[POWERGAPBEHAVIOUR].v == 3)
				{
					gappedPickups += (ptr->op == true ? 1 : -1);
					if(gappedPickups == 0 || gappedPickups == pickupPoints)
					{
						OperatePower(PWR_NEUTRAL, ptr->op);
						if(gOpts[POWERINDICATOR].v != -1)
							gPanel[gOpts[POWERINDICATOR].v] = !ptr->op;
					}
				}
				else if(pickupPoints > 0)
				{
					if(ptr->op == true)
						gappedPickups++;
					OperatePower(PWR_MAX - (int)(PWR_MAX * ((float)gappedPickups / pickupPoints) + 0.5), ptr->op);
					if(ptr->op == false)
						gappedPickups--;

					if(gOpts[POWERINDICATOR].v != -1)
						gPanel[gOpts[POWERINDICATOR].v] = (gappedPickups != pickupPoints);
				}
			}
			
			if(gState.Location > ptr->loc + ptr->length)
			{
				if(prev == NULL)
					gaplist = ptr->next;
				else
 					prev->next = ptr->next;
				Gap *temp = ptr;
				delete temp;
			}
			else
				prev = ptr;
				
			ptr = ptr->next;
		}
		else
		{
			prev = ptr;
			ptr = ptr->next;
		}
	}
}


void PowerControlKeyPressed(int key)
{
	if(key == gOpts[REMINDERKEY].v)
	{
		reminder = !reminder;
		OperatePower(PWR_NEUTRAL, reminder);
		if(gOpts[REMINDERINDICATOR].v != -1)
			gPanel[gOpts[REMINDERINDICATOR].v] = reminder;
	}
}

