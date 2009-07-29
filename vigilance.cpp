#include "vigilance.h"

Timer vigilance = 0;
int phase = 0;


void InitVigilance()
{
	vigilance = 0;
	phase = 0;
	if(gOpts[VIGILANCELAMP].v != -1)
		gPanel[gOpts[VIGILANCELAMP].v] = 0;
	if(gOpts[VIGILANCEALARM].v != -1)
		gSound[gOpts[VIGILANCEALARM].v] = ATS_SOUND_STOP;
}

void DestroyVigilance()
{
}


void CheckVigilance()
{
	if(gOpts[VIGILANCE].v == 0)
		return;

	if(phase == 0)
	{
		if(gOpts[VIGILANCEINACTIVESPEED].v == -2 && gHandles.Reverser == 0)
			ResetTimer(vigilance);
		else if(gOpts[VIGILANCEINACTIVESPEED].v != -2 && abs(gState.Speed) <= gOpts[VIGILANCEINACTIVESPEED].v)
			ResetTimer(vigilance);
		else
		{
			PLUGIN_OPTION *ptr = &gOpts[VIGILANCEINTERVAL];
			for(int i = 0; i < gHandles.Power; i++)
			{
				if(ptr->next != NULL)
					ptr = ptr->next;
 				else break;
			}
			int interval = ptr->v;

			if(tOn(vigilance, gState.Time, interval))
			{
				if(gOpts[VIGILANCELAMP].v != -1)
					gPanel[gOpts[VIGILANCELAMP].v] = 1;
				phase = 1;
			}
		}
	}
	else if(phase == 1)
	{
		if(tOn(vigilance, gState.Time, gOpts[VIGILANCEDELAY1].v))
		{
			if(gOpts[VIGILANCEALARM].v != -1)
				gSound[gOpts[VIGILANCEALARM].v] = ATS_SOUND_PLAYLOOPING;
			phase = 2;
		}
	}
	else if(phase == 2)
	{
		if(tOn(vigilance, gState.Time, gOpts[VIGILANCEDELAY2].v))
		{
			OperateBrake(BRK_EMG, true);
			phase = 3;
		}
	}
	else if(phase == 3)
	{
		if(gOpts[VIGILANCEAUTORELEASE].v == 1 && gState.Speed == 0)
		{
			if(gOpts[VIGILANCELAMP].v != -1)
				gPanel[gOpts[VIGILANCELAMP].v] = 0;
			if(gOpts[VIGILANCEALARM].v != -1)
				gSound[gOpts[VIGILANCEALARM].v] = ATS_SOUND_STOP;
			OperateBrake(BRK_EMG, false);
			ResetTimer(vigilance);
			phase = 0;
		}
	}
}

void VigilanceKeyPressed(int key, bool ats)
{
	if(gOpts[VIGILANCE].v == 0)
		return;

	//ei jarrutusta +
	//painaa mitä tahansa nappia kun vigilance ei independent
	//tai painaa vigilancenappia
	if(phase != 3 && (gOpts[INDEPENDENTVIGILANCE].v == 0 || (ats && key == gOpts[VIGILANCEKEY].v)))
	{
		if(gOpts[VIGILANCELAMP].v != -1)
			gPanel[gOpts[VIGILANCELAMP].v] = 0;
		if(gOpts[VIGILANCEALARM].v != -1)
			gSound[gOpts[VIGILANCEALARM].v] = ATS_SOUND_STOP;
		ResetTimer(vigilance);
		phase = 0;
	}
	
	//jarrutus ohi - painaa nappia
	else if(ats && key == gOpts[VIGILANCEKEY].v)
	{
		if(phase == 3 && (gState.Speed == 0 || gOpts[VIGILANCECANCELLABLE].v == 1))
		{
			if(gOpts[VIGILANCELAMP].v != -1)
				gPanel[gOpts[VIGILANCELAMP].v] = 0;
			if(gOpts[VIGILANCEALARM].v != -1)
				gSound[gOpts[VIGILANCEALARM].v] = ATS_SOUND_STOP;
			OperateBrake(BRK_EMG, false);
			ResetTimer(vigilance);
			phase = 0;
		}
	}
}

