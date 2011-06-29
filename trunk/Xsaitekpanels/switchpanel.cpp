/****** switchpanels.cpp **********/
/****  William R. Good  ********/ 


#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include <linux/hidraw.h>

#include "saitekpanels.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <asm/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define testbit(x, y)  ( ( ((const char*)&(x))[(y)>>3] & 0x80 >> ((y)&0x07)) >> (7-((y)&0x07) ) )

/****************** Switch Panel variables *******************************/
static int batnum = 0, gennum = 0, engnum = 0;
static int rres;

static float opencowl = 1, closecowl = 0;

static int MAG_OFF = 10, MAG_RIGHT = 9, MAG_LEFT = 8, MAG_BOTH = 23;
static int ENG_START = 22;
static int MASTER_BATTERY = 6, MASTER_ALTENATOR = 7;
static int AVIONICS_POWER = 5, FUEL_PUMP = 4;
static int DE_ICE = 3, PITOT_HEAT = 2;
static int COWL_FLAPS = 1, PANEL_LIGHTS = 0;
static int BEACON_LIGHTS = 15, NAV_LIGHTS = 14;
static int STROBE_LIGHTS = 13, TAXI_LIGHTS = 12;
static int LANDING_LIGHTS = 11;
static int GEAR_SWITCH_UP = 21, GEAR_SWITCH_DN = 20; 
static int BatArrayOn[8];

static unsigned char switchbuf[3];
static char switchwbuf[2], gearled;


/** Switch Panel Process  **/
void process_switch_panel()

{

  switchwbuf[0] = 0;
  switchwbuf[1] = gearled;

/******* Only do a read if something new to be read ********/   
  int             switchres;
  fd_set          switchsready;
  struct timeval  switchnowait;

  FD_ZERO(&switchsready);
  FD_SET((unsigned int)switchfd,&switchsready);
  switchnowait.tv_sec = 0;    // specify how many seconds you would like to wait for timeout
  switchnowait.tv_usec = 0;   // how many microseconds? If both is zero, select will return immediately

  switchres = select(switchfd+1,&switchsready,NULL,NULL,&switchnowait);
  if( FD_ISSET(switchfd,&switchsready) ) {
      rres = read(switchfd, switchbuf, sizeof(switchbuf));
  }
  else {
  }

  /* if no gear do not write */

  if(XPLMGetDatai(GearRetract) > 0){
    ioctl(switchfd, HIDIOCSFEATURE(2), switchwbuf);
    /*write(switchfd, switchwbuf, sizeof(switchwbuf));*/
  }
  else {
  }

  batnum = XPLMGetDatai(BatNum), gennum = XPLMGetDatai(GenNum), engnum = XPLMGetDatai(EngNum);

/***************** Engines Mag Off ********************/

	if(testbit(switchbuf,MAG_OFF)) {
	  if(engnum == 1){
	    XPLMCommandOnce(MagOff1);
	  }   
	  if(engnum == 2){
	    XPLMCommandOnce(MagOff1);
	    XPLMCommandOnce(MagOff2);
	  } 
	  if(engnum == 3){
	    XPLMCommandOnce(MagOff1);
	    XPLMCommandOnce(MagOff2);
	    XPLMCommandOnce(MagOff3);
	  }
	  if(engnum == 4){
	    XPLMCommandOnce(MagOff1);
	    XPLMCommandOnce(MagOff2);
	    XPLMCommandOnce(MagOff3);
	    XPLMCommandOnce(MagOff4);
	  }
 	}

/***************** Engines Right Mag *******************/

	if(testbit(switchbuf,MAG_RIGHT)) {
	  if(engnum == 1){
	    XPLMCommandOnce(MagRight1);
	  }   
	  if(engnum == 2){
	    XPLMCommandOnce(MagRight1);
	    XPLMCommandOnce(MagRight2);
	  }
	  if(engnum == 3){
	    XPLMCommandOnce(MagRight1);
	    XPLMCommandOnce(MagRight2);
	    XPLMCommandOnce(MagRight3);
	  }
	  if(engnum == 4){
	    XPLMCommandOnce(MagRight1);
	    XPLMCommandOnce(MagRight2);
	    XPLMCommandOnce(MagRight3);
	    XPLMCommandOnce(MagRight4);
	  }
 	}

/***************** Engines Left Mag *******************/

	if(testbit(switchbuf,MAG_LEFT)) {
	  if(engnum == 1){
	    XPLMCommandOnce(MagLeft1);
	  }
	  if(engnum == 2){
	    XPLMCommandOnce(MagLeft1);
	    XPLMCommandOnce(MagLeft2);
	  }
	  if(engnum == 3){
	    XPLMCommandOnce(MagLeft1);
	    XPLMCommandOnce(MagLeft2);
	    XPLMCommandOnce(MagLeft3);
	  }
	  if(engnum == 4){
	    XPLMCommandOnce(MagLeft1);
	    XPLMCommandOnce(MagLeft2);
	    XPLMCommandOnce(MagLeft3);
	    XPLMCommandOnce(MagLeft4);
	  }
 	}

/***************** Engines Both Mags *******************/

	if(testbit(switchbuf,MAG_BOTH)) {
	  if(engnum == 1){
	    XPLMCommandOnce(MagBoth1);
	  }
	  if(engnum == 2){
	    XPLMCommandOnce(MagBoth1);
	    XPLMCommandOnce(MagBoth2);
	  } 
	  if(engnum == 3){
	    XPLMCommandOnce(MagBoth1);
	    XPLMCommandOnce(MagBoth2);
	    XPLMCommandOnce(MagBoth3);
	  } 
	  if(engnum == 4){
	    XPLMCommandOnce(MagBoth1);
	    XPLMCommandOnce(MagBoth2);
	    XPLMCommandOnce(MagBoth3);
	    XPLMCommandOnce(MagBoth4);
	  }
	}

/***************** Engines Starting *******************/

	if(testbit(switchbuf,ENG_START)) {
	  if(engnum == 1){
	    XPLMCommandOnce(EngStart1);
	  }
	  if(engnum == 2){
	    XPLMCommandOnce(EngStart1);
	    XPLMCommandOnce(EngStart2);
	  }
	  if(engnum == 3){
	    XPLMCommandOnce(EngStart1);
	    XPLMCommandOnce(EngStart2);
	    XPLMCommandOnce(EngStart3);
	  }
	  if(engnum == 4){
	    XPLMCommandOnce(EngStart1);
	    XPLMCommandOnce(EngStart2);
	    XPLMCommandOnce(EngStart3);
	    XPLMCommandOnce(EngStart4);
	  }
	}

/***************** Master Battery *******************/

	if(testbit(switchbuf,MASTER_BATTERY)) {
          if(batnum == 1){
            BatArrayOn[0] = 1;
          }
          if(batnum == 2){
            BatArrayOn[0] = 1;
            BatArrayOn[1] = 1;
          }
          if(batnum == 3){
            BatArrayOn[0] = 1;
            BatArrayOn[1] = 1;
            BatArrayOn[2] = 1;
          }
          if(batnum == 4){
            BatArrayOn[0] = 1;
            BatArrayOn[1] = 1;
            BatArrayOn[2] = 1;
            BatArrayOn[3] = 1;
          }
          if(batnum == 5){
            BatArrayOn[0] = 1;
            BatArrayOn[1] = 1;
            BatArrayOn[2] = 1;
            BatArrayOn[3] = 1;
            BatArrayOn[4] = 1;
          }
          if(batnum == 6){
            BatArrayOn[0] = 1;
            BatArrayOn[1] = 1;
            BatArrayOn[2] = 1;
            BatArrayOn[3] = 1;
            BatArrayOn[4] = 1;
            BatArrayOn[5] = 1;
          }
          if(batnum == 7){
            BatArrayOn[0] = 1;
            BatArrayOn[1] = 1;
            BatArrayOn[2] = 1;
            BatArrayOn[3] = 1;
            BatArrayOn[4] = 1;
            BatArrayOn[5] = 1;
            BatArrayOn[6] = 1;
          }
          if(batnum == 8){
            BatArrayOn[0] = 1;
            BatArrayOn[1] = 1;
            BatArrayOn[2] = 1;
            BatArrayOn[3] = 1;
            BatArrayOn[4] = 1;
            BatArrayOn[5] = 1;
            BatArrayOn[6] = 1;
            BatArrayOn[7] = 1;
          }
        }
	if(!testbit(switchbuf,MASTER_BATTERY)) {
          if(batnum == 1){
            BatArrayOn[0] = 0;
          }
          if(batnum == 2){
            BatArrayOn[0] = 0;
            BatArrayOn[1] = 0;
          }
          if(batnum == 3){
            BatArrayOn[0] = 0;
            BatArrayOn[1] = 0;
            BatArrayOn[2] = 0;
          }
          if(batnum == 4){
            BatArrayOn[0] = 0;
            BatArrayOn[1] = 0;
            BatArrayOn[2] = 0;
            BatArrayOn[3] = 0;
          }
          if(batnum == 5){
            BatArrayOn[0] = 0;
            BatArrayOn[1] = 0;
            BatArrayOn[2] = 0;
            BatArrayOn[3] = 0;
            BatArrayOn[4] = 0;
          }
          if(batnum == 6){
            BatArrayOn[0] = 0;
            BatArrayOn[1] = 0;
            BatArrayOn[2] = 0;
            BatArrayOn[3] = 0;
            BatArrayOn[4] = 0;
            BatArrayOn[5] = 0;
          }
          if(batnum == 7){
            BatArrayOn[0] = 0;
            BatArrayOn[1] = 0;
            BatArrayOn[2] = 0;
            BatArrayOn[3] = 0;
            BatArrayOn[4] = 0;
            BatArrayOn[5] = 0;
            BatArrayOn[6] = 0;
          }
          if(batnum == 8){
            BatArrayOn[0] = 0;
            BatArrayOn[1] = 0;
            BatArrayOn[2] = 0;
            BatArrayOn[3] = 0;
            BatArrayOn[4] = 0;
            BatArrayOn[5] = 0;
            BatArrayOn[6] = 0;
            BatArrayOn[7] = 0;
          }
        }
        XPLMSetDatavi(BatArrayOnDR, BatArrayOn, 0, 8);

/***************** Master Altenator *******************/

	if(testbit(switchbuf,MASTER_ALTENATOR)) {
	  if(gennum == 1){
	    XPLMCommandOnce(GenOn1);
	  }
	  if(gennum == 2){
	    XPLMCommandOnce(GenOn1);
	    XPLMCommandOnce(GenOn2);
	  }
	  if(gennum == 3){
	    XPLMCommandOnce(GenOn1);
	    XPLMCommandOnce(GenOn2);
	    XPLMCommandOnce(GenOn3);
	  }
	  if(gennum == 4){
	    XPLMCommandOnce(GenOn1);
	    XPLMCommandOnce(GenOn2);
	    XPLMCommandOnce(GenOn3);
	    XPLMCommandOnce(GenOn4);
	  }
 	}

	if(!testbit(switchbuf,MASTER_ALTENATOR)) {
	  if(gennum == 1){
	    XPLMCommandOnce(GenOff1);
	  }
	  if(gennum == 2){
	    XPLMCommandOnce(GenOff1);
	    XPLMCommandOnce(GenOff2);
	  }
	  if(gennum == 3){
	    XPLMCommandOnce(GenOff1);
	    XPLMCommandOnce(GenOff2);
	    XPLMCommandOnce(GenOff3);
	  }
	  if(gennum == 4){
	    XPLMCommandOnce(GenOff1);
	    XPLMCommandOnce(GenOff2);
	    XPLMCommandOnce(GenOff3);
	    XPLMCommandOnce(GenOff4);
	  }
 	}

/***************** Avionics Power *******************/

	if(testbit(switchbuf,AVIONICS_POWER)) {
          XPLMCommandOnce(AvLtOn);
 	}
	if(!testbit(switchbuf,AVIONICS_POWER)) {
          XPLMCommandOnce(AvLtOff);
 	}

/***************** Fuel Pump *******************/

	if(testbit(switchbuf,FUEL_PUMP)) {
	  if(engnum == 1){
	    XPLMCommandOnce(FuelPumpOn1);
	  }
	  if(engnum == 2){
	    XPLMCommandOnce(FuelPumpOn1);
	    XPLMCommandOnce(FuelPumpOn2);
	  }
	  if(engnum == 3){
	    XPLMCommandOnce(FuelPumpOn1);
	    XPLMCommandOnce(FuelPumpOn2);
	    XPLMCommandOnce(FuelPumpOn3);
	  }
	  if(engnum == 4){
	    XPLMCommandOnce(FuelPumpOn1);
	    XPLMCommandOnce(FuelPumpOn2);
	    XPLMCommandOnce(FuelPumpOn3);
	    XPLMCommandOnce(FuelPumpOn4);
	  }
 	}
	if(!testbit(switchbuf,FUEL_PUMP)) {
	  if(engnum == 1){
	    XPLMCommandOnce(FuelPumpOff1);
	  }
	  if(engnum == 2){
	    XPLMCommandOnce(FuelPumpOff1);
	    XPLMCommandOnce(FuelPumpOff2);
	  }
	  if(engnum == 3){
	    XPLMCommandOnce(FuelPumpOff1);
	    XPLMCommandOnce(FuelPumpOff2);
	    XPLMCommandOnce(FuelPumpOff3);
	  }
	  if(engnum == 4){
	    XPLMCommandOnce(FuelPumpOff1);
	    XPLMCommandOnce(FuelPumpOff2);
	    XPLMCommandOnce(FuelPumpOff3);
	    XPLMCommandOnce(FuelPumpOff4);
	  }
 	}

/***************** De Ice *******************/

	if(testbit(switchbuf,DE_ICE)) {
          XPLMSetDatai(AntiIce, 1);
 	}
	if(!testbit(switchbuf,DE_ICE)) {
          XPLMSetDatai(AntiIce, 0);
 	}

/***************** Pitot Heat *******************/

	if(testbit(switchbuf,PITOT_HEAT)) {
          XPLMCommandOnce(PtHtOn);
 	}
	if(!testbit(switchbuf,PITOT_HEAT)) {
          XPLMCommandOnce(PtHtOff);
 	}

/***************** Cowl Flaps *******************/

	if(testbit(switchbuf,COWL_FLAPS)) {
	  if(engnum == 1){
            XPLMSetDatavf(CowlFlaps, &opencowl, 0, 1);
	  }

	  if(engnum == 2){
            XPLMSetDatavf(CowlFlaps, &opencowl, 0, 1);
            XPLMSetDatavf(CowlFlaps, &opencowl, 1, 1);
	  }
	  if(engnum == 3){
            XPLMSetDatavf(CowlFlaps, &opencowl, 0, 1);
            XPLMSetDatavf(CowlFlaps, &opencowl, 1, 1);
            XPLMSetDatavf(CowlFlaps, &opencowl, 2, 1);
	  }
	  if(engnum == 4){
            XPLMSetDatavf(CowlFlaps, &opencowl, 0, 1);
            XPLMSetDatavf(CowlFlaps, &opencowl, 1, 1);
            XPLMSetDatavf(CowlFlaps, &opencowl, 2, 1);
            XPLMSetDatavf(CowlFlaps, &opencowl, 3, 1);
	  }
 	}

	if(!testbit(switchbuf,COWL_FLAPS)) {
	  if(engnum == 1){
            XPLMSetDatavf(CowlFlaps, &closecowl, 0, 1);
	  }
	  if(engnum == 2){
            XPLMSetDatavf(CowlFlaps, &closecowl, 0, 1);
            XPLMSetDatavf(CowlFlaps, &opencowl, 1, 1);
	  }
	  if(engnum == 3){
            XPLMSetDatavf(CowlFlaps, &closecowl, 0, 1);
            XPLMSetDatavf(CowlFlaps, &closecowl, 1, 1);
            XPLMSetDatavf(CowlFlaps, &closecowl, 2, 1);
	  }
	  if(engnum == 4){
            XPLMSetDatavf(CowlFlaps, &closecowl, 0, 1);
            XPLMSetDatavf(CowlFlaps, &closecowl, 1, 1);
            XPLMSetDatavf(CowlFlaps, &closecowl, 2, 1);
            XPLMSetDatavf(CowlFlaps, &closecowl, 3, 1);
	  }
 	}

/***************** Panel Lights *******************/

	if(testbit(switchbuf,PANEL_LIGHTS)) {
	  XPLMSetDataf(CockpitLights, 1);
	}
	if(!testbit(switchbuf,PANEL_LIGHTS)) {
	  XPLMSetDataf(CockpitLights, 0);
	}

/***************** Beacon Lights *******************/

	if(testbit(switchbuf,BEACON_LIGHTS)) {
          XPLMCommandOnce(BcLtOn);
 	}
	if(!testbit(switchbuf,BEACON_LIGHTS)) {
          XPLMCommandOnce(BcLtOff);
 	}

/***************** Nav Lights *******************/

	if(testbit(switchbuf,NAV_LIGHTS)) {
          XPLMCommandOnce(NvLtOn);
 	}
	if(!testbit(switchbuf,NAV_LIGHTS)) {
          XPLMCommandOnce(NvLtOff);
 	}

/***************** Strobe Lights *******************/

	if(testbit(switchbuf,STROBE_LIGHTS)) {
          XPLMCommandOnce(StLtOn);
 	}
	if(!testbit(switchbuf,STROBE_LIGHTS)) {
          XPLMCommandOnce(StLtOff);
 	}

/***************** Taxi Lights *******************/

	if(testbit(switchbuf,TAXI_LIGHTS)) {
          XPLMCommandOnce(TxLtOn);
 	}
	if(!testbit(switchbuf,TAXI_LIGHTS)) {
          XPLMCommandOnce(TxLtOff);
 	}

/***************** Landing Lights *******************/

	if(testbit(switchbuf,LANDING_LIGHTS)) {
          XPLMCommandOnce(LnLtOn);
 	}
	if(!testbit(switchbuf,LANDING_LIGHTS)) {
          XPLMCommandOnce(LnLtOff);
 	}

/***************** Gear Switch *******************/
	
	if(XPLMGetDatai(GearRetract) > 0){
	  if(testbit(switchbuf,GEAR_SWITCH_UP)) {
            XPLMCommandOnce(GearUp);
            if(XPLMGetDatai(OnGround) > 0){
	      gearled = 0x38;
            }
            if(XPLMGetDatai(OnGround) == 0){
	      gearled = 0x00;
            }
 	  }
	  if(testbit(switchbuf,GEAR_SWITCH_DN)) {
            XPLMCommandOnce(GearDn);
	    gearled = 0x07;	  
 	  }
	}
	if(XPLMGetDatai(GearRetract) == 0){
	  gearled = 0x00;
	}

  return;
}
