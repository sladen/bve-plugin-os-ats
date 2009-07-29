#include "speedcontrol.h"

int scSpeed;
bool brakingSequence;


void InitSpeedControl()
{
	scSpeed = 0;
	brakingSequence = false;
}


void DestroySpeedControl()
{
}


void CheckSpeed(float speed)
{
	if(gOpts[OVERSPEEDCONTROL].v == 0 || (int)speed == scSpeed)
		return;
	
	scSpeed = (int)abs(speed);
	
	if(scSpeed >= gOpts[WARNINGSPEED].v)
	{
		if(gOpts[OVERSPEEDINDICATOR].v != -1)
			gPanel[gOpts[OVERSPEEDINDICATOR].v] = 1;
		if(gOpts[OVERSPEEDALARM].v != -1)
			gSound[gOpts[OVERSPEEDALARM].v] = ATS_SOUND_PLAYLOOPING;
	}
	
	if(scSpeed >= gOpts[OVERSPEED].v && brakingSequence == false)
	{
		if(gOpts[OVERSPEEDINDICATOR].v != -1)
			gPanel[gOpts[OVERSPEEDINDICATOR].v] = 1;
		if(gOpts[OVERSPEEDALARM].v != -1)
			gSound[gOpts[OVERSPEEDALARM].v] = ATS_SOUND_PLAYLOOPING;
		OperateBrake(BRK_MAX, true);
		brakingSequence = true;
	}
	else if(scSpeed <= gOpts[SAFESPEED].v)
	{
		if(gOpts[OVERSPEEDINDICATOR].v != -1)
			gPanel[gOpts[OVERSPEEDINDICATOR].v] = 0;
		if(gOpts[OVERSPEEDALARM].v != -1)
			gSound[gOpts[OVERSPEEDALARM].v] = ATS_SOUND_STOP;
		if(brakingSequence == true)
			OperateBrake(BRK_MAX, false);
		brakingSequence = false;
	}
}

