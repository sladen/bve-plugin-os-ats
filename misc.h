//tarkistettu 180105

#ifndef MISC_H
#define MISC_H

#include "atsplugin.h"

void InitMiscellaneous();
void DestroyMiscellaneous();

void RunMiscellaneous();

void setDoorIndicators();
void setDoorState(bool open);
void setDoorSide(char side);

void hornSounded(int hornType);
void miscKeyPressed(int key);

#endif

