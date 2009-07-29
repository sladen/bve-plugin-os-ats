#include "br_safety.h"

#define AWS_TIMER_OFF -1
#define AWS_TIMER_BRAKE -2

Timer awsTimer;
bool awsStop;
bool awsRelease;
bool tpwsTrainstop;
bool tpwsRelease;
char tpwsStartupTest;
Indicator brakeDemand;
Timer stopTimer;
Timer overrideTimer;
Indicator stopOverride;


void InitBRSafety()
{
	awsTimer = TIMER_OFF;
	awsStop = false;
	awsRelease = false;
	tpwsTrainstop = false;
	tpwsRelease = false;
	tpwsStartupTest = min(gOpts[STARTUPTEST].v, 1);
	if(gOpts[AWSWARNINGSOUND].v != -1)
		gSound[gOpts[AWSWARNINGSOUND].v] = ATS_SOUND_STOP;
	if(gOpts[AWSCLEARSOUND].v != -1)
		gSound[gOpts[AWSCLEARSOUND].v] = ATS_SOUND_STOP;
	brakeDemand = NewIndicator(TPWSINDICATOR, 0);
	stopTimer = TIMER_OFF;
	overrideTimer = TIMER_OFF;
	stopOverride = NewIndicator(TPWSINDICATOR2, 0);
}

void DestroyBRSafety()
{
}


void BRSafety()
{
	if(gOpts[SYSTEM].v == 0)
		return;
		
	if(awsTimer != AWS_TIMER_OFF && awsTimer != AWS_TIMER_BRAKE)
	{
		if(tOn(awsTimer, gState.Time, gOpts[AWSDELAY].v) && awsStop == false)
		{
			awsTimer = AWS_TIMER_BRAKE;
			awsStop = true;
			OperateBrake(BRK_EMG, true);
			awsRelease = false;
		}
	}
	
	ShowIndicator(brakeDemand);
	ShowIndicator(stopOverride);
	
	if(gState.Speed == 0)
	{
 		if(stopTimer == TIMER_OFF)
			ResetTimer(stopTimer);
		if(awsRelease)
		{
			OperateBrake(BRK_EMG, false);
			if(gOpts[AWSINDICATOR].v != -1)
				gPanel[gOpts[AWSINDICATOR].v] = 0;
			awsTimer = -1;
			awsStop = false;
			awsRelease = false;
		}
		if(tpwsRelease && tOn(stopTimer, gState.Time, gOpts[TPWSSTOPDELAY].v * 1000))
		{
			OperateBrake(BRK_EMG, false);
			SetIndicator(brakeDemand, 0);
			tpwsTrainstop = false;
			tpwsRelease = false;
		}
	}
	
	if(overrideTimer != TIMER_OFF)
		if(tOn(overrideTimer, gState.Time, gOpts[TPWSOVERRIDELIFETIME].v * 1000))
		{
			overrideTimer = TIMER_OFF;
			SetIndicator(stopOverride, 0);
		}
}


void SetBRSafetyStatus(ATS_BEACONDATA data)
{
	if(data.Type < BCN_AWS_SIGNAL || data.Type > BCN_TPWS_SPEED || gOpts[SYSTEM].v == 0 || gState.Speed < 0)
		return;

	if((gOpts[SYSTEM].v == 1 || gOpts[SYSTEM].v == 2) &&
 		(data.Type == BCN_AWS_SIGNAL || data.Type == BCN_AWS_FIXED)) //aws
	{
		if(data.Signal < CLEAR_SIGNAL_RANGE_BASE || gState.Speed <= gOpts[AWSMINSPEED].v || data.Type == BCN_AWS_FIXED) //varoitus
		{
			if(gOpts[AWSINDICATOR].v != -1 && gOpts[AWSINDICATORAPPEAR].v == 1)
				gPanel[gOpts[AWSINDICATOR].v] = 1;
			else if(gOpts[AWSINDICATOR].v != -1)
				gPanel[gOpts[AWSINDICATOR].v] = 0;
			if(gOpts[AWSWARNINGSOUND].v != -1)
				gSound[gOpts[AWSWARNINGSOUND].v] = ATS_SOUND_PLAYLOOPING;
			if(gOpts[AWSCLEARSOUND].v != -1)
				gSound[gOpts[AWSCLEARSOUND].v] = ATS_SOUND_STOP;
			if(awsTimer == AWS_TIMER_OFF)
				ResetTimer(awsTimer);
		}
		else
		{
			if(gOpts[AWSINDICATOR].v != -1)
				gPanel[gOpts[AWSINDICATOR].v] = 0;
			if(gOpts[AWSWARNINGSOUND].v != -1)
				gSound[gOpts[AWSWARNINGSOUND].v] = ATS_SOUND_STOP;
			if(gOpts[AWSCLEARSOUND].v != -1)
				gSound[gOpts[AWSCLEARSOUND].v] = ATS_SOUND_PLAY;
			awsTimer = AWS_TIMER_OFF;
		}
	}
	
	if(gOpts[SYSTEM].v == 2 && tpwsTrainstop == false) //tpws
	{
		if(overrideTimer != TIMER_OFF)
		{
			if(data.Type == BCN_TRAINSTOP)
			{
				overrideTimer = TIMER_OFF;
				SetIndicator(stopOverride, 0);
			}
		}
		else if(
			(data.Type == BCN_TPWS_SPEED && gState.Speed > data.Optional) ||
   			(data.Type == BCN_TPWS_SIGNAL && data.Signal <= DANGER_SIGNAL_RANGE_LIMIT && gState.Speed > data.Optional) ||
      		(data.Type == BCN_TRAINSTOP && data.Signal <= DANGER_SIGNAL_RANGE_LIMIT))
		{
			BlinkIndicator(brakeDemand, 1);
			if(gOpts[TPWSWARNINGSOUND].v != -1)
				gSound[gOpts[TPWSWARNINGSOUND].v] = ATS_SOUND_PLAYLOOPING;
			OperateBrake(BRK_EMG, true);
			tpwsTrainstop = true;
			stopTimer = TIMER_OFF;
			tpwsRelease = false;
		}
	}

	if(gOpts[SYSTEM].v == 3 && data.Type == BCN_TRAINSTOP && tpwsTrainstop == false) //trainstop
	{
		if(overrideTimer != TIMER_OFF)
		{
			overrideTimer = TIMER_OFF;
			SetIndicator(stopOverride, 0);
		}
		else if(data.Signal <= DANGER_SIGNAL_RANGE_LIMIT)
		{
			BlinkIndicator(brakeDemand, 1);
			if(gOpts[TPWSWARNINGSOUND].v != -1)
				gSound[gOpts[TPWSWARNINGSOUND].v] = ATS_SOUND_PLAYLOOPING;
			OperateBrake(BRK_EMG, true);
			tpwsTrainstop = true;
			stopTimer = TIMER_OFF;
			tpwsRelease = false;
		}
	}
}


void BRSafetyKeyPressed(int key)
{
	if(key == gOpts[AWSACKNOWLEDGEKEY].v)
	{
		if(gOpts[AWSACKNOWLEDGEKEY].next != NULL)
			gPanel[gOpts[AWSACKNOWLEDGEKEY].next->v] = 1;
		
		if(tpwsStartupTest == 2)
		{
			if(gOpts[AWSINDICATOR].v != -1)
				gPanel[gOpts[AWSINDICATOR].v] = 1;
			if(gOpts[AWSWARNINGSOUND].v != -1)
				gSound[gOpts[AWSWARNINGSOUND].v] = ATS_SOUND_STOP;
				
			if(gOpts[STARTUPTEST].v == 2)
			{
				if(gOpts[AWSCLEARSOUND].v != -1)
					gSound[gOpts[AWSCLEARSOUND].v] = ATS_SOUND_PLAY;
				SetIndicator(brakeDemand, 0);
				SetIndicator(stopOverride, 0);
				if(gOpts[TPWSINDICATOR3].v != -1)
					gPanel[gOpts[TPWSINDICATOR3].v] = 0;
			}
			tpwsStartupTest = 0;
		}
		else if(awsTimer != AWS_TIMER_OFF && awsTimer != AWS_TIMER_BRAKE)
		{
			if(gOpts[AWSINDICATOR].v != -1)
				gPanel[gOpts[AWSINDICATOR].v] = 1;
			if(gOpts[AWSWARNINGSOUND].v != -1)
				gSound[gOpts[AWSWARNINGSOUND].v] = ATS_SOUND_STOP;
			awsTimer = AWS_TIMER_OFF;
		}
		else if(awsTimer == AWS_TIMER_BRAKE && (gState.Speed == 0 || gOpts[AWSBRAKECANCEL].v != 0) && awsStop == true)
		{
			if(gOpts[AWSBRAKECANCEL].v == 1)
				awsRelease = true;
			else
			{
				OperateBrake(BRK_EMG, false);
				if(gOpts[AWSINDICATOR].v != -1)
					gPanel[gOpts[AWSINDICATOR].v] = 0;
				if(gOpts[AWSWARNINGSOUND].v != -1)
					gSound[gOpts[AWSWARNINGSOUND].v] = ATS_SOUND_STOP;
				awsTimer = AWS_TIMER_OFF;
				awsStop = false;
			}
		}
	}
	
	if(key == gOpts[TPWSRESETKEY].v && tpwsTrainstop == true)
	{
		SetIndicator(brakeDemand, 1);
 		if(gOpts[TPWSBRAKECANCEL].v != 0 || (gState.Speed == 0 && tOn(stopTimer, gState.Time, gOpts[TPWSSTOPDELAY].v * 1000)))
 		{
 			if(gOpts[TPWSBRAKECANCEL].v == 1)
				tpwsRelease = true;
			else
			{
				OperateBrake(BRK_EMG, false);
				SetIndicator(brakeDemand, 0);
				tpwsTrainstop = false;
			}

			if(gOpts[TPWSWARNINGSOUND].v != -1)
				gSound[gOpts[TPWSWARNINGSOUND].v] = ATS_SOUND_STOP;
		}
	}
	
	if(key == gOpts[TPWSOVERRIDEKEY].v && tpwsTrainstop == false && tpwsStartupTest == 0)
	{
		if(overrideTimer == TIMER_OFF)
		{
			ResetTimer(overrideTimer);
			BlinkIndicator(stopOverride, 1);
		}
		else
		{
			overrideTimer = TIMER_OFF;
			SetIndicator(stopOverride, 0);
		}
	}
}


void BRSafetyKeyReleased(int key)
{
	if(key == gOpts[AWSACKNOWLEDGEKEY].v)
		if(gOpts[AWSACKNOWLEDGEKEY].next != NULL)
			gPanel[gOpts[AWSACKNOWLEDGEKEY].next->v] = 0;
}


void BRSafetyReverserMovedOff()
{
	if(tpwsStartupTest != 1)
		return;
		
	if(gOpts[AWSWARNINGSOUND].v != -1)
		gSound[gOpts[AWSWARNINGSOUND].v] = ATS_SOUND_PLAYLOOPING;
	if(gOpts[STARTUPTEST].v == 2)
	{
		SetIndicator(brakeDemand, 1);
		SetIndicator(stopOverride, 1);
		if(gOpts[TPWSINDICATOR3].v != -1)
			gPanel[gOpts[TPWSINDICATOR3].v] = 1;
	}
	tpwsStartupTest = 2;
}

