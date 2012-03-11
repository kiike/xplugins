// ****** switchpanels.cpp **********
// ****  William R. Good  ********


#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"

#include "saitekpanels.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define testbit(x, y)  ( ( ((const char*)&(x))[(y)>>3] & 0x80 >> ((y)&0x07)) >> (7-((y)&0x07) ) )

// ****************** Switch Panel variables *******************************
static int switchnowrite = 0;
static int switchres, switchwres;

static int batnum = 0, gennum = 0, engnum = 0;

static float opencowl[8], closecowl[8];

static int MAG_OFF = 10, MAG_RIGHT = 9, MAG_LEFT = 8, MAG_BOTH = 23;
static int ENG_START = 22;
static int MASTER_BATTERY = 7, MASTER_ALTENATOR = 6;
static int AVIONICS_POWER = 5, FUEL_PUMP = 4;
static int DE_ICE = 3, PITOT_HEAT = 2;
static int COWL_FLAPS = 1, PANEL_LIGHTS = 0;
static int BEACON_LIGHTS = 15, NAV_LIGHTS = 14;
static int STROBE_LIGHTS = 13, TAXI_LIGHTS = 12;
static int LANDING_LIGHTS = 11;
static int GEAR_SWITCH_UP = 21, GEAR_SWITCH_DN = 20; 
static int BatArrayOn[8];
static int failed1up = 0, failed2up = 0, failed3up = 0;
static int failed1dn = 0, failed2dn = 0, failed3dn = 0;

static unsigned char switchbuf[4];
static unsigned char switchwbuf[2], gearled;


void process_switch_menu()
{

    XPLMClearAllMenuItems(SwitchMenuId);
    XPLMAppendMenuItem(SwitchMenuId, "Bat Alt Normal or Alt Bat Cessna", (void *) "VOID", 4);
    XPLMAppendMenuSeparator(SwitchMenuId);
    XPLMAppendMenuItem(SwitchMenuId, "NORMAL", (void *) "NORMAL", 4);
    XPLMAppendMenuItem(SwitchMenuId, "CESSNA", (void *) "CESSNA", 4);

    if (bataltinverse == 0) {
       XPLMCheckMenuItem(SwitchMenuId, 2, xplm_Menu_Checked);
    }
    if (bataltinverse != 0) {
       XPLMCheckMenuItem(SwitchMenuId, 2, xplm_Menu_Unchecked);
    }
    if (bataltinverse == 1) {
       XPLMCheckMenuItem(SwitchMenuId, 3, xplm_Menu_Checked);
    }
    if (bataltinverse != 1) {
       XPLMCheckMenuItem(SwitchMenuId, 3, xplm_Menu_Unchecked);
    }

  switchwbuf[0] = 0;
  switchwbuf[1] = gearled;
  if (bataltinverse == 0) {
    MASTER_BATTERY = 7, MASTER_ALTENATOR = 6;
  }
  if (bataltinverse == 1) {
    MASTER_BATTERY = 6, MASTER_ALTENATOR = 7;
  }

}

void process_engines_mag_off_switch()
{

// ***************** Engines Mag Off ********************

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

}

void process_engines_right_mag_switch()
{

// ***************** Engines Right Mag *******************

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
}

void process_engines_left_mag_switch()
{

// ***************** Engines Left Mag *******************

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
}

void process_engines_both_mag_switch()
{

// ***************** Engines Both Mags *******************

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
}

void process_engines_start_switch()
{

// ***************** Engines Starting *******************

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
}

void process_master_battery_switch()
{

// ***************** Master Battery *******************

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
}

void process_master_altenator_switch()
{

// ***************** Master Altenator *******************

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
}

void process_avionics_power_switch()
{

// ***************** Avionics Power *******************

	if(testbit(switchbuf,AVIONICS_POWER)) {
          XPLMCommandOnce(AvLtOn);
 	}
	if(!testbit(switchbuf,AVIONICS_POWER)) {
          XPLMCommandOnce(AvLtOff);
 	}
}

void process_fuel_pump_switch()
{

// ***************** Fuel Pump *******************

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
}

void process_de_ice_switch()
{

// ***************** De Ice *******************

	if(testbit(switchbuf,DE_ICE)) {

            if (loaded737 == 1){
                XPLMCommandOnce(x737ice_wing_on);
            } else {
                XPLMSetDatai(AntiIce, 1);
            }

 	}
	if(!testbit(switchbuf,DE_ICE)) {

            if (loaded737 == 1){
                XPLMCommandOnce(x737ice_wing_off);
            } else {
                XPLMSetDatai(AntiIce, 0);
            }
     }
}

void process_pitot_heat_switch()
{

// ***************** Pitot Heat *******************

	if(testbit(switchbuf,PITOT_HEAT)) {

            if (loaded737 == 1){
                XPLMCommandOnce(x737ice_pitot1_on);
                XPLMCommandOnce(x737ice_pitot2_on);
            } else {
                if (wrgXPlaneVersion > 9700){
                    XPLMCommandOnce(PtHt0On);
                    XPLMCommandOnce(PtHt1On);
                } else {
                    XPLMCommandOnce(PtHtOn);
                }
            }

 	}
	if(!testbit(switchbuf,PITOT_HEAT)) {

            if (loaded737 == 1){
                XPLMCommandOnce(x737ice_pitot1_off);
                XPLMCommandOnce(x737ice_pitot2_off);
            } else {
                if (wrgXPlaneVersion > 9700){
                    XPLMCommandOnce(PtHt0Off);
                    XPLMCommandOnce(PtHt1Off);
                } else {
                    XPLMCommandOnce(PtHtOff);
                }
            }

 	}
}

void process_cowl_flaps_switch()
{

// ***************** Cowl Flaps *******************

        if(!testbit(switchbuf,COWL_FLAPS)) {
          if (loaded737 == 1){
                XPLMCommandOnce(x737ice_engine1_on);
                XPLMCommandOnce(x737ice_engine2_on);
          } else {
          if(engnum == 1){
            opencowl[0] = 1;
          }

	  if(engnum == 2){
            opencowl[0] = 1;
            opencowl[1] = 1;

	  }
	  if(engnum == 3){
            opencowl[0] = 1;
            opencowl[1] = 1;
            opencowl[2] = 1;

	  }
	  if(engnum == 4){
            opencowl[0] = 1;
            opencowl[1] = 1;
            opencowl[2] = 1;
            opencowl[3] = 1;

	  }
          XPLMSetDatavf(CowlFlaps, opencowl, 0, 8);
 	}
    }

        if(testbit(switchbuf,COWL_FLAPS)) {
            if (loaded737 == 1){
                XPLMCommandOnce(x737ice_engine1_off);
                XPLMCommandOnce(x737ice_engine2_off);
            } else {
              if(engnum == 1){
                closecowl[0] = 0;

	  }
	  if(engnum == 2){
            closecowl[0] = 0;
            closecowl[1] = 0;

	  }
	  if(engnum == 3){
            closecowl[0] = 0;
            closecowl[1] = 0;
            closecowl[2] = 0;

	  }
	  if(engnum == 4){
            closecowl[0] = 0;
            closecowl[1] = 0;
            closecowl[2] = 0;
            closecowl[3] = 0;

	  }
          XPLMSetDatavf(CowlFlaps, closecowl, 0, 8);
 	}
    }
}

void process_panel_lights_switch()
{

// ***************** Panel Lights *******************

	if(testbit(switchbuf,PANEL_LIGHTS)) {
	  XPLMSetDataf(CockpitLights, 1);
	}
	if(!testbit(switchbuf,PANEL_LIGHTS)) {
	  XPLMSetDataf(CockpitLights, 0);
	}
}

void process_beacon_lights_switch()
{

// ***************** Beacon Lights *******************

	if(testbit(switchbuf,BEACON_LIGHTS)) {
          XPLMCommandOnce(BcLtOn);
 	}
	if(!testbit(switchbuf,BEACON_LIGHTS)) {
          XPLMCommandOnce(BcLtOff);
 	}
}

void process_nav_lights_switch()
{

// ***************** Nav Lights *******************

	if(testbit(switchbuf,NAV_LIGHTS)) {
          XPLMCommandOnce(NvLtOn);
 	}
	if(!testbit(switchbuf,NAV_LIGHTS)) {
          XPLMCommandOnce(NvLtOff);
 	}
}

void process_strobe_lights_switch()
{

// ***************** Strobe Lights *******************

	if(testbit(switchbuf,STROBE_LIGHTS)) {
          XPLMCommandOnce(StLtOn);
 	}
	if(!testbit(switchbuf,STROBE_LIGHTS)) {
          XPLMCommandOnce(StLtOff);
 	}
}

void process_taxi_lights_switch()
{

// ***************** Taxi Lights *******************

	if(testbit(switchbuf,TAXI_LIGHTS)) {
          XPLMCommandOnce(TxLtOn);
 	}
	if(!testbit(switchbuf,TAXI_LIGHTS)) {
          XPLMCommandOnce(TxLtOff);
 	}
}

void process_landing_lights_switch()
{

// ***************** Landing Lights *******************

	if(testbit(switchbuf,LANDING_LIGHTS)) {
          XPLMCommandOnce(LnLtOn);
 	}
	if(!testbit(switchbuf,LANDING_LIGHTS)) {
          XPLMCommandOnce(LnLtOff);
 	}
}

void process_gear_switch_switch()
{

// ***************** Gear Switch *******************
	
	if(XPLMGetDatai(GearRetract) > 0){
          XPLMGetDatavf(LandingGearStatus, LandingGearDeployRatio, 0, 10);
          // Gear is down and locked
          if (LandingGearDeployRatio[0] == 1) {
              gearled |= (1<<0);   // * set bit 0 in gearled to 1 *
              gearled &= ~(1<<3);   // * clear bit 3 in gearled to 0 *
          }
          if (LandingGearDeployRatio[1] == 1) {
              gearled |= (1<<1);   // * set bit 1 in gearled to 1 *
              gearled &= ~(1<<4);   // * clear bit 4 in gearled to 0 *
          }
          if (LandingGearDeployRatio[2] == 1) {
              gearled |= (1<<2);   // * set bit 2 in gearled to 1 *
              gearled &= ~(1<<5);   // * clear bit 5 in gearled to 0 *
          }
          // Gear is in motion
          if ((LandingGearDeployRatio[0] > 0) &&(LandingGearDeployRatio[0] < 1)) {
              gearled |= (1<<3);   // * set bit 3 in gearled to 1 *
              gearled &= ~(1<<0);   // * clear bit 0 in gearled to 0 *
          }
          if ((LandingGearDeployRatio[1] > 0) &&(LandingGearDeployRatio[1] < 1)) {
              gearled |= (1<<4);   // * set bit 4 in gearled to 1 *
              gearled &= ~(1<<1);   // * clear bit 1 in gearled to 0 *
          }
          if ((LandingGearDeployRatio[2] > 0) &&(LandingGearDeployRatio[2] < 1)) {
              gearled |= (1<<5);   // * set bit 5 in gearled to 1 *
              gearled &= ~(1<<2);   // * clear bit 2 in gearled to 0 *
          }
          // Gear is up
          if (LandingGearDeployRatio[0] == 0) {
              gearled &= ~(1<<0);   // * clear bit 0 in gearled to 0 *
              gearled &= ~(1<<3);   // * clear bit 3 in gearled to 0 *
          }
          if (LandingGearDeployRatio[1] == 0) {
              gearled &= ~(1<<1);   // * clear bit 1 in gearled to 0 *
              gearled &= ~(1<<4);   // * clear bit 4 in gearled to 0 *
          }
          if (LandingGearDeployRatio[2] == 0) {
              gearled &= ~(1<<2);   // * clear bit 2 in gearled to 0 *
              gearled &= ~(1<<5);   // * clear bit 5 in gearled to 0 *
          }
          // Gear has failed

          if(testbit(switchbuf,GEAR_SWITCH_UP)) {

             // ********************************************************************************************
             // Nose gear had failed

             if (failed1dn == 0) {
               if (XPLMGetDatai(Gear1Fail) == 6) {
                 failed1up = 1;
               }
               if (XPLMGetDatai(Gear1Fail) == 0) {
                 failed1up = 0;
               }
               if (failed1up == 1) {
                   gearled &= ~(1<<3);   // * set bit 3 in gearled to 1 *
                   gearled &= ~(1<<0);   // * clear bit 1 in gearled to 0 *
               }

             }
             if (failed1dn == 1) {
                 gearled |= (1<<3);   // * set bit 3 in gearled to 1 *
                 gearled &= ~(1<<0);   // * clear bit 0 in gearled to 0 *
             }

             // *********************************************************************************************
             // Left gear has failed

             if (failed2dn == 0) {
               if (XPLMGetDatai(Gear2Fail) == 6) {
                 failed2up = 1;
               }
               if (XPLMGetDatai(Gear2Fail) == 0) {
                 failed2up = 0;
               }
               if (failed2up == 1) {
                   gearled &= ~(1<<4);   // * set bit 3 in gearled to 1 *
                   gearled &= ~(1<<1);   // * clear bit 1 in gearled to 0 *
               }

             }
             if (failed2dn == 1) {
                 gearled |= (1<<4);   // * set bit 3 in gearled to 1 *
                 gearled &= ~(1<<1);   // * clear bit 0 in gearled to 0 *
             }

             // **************************************************************************************************
             // Right gear has failed

             if (failed3dn == 0) {
               if (XPLMGetDatai(Gear3Fail) == 6) {
                 failed3up = 1;
               }
               if (XPLMGetDatai(Gear3Fail) == 0) {
                 failed3up = 0;
               }
               if (failed3up == 1) {
                   gearled &= ~(1<<5);   // * set bit 3 in gearled to 1 *
                   gearled &= ~(1<<2);   // * clear bit 1 in gearled to 0 *
               }

             }
             if (failed3dn == 1) {
                 gearled |= (1<<5);   // * set bit 3 in gearled to 1 *
                 gearled &= ~(1<<2);   // * clear bit 0 in gearled to 0 *
             }



           }

          if(testbit(switchbuf,GEAR_SWITCH_DN)) {

              // ******************************************************************************************************
             // Nose gear has failed

             if (failed1up == 0) {
               if (XPLMGetDatai(Gear1Fail) == 6) {
                 failed1dn = 1;
               }
               if (XPLMGetDatai(Gear1Fail) == 0) {
                 failed1dn = 0;
               }
               if (failed1dn == 1) {
                   gearled |= (1<<0);   // * set bit 0 in gearled to 1 *
                   gearled &= ~(1<<3);   // * clear bit 3 in gearled to 0 *
               }

             }
             if (failed1up == 1) {
                 gearled |= (1<<3);   // * set bit 3 in gearled to 1 *
                 gearled &= ~(1<<0);   // * clear bit 0 in gearled to 0 *
             }

             // *******************************************************************************************************
             // Left gear has failed

             if (failed2up == 0) {
               if (XPLMGetDatai(Gear2Fail) == 6) {
                 failed2dn = 1;
               }
               if (XPLMGetDatai(Gear2Fail) == 0) {
                 failed2dn = 0;
               }
               if (failed2dn == 1) {
                   gearled |= (1<<1);   // * set bit 0 in gearled to 1 *
                   gearled &= ~(1<<4);   // * clear bit 3 in gearled to 0 *
               }

             }
             if (failed2up == 1) {
                 gearled |= (1<<4);   // * set bit 3 in gearled to 1 *
                 gearled &= ~(1<<1);   // * clear bit 0 in gearled to 0 *
             }

             // *******************************************************************************************************
             // Right gear has failed

             if (failed3up == 0) {
               if (XPLMGetDatai(Gear3Fail) == 6) {
                 failed3dn = 1;
               }
               if (XPLMGetDatai(Gear3Fail) == 0) {
                 failed3dn = 0;
               }
               if (failed3dn == 1) {
                   gearled |= (1<<2);   // * set bit 0 in gearled to 1 *
                   gearled &= ~(1<<5);   // * clear bit 3 in gearled to 0 *
               }

             }
             if (failed3up == 1) {
                 gearled |= (1<<5);   // * set bit 3 in gearled to 1 *
                 gearled &= ~(1<<2);   // * clear bit 0 in gearled to 0 *
             }

          }

	  if(testbit(switchbuf,GEAR_SWITCH_UP)) {
            XPLMCommandOnce(GearUp);
            if(XPLMGetDatai(OnGround) > 0){
              gearled = 0x38;
            }

          }

	  if(testbit(switchbuf,GEAR_SWITCH_DN)) {
            XPLMCommandOnce(GearDn);
 	  }

	}
	if(XPLMGetDatai(GearRetract) == 0){
	  gearled = 0x00;
	}
}


/** Switch Panel Process  **/
void process_switch_panel()

{
    process_switch_menu();

    // ******* Only do a read if something new to be read ********

      hid_set_nonblocking(switchhandle, 1);
      int switch_safety_cntr = 30;
      do{
        switchres = hid_read(switchhandle, switchbuf, sizeof(switchbuf));
        process_engines_mag_off_switch();
        process_engines_right_mag_switch();
        process_engines_left_mag_switch();
        process_engines_both_mag_switch();
        process_engines_start_switch();
        process_master_battery_switch();
        process_master_altenator_switch();
        process_avionics_power_switch();
        process_fuel_pump_switch();
        process_de_ice_switch();
        process_pitot_heat_switch();
        process_cowl_flaps_switch();
        process_panel_lights_switch();
        process_beacon_lights_switch();
        process_nav_lights_switch();
        process_strobe_lights_switch();
        process_taxi_lights_switch();
        process_landing_lights_switch();
        process_gear_switch_switch();

        --switch_safety_cntr;
      }while((switchres > 0) && (switch_safety_cntr > 0));

      switchnowrite = 1;

    // * if no gear do not write *

      if(XPLMGetDatai(GearRetract) > 0){
        if (XPLMGetDatai(BatPwrOn) == 0) {
              switchwbuf[0] = 0, switchwbuf[1] = 0;
              switchwres = hid_send_feature_report(switchhandle, switchwbuf, 2);
        }
        if (XPLMGetDatai(BatPwrOn) == 1) {
              switchwres = hid_send_feature_report(switchhandle, switchwbuf, 2);
        }
      }
      else {
      }

      batnum = XPLMGetDatai(BatNum), gennum = XPLMGetDatai(GenNum), engnum = XPLMGetDatai(EngNum);

  return;
}
