//tarkistettu 180105

#ifndef ATSPLUGIN_H
#define ATSPLUGIN_H

#include <windows.h>
#include "defs.h"
#include "confload.h"
#include "tools.h"
#define ATS_EXPORTS

#define abs(x) ((x) < 0 ? -(x) : (x))
#define swap(x, y, t) { t a=x; x=y; y=a; }

//=============================
// BVE ATS Plug-in Header File
//
//             Rock_On, mackoy
//=============================

#ifdef ATS_EXPORTS
#define ATS_API __declspec(dllexport)
#else
#define ATS_API __declspec(dllimport)
#endif

// ATS Plug-in Version
#define ATS_VERSION	0x00020000

// ATS Keys
#define ATS_KEY_S	0	// S Key
#define ATS_KEY_A1	1	// A1 Key
#define ATS_KEY_A2	2	// A2 Key
#define ATS_KEY_B1	3	// B1 Key
#define ATS_KEY_B2	4	// B2 Key
#define ATS_KEY_C1	5	// C1 Key
#define ATS_KEY_C2	6	// C2 Key
#define ATS_KEY_D	7	// D Key
#define ATS_KEY_E	8	// R Key
#define ATS_KEY_F	9	// F Key
#define ATS_KEY_G	10	// G Key
#define ATS_KEY_H	11	// H Key
#define ATS_KEY_I	12	// I Key
#define ATS_KEY_J	13	// J Key
#define ATS_KEY_K	14	// K Key
#define ATS_KEY_L	15	// L Key

// Initial Handle Position
#define ATS_INIT_REMOVED	2	// Handle Removed
#define ATS_INIT_EMG		1	// Emergency Brake
#define ATS_INIT_SVC		0	// Service Brake

// Sound Control Instruction
#define ATS_SOUND_STOP			-10000	// Stop
#define ATS_SOUND_PLAY			1		// Play Once
#define ATS_SOUND_PLAYLOOPING	0		// Play Repeatedly
#define ATS_SOUND_CONTINUE		2		// Continue

// Type of Horn
#define ATS_HORN_PRIMARY	0	// Horn 1
#define ATS_HORN_SECONDARY	1	// Horn 2
#define ATS_HORN_MUSIC		2	// Music Horn

// Fixed Speed Control Instruction
#define ATS_FIXED_CONTINUE	0	// Continue
#define ATS_FIXED_ENABLE	1	// Enable
#define ATS_FIXED_DISABLE	2	// Disable


// Vehicle Specification
struct ATS_VEHICLESPEC
{
	int BrakeNotches;	// Number of Brake Notches
	int PowerNotches;	// Number of Power Notches
	int AtsNotch;		// ATS Cancel Notch
	int B67Notch;		// 80% Brake (67 degree)
	int Cars;			// Number of Cars
};

// State Quantity of Vehicle
struct ATS_VEHICLESTATE
{
	double Location;	// Train Position (Z-axis) (m)
	float Speed;		// Train Speed (km/h)
	int Time;			// Time (ms)
	float BcPressure;	// Pressure of Brake Cylinder (Pa)
	float MrPressure;	// Pressure of MR (Pa)
	float ErPressure;	// Pressure of ER (Pa)
	float BpPressure;	// Pressure of BP (Pa)
	float SapPressure;	// Pressure of SAP (Pa)
	float Current;		// Current (A)
};

// Received Data from Beacon
struct ATS_BEACONDATA
{
	int Type;		// Type of Beacon
	int Signal;		// Signal of Connected Section
	float Distance;	// Distance to Connected Section (m)
	int Optional;	// Optional Data
};

// Train Operation Instruction
struct ATS_HANDLES
{
	int Brake;		// Brake Notch
	int Power;		// Power Notch
	int Reverser;	// Reverser Position
	int Fixed;		// Fixed Speed Control
};


#include "traincontrol.h"
#include "powercontrol.h"
#include "windshield.h"
#include "br_safety.h"
#include "vigilance.h"
#include "speedcontrol.h"
#include "misc.h"
#include "traction.h"


// Function called when this plug-in is loaded
ATS_API void WINAPI Load(void);

// Function called when this plug-in is unloaded
ATS_API void WINAPI Dispose(void);

// Returns the version numbers of ATS plug-in
ATS_API int WINAPI GetPluginVersion(void);

// Function called when the train is loaded
ATS_API void WINAPI SetVehicleSpec(ATS_VEHICLESPEC);

// Function called when the game is started
ATS_API void WINAPI Initialize(int);

// Function called every frame
ATS_API ATS_HANDLES WINAPI Elapse(ATS_VEHICLESTATE, int *, int *);

// Function called when the power is changed
ATS_API void WINAPI SetPower(int);

// Function called when the brake is changed
ATS_API void WINAPI SetBrake(int);

// Function called when the reverser is changed
ATS_API void WINAPI SetReverser(int);

// Function called when the ATS key is pushed
ATS_API void WINAPI KeyDown(int);

// Function called when the ATS key is released
ATS_API void WINAPI KeyUp(int);

// Function called when the horn is used
ATS_API void WINAPI HornBlow(int);

// Function called when the door is opened
ATS_API void WINAPI DoorOpen(void);

// Function called when the door is closed
ATS_API void WINAPI DoorClose(void);

// Function called when current signal is changed
ATS_API void WINAPI SetSignal(int);

// Function called when the beacon data is received
ATS_API void WINAPI SetBeaconData(ATS_BEACONDATA);

extern ATS_VEHICLESPEC gTrainspecs;
extern ATS_VEHICLESTATE gState;
extern ATS_HANDLES gHandles;
extern ATS_HANDLES gDriver;
extern int *gPanel, *gSound;

#endif
