//tarkistettu 180105
//vaikuttaisi olevan ok

#ifndef WINDSHIELD
#define WINDSHIELD

#include "atsplugin.h"

void InitWindshield();
void DestroyWindshield();

void AnimateWindshield();
void WindshieldKeyPressed(int key);
void WindshieldBeacon(ATS_BEACONDATA data);

#endif

