//tarkistettu 190105

#ifndef POWERCONTROL_H
#define POWERCONTROL_H

#include "atsplugin.h"

void InitPowerControl();
void DestroyPowerControl();

void PowerBeacon(ATS_BEACONDATA p);
//void CreateGap(int length);
void PowerControl();
void PowerControlKeyPressed(int key);

#endif

