#include "tools.h"

bool tOn(Timer &t, int currentms, int interval)
{
	if(t == 0)
		t = currentms;
	else if(currentms - t >= interval)
	{
		t += interval;
		return true;
	}
	return false;
}


Indicator NewIndicator(int opt, int startval)
{
	Indicator i;
	i.p = &gOpts[opt];
	i.v = startval;
	i.t = TIMER_OFF;
	i.on = false;
	return i;
}

void ShowIndicator(Indicator &i)
{
	if(i.p->v == -1)
		return;
		
	if(i.t != TIMER_OFF && i.p->next != NULL)
		if(tOn(i.t, gState.Time, i.p->next->v))
			i.on = !i.on;
	
	if(i.on)
		gPanel[i.p->v] = i.v;
	else
		gPanel[i.p->v] = 0;
}

