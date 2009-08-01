#ifndef __WIN32__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#endif
#include "confload.h"

#ifdef __WIN32__
//stdio imitation
DWORD stdio_imitation_temp;
#define fopenr(f) CreateFile(f, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)
#define fopenw(f) CreateFile(f, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)
//#define fputc(f, c) WriteFile(f, &c, 1, &stdio_imitation_temp, NULL)
//#define fputs(f, s, n) WriteFile(f, s, n, &stdio_imitation_temp, NULL)
#define fclose(f) CloseHandle(f)
#define EOF 256
int fgetc(HANDLE f)
{
	char feed[2];
	BOOL success = ReadFile(f, &feed[0], 1, &stdio_imitation_temp, NULL);
	if(success == FALSE || stdio_imitation_temp == 0) return EOF;
	return feed[0];
}
int fgets(char *buf, int maxlen, HANDLE f)
{
	int feed, read = 0;
	while(read < maxlen - 1) {
		feed = fgetc(f);
		if(feed == EOF || feed == '\n')
			break;
		*(buf++) = (char)feed;
		read++;
	}
	*buf = '\0';
	return read;
}
//end stdio imitation
#endif


char commands[NUM_PLUGIN_OPTIONS][COMMAND_LENGTH] = {
"system=", "awsindicator=", "awswarningsound=", "awsclearsound=", "awsacknowledgekey=", "awsminspeed=",
"awsdelay=", "awsindicatorappear=", "awsbrakecancel=", "tpwsindicator=", "tpwsindicator2=", "tpwsindicator3=",
"tpwswarningsound=", "tpwsresetkey=", "tpwsoverridekey=", "tpwsstopdelay=", "tpwsoverridelifetime=",
"tpwsbrakecancel=", "startuptest=",

"overspeedcontrol=", "warningspeed=", "overspeed=", "safespeed=", "overspeedindicator=", "overspeedalarm=",

"vigilance=", "vigilancelamp=", "vigilancealarm=", "vigilancekey=", "vigilanceinterval=",
"vigilancedelay1=", "vigilancedelay2=", "independentvigilance=", "vigilanceinactivespeed=",
"vigilancecancellable=", "vigilanceautorelease=",

"powergapbehaviour=", "powerpickuppoints=", "powerindicator=",
"doorpowerlock=", "doorapplybrake=", "tractioninterlock=", "neutralrvrbrake=",
"reversercontrol=", "reverserindex=", "effectivervrindex=",
"reminderindicator=", "reminderkey=",

"wiperindex=", "wiperrate=", "wiperholdposition=", "wiperdelay=", "wiperonkey=",
"wiperoffkey=", "wipersound=", "dropstartindex=", "numberofdrops=", "dropsound=",

"travelmetermode=", "travelmeter100=", "travelmeter10=",
"travelmeter1=", "travelmeter01=", "travelmeter001=",

"traction=", "automatic=", "automaticindicator=", "ammeter=", "ammetervalues=", "fuelconsumption=",
"fuelcapacity=", "fuelstartamount=", "fuelfillspeed=", "fuelindicator=", "gearratios=",
"gearfadeinrange=", "gearfadeoutrange=", "gearindicator=", "gearupkey=", "geardownkey=",
"gearchangesound=", "tachometer=", "heatingpart=", "heatingrate=", "overheatwarn=", "overheat=",
"overheatresult=", "thermometer=", "overheatindicator=", "overheatalarm=",

"cutoffmax=", "cutoffmin=", "cutoffineffective=", "cutoffratiobase=", "cutoffratio=",
"cutoffdeviation=", "cutoffincreasekey=", "cutoffdecreasekey=",
"cutoffchangespeed=", "cutoffindicator=",
"regulatorpressureuse=", //"cutoffpressureuse=",
"boilermaxpressure=", "boilerminpressure=", "boilerblowoffpressure=", "boilerpressureindicator=",
"boilermaxwaterlevel=", "boilerwatertosteamrate=", "boilerwaterlevelindicator=",
"injectorrate=", "injectorindicator=", "injectortogglekey=",

"doorindicator=", "klaxonindicator=", "gearloopsound=", "fuelfillindicator=", "customindicators="
};

PLUGIN_OPTION gOpts[NUM_PLUGIN_OPTIONS];
bool pointersNullified = false;


void SetDefaults()
{
	for(int c = 0; c < NUM_PLUGIN_OPTIONS; c++)
		gOpts[c].next = NULL;
	pointersNullified = true;

	gOpts[SYSTEM].v = 0;
	gOpts[AWSINDICATOR].v = -1;
	gOpts[AWSWARNINGSOUND].v = -1;
	gOpts[AWSCLEARSOUND].v = -1;
	gOpts[AWSACKNOWLEDGEKEY].v = 0;
	gOpts[AWSMINSPEED].v = 0;
	gOpts[AWSDELAY].v = 3000;
	gOpts[AWSINDICATORAPPEAR].v = 0;
	gOpts[AWSBRAKECANCEL].v = 0;
	gOpts[TPWSINDICATOR].v = -1;
	gOpts[TPWSINDICATOR2].v = -1;
	gOpts[TPWSINDICATOR3].v = -1;
	gOpts[TPWSWARNINGSOUND].v = -1;
	gOpts[TPWSRESETKEY].v = 0;
	gOpts[TPWSOVERRIDEKEY].v = -1;
	gOpts[TPWSSTOPDELAY].v = 60;
	gOpts[TPWSOVERRIDELIFETIME].v = 60;
	gOpts[TPWSBRAKECANCEL].v = 0;
	gOpts[STARTUPTEST].v = 0;

	gOpts[OVERSPEEDCONTROL].v = 0;
	gOpts[WARNINGSPEED].v = -1;
	gOpts[OVERSPEED].v = 1000;
	gOpts[SAFESPEED].v = 0;
	gOpts[OVERSPEEDINDICATOR].v = -1;
	gOpts[OVERSPEEDALARM].v = -1;

	gOpts[VIGILANCE].v = 0;
	gOpts[VIGILANCELAMP].v = -1;
	gOpts[VIGILANCEALARM].v = -1;
	gOpts[VIGILANCEKEY].v = 2;
	gOpts[VIGILANCEINTERVAL].v = 60000;
	gOpts[VIGILANCEDELAY1].v = 3000;
	gOpts[VIGILANCEDELAY2].v = 3000;
	gOpts[INDEPENDENTVIGILANCE].v = 0;
	gOpts[VIGILANCEINACTIVESPEED].v = 0;
	gOpts[VIGILANCECANCELLABLE].v = 0;
	gOpts[VIGILANCEAUTORELEASE].v = 0;
	
	gOpts[POWERGAPBEHAVIOUR].v = 0;
	gOpts[POWERPICKUPPOINTS].v = 0;
	gOpts[POWERINDICATOR].v = -1;
	gOpts[DOORPOWERLOCK].v = 0;
	gOpts[DOORAPPLYBRAKE].v = 0;
	gOpts[TRACTIONINTERLOCK].v = 0;
	gOpts[NEUTRALRVRBRAKE].v = 0;
	gOpts[REVERSERCONTROL].v = 0;
	gOpts[REVERSERINDEX].v = -1;
	gOpts[EFFECTIVERVRINDEX].v = -1;
	gOpts[REMINDERINDICATOR].v = -1;
	gOpts[REMINDERKEY].v = -1;

	gOpts[WIPERINDEX].v = -1;
	gOpts[WIPERRATE].v = 1000;
	gOpts[WIPERHOLDPOSITION].v = 0;
	gOpts[WIPERDELAY].v = 0;
	gOpts[WIPERONKEY].v = 5;
	gOpts[WIPEROFFKEY].v = 6;
	gOpts[WIPERSOUND].v = -1;
	gOpts[DROPSTARTINDEX].v = 0;
	gOpts[NUMBEROFDROPS].v = 0;
	gOpts[DROPSOUND].v = -1;

	gOpts[TRAVELMETERMODE].v = -1;
	gOpts[TRAVELMETER100].v = -1;
	gOpts[TRAVELMETER10].v = -1;
	gOpts[TRAVELMETER1].v = -1;
	gOpts[TRAVELMETER01].v = -1;
	gOpts[TRAVELMETER001].v = -1;

	gOpts[TRACTION].v = 0;
	gOpts[AUTOMATIC].v = -1;
	gOpts[AUTOMATICINDICATOR].v = -1;
	gOpts[AMMETER].v = -1;
	gOpts[AMMETERVALUES].v = 0;
	gOpts[FUELCONSUMPTION].v = 0;
	gOpts[FUELCAPACITY].v = 1;
	gOpts[FUELSTARTAMOUNT].v = 1;
	gOpts[FUELFILLSPEED].v = 50;
	gOpts[FUELINDICATOR].v = -1;
	gOpts[GEARRATIOS].v = 0;
	gOpts[GEARFADEINRANGE].v = 0;
	gOpts[GEARFADEOUTRANGE].v = 0;
	gOpts[GEARINDICATOR].v = -1;
	gOpts[GEARUPKEY].v = 3;
	gOpts[GEARDOWNKEY].v = 4;
	gOpts[GEARCHANGESOUND].v = -1;
	gOpts[TACHOMETER].v = -1;
	gOpts[HEATINGPART].v = 0;
	gOpts[HEATINGRATE].v = 0;
	gOpts[OVERHEATWARN].v = 0;
	gOpts[OVERHEAT].v = 0;
	gOpts[OVERHEATRESULT].v = 0;
	gOpts[THERMOMETER].v = -1;
	gOpts[OVERHEATINDICATOR].v = -1;
	gOpts[OVERHEATALARM].v = -1;

	gOpts[CUTOFFMAX].v = 75;
	gOpts[CUTOFFMIN].v = -55;
	gOpts[CUTOFFINEFFECTIVE].v = 15;
	gOpts[CUTOFFRATIOBASE].v = 30;
	gOpts[CUTOFFRATIO].v = 10;
	gOpts[CUTOFFDEVIATION].v = 8;
	gOpts[CUTOFFINCREASEKEY].v = 5;
	gOpts[CUTOFFDECREASEKEY].v = 6;
	gOpts[CUTOFFCHANGESPEED].v = 40;
	gOpts[CUTOFFINDICATOR].v = -1;
	gOpts[REGULATORPRESSUREUSE].v = 32;
	gOpts[BOILERMAXPRESSURE].v = 20000;
	gOpts[BOILERMINPRESSURE].v = 12000;
	gOpts[BOILERBLOWOFFPRESSURE].v = 21000;
	gOpts[BOILERPRESSUREINDICATOR].v = -1;
	gOpts[BOILERMAXWATERLEVEL].v = 16000;
	gOpts[BOILERWATERTOSTEAMRATE].v = 1500;
	gOpts[BOILERWATERLEVELINDICATOR].v = -1;
	gOpts[INJECTORRATE].v = 100;
	gOpts[INJECTORINDICATOR].v = -1;
	gOpts[INJECTORTOGGLEKEY].v = -1;
	
	gOpts[DOORINDICATOR].v = -1;
	gOpts[KLAXONINDICATOR].v = -1;
	gOpts[GEARLOOPSOUND].v = -1;
	gOpts[FUELFILLINDICATOR].v = -1;
	gOpts[CUSTOMINDICATORS].v = -1;
}


bool LoadConfig()
{
//	char buf[MAX_PATH + 1 + 12] = "";
#ifdef __WIN32__
	char *buf = new char[MAX_PATH + 1 + 12];

	if(buf == NULL)
		return false;
	*buf = '\0';
	
	if(GetModuleFileName(GetModuleHandle(ATS_MODULE_NAME), buf, MAX_PATH + 12) <= 2)
	{
		delete buf;
		return false;
	}
	
	int len = strlen(buf);
 	buf[len - 3] = 'c';
	buf[len - 2] = 'f';
	buf[len - 1] = 'g';

			
	HANDLE f = fopenr(buf);
	if(f == INVALID_HANDLE_VALUE)
	{
		delete buf;
		return false;
	}
#else
	// Hardcoding, but still need to figure out how to get the Train's directory -sladen
	char *cwd = get_current_dir_name();
	fprintf(stderr, "OS_Ats1: LoadConfig() cwd=\"%s\"\n", cwd);
	free(cwd);
	FILE *f = fopen("OS_Ats1.cfg", "r");
	if (f == NULL)
	  return false;
#endif
	fprintf(stderr, "OS_Ats1: LoadConfig(), found the config!\n");
		
//	char line[LINE_LENGTH+1];
	char *line = new char[LINE_LENGTH + 1];
	if(line == NULL)
	{
#ifdef __WIN32__
		delete buf;
#endif
		return false;
	}
	
	while(fgets(line, LINE_LENGTH, f) > 0)
	{
		if(line[0] == ';')
			continue;
			
		for(int c = 0; c < NUM_PLUGIN_OPTIONS; c++)
		{
			bool found = true;
			for(unsigned int i = 0; i < min(strlen(commands[c]), strlen(line)); i++)
				if(line[i] != commands[c][i])
				{
					found = false;
					break;
				}
				
			if(!found)
				continue;
				
			PLUGIN_OPTION *ptr = &gOpts[c];
			ptr->v = 0;
			ptr->next = NULL;
			bool negative = false;
			
			for(unsigned int i = strlen(commands[c]); i < strlen(line); i++)
			{
				if(line[i] == '-')
					negative = true;
				else if(line[i] == ',')
 				{
 					ptr->next = new PLUGIN_OPTION;
					if(ptr->next == NULL)
						break;
  					ptr = ptr->next;
  					ptr->v = 0;
  					ptr->next = NULL;
					negative = false;
 				}
				else if(line[i] >= '0' && line[i] <= '9')
				{
					ptr->v *= 10;
					ptr->v += line[i] - '0';
					if(negative)
						ptr->v = -ptr->v;
					negative = false;
				}
				else break;
			}
			
			break;
		}
	}
	
	fclose(f);
	
	delete line;
#ifdef __WIN32__
	delete buf;
#endif
	return true;
}


void ScrapConfig()
{
//	MessageBox(NULL, "scrapping config", "dispose called", MB_ICONHAND | MB_SYSTEMMODAL);
	if(pointersNullified == false)
		return;
		
	for(int c = 0; c < NUM_PLUGIN_OPTIONS; c++)
		while(gOpts[c].next != NULL)
		{
			PLUGIN_OPTION *ptr = gOpts[c].next;
			PLUGIN_OPTION *prev = &gOpts[c];
			while(ptr->next != NULL)
			{
				prev = ptr;
				ptr = ptr->next;
			}
			delete ptr;
			prev->next = NULL;
		}
}

