#ifndef __WIN32__
#include <stdlib.h>
#endif
#include "traction.h"

int hpTemperature = 0;
Timer tempTimer = TIMER_OFF;

int fuel;
bool fuelFill;
Timer fuelTimer;

int gear;
int gears;
int gear_maxPower;
Timer gearLoopTimer;

int cutoff;
Timer cutoffChangeTimer;
int stm_reverser;
int stm_power;
int stm_boilerPressure;
int stm_boilerWater;
Timer boilingTimer;
Timer drainTimer;
bool stm_injector;
Timer injectorTimer;
bool stm_blowOff;

bool automatic;
Timer autoTimer;


void InitTraction()
{
	hpTemperature = 0;
	ResetTimer(tempTimer);
	if(gOpts[THERMOMETER].v != -1)
		gPanel[gOpts[THERMOMETER].v] = 0;
		
	fuel = gOpts[FUELSTARTAMOUNT].v;
	if(gOpts[FUELINDICATOR].v != -1)
		gPanel[gOpts[FUELINDICATOR].v] = fuel;
	fuelFill = false;
	ResetTimer(fuelTimer);
	
	gear = 0;
	gears = 1;
	PLUGIN_OPTION *ptr = &gOpts[GEARRATIOS];
	while(ptr->next != NULL)
	{
		ptr = ptr->next;
		gears++;
	}
	gear_maxPower = 0;
	if(gOpts[TRACTION].v == 2 && gOpts[GEARRATIOS].v > 0)
		OperatePower(gear_maxPower, true);
	gearLoopTimer = TIMER_OFF;

	cutoff = 0;
	ResetTimer(cutoffChangeTimer);
	stm_reverser = RVR_NEUTRAL;
	stm_power = PWR_NEUTRAL;
	if(gOpts[TRACTION].v == 1)
	{
		OperateReverser(stm_reverser, true);
		OperatePower(stm_power, true);
	}
	if(gOpts[CUTOFFINDICATOR].v != -1)
		gPanel[gOpts[CUTOFFINDICATOR].v] = cutoff;
	stm_boilerPressure = gOpts[BOILERMAXPRESSURE].v;
	stm_boilerWater = gOpts[BOILERMAXWATERLEVEL].v;
	ResetTimer(boilingTimer);
	ResetTimer(drainTimer);
	stm_injector = false;
	ResetTimer(injectorTimer);
	stm_blowOff = false;
	
	automatic = false;
}

void DestroyTraction()
{
}


void IncreaseTemperature(int increase)
{
		hpTemperature += increase;
		if(hpTemperature < 0)
			hpTemperature = 0;

		if(gOpts[THERMOMETER].v != -1)
			gPanel[gOpts[THERMOMETER].v] = hpTemperature;

		if(gOpts[OVERHEATINDICATOR].v != -1)
			gPanel[gOpts[OVERHEATINDICATOR].v] = (hpTemperature >= gOpts[OVERHEATWARN].v);
		if(gOpts[OVERHEATALARM].v != -1)
			gSound[gOpts[OVERHEATALARM].v] = (hpTemperature < gOpts[OVERHEATWARN].v ? ATS_SOUND_STOP : ATS_SOUND_PLAYLOOPING);

		if(hpTemperature >= gOpts[OVERHEAT].v && gOpts[OVERHEATRESULT].v == 1)
			OperatePower(PWR_NEUTRAL, true);
}


void ChangeGear(int g)
{
	if(g == gear)
		return;
	if(gOpts[GEARCHANGESOUND].v != -1 && (gOpts[GEARCHANGESOUND].next == NULL || g > gear))
		gSound[gOpts[GEARCHANGESOUND].v] = ATS_SOUND_PLAY;
	else if(gOpts[GEARCHANGESOUND].next != NULL && g < gear)
		gSound[gOpts[GEARCHANGESOUND].next->v] = ATS_SOUND_PLAY;
	gear = g;
	if(gOpts[GEARINDICATOR].v != -1)
		gPanel[gOpts[GEARINDICATOR].v] = gear;
	if(gOpts[GEARLOOPSOUND].v != -1 && gOpts[GEARLOOPSOUND].next != NULL)
	{
		if(gear != 0)
			ResetTimer(gearLoopTimer);
		else
			gearLoopTimer = TIMER_OFF;
		gSound[gOpts[GEARLOOPSOUND].v] = ATS_SOUND_STOP;
	}
}


void RunTraction()
{
	//heating part
	if(gOpts[HEATINGPART].v == 1)
		if(tOn(tempTimer, gState.Time, 1000))
		{
			PLUGIN_OPTION *ptr = &gOpts[HEATINGRATE];
			if(gHandles.Reverser != RVR_NEUTRAL && gHandles.Brake == BRK_RELEASE)
				for(int i = 0; i < gHandles.Power; i++)
				{
					if(ptr->next != NULL)
						ptr = ptr->next;
					else break;
				}
			IncreaseTemperature(ptr->v);
		}
	
	
	
	//diesel
	if(gOpts[TRACTION].v == 2)
	{
		bool fuelTimerOn = tOn(fuelTimer, gState.Time, 1000);
		if(gOpts[FUELCONSUMPTION].v > 0)
		{
			if(fuelTimerOn)
			{
				PLUGIN_OPTION *ptr = &gOpts[FUELCONSUMPTION];
				if(gHandles.Reverser != RVR_NEUTRAL && gHandles.Brake == BRK_RELEASE)
					for(int i = 0; i < gHandles.Power; i++)
					{
						if(ptr->next != NULL)
							ptr = ptr->next;
						else break;
					}

				if(fuel > 0)
				{
					fuel -= ptr->v;
					if(fuel <= 0)
					{
						OperatePower(PWR_NEUTRAL, true);
						fuel = 0;
					}
				}
					
				if(gOpts[FUELINDICATOR].v != -1)
				{
					if(gOpts[FUELINDICATOR].next == NULL)
						gPanel[gOpts[FUELINDICATOR].v] = fuel;
					else if(gOpts[FUELCAPACITY].v > 0)
						gPanel[gOpts[FUELINDICATOR].v] = (int)((100 * fuel / gOpts[FUELCAPACITY].v) <= gOpts[FUELINDICATOR].next->v ? 1 : 0);
				}
			}
		}
		
		if(fuelFill && gState.Speed == 0)
		{
			if(gOpts[FUELFILLINDICATOR].v != -1)
				gPanel[gOpts[FUELFILLINDICATOR].v] = 1;
			if(fuelTimerOn)
			{
				if(fuel == 0)
					OperatePower(PWR_NEUTRAL, false);
				fuel += gOpts[FUELFILLSPEED].v;
				if(fuel > gOpts[FUELCAPACITY].v)
					fuel = gOpts[FUELCAPACITY].v;
				if(gOpts[FUELINDICATOR].v != -1)
					gPanel[gOpts[FUELINDICATOR].v] = fuel;
			}
		}
		else if(gOpts[FUELFILLINDICATOR].v != -1)
			gPanel[gOpts[FUELFILLINDICATOR].v] = 0;

		//mechanical transmission
		if(gOpts[GEARRATIOS].v > 0)
		{
			int new_maxPower = gear_maxPower;
			int tacho = 0, pwr_limit = 0;
			
			PLUGIN_OPTION *ptr = &gOpts[GEARRATIOS];
			for(int i = 0; i < gear; i++)
			{
				tacho = (int)(abs(gState.Speed) * ptr->v);
				if(ptr->next != NULL)
					ptr = ptr->next;
				else break;
			}

			if(tacho > 1000)
				tacho = 1000;
			if(gOpts[TACHOMETER].v != -1)
				gPanel[gOpts[TACHOMETER].v] = tacho * (fuel > 0);

				
			//heating part
			if(gOpts[HEATINGPART].v == 2)
				if(tOn(tempTimer, gState.Time, 1000))
				{
					PLUGIN_OPTION *ptr = &gOpts[HEATINGRATE];
					for(int i = 0; i < (tacho * gDriver.Power / PWR_MAX * (fuel > 0) * (gHandles.Power != PWR_NEUTRAL) * (gHandles.Reverser != RVR_NEUTRAL)) / 100; i++)
					{
						if(ptr->next != NULL)
							ptr = ptr->next;
						else break;
					}
					IncreaseTemperature(ptr->v);
				}


			//automatic gearbox
			if(automatic && gHandles.Reverser != RVR_NEUTRAL && gHandles.Brake == BRK_RELEASE &&
   				gDriver.Power != PWR_NEUTRAL && gear == 0)
				ChangeGear(1);
				

			if(gear > 0)
			{
				int fadeinrange = 0;
				ptr = &gOpts[GEARFADEINRANGE];
				for(int i = 0; i < gear; i++)
				{
					fadeinrange = ptr->v;
					if(ptr->next != NULL)
						ptr = ptr->next;
					else break;
				}
			
				int fadeoutrange = 0;
				ptr = &gOpts[GEARFADEOUTRANGE];
				for(int i = 0; i < gear; i++)
				{
					fadeoutrange = ptr->v;
					if(ptr->next != NULL)
						ptr = ptr->next;
					else break;
				}
				
				if(tacho < fadeinrange)
					pwr_limit = (int)((float)(tacho) / fadeinrange * PWR_MAX);
				else if(tacho > 1000 - fadeoutrange)
					pwr_limit = (int)(PWR_MAX - (float)(tacho - (1000 - fadeoutrange)) / fadeoutrange * PWR_MAX);
				else
					pwr_limit = PWR_MAX;
					
				//automatic gearbox
				if(automatic && tOn(autoTimer, gState.Time, 2000))
				{
					ResetTimer(autoTimer);
					if(gHandles.Reverser != RVR_NEUTRAL && gHandles.Brake == BRK_RELEASE)
					{
						if(tacho > min((2000 - fadeoutrange) / 2, 800) && gear < gears)
							ChangeGear(gear + 1);
  						else if(tacho < max(fadeinrange / 2, 200) && gear > 1)
  							ChangeGear(gear - 1);
  					}
					else ChangeGear(0);
				}
			}
			else
   				pwr_limit = PWR_NEUTRAL;

			new_maxPower = max(gDriver.Power - PWR_MAX + pwr_limit, PWR_NEUTRAL);
			
			if(new_maxPower != gear_maxPower)
			{
				OperatePower(gear_maxPower, false);
				gear_maxPower = new_maxPower;
				OperatePower(gear_maxPower, true);
			}
			
			if(gearLoopTimer != TIMER_OFF)
				if(tOn(gearLoopTimer, gState.Time, gOpts[GEARLOOPSOUND].next->v))
 				{
					gSound[gOpts[GEARLOOPSOUND].v] = ATS_SOUND_PLAYLOOPING;
					gearLoopTimer = TIMER_OFF;
 				}
		}
	}
	
	
	
	//steam traction
	else if(gOpts[TRACTION].v == 1)
	{
		int new_reverser = stm_reverser;
		int new_power = stm_power;
		
		//automatic cutoff
		if(automatic)
			if(gDriver.Reverser == RVR_FORWARDS && cutoff <= gOpts[CUTOFFINEFFECTIVE].v)
				cutoff = gOpts[CUTOFFMAX].v;
			else if(gDriver.Reverser == RVR_BACKWARDS && cutoff >= -gOpts[CUTOFFINEFFECTIVE].v)
				cutoff = gOpts[CUTOFFMIN].v;

		
		if(cutoff > gOpts[CUTOFFINEFFECTIVE].v && gOpts[CUTOFFDEVIATION].v != 0)
		{
			new_reverser = RVR_FORWARDS;
			if(gState.Speed > gOpts[CUTOFFRATIOBASE].v)
			{
				float speed = gState.Speed - gOpts[CUTOFFRATIOBASE].v;
				float optimalCutoff = gOpts[CUTOFFMAX].v - speed * gOpts[CUTOFFRATIO].v / 10.0;
				new_power = max((int)(PWR_MAX - abs(optimalCutoff - cutoff) / gOpts[CUTOFFDEVIATION].v), 0);
				
				if(automatic)
					cutoff = (int)max(optimalCutoff, gOpts[CUTOFFINEFFECTIVE].v + 1);
			}
			else
				new_power = max((int)(PWR_MAX - abs(gOpts[CUTOFFMAX].v - cutoff) / (float)gOpts[CUTOFFDEVIATION].v), 0);
		}
		else if(cutoff < -gOpts[CUTOFFINEFFECTIVE].v && gOpts[CUTOFFDEVIATION].v != 0)
		{
			new_reverser = RVR_BACKWARDS;
			if(abs(gState.Speed) > gOpts[CUTOFFRATIOBASE].v)
			{
				float speed = abs(gState.Speed) - gOpts[CUTOFFRATIOBASE].v;
				float optimalCutoff = gOpts[CUTOFFMIN].v + speed * gOpts[CUTOFFRATIO].v / 10.0;
				new_power = max((int)(PWR_MAX - abs(optimalCutoff - cutoff) / gOpts[CUTOFFDEVIATION].v), 0);
				
				if(automatic)
					cutoff = (int)min(optimalCutoff, -gOpts[CUTOFFINEFFECTIVE].v - 1);
			}
			else
				new_power = max((int)(PWR_MAX - abs(gOpts[CUTOFFMIN].v - cutoff) / (float)gOpts[CUTOFFDEVIATION].v), 0);
		}
		else
		{
			new_reverser = RVR_NEUTRAL;
			new_power = PWR_NEUTRAL;
		}
		
		if(new_reverser != stm_reverser)
		{
			OperateReverser(stm_reverser, false);
			stm_reverser = new_reverser;
			OperateReverser(stm_reverser, true);
		}
		
		if(gOpts[CUTOFFINDICATOR].v != -1)
			gPanel[gOpts[CUTOFFINDICATOR].v] = cutoff;
			
		//pressure power drop
		int bp = max(stm_boilerPressure - gOpts[BOILERMINPRESSURE].v, 0);
		int bp_range = gOpts[BOILERMAXPRESSURE].v - gOpts[BOILERMINPRESSURE].v;
		int pwr_limit = min(new_power, (int)((bp / (float)(bp_range) + (1.0 / PWR_MAX - 0.01)) * PWR_MAX));
		new_power = max(gDriver.Power - PWR_MAX + pwr_limit, PWR_NEUTRAL);
		
		if(new_power != stm_power)
		{
			OperatePower(stm_power, false);
			stm_power = new_power;
			OperatePower(stm_power, true);
		}
		
		//pressuregen
		int limit = 100;
		while(tOn(boilingTimer, gState.Time, 60000 / gOpts[BOILERWATERTOSTEAMRATE].v) &&
			stm_boilerWater > 0 && limit--)
		{
			stm_boilerWater--;
			stm_boilerPressure++;
			if(gOpts[BOILERWATERLEVELINDICATOR].v != -1)
				gPanel[gOpts[BOILERWATERLEVELINDICATOR].v] = stm_boilerWater;
				
			if(stm_blowOff)
				stm_boilerPressure -= 4;
		}
		
		if(stm_boilerPressure > gOpts[BOILERBLOWOFFPRESSURE].v && !stm_blowOff)
			stm_blowOff = true;
		else if(stm_boilerPressure < gOpts[BOILERMAXPRESSURE].v)
			stm_blowOff = false;
		
		
		//automatic injector
		if(automatic)
			if(stm_injector && stm_boilerWater > gOpts[BOILERMAXWATERLEVEL].v / 4)
			{
				stm_injector = false;
				ResetTimer(injectorTimer);
				if(gOpts[INJECTORINDICATOR].v != -1)
					gPanel[gOpts[INJECTORINDICATOR].v] = stm_injector;
			}
			else if(!stm_injector && stm_boilerWater < gOpts[BOILERMAXWATERLEVEL].v / 8)
			{
				stm_injector = true;
				ResetTimer(injectorTimer);
				if(gOpts[INJECTORINDICATOR].v != -1)
					gPanel[gOpts[INJECTORINDICATOR].v] = stm_injector;
			}

		//injector
		if(stm_injector)
		{
			if(stm_boilerPressure > 0 && fuel > 0 && stm_boilerWater < gOpts[BOILERMAXWATERLEVEL].v && tOn(injectorTimer, gState.Time, 1000))
			{
				fuel -= gOpts[INJECTORRATE].v;
				if(gOpts[FUELINDICATOR].v != -1)
					gPanel[gOpts[FUELINDICATOR].v] = fuel;
				stm_boilerWater += gOpts[INJECTORRATE].v;
				stm_boilerPressure -= gOpts[INJECTORRATE].v / 4;
			}
		}
		
		//pressure use
		if(stm_reverser != RVR_NEUTRAL)
		{
			if(tOn(drainTimer, gState.Time, 1000))
			{
				int regPrUse = (int)(gDriver.Power / (float)PWR_MAX * gOpts[REGULATORPRESSUREUSE].v);
				float cutPrBoost = abs(cutoff) / abs((float)gOpts[CUTOFFMAX].v);
				float spdPrUse = 1 + abs(gState.Speed) / 25;
				stm_boilerPressure -= (int)(regPrUse * cutPrBoost * spdPrUse);
			}
		}
		
		if(stm_boilerPressure < 0)
			stm_boilerPressure = 0;
			
		if(gOpts[BOILERPRESSUREINDICATOR].v != -1)
			gPanel[gOpts[BOILERPRESSUREINDICATOR].v] = stm_boilerPressure;
				
		if(fuelFill && gState.Speed == 0)
		{
			if(gOpts[FUELFILLINDICATOR].v != -1)
				gPanel[gOpts[FUELFILLINDICATOR].v] = 1;
			if(tOn(fuelTimer, gState.Time, 1000))
			{
				fuel += gOpts[FUELFILLSPEED].v;
				if(fuel > gOpts[FUELCAPACITY].v)
					fuel = gOpts[FUELCAPACITY].v;
				if(gOpts[FUELINDICATOR].v != -1)
					gPanel[gOpts[FUELINDICATOR].v] = fuel;
				ResetTimer(fuelTimer);
			}
		}
		else if(gOpts[FUELFILLINDICATOR].v != -1)
			gPanel[gOpts[FUELFILLINDICATOR].v] = 0;
	}
	
	
	//ammeter
	if(gOpts[AMMETER].v != -1)
	{
		if(gHandles.Reverser == RVR_NEUTRAL || gHandles.Brake != BRK_RELEASE)
			gPanel[gOpts[AMMETER].v] = 0;
		else
		{
			PLUGIN_OPTION *ptr = &gOpts[AMMETERVALUES];
			for(int i = 0; i < gHandles.Power; i++)
			{
				if(ptr->next != NULL)
					ptr = ptr->next;
				else break;
			}
			gPanel[gOpts[AMMETER].v] = ptr->v;
		}
	}
}


void TractionSetData(ATS_BEACONDATA data)
{
	if(data.Type != BCN_FUELING)
		return;
	if(gState.Speed > 0)
		fuelFill = (data.Optional == 1);
	else if(gState.Speed < 0)
		fuelFill = !(data.Optional == 1);
}


void TractionKeyPressed(int key)
{
	if(tOn(cutoffChangeTimer, gState.Time, gOpts[CUTOFFCHANGESPEED].v))
	{
		ResetTimer(cutoffChangeTimer);
		if(key == gOpts[CUTOFFINCREASEKEY].v)
			cutoff++;
		else if(key == gOpts[CUTOFFDECREASEKEY].v)
			cutoff--;

		if(cutoff > gOpts[CUTOFFMAX].v)
			cutoff = gOpts[CUTOFFMAX].v;
		else if(cutoff < gOpts[CUTOFFMIN].v)
			cutoff = gOpts[CUTOFFMIN].v;
	}
	
	if(key == gOpts[INJECTORTOGGLEKEY].v)
	{
		ResetTimer(injectorTimer);
		stm_injector = !stm_injector;
		if(gOpts[INJECTORINDICATOR].v != -1)
			gPanel[gOpts[INJECTORINDICATOR].v] = stm_injector;
	}
	
	if(key == gOpts[GEARDOWNKEY].v && gear > 0 && gDriver.Power == PWR_NEUTRAL)
		ChangeGear(gear - 1);
	
	if(key == gOpts[GEARUPKEY].v && gear < gears && gDriver.Power == PWR_NEUTRAL)
		ChangeGear(gear + 1);
	
	if(key == gOpts[AUTOMATIC].v)
	{
		automatic = !automatic;
		if(gOpts[AUTOMATICINDICATOR].v != -1)
			gPanel[gOpts[AUTOMATICINDICATOR].v] = (int)automatic;
	}
}

