//tarkistettu 180105

#ifndef TIMER_H
#define TIMER_H

#include "confload.h"

#define ResetTimer(x) (x = gState.Time)
#define TIMER_OFF -1

typedef long Timer;
bool tOn(Timer &t, int currentms, int interval);

typedef struct Indicator
{
	struct PLUGIN_OPTION *p;
	int v;
	Timer t;
	bool on;
};
Indicator NewIndicator(int opt, int startval);
#define SetIndicator(i, x) { i.v = x; i.t = TIMER_OFF; i.on = true; }
#define BlinkIndicator(i, x) { i.v = x; ResetTimer(i.t); i.on = true; }
void ShowIndicator(Indicator &i);

#endif

