#include "atsplugin.h"

char copyright[] = "Copyright Oskari Saarekas 2004, 2005";

ATS_VEHICLESPEC gTrainspecs;
bool trainspecsSet = false;
int initBrake = ATS_INIT_EMG;

ATS_VEHICLESTATE gState;
ATS_HANDLES gHandles;
ATS_HANDLES gDriver;
int memPanelReserve[256];
int memSoundReserve[256];
int *gPanel = &memPanelReserve[0];
int *gSound = &memSoundReserve[0];



// Function called when this plug-in is loaded
ATS_API void WINAPI Load(void)
{
	for(int i = 0; i < 256; i++)
	{
		memPanelReserve[i] = 0;
		memSoundReserve[i] = ATS_SOUND_STOP;
	}
	gPanel = &memPanelReserve[0];
	gSound = &memSoundReserve[0];
}

// Function called when this plug-in is unloaded
ATS_API void WINAPI Dispose(void)
{
	ScrapConfig();
	DestroyTrainControl();
	DestroyPowerControl();
	DestroyWindshield();
	DestroyBRSafety();
	DestroyVigilance();
	DestroySpeedControl();
	DestroyMiscellaneous();
	DestroyTraction();
}

ATS_API int WINAPI GetPluginVersion(void)
{
	return ATS_VERSION;
}

// Function called when the train is loaded
ATS_API void WINAPI SetVehicleSpec(ATS_VEHICLESPEC vs) {
	gTrainspecs = vs;
	if(trainspecsSet == false)
	{
		trainspecsSet = true;
		Initialize(initBrake);
	}
}

// Function called when the game is started
ATS_API void WINAPI Initialize(int brake)
{
	gPanel = &memPanelReserve[0];
	gSound = &memSoundReserve[0];
	
	initBrake = brake;
	if(trainspecsSet == false)
		return;
		
	ScrapConfig();
	SetDefaults();
	LoadConfig();
	
	DestroyTrainControl();
	InitTrainControl(brake);
	DestroyPowerControl();
	InitPowerControl();
	DestroyWindshield();
	InitWindshield();
	DestroyBRSafety();
	InitBRSafety();
	DestroyVigilance();
	InitVigilance();
	DestroySpeedControl();
	InitSpeedControl();
	DestroyMiscellaneous();
	InitMiscellaneous();
	DestroyTraction();
	InitTraction();
}

// Function called every frame
ATS_API ATS_HANDLES WINAPI Elapse(ATS_VEHICLESTATE vs, int *panel, int *sound)
{
	if(gPanel != panel)
	{
		gPanel = panel;
		for(int i = 0; i < 256; i++)
			panel[i] = memPanelReserve[i];
	}
	
	if(gSound != sound)
	{
		gSound = sound;
		for(int i = 0; i < 256; i++)
			sound[i] = memSoundReserve[i];
	}
	
	gState = vs;
	
	PowerControl();
	AnimateWindshield();
	BRSafety();
	CheckVigilance();
	CheckSpeed(gState.Speed);
	RunMiscellaneous();
	RunTraction();
	
  	return gHandles;
}

// Function called when the power is changed
ATS_API void WINAPI SetPower(int setting)
{
	SetPowerHandle(setting);
	VigilanceKeyPressed(setting, false);
}

// Function called when the brake is changed
ATS_API void WINAPI SetBrake(int setting)
{
	SetBrakeHandle(setting);
	VigilanceKeyPressed(setting, false);
}

// Function called when the reverser is changed
ATS_API void WINAPI SetReverser(int setting)
{
	if(gOpts[REVERSERINDEX].v != -1)
		gPanel[gOpts[REVERSERINDEX].v] = setting + (setting == -1 ? 3 : 0);
	
	SetReverserHandle(setting);
	VigilanceKeyPressed(setting, false);
	if(setting != 0)
		BRSafetyReverserMovedOff();
}

// Function called when the ATS key is pushed
ATS_API void WINAPI KeyDown(int key)
{
	PowerControlKeyPressed(key);
	WindshieldKeyPressed(key);
	BRSafetyKeyPressed(key);
	VigilanceKeyPressed(key, true);
	TractionKeyPressed(key);
	miscKeyPressed(key);
}

// Function called when the ATS key is released
ATS_API void WINAPI KeyUp(int key)
{
	BRSafetyKeyReleased(key);
}

// Function called when the horn is used
ATS_API void WINAPI HornBlow(int n)
{
	VigilanceKeyPressed(n, false);
	hornSounded(n);
}

// Function called when the door is opened
ATS_API void WINAPI DoorOpen(void)
{
	if(gOpts[DOORPOWERLOCK].v == 1)
		OperatePower(PWR_NEUTRAL, true);
		
	if(gOpts[DOORAPPLYBRAKE].v == 1)
		OperateBrake(BRK_MAX, true);
	else if(gOpts[DOORAPPLYBRAKE].v == 2)
		OperateBrake(BRK_EMG, true);

	setDoorState(true);
}

// Function called when the door is closed
ATS_API void WINAPI DoorClose(void)
{
	if(gOpts[DOORPOWERLOCK].v == 1)
		OperatePower(PWR_NEUTRAL, false);
		
	if(gOpts[DOORAPPLYBRAKE].v == 1)
		OperateBrake(BRK_MAX, false);
	else if(gOpts[DOORAPPLYBRAKE].v == 2)
		OperateBrake(BRK_EMG, false);

	setDoorState(false);
}

// Function called when current signal is changed
ATS_API void WINAPI SetSignal(int n) {}

// Function called when the beacon data is received
ATS_API void WINAPI SetBeaconData(ATS_BEACONDATA p)
{
	PowerBeacon(p);
	WindshieldBeacon(p);
	SetBRSafetyStatus(p);
	TractionSetData(p);
	
	if(p.Type == BCN_DOORSIDE)
		setDoorSide((char)p.Optional);
}

