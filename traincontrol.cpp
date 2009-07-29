#include "traincontrol.h"

int reverserLock[3] = { 0, 0, 0 };
int *powerLock = NULL, *brakeLock = NULL;
bool tractionInterlock = false;
static bool initialized = false;


bool InitTrainControl(int brake)
{
	gDriver.Reverser = gDriver.Power = gDriver.Brake = 0;
	tractionInterlock = false;

	if(brake == ATS_INIT_EMG)
		gDriver.Brake = BRK_EMG;
	else if(brake == ATS_INIT_SVC)
		gDriver.Brake = BRK_MAX;
	else
		gDriver.Brake = BRK_RELEASE;

	powerLock = new int[PWR_MAX + 1];
	brakeLock = new int[BRK_EMG + 1];

	reverserLock[0] = reverserLock[1] = reverserLock[2] = 0;
	if(powerLock != NULL)
		for(int i = 0; i <= PWR_MAX; i++)
			powerLock[i] = 0;
	if(brakeLock != NULL)
		for(int i = 0; i <= BRK_EMG; i++)
			brakeLock[i] = 0;

	OperateReverser(gDriver.Reverser, true);
	OperatePower(gDriver.Power, true);
	OperateBrake(gDriver.Brake, true);
	initialized = true;

	return true;
}

void DestroyTrainControl()
{
	if(!initialized)
		return;
	if(powerLock != NULL)
		delete powerLock;
	if(brakeLock != NULL)
		delete brakeLock;
}


void OperateReverser(int setting, bool lock)
{
	if(setting < -1 || setting > 1)
		return;
	reverserLock[setting + 1] += (lock ? 1 : -1);
	
	if(reverserLock[1] > 0)
		gHandles.Reverser = 0;
	else if(reverserLock[2] > 0)
 		gHandles.Reverser = 1;
	else if(reverserLock[0] > 0)
 		gHandles.Reverser = -1;
 		
 	if(gOpts[EFFECTIVERVRINDEX].v != -1)
		gPanel[gOpts[EFFECTIVERVRINDEX].v] = gHandles.Reverser + (gHandles.Reverser == -1 ? 3 : 0);
}


void OperatePower(int setting, bool lock)
{
	if(powerLock == NULL || setting < 0 || setting > PWR_MAX)
		return;

	powerLock[setting] += (lock ? 1 : -1);
	
	if(tractionInterlock && gOpts[TRACTIONINTERLOCK].v == 1)
		gHandles.Power = PWR_NEUTRAL;
	else
		for(int i = PWR_NEUTRAL; i <= PWR_MAX; i++)
			if(powerLock[i] > 0)
			{
				gHandles.Power = i;
				break;
			}
			
//	if(gOpts[DEBUG].v != -1 && gHandles.Power == PWR_NEUTRAL)
//		gSound[gOpts[DEBUG].v] = ATS_SOUND_PLAY;
}


void OperateBrake(int setting, bool lock)
{
	if(brakeLock == NULL || setting < 0 || setting > BRK_EMG)
		return;

	brakeLock[setting] += (lock ? 1 : -1);
	for(int i = BRK_EMG; i >= BRK_RELEASE; i--)
		if(brakeLock[i] > 0)
		{
			gHandles.Brake = i;
			if(i > 0)
			{
				tractionInterlock = true;
				if(gOpts[TRACTIONINTERLOCK].v == 1)
					gHandles.Power = PWR_NEUTRAL;
			}
			break;
		}
}


void SetReverserHandle(int setting)
{
	if(gOpts[REVERSERCONTROL].v == RVRCTRL_SPEED && gState.Speed != 0 && setting != RVR_NEUTRAL)
		return;

	OperateReverser(gDriver.Reverser, false);
	OperateReverser(setting, true);
	gDriver.Reverser = setting;
	
	if(gOpts[NEUTRALRVRBRAKE].v != 0)
 		OperateBrake((gOpts[NEUTRALRVRBRAKE].v == 1 ? BRK_MAX : BRK_EMG), (setting == 0));

	if(gOpts[REVERSERCONTROL].v == RVRCTRL_BREAK && gState.Speed != 0)
		OperateReverser(RVR_NEUTRAL, true);
}


void SetPowerHandle(int setting)
{
	if(setting == PWR_NEUTRAL || gDriver.Power == PWR_NEUTRAL)
		tractionInterlock = false;
		
	OperatePower(gDriver.Power, false);
	OperatePower(setting, true);
	gDriver.Power = setting;
}

void SetBrakeHandle(int setting)
{
	OperateBrake(gDriver.Brake, false);
	OperateBrake(setting, true);
	gDriver.Brake = setting;
}

