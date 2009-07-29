//tarkistettu 190105

#ifndef TRACTION_H
#define TRACTION_H

#include "atsplugin.h"

void InitTraction();
void DestroyTraction();

void RunTraction();
void TractionSetData(ATS_BEACONDATA data);
void TractionKeyPressed(int key);

#endif

