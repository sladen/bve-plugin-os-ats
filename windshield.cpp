#include "windshield.h"

#define MAXDROPS 256

Timer wiper = TIMER_OFF;
Timer holdWiper = TIMER_OFF;
int wiperPosition = 0;
int wiperDirection = 0;
int wiperMode = 0;
int rain = 0;
Timer dropTimer = TIMER_OFF;
int dropInterval;

int dropList[MAXDROPS];
int dropListPos[MAXDROPS];
int unusedDrops;


void InitWindshield()
{
	wiper = 0;
	holdWiper = TIMER_OFF;
	wiperPosition = gOpts[WIPERHOLDPOSITION].v;
	wiperDirection = 0;
	wiperMode = 0;
	rain = 0;
	dropTimer = 0;
	dropInterval = 0;
	
	if(gOpts[NUMBEROFDROPS].v < 0)
		gOpts[NUMBEROFDROPS].v = 0;
	if(gOpts[NUMBEROFDROPS].v > MAXDROPS)
		gOpts[NUMBEROFDROPS].v = MAXDROPS;
	unusedDrops = gOpts[NUMBEROFDROPS].v;

	for(int i = 0; i < MAXDROPS; i++)
	{
		dropList[i] = i + gOpts[DROPSTARTINDEX].v;
		dropListPos[i] = i;
	}
}

void DestroyWindshield()
{
}


void AnimateWindshield()
{
	if(gOpts[WIPERINDEX].v != -1)
	{
		int limit = 20;
		while(tOn(wiper, gState.Time, gOpts[WIPERRATE].v / 100) && limit--)
		{
			if(wiperPosition == gOpts[WIPERHOLDPOSITION].v)
			{
				if(wiperMode == 0)
					wiperDirection = 0;
 				else if(wiperMode == 1)
 				{
 					if(holdWiper == TIMER_OFF)
 					{
 						ResetTimer(holdWiper);
 						wiperDirection = 0;
   					}
   					else if(tOn(holdWiper, gState.Time, gOpts[WIPERDELAY].v))
   					{
   						holdWiper = TIMER_OFF;
   						wiperDirection = (wiperPosition == 0 ? -1 : 1);
   					}
 				}
 				else
					wiperDirection = (wiperPosition == 0 ? -1 : 1);
					
				if(wiperDirection != 0 && gOpts[WIPERSOUND].v != -1)
					gSound[gOpts[WIPERSOUND].v] = ATS_SOUND_PLAY;
   			}
			
			if(wiperPosition == 0 || wiperPosition == 100)
			{
				wiperDirection *= -1;
				if(wiperDirection != 0 && gOpts[WIPERSOUND].v != -1 && gOpts[WIPERSOUND].next != NULL)
					gSound[gOpts[WIPERSOUND].v] = ATS_SOUND_PLAY;
			}
			wiperPosition += wiperDirection;
			
			gPanel[gOpts[WIPERINDEX].v] = wiperPosition;

			if(gOpts[NUMBEROFDROPS].v > 0)
			{
				int cleardrop = (int)(wiperPosition / (100.0 / gOpts[NUMBEROFDROPS].v) + gOpts[DROPSTARTINDEX].v);
				if(gPanel[cleardrop] > 0)
				{
   					swap(dropList[dropListPos[cleardrop - gOpts[DROPSTARTINDEX].v]], dropList[unusedDrops], int);
   					unusedDrops++;
   				}
				gPanel[cleardrop] = 0;
			}
		}
	}
	
	if(gOpts[NUMBEROFDROPS].v > 0)
	{
		if(rain > 0)
		{
			if(tOn(dropTimer, gState.Time, dropInterval))
			{
				if(gOpts[DROPSOUND].v != -1)
				{
					int si = gOpts[DROPSOUND].v;
					if(gOpts[DROPSOUND].next != NULL)
						si += rand() % (gOpts[DROPSOUND].next->v);
					gSound[si] = ATS_SOUND_PLAY;
				}
				
				if(unusedDrops > 0)
				{
					int listindex = rand() % (int)(unusedDrops * 0.8 + 1);
					int pisara = dropList[listindex];

					if(gPanel[pisara] == 0)
					{
						gPanel[pisara] = 1;
						swap(dropList[unusedDrops - 1], dropList[listindex], int);
						dropListPos[pisara - gOpts[DROPSTARTINDEX].v] = unusedDrops - 1;
						unusedDrops--;
					}
				}

				int dev = (int)(0.4 * 20000 / rain);
 				dropInterval = (20000 / rain) + (rand() % (dev * 2) - dev);
			}
		}
		else
			ResetTimer(dropTimer);
	}
}


void WindshieldKeyPressed(int key)
{
	if(key == gOpts[WIPERONKEY].v)
	{
		if(gOpts[WIPERDELAY].v == 0 || wiperMode == 1)
			wiperMode = 2;
		else if(wiperMode < 2)
		{
			if(gOpts[WIPERDELAY].v != 0 && wiperMode == 0)
			{
   				ResetTimer(holdWiper);
   				holdWiper -= gOpts[WIPERDELAY].v + 1;
   			}
   			wiperMode++;
		}
	}
	else if(key == gOpts[WIPEROFFKEY].v && wiperMode > 0)
	{
		if(gOpts[WIPERDELAY].v == 0)
			wiperMode = 0;
		else
		{
			if(gOpts[WIPERDELAY].v != 0 && wiperMode == 2)
				holdWiper = TIMER_OFF;
			wiperMode--;
		}
	}
}


void WindshieldBeacon(ATS_BEACONDATA data)
{
	if(data.Type != BCN_RAIN || data.Optional < 0 || data.Optional > 100)
		return;
	rain = data.Optional;
	dropInterval = 0;
	ResetTimer(dropTimer);
}

