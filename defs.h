//tarkistettu 190105

#ifndef DEFS_H
#define DEFS_H

#define ATS_MODULE_NAME "OS_Ats1.dll"

//general
#define NORMAL 0

//reverser
#define RVR_FORWARDS 1
#define RVR_NEUTRAL 0
#define RVR_BACKWARDS -1

//reverser control
#define RVRCTRL_SPEED 1
#define RVRCTRL_BREAK 2

//power
#define PWR_NEUTRAL 0
#define PWR_MAX gTrainspecs.PowerNotches
//#define PWR_MAX (gTrainspecs.PowerNotches - 1)

//brake
#define BRK_RELEASE 0
#define BRK_MAX gTrainspecs.BrakeNotches
#define BRK_EMG (gTrainspecs.BrakeNotches + 1)
//#define BRK_MAX (gTrainspecs.BrakeNotches - 2)
//#define BRK_EMG (gTrainspecs.BrakeNotches - 1)

//power gap behaviour
#define PGAPB_REDUCEDPOWER 1
#define PGAPB_POWEROFF 2

//beacons
//näiden muuttaminen erittäin varovaisesti!

#define BCN_POWERGAP 20 //erotusjakso
#define BCN_RAIN 21 //sade
#define BCN_FUELING 22 //tankkaus
#define BCN_DOORSIDE 23 //ovien aukaisupuolen

#define BCN_AWS_SIGNAL 44000 //AWS opastimen magneetti
#define BCN_AWS_FIXED 44001 //AWS joka tapauksessa varoittava
#define BCN_TPWS_SIGNAL 44002 //TPWS opastimeen reagoiva (opt: nopeus)
#define BCN_TRAINSTOP 44003 //TPWS / Trainstop opastimen stop
#define BCN_TPWS_SPEED 44004 //TPWS nopeusrajoitus (opt: nopeus)


//aja-opastetta vastaavien opasteiden indexin alku
#define CLEAR_SIGNAL_RANGE_BASE 4
//seis-opastetta vastaavien opasteiden indexin loppu
#define DANGER_SIGNAL_RANGE_LIMIT 0

#endif

