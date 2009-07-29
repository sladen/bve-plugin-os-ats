//tarkistettu 180105

#ifndef BR_SAFETY
#define BR_SAFETY

#include "atsplugin.h"

void InitBRSafety();
void DestroyBRSafety();

void BRSafety();
void SetBRSafetyStatus(ATS_BEACONDATA);
void BRSafetyKeyPressed(int key);
void BRSafetyKeyReleased(int key);
void BRSafetyReverserMovedOff();

#endif

