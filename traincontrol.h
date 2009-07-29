//tarkistettu 190105

#ifndef TRAINCONTROL_H
#define TRAINCONTROL_H

#include "atsplugin.h"

bool InitTrainControl(int brake);
void DestroyTrainControl();

//ats actions
void OperateReverser(int setting, bool lock);
void OperatePower(int setting, bool lock);
void OperateBrake(int setting, bool lock);

//driver's actions
void SetReverserHandle(int setting);
void SetPowerHandle(int setting);
void SetBrakeHandle(int setting);

#endif

