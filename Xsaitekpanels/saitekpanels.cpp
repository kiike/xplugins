/****** saitekpanels.cpp ***********/
/****  William R. Good   ***********/
/******** ver 1.19   ***************/
/****** Aug 06 2011   **************/

#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"

#include "hidapi.h"


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

/************* Radio Panel Command Ref ****************/
XPLMCommandRef Com1StbyFineDn = NULL, Com1StbyFineUp = NULL, Com1StbyCorseDn = NULL, Com1StbyCorseUp = NULL;
XPLMCommandRef Com2StbyFineDn = NULL, Com2StbyFineUp = NULL, Com2StbyCorseDn = NULL, Com2StbyCorseUp = NULL;
XPLMCommandRef	Nav1StbyFineDn = NULL, Nav1StbyFineUp = NULL, Nav1StbyCorseDn = NULL, Nav1StbyCorseUp = NULL;
XPLMCommandRef	Nav2StbyFineDn = NULL, Nav2StbyFineUp = NULL, Nav2StbyCorseDn = NULL, Nav2StbyCorseUp = NULL;

XPLMCommandRef	Afd1HunUp = NULL, Afd1HunDn = NULL, Afd1TensUp = NULL, Afd1TensDn = NULL;
XPLMCommandRef	Afd1OnesUp = NULL, Afd1OnesDn = NULL;

XPLMCommandRef	XpdrThUp = NULL, XpdrThDn = NULL, XpdrHunUp = NULL, XpdrHunDn = NULL;
XPLMCommandRef	XpdrTensUp = NULL, XpdrTensDn = NULL, XpdrOnesUp = NULL, XpdrOnesDn = NULL;

XPLMCommandRef Com1ActStby = NULL, Com2ActStby = NULL, Nav1ActStby = NULL, Nav2ActStby = NULL;


/************* Radio Panel Data Ref ****************/
XPLMDataRef Com1ActFreq = NULL, Com2ActFreq = NULL, Nav1ActFreq = NULL, Nav2ActFreq = NULL;
XPLMDataRef Com1StbyFreq = NULL, Com2StbyFreq = NULL, Nav1StbyFreq = NULL, Nav2StbyFreq = NULL;

XPLMDataRef XpdrCode = NULL, AdfFreq = NULL, Nav1DmeNmDist = NULL, Nav1DmeSpeed = NULL;
XPLMDataRef Nav2DmeNmDist = NULL, Nav2DmeSpeed = NULL, DmeSlvSource;

XPLMDataRef AvPwrOn = NULL, BatPwrOn = NULL;

XPLMDataRef Nav1PwrOn = NULL, Nav2PwrOn = NULL, Com1PwrOn = NULL, Com2PwrOn = NULL;
XPLMDataRef Afd1PwrOn = NULL, DmePwrOn = NULL;


/****************** Multi Panel Command Ref **********************/
XPLMCommandRef ApAltDn = NULL, ApAltUp = NULL, ApVsDn = NULL, ApVsUp = NULL;
XPLMCommandRef ApAsDn = NULL, ApAsUp = NULL, ApHdgDn = NULL, ApHdgUp = NULL;
XPLMCommandRef ApCrsDn = NULL, ApCrsUp = NULL;

XPLMCommandRef ApMstrBtnUp = NULL, ApMstrBtnDn = NULL, ApMstrBtnOff = NULL;
XPLMCommandRef ApHdgBtn = NULL, ApNavBtn = NULL, ApAltBtn = NULL;
XPLMCommandRef ApIasBtn = NULL;

XPLMCommandRef ApVsBtn = NULL, ApAprBtn = NULL, ApRevBtn = NULL;

XPLMCommandRef ApAutThrToggle = NULL, FlapsDn = NULL, FlapsUp = NULL;
XPLMCommandRef PitchTrimDn = NULL, PitchTrimUp = NULL, PitchTrimTkOff = NULL;

/***************** Multi Panel Data Ref *********************/
XPLMDataRef ApAlt = NULL, ApVs = NULL, ApAs = NULL, ApHdg = NULL, ApCrs = NULL;

XPLMDataRef ApMstrStat = NULL, ApHdgStat = NULL, ApNavStat = NULL, ApIasStat = NULL;
XPLMDataRef ApAltStat = NULL, ApVsStat = NULL, ApAprStat = NULL, ApRevStat = NULL;
XPLMDataRef x737athr_armed = NULL, x737swBatBus = NULL, x737stbyPwr = NULL;
XPLMDataRef ApState = NULL;


/*************** Switch Panel Command Ref *******************/
XPLMCommandRef ClFlOpn = NULL, ClFlCls = NULL, PtHtOn = NULL, PtHtOff = NULL;
XPLMCommandRef AvLtOn = NULL, AvLtOff = NULL, BatOn = NULL, BatOff = NULL;
XPLMCommandRef LnLtOn = NULL, LnLtOff = NULL, TxLtOn = NULL, TxLtOff = NULL;
XPLMCommandRef StLtOn = NULL, StLtOff = NULL, NvLtOn = NULL, NvLtOff = NULL;
XPLMCommandRef BcLtOn = NULL, BcLtOff = NULL, GearUp = NULL, GearDn = NULL;

XPLMCommandRef MagOff1 = NULL, MagOff2 = NULL, MagOff3 = NULL, MagOff4 = NULL;
XPLMCommandRef MagOff5 = NULL, MagOff6 = NULL, MagOff7 = NULL, MagOff8 = NULL;

XPLMCommandRef MagLeft1 = NULL, MagLeft2 = NULL, MagLeft3 = NULL, MagLeft4 = NULL;
XPLMCommandRef MagLeft5 = NULL, MagLeft6 = NULL, MagLeft7 = NULL, MagLeft8 = NULL;

XPLMCommandRef MagRight1 = NULL, MagRight2 = NULL, MagRight3 = NULL, MagRight4 = NULL;
XPLMCommandRef MagRight5 = NULL, MagRight6 = NULL, MagRight7 = NULL, MagRight8 = NULL;

XPLMCommandRef MagBoth1 = NULL, MagBoth2 = NULL, MagBoth3 = NULL, MagBoth4 = NULL;
XPLMCommandRef MagBoth5 = NULL, MagBoth6 = NULL, MagBoth7 = NULL, MagBoth8 = NULL;

XPLMCommandRef EngStart1 = NULL, EngStart2 = NULL, EngStart3 = NULL, EngStart4 = NULL;
XPLMCommandRef EngStart5 = NULL, EngStart6 = NULL, EngStart7 = NULL, EngStart8 = NULL;

XPLMCommandRef BatOn1 = NULL, BatOn2 = NULL, BatOff1 = NULL, BatOff2 = NULL;

XPLMCommandRef GenOn1 = NULL, GenOn2 = NULL, GenOn3 = NULL, GenOn4 = NULL;
XPLMCommandRef GenOn5 = NULL, GenOn6 = NULL, GenOn7 = NULL, GenOn8 = NULL;

XPLMCommandRef GenOff1 = NULL, GenOff2 = NULL, GenOff3 = NULL, GenOff4 = NULL;
XPLMCommandRef GenOff5 = NULL, GenOff6 = NULL, GenOff7 = NULL, GenOff8 = NULL;

XPLMCommandRef FuelPumpOn1 = NULL, FuelPumpOn2 = NULL, FuelPumpOn3 = NULL, FuelPumpOn4 = NULL;
XPLMCommandRef FuelPumpOn5 = NULL, FuelPumpOn6 = NULL, FuelPumpOn7 = NULL, FuelPumpOn8 = NULL;

XPLMCommandRef FuelPumpOff1 = NULL, FuelPumpOff2 = NULL, FuelPumpOff3 = NULL, FuelPumpOff4 = NULL;
XPLMCommandRef FuelPumpOff5 = NULL, FuelPumpOff6 = NULL, FuelPumpOff7 = NULL, FuelPumpOff8 = NULL;

/******************* Switch Panel Data Ref ********************/
XPLMDataRef BatNum = NULL, GenNum = NULL, EngNum = NULL;
XPLMDataRef BatArrayOnDR = NULL;


XPLMDataRef CowlFlaps = NULL, CockpitLights = NULL, AntiIce = NULL;
XPLMDataRef GearRetract = NULL, OnGround = NULL;

/********************** Radio Panel variables ************************/
int radcnt = 0, radiores, stopradcnt;
int radres, radnum = 0;
float interval = -1;
static unsigned char blankradiowbuf[4][23];
static unsigned char radiobuf[4][4], radiowbuf[4][23];

unsigned char radbuf[4], radwbuf[21];

hid_device *radhandle[4];

/********************** Multi Panel variables ***********************/
int multicnt = 0, multires, stopmulticnt;
static unsigned char blankmultiwbuf[13];
unsigned char multibuf[4], multiwbuf[13];

hid_device *multihandle;

/****************** Switch Panel variables *******************************/
int switchcnt = 0, switchres, stopswitchcnt;
static unsigned char blankswitchwbuf[2];
unsigned char switchbuf[4], switchwbuf[2];

hid_device *switchhandle;

/********************* MyPanelsFlightLoopCallback **************************/
float	MyPanelsFlightLoopCallback(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon);

void process_radio_panel();
void process_multi_panel();
void process_switch_panel();

int loaded737 = 0;

/******************Plugin Calls ******************/
PLUGIN_API int XPluginStart(char *		outName,
			    char *		outSig,
			    char *		outDesc)
{

	/* First set up our plugin info. */
  strcpy(outName, "Xsaitekpanels v1.19");
  strcpy(outSig, "saitekpanels.hardware uses hidapi interface");
  strcpy(outDesc, "A plugin allows use of Saitek Pro Flight Panels on all platforms");

/************ Find Radio Panel Commands Ref ******************/
  Com1StbyFineDn = XPLMFindCommand("sim/radios/stby_com1_fine_down");
  Com1StbyFineUp = XPLMFindCommand("sim/radios/stby_com1_fine_up");
  Com1StbyCorseDn = XPLMFindCommand("sim/radios/stby_com1_coarse_down");
  Com1StbyCorseUp = XPLMFindCommand("sim/radios/stby_com1_coarse_up");

  Com2StbyFineDn = XPLMFindCommand("sim/radios/stby_com2_fine_down");
  Com2StbyFineUp = XPLMFindCommand("sim/radios/stby_com2_fine_up");
  Com2StbyCorseDn = XPLMFindCommand("sim/radios/stby_com2_coarse_down");
  Com2StbyCorseUp = XPLMFindCommand("sim/radios/stby_com2_coarse_up");

  Nav1StbyFineDn = XPLMFindCommand("sim/radios/stby_nav1_fine_down");
  Nav1StbyFineUp = XPLMFindCommand("sim/radios/stby_nav1_fine_up");
  Nav1StbyCorseDn = XPLMFindCommand("sim/radios/stby_nav1_coarse_down");
  Nav1StbyCorseUp = XPLMFindCommand("sim/radios/stby_nav1_coarse_up");

  Nav2StbyFineDn = XPLMFindCommand("sim/radios/stby_nav2_fine_down");
  Nav2StbyFineUp = XPLMFindCommand("sim/radios/stby_nav2_fine_up");
  Nav2StbyCorseDn = XPLMFindCommand("sim/radios/stby_nav2_coarse_down");
  Nav2StbyCorseUp = XPLMFindCommand("sim/radios/stby_nav2_coarse_up");

  Afd1HunUp	= XPLMFindCommand("sim/radios/actv_adf1_hundreds_up");
  Afd1HunDn	= XPLMFindCommand("sim/radios/actv_adf1_hundreds_down");
  Afd1TensUp	= XPLMFindCommand("sim/radios/actv_adf1_tens_up");
  Afd1TensDn	= XPLMFindCommand("sim/radios/actv_adf1_tens_down");
  Afd1OnesUp	= XPLMFindCommand("sim/radios/actv_adf1_ones_up");
  Afd1OnesDn	= XPLMFindCommand("sim/radios/actv_adf1_ones_down");

  XpdrThUp	= XPLMFindCommand("sim/transponder/transponder_thousands_up");
  XpdrThDn	= XPLMFindCommand("sim/transponder/transponder_thousands_down");
  XpdrHunUp	= XPLMFindCommand("sim/transponder/transponder_hundreds_up");
  XpdrHunDn	= XPLMFindCommand("sim/transponder/transponder_hundreds_down");
  XpdrTensUp	= XPLMFindCommand("sim/transponder/transponder_tens_up");
  XpdrTensDn	= XPLMFindCommand("sim/transponder/transponder_tens_down");
  XpdrOnesUp	= XPLMFindCommand("sim/transponder/transponder_ones_up");
  XpdrOnesDn	= XPLMFindCommand("sim/transponder/transponder_ones_down");  	

  Com1ActStby = XPLMFindCommand("sim/radios/com1_standy_flip");
  Com2ActStby = XPLMFindCommand("sim/radios/com2_standy_flip");
  Nav1ActStby = XPLMFindCommand("sim/radios/nav1_standy_flip");
  Nav2ActStby = XPLMFindCommand("sim/radios/nav2_standy_flip");

/************ Find Radio Panel Data Ref ******************/
  Com1ActFreq = XPLMFindDataRef("sim/cockpit/radios/com1_freq_hz");
  Com2ActFreq = XPLMFindDataRef("sim/cockpit/radios/com2_freq_hz");
  Nav1ActFreq = XPLMFindDataRef("sim/cockpit/radios/nav1_freq_hz");
  Nav2ActFreq = XPLMFindDataRef("sim/cockpit/radios/nav2_freq_hz");

  Com1StbyFreq = XPLMFindDataRef("sim/cockpit/radios/com1_stdby_freq_hz");
  Com2StbyFreq = XPLMFindDataRef("sim/cockpit/radios/com2_stdby_freq_hz");
  Nav1StbyFreq = XPLMFindDataRef("sim/cockpit/radios/nav1_stdby_freq_hz");
  Nav2StbyFreq = XPLMFindDataRef("sim/cockpit/radios/nav2_stdby_freq_hz");

  XpdrCode	= XPLMFindDataRef("sim/cockpit/radios/transponder_code");
  AdfFreq	= XPLMFindDataRef("sim/cockpit/radios/adf1_freq_hz");
  Nav1DmeNmDist	= XPLMFindDataRef("sim/cockpit2/radios/indicators/nav1_dme_distance_nm");
  Nav1DmeSpeed	= XPLMFindDataRef("sim/cockpit2/radios/indicators/nav1_dme_speed_kts");
  Nav2DmeNmDist	= XPLMFindDataRef("sim/cockpit2/radios/indicators/nav2_dme_distance_nm");
  Nav2DmeSpeed	= XPLMFindDataRef("sim/cockpit2/radios/indicators/nav2_dme_speed_kts");
  DmeSlvSource  = XPLMFindDataRef("sim/cockpit2/radios/actuators/DME_slave_source");

  AvPwrOn = XPLMFindDataRef("sim/cockpit/electrical/avionics_on");
  BatPwrOn = XPLMFindDataRef("sim/cockpit/electrical/battery_on");

  Nav1PwrOn = XPLMFindDataRef("sim/cockpit2/radios/actuators/nav1_power");
  Nav2PwrOn = XPLMFindDataRef("sim/cockpit2/radios/actuators/nav2_power");
  Com1PwrOn = XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_power");
  Com2PwrOn = XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_power");
  Afd1PwrOn = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf1_power");
  DmePwrOn = XPLMFindDataRef("sim/cockpit2/radios/actuators/dme_power");


/************ Find Multi Panel Commands Ref ***************/
  ApAltDn = XPLMFindCommand("sim/autopilot/altitude_down");
  ApAltUp = XPLMFindCommand("sim/autopilot/altitude_up");
  ApVsDn = XPLMFindCommand("sim/autopilot/vertical_speed_down");
  ApVsUp = XPLMFindCommand("sim/autopilot/vertical_speed_up");
  ApAsDn = XPLMFindCommand("sim/autopilot/airspeed_down");
  ApAsUp = XPLMFindCommand("sim/autopilot/airspeed_up");
  ApHdgDn = XPLMFindCommand("sim/autopilot/heading_down");
  ApHdgUp = XPLMFindCommand("sim/autopilot/heading_up");
  ApCrsDn = XPLMFindCommand("sim/radios/obs_HSI_down");
  ApCrsUp = XPLMFindCommand("sim/radios/obs_HSI_up");

  ApMstrBtnUp = XPLMFindCommand("sim/autopilot/fdir_servos_up_one");
  ApMstrBtnDn = XPLMFindCommand("sim/autopilot/fdir_servos_down_one");
  ApMstrBtnOff = XPLMFindCommand("sim/autopilot/servos_and_flight_dir_off");
  ApHdgBtn = XPLMFindCommand("sim/autopilot/heading");
  ApNavBtn = XPLMFindCommand("sim/autopilot/NAV");
   ApIasBtn = XPLMFindCommand("sim/autopilot/level_change");
  ApAltBtn = XPLMFindCommand("sim/autopilot/altitude_hold");
  ApVsBtn = XPLMFindCommand("sim/autopilot/vertical_speed");
  ApAprBtn = XPLMFindCommand("sim/autopilot/approach");
  ApRevBtn = XPLMFindCommand("sim/autopilot/back_course");

  ApAutThrToggle = XPLMFindCommand("sim/autopilot/autothrottle_toggle");

  PitchTrimDn = XPLMFindCommand("sim/flight_controls/pitch_trim_down");
  PitchTrimUp = XPLMFindCommand("sim/flight_controls/pitch_trim_up");
  PitchTrimTkOff = XPLMFindCommand("sim/flight_controls/pitch_trim_takeoff");
  FlapsDn = XPLMFindCommand("sim/flight_controls/flaps_down");
  FlapsUp = XPLMFindCommand("sim/flight_controls/flaps_up");

  ApAlt = XPLMFindDataRef("sim/cockpit/autopilot/altitude");
  ApVs = XPLMFindDataRef("sim/cockpit/autopilot/vertical_velocity");
  ApAs = XPLMFindDataRef("sim/cockpit/autopilot/airspeed");
  ApHdg = XPLMFindDataRef("sim/cockpit/autopilot/heading_mag");
  ApCrs = XPLMFindDataRef("sim/cockpit2/radios/actuators/hsi_obs_deg_mag_pilot");

/**************** Find Multi Panel Data Ref ********************/
  ApMstrStat = XPLMFindDataRef("sim/cockpit2/autopilot/flight_director_mode");
  ApState = XPLMFindDataRef("sim/cockpit/autopilot/autopilot_state");
  ApHdgStat = XPLMFindDataRef("sim/cockpit2/autopilot/heading_status");
  ApNavStat = XPLMFindDataRef("sim/cockpit2/autopilot/nav_status");
  ApIasStat = XPLMFindDataRef("sim/cockpit2/autopilot/speed_status");
  ApAltStat = XPLMFindDataRef("sim/cockpit2/autopilot/altitude_hold_status");
  ApVsStat = XPLMFindDataRef("sim/cockpit2/autopilot/vvi_status");
  ApAprStat = XPLMFindDataRef("sim/cockpit2/autopilot/approach_status");
  ApRevStat = XPLMFindDataRef("sim/cockpit2/autopilot/backcourse_status");
  AvPwrOn = XPLMFindDataRef("sim/cockpit/electrical/avionics_on");
  BatPwrOn = XPLMFindDataRef("sim/cockpit/electrical/battery_on");


/**************** Find Switch Panel Commands Ref *******************/
  ClFlOpn  = XPLMFindCommand("sim/flight_controls/cowl_flaps_open");
  ClFlCls  = XPLMFindCommand("sim/flight_controls/cowl_flaps_closed");
  PtHtOn   = XPLMFindCommand("sim/ice/pitot_heat_on");
  PtHtOff  = XPLMFindCommand("sim/ice/pitot_heat_off");

  AvLtOn   = XPLMFindCommand("sim/systems/avionics_on");
  AvLtOff  = XPLMFindCommand("sim/systems/avionics_off");
  BatOn   = XPLMFindCommand("sim/electrical/battery_1_on");
  BatOff  = XPLMFindCommand("sim/electrical/battery_1_off");

  LnLtOn   = XPLMFindCommand("sim/lights/landing_lights_on");
  LnLtOff  = XPLMFindCommand("sim/lights/landing_lights_off");
  TxLtOn   = XPLMFindCommand("sim/lights/taxi_lights_on");
  TxLtOff  = XPLMFindCommand("sim/lights/taxi_lights_off");
  StLtOn   = XPLMFindCommand("sim/lights/strobe_lights_on");
  StLtOff  = XPLMFindCommand("sim/lights/strobe_lights_off");
  NvLtOn   = XPLMFindCommand("sim/lights/nav_lights_on");
  NvLtOff  = XPLMFindCommand("sim/lights/nav_lights_off");
  BcLtOn   = XPLMFindCommand("sim/lights/beacon_lights_on");
  BcLtOff  = XPLMFindCommand("sim/lights/beacon_lights_off");
  GearUp   = XPLMFindCommand("sim/flight_controls/landing_gear_up");
  GearDn   = XPLMFindCommand("sim/flight_controls/landing_gear_down");

  MagOff1   = XPLMFindCommand("sim/magnetos/magnetos_off_1");
  MagOff2   = XPLMFindCommand("sim/magnetos/magnetos_off_2");
  MagOff3   = XPLMFindCommand("sim/magnetos/magnetos_off_3");
  MagOff4   = XPLMFindCommand("sim/magnetos/magnetos_off_4");
  MagOff5   = XPLMFindCommand("sim/magnetos/magnetos_off_5");
  MagOff6   = XPLMFindCommand("sim/magnetos/magnetos_off_6");
  MagOff7   = XPLMFindCommand("sim/magnetos/magnetos_off_7");
  MagOff8   = XPLMFindCommand("sim/magnetos/magnetos_off_8");

  MagLeft1  = XPLMFindCommand("sim/magnetos/magnetos_left_1");
  MagLeft2  = XPLMFindCommand("sim/magnetos/magnetos_left_2");
  MagLeft3  = XPLMFindCommand("sim/magnetos/magnetos_left_3");
  MagLeft4  = XPLMFindCommand("sim/magnetos/magnetos_left_4");
  MagLeft5  = XPLMFindCommand("sim/magnetos/magnetos_left_5");
  MagLeft6  = XPLMFindCommand("sim/magnetos/magnetos_left_6");
  MagLeft7  = XPLMFindCommand("sim/magnetos/magnetos_left_7");
  MagLeft8  = XPLMFindCommand("sim/magnetos/magnetos_left_8");

  MagRight1 = XPLMFindCommand("sim/magnetos/magnetos_right_1");
  MagRight2 = XPLMFindCommand("sim/magnetos/magnetos_right_2");
  MagRight3 = XPLMFindCommand("sim/magnetos/magnetos_right_3");
  MagRight4 = XPLMFindCommand("sim/magnetos/magnetos_right_4");
  MagRight5 = XPLMFindCommand("sim/magnetos/magnetos_right_5");
  MagRight6 = XPLMFindCommand("sim/magnetos/magnetos_right_6");
  MagRight7 = XPLMFindCommand("sim/magnetos/magnetos_right_7");
  MagRight8 = XPLMFindCommand("sim/magnetos/magnetos_right_8");

  MagBoth1  = XPLMFindCommand("sim/magnetos/magnetos_both_1");
  MagBoth2  = XPLMFindCommand("sim/magnetos/magnetos_both_2");
  MagBoth3  = XPLMFindCommand("sim/magnetos/magnetos_both_3");
  MagBoth4  = XPLMFindCommand("sim/magnetos/magnetos_both_4");
  MagBoth5  = XPLMFindCommand("sim/magnetos/magnetos_both_5");
  MagBoth6  = XPLMFindCommand("sim/magnetos/magnetos_both_6");
  MagBoth7  = XPLMFindCommand("sim/magnetos/magnetos_both_7");
  MagBoth8  = XPLMFindCommand("sim/magnetos/magnetos_both_8");

  EngStart1 = XPLMFindCommand("sim/starters/engage_starter_1");
  EngStart2 = XPLMFindCommand("sim/starters/engage_starter_2");
  EngStart3 = XPLMFindCommand("sim/starters/engage_starter_3");
  EngStart4 = XPLMFindCommand("sim/starters/engage_starter_4");
  EngStart5 = XPLMFindCommand("sim/starters/engage_starter_5");
  EngStart6 = XPLMFindCommand("sim/starters/engage_starter_6");
  EngStart7 = XPLMFindCommand("sim/starters/engage_starter_7");
  EngStart8 = XPLMFindCommand("sim/starters/engage_starter_8");

  BatOn1 = XPLMFindCommand("sim/electrical/battery_1_on");
  BatOn2 = XPLMFindCommand("sim/electrical/battery_2_on");

  BatOff1 = XPLMFindCommand("sim/electrical/battery_1_off");
  BatOff2 = XPLMFindCommand("sim/electrical/battery_2_off");

  GenOn1 = XPLMFindCommand("sim/electrical/generator_1_on");
  GenOn2 = XPLMFindCommand("sim/electrical/generator_2_on");
  GenOn3 = XPLMFindCommand("sim/electrical/generator_3_on");
  GenOn4 = XPLMFindCommand("sim/electrical/generator_4_on");
  GenOn5 = XPLMFindCommand("sim/electrical/generator_5_on");
  GenOn6 = XPLMFindCommand("sim/electrical/generator_6_on");
  GenOn7 = XPLMFindCommand("sim/electrical/generator_7_on");
  GenOn8 = XPLMFindCommand("sim/electrical/generator_8_on");

  GenOff1 = XPLMFindCommand("sim/electrical/generator_1_off");
  GenOff2 = XPLMFindCommand("sim/electrical/generator_2_off");
  GenOff3 = XPLMFindCommand("sim/electrical/generator_3_off");
  GenOff4 = XPLMFindCommand("sim/electrical/generator_4_off");
  GenOff5 = XPLMFindCommand("sim/electrical/generator_5_off");
  GenOff6 = XPLMFindCommand("sim/electrical/generator_6_off");
  GenOff7 = XPLMFindCommand("sim/electrical/generator_7_off");
  GenOff8 = XPLMFindCommand("sim/electrical/generator_8_off");

  FuelPumpOn1 = XPLMFindCommand("sim/fuel/fuel_pump_1_on");
  FuelPumpOn2 = XPLMFindCommand("sim/fuel/fuel_pump_2_on");
  FuelPumpOn3 = XPLMFindCommand("sim/fuel/fuel_pump_3_on");
  FuelPumpOn4 = XPLMFindCommand("sim/fuel/fuel_pump_4_on");
  FuelPumpOn5 = XPLMFindCommand("sim/fuel/fuel_pump_5_on");
  FuelPumpOn6 = XPLMFindCommand("sim/fuel/fuel_pump_6_on");
  FuelPumpOn7 = XPLMFindCommand("sim/fuel/fuel_pump_7_on");
  FuelPumpOn8 = XPLMFindCommand("sim/fuel/fuel_pump_8_on");

  FuelPumpOff1 = XPLMFindCommand("sim/fuel/fuel_pump_1_off");
  FuelPumpOff2 = XPLMFindCommand("sim/fuel/fuel_pump_2_off");
  FuelPumpOff3 = XPLMFindCommand("sim/fuel/fuel_pump_3_off");
  FuelPumpOff4 = XPLMFindCommand("sim/fuel/fuel_pump_4_off");
  FuelPumpOff5 = XPLMFindCommand("sim/fuel/fuel_pump_5_off");
  FuelPumpOff6 = XPLMFindCommand("sim/fuel/fuel_pump_6_off");
  FuelPumpOff7 = XPLMFindCommand("sim/fuel/fuel_pump_7_off");
  FuelPumpOff8 = XPLMFindCommand("sim/fuel/fuel_pump_8_off");

/**************** Find Switch Panel Data Ref *******************/
  AntiIce        = XPLMFindDataRef("sim/cockpit/switches/anti_ice_on");
  CockpitLights  = XPLMFindDataRef("sim/cockpit/electrical/cockpit_lights");
  CowlFlaps      = XPLMFindDataRef("sim/flightmodel/engine/ENGN_cowl");
  GearRetract    = XPLMFindDataRef("sim/aircraft/gear/acf_gear_retract");
  OnGround       = XPLMFindDataRef("sim/flightmodel/failures/onground_any");

  BatNum         = XPLMFindDataRef("sim/aircraft/electrical/num_batteries");
  GenNum         = XPLMFindDataRef("sim/aircraft/electrical/num_generators");
  EngNum         = XPLMFindDataRef("sim/aircraft/engine/acf_num_engines");
  BatArrayOnDR   = XPLMFindDataRef("sim/cockpit/electrical/battery_array_on");


/************* Open any Radio that is connected *****************/

  struct hid_device_info *rad_devs, *rad_cur_dev;

  rad_devs = hid_enumerate(0x6a3, 0x0d05);
  rad_cur_dev = rad_devs;
  while (rad_cur_dev) {
        radhandle[radcnt] = hid_open_path(rad_cur_dev->path);
        hid_set_nonblocking(radhandle[radcnt], 1);
        radiores = hid_read(radhandle[radcnt], radiobuf[radcnt], sizeof(radiobuf[radcnt]));
        hid_send_feature_report(radhandle[radcnt], radiowbuf[radcnt], 23);
        radcnt++;
        rad_cur_dev = rad_cur_dev->next;
  }
  hid_free_enumeration(rad_devs);

/*** Find Connected Multi Panel *****/

  struct hid_device_info *multi_devs, *multi_cur_dev;

  multi_devs = hid_enumerate(0x6a3, 0x0d06);
  multi_cur_dev = multi_devs;
  while (multi_cur_dev) {
        multihandle = hid_open_path(multi_cur_dev->path);
        hid_set_nonblocking(multihandle, 1);
        multires = hid_read(multihandle, multibuf, sizeof(multibuf));
        hid_send_feature_report(multihandle, multiwbuf, 13);
        multicnt++;
        multi_cur_dev = multi_cur_dev->next;
  }
  hid_free_enumeration(multi_devs);

/*** Find Connected Switch Panel *****/

  struct hid_device_info *switch_devs, *switch_cur_dev;

  switch_devs = hid_enumerate(0x6a3, 0x0d67);
  switch_cur_dev = switch_devs;
  while (switch_cur_dev) {
        switchhandle = hid_open_path(switch_cur_dev->path);
        hid_set_nonblocking(switchhandle, 1);
        switchres = hid_read(switchhandle, switchbuf, sizeof(switchbuf));
        hid_send_feature_report(switchhandle, switchwbuf, 2);
        switchcnt++;
        switch_cur_dev = switch_cur_dev->next;
  }
  hid_free_enumeration(switch_devs);

  /* Register our callback for every loop. Positive intervals
  * are in seconds, negative are the negative of sim frames.  Zero
  * registers but does not schedule a callback for time. */
  XPLMRegisterFlightLoopCallback(		
			MyPanelsFlightLoopCallback,	/* Callback */
			interval,			/* Interval -1 every loop*/
			NULL);				/* refcon not used. */

  return 1;
}

PLUGIN_API void	XPluginStop(void)
{
  /********** Unregitser the callback on quit. *************/
  XPLMUnregisterFlightLoopCallback(MyPanelsFlightLoopCallback, NULL);
  stopradcnt = radcnt - 1;

  // *** if open blank display and then close that radio panel ****

    if (stopradcnt == 3) {

      blankradiowbuf[3][0] = 0, blankradiowbuf[3][1] = 11, blankradiowbuf[3][2] = 11;
      blankradiowbuf[3][3] = 11, blankradiowbuf[3][4] = 11, blankradiowbuf[3][5] = 11;
      blankradiowbuf[3][6] = 11, blankradiowbuf[3][7] = 11, blankradiowbuf[3][8] = 11;
      blankradiowbuf[3][9] = 11, blankradiowbuf[3][10] = 11, blankradiowbuf[3][11] = 11;
      blankradiowbuf[3][12] = 11, blankradiowbuf[3][13] = 11, blankradiowbuf[3][14] = 11;
      blankradiowbuf[3][15] = 11, blankradiowbuf[3][16] = 11, blankradiowbuf[3][17] = 11;
      blankradiowbuf[3][18] = 11, blankradiowbuf[3][19] = 11, blankradiowbuf[3][20] = 11;

      radres = hid_send_feature_report(radhandle[stopradcnt], blankradiowbuf[stopradcnt], 23);
      hid_close(radhandle[stopradcnt]);
      stopradcnt--;
    }

    if (stopradcnt == 2) {

      blankradiowbuf[2][0] = 0, blankradiowbuf[2][1] = 11, blankradiowbuf[2][2] = 11;
      blankradiowbuf[2][3] = 11, blankradiowbuf[2][4] = 11, blankradiowbuf[2][5] = 11;
      blankradiowbuf[2][6] = 11, blankradiowbuf[2][7] = 11, blankradiowbuf[2][8] = 11;
      blankradiowbuf[2][9] = 11, blankradiowbuf[2][10] = 11, blankradiowbuf[2][11] = 11;
      blankradiowbuf[2][12] = 11, blankradiowbuf[2][13] = 11, blankradiowbuf[2][14] = 11;
      blankradiowbuf[2][15] = 11, blankradiowbuf[2][16] = 11, blankradiowbuf[2][17] = 11;
      blankradiowbuf[2][18] = 11, blankradiowbuf[2][19] = 11, blankradiowbuf[2][20] = 11;

      radres = hid_send_feature_report(radhandle[stopradcnt], blankradiowbuf[stopradcnt], 23);
      hid_close(radhandle[stopradcnt]);
      stopradcnt--;
    }

    if (stopradcnt == 1) {

      blankradiowbuf[1][0] = 0, blankradiowbuf[1][1] = 11, blankradiowbuf[1][2] = 11;
      blankradiowbuf[1][3] = 11, blankradiowbuf[1][4] = 11, blankradiowbuf[1][5] = 11;
      blankradiowbuf[1][6] = 11, blankradiowbuf[1][7] = 11, blankradiowbuf[1][8] = 11;
      blankradiowbuf[1][9] = 11, blankradiowbuf[1][10] = 11, blankradiowbuf[1][11] = 11;
      blankradiowbuf[1][12] = 11, blankradiowbuf[1][13] = 11, blankradiowbuf[1][14] = 11;
      blankradiowbuf[1][15] = 11, blankradiowbuf[1][16] = 11, blankradiowbuf[1][17] = 11;
      blankradiowbuf[1][18] = 11, blankradiowbuf[1][19] = 11, blankradiowbuf[1][20] = 11;

      radres = hid_send_feature_report(radhandle[stopradcnt], blankradiowbuf[stopradcnt], 23);
      hid_close(radhandle[stopradcnt]);
      stopradcnt--;
    }

    if (stopradcnt == 0) {

      blankradiowbuf[0][0] = 0, blankradiowbuf[0][1] = 11, blankradiowbuf[0][2] = 11;
      blankradiowbuf[0][3] = 11, blankradiowbuf[0][4] = 11, blankradiowbuf[0][5] = 11;
      blankradiowbuf[0][6] = 11, blankradiowbuf[0][7] = 11, blankradiowbuf[0][8] = 11;
      blankradiowbuf[0][9] = 11, blankradiowbuf[0][10] = 11, blankradiowbuf[0][11] = 11;
      blankradiowbuf[0][12] = 11, blankradiowbuf[0][13] = 11, blankradiowbuf[0][14] = 11;
      blankradiowbuf[0][15] = 11, blankradiowbuf[0][16] = 11, blankradiowbuf[0][17] = 11;
      blankradiowbuf[0][18] = 11, blankradiowbuf[0][19] = 11, blankradiowbuf[0][20] = 11;

      radres = hid_send_feature_report(radhandle[stopradcnt], blankradiowbuf[stopradcnt], 23);
      hid_close(radhandle[stopradcnt]);
    }



  // *** if open blank display and then close that multi panel ***
    if (multicnt > 0) {
        blankmultiwbuf[0] = 0, blankmultiwbuf[1] = 11, blankmultiwbuf[2] = 11;
        blankmultiwbuf[3] = 11, blankmultiwbuf[4] = 11, blankmultiwbuf[5] = 11;
        blankmultiwbuf[6] = 11, blankmultiwbuf[7] = 11, blankmultiwbuf[8] = 11;
        blankmultiwbuf[9] = 11, blankmultiwbuf[10] = 11, blankmultiwbuf[11] = 0;
        multires = hid_send_feature_report(multihandle, blankmultiwbuf, 13);
        hid_close(multihandle);

    }


// *** if open close that switch panel ***

    if (switchcnt > 0) {

      blankswitchwbuf[0] = 0;
      blankswitchwbuf[1] = 0;
      switchres = hid_send_feature_report(switchhandle, blankswitchwbuf, 2);
      hid_close(switchhandle);
    }


}

PLUGIN_API void XPluginDisable(void)
{
}

PLUGIN_API int XPluginEnable(void)
{
         return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID	inFromWho,
				      long		inMessage,
				      void *		inParam)
{
    (void) inFromWho; // To get rid of warnings on unused variables
    (void) inMessage; // To get rid of warnings on unused variables
    (void) inParam; // To get rid of warnings on unused variables
}

/************************* Panels Callback  *************************/
float	MyPanelsFlightLoopCallback(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon)

{
    (void) inElapsedSinceLastCall; // To get rid of warnings on unused variables
    (void) inElapsedTimeSinceLastFlightLoop; // To get rid of warnings on unused variables
    (void) inCounter; // To get rid of warnings on unused variables
    (void) inRefcon; // To get rid of warnings on unused variables

  if(radcnt > 0){
    process_radio_panel();
  }

  if (multicnt > 0) {
        process_multi_panel();
  }


  if(switchcnt > 0){
    process_switch_panel();
  }


  return interval;
}

