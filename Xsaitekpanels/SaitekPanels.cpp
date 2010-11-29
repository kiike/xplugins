//*********************************
//
//     Xsaitekpanels
//     William R. Good
//     Nov 29 2010
//
//**********************************

#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"
#include "libusb_interface.h"


#include <linux/hidraw.h>

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



/******** Radio Panel Defines ********/
#define RADIO0 "/dev/saitekradiopanel0"
#define RADIO1 "/dev/saitekradiopanel1"
#define RADIO2 "/dev/saitekradiopanel2"
#define RADIO3 "/dev/saitekradiopanel3"
#define RADIO4 "/dev/saitekradiopanel4"
#define RADIO5 "/dev/saitekradiopanel5"
#define RADIO6 "/dev/saitekradiopanel6"
#define RADIO7 "/dev/saitekradiopanel7"
#define RADIO8 "/dev/saitekradiopanel8"
#define RADIO9 "/dev/saitekradiopanel9"
#define RADIO10 "/dev/saitekradiopanel10"
#define RADIO11 "/dev/saitekradiopanel11"
#define RADIO12 "/dev/saitekradiopanel12"
#define RADIO13 "/dev/saitekradiopanel13"
#define RADIO14 "/dev/saitekradiopanel14"
#define RADIO15 "/dev/saitekradiopanel15"

/******* Multi Panel Defines *******/
#define MULTI "/dev/saitekmultipanel"

/* Switch Panel Defines */
#define SWITCH "/dev/saitekswitchpanel"

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
XPLMCommandRef ApHdgBtn = NULL, ApNavBtn = NULL, ApIasBtn = NULL, ApAltBtn = NULL;
XPLMCommandRef ApVsBtn = NULL, ApAprBtn = NULL, ApRevBtn = NULL;

XPLMCommandRef ApAutThrOn = NULL, ApAutThrOff = NULL, FlapsDn = NULL, FlapsUp = NULL;
XPLMCommandRef PitchTrimDn = NULL, PitchTrimUp = NULL, PitchTrimTkOff = NULL;

/***************** Multi Panel Data Ref *********************/
XPLMDataRef ApAlt = NULL, ApVs = NULL, ApAs = NULL, ApHdg = NULL, ApCrs = NULL;

XPLMDataRef ApMstrStat = NULL, ApHdgStat = NULL, ApNavStat = NULL, ApIasStat = NULL;
XPLMDataRef ApAltStat = NULL, ApVsStat = NULL, ApAprStat = NULL, ApRevStat = NULL;


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

XPLMDataRef CowlFlaps = NULL, CockpitLights = NULL, AntiIce = NULL;
XPLMDataRef GearRetract = NULL, OnGround = NULL;

/********************** Radio Panel variables ************************/
int radio0fd, radio1fd, radio2fd, radio3fd, radio4fd, radio5fd, radio6fd, radio7fd;
int radio8fd, radio9fd, radio10fd, radio11fd, radio12fd, radio13fd, radio14fd, radio15fd;
int radiofd[4] = {-1,-1,-1,-1}, radcnt = 0, wres;
float interval = -1;
static char blankradiowbuf[24]= {1, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11};


/********************** Multi Panel variables ***********************/
int multifd = -1;
static char blankmultiwbuf[12] = {1, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 0};

/****************** Switch Panel variables *******************************/
int switchfd = -1;
static char blankswitchwbuf[2];

/********************* MyPanelsFlightLoopCallback **************************/
float	MyPanelsFlightLoopCallback(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon);

void process_radio_panel();
void process_multi_panel();
void process_switch_panel();

/******************Plugin Calls ******************/
PLUGIN_API int XPluginStart(char *		outName,
			    char *		outSig,
			    char *		outDesc)
{

	/* First set up our plugin info. */
  strcpy(outName, "Xsaitekpanels");
  strcpy(outSig, "Xsaitekpanels.hardware ver to use libusb");
  strcpy(outDesc, "A plugin allows use of Saitek Pro Flight Panels");

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

  ApAutThrOn = XPLMFindCommand("sim/autopilot/autothrottle_on");
  ApAutThrOff = XPLMFindCommand("sim/autopilot/autothrottle_off");

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


  if(find_saitek_panels())
     printf("\n*************   returned true from find_saitek_panels   ******************\n\n");

/************* Open any Radio that is connected *****************/
  radio0fd = open(RADIO0, O_RDWR), radio1fd = open(RADIO1, O_RDWR);
  radio2fd = open(RADIO2, O_RDWR), radio3fd = open(RADIO3, O_RDWR);
  radio4fd = open(RADIO4, O_RDWR), radio5fd = open(RADIO5, O_RDWR);
  radio6fd = open(RADIO6, O_RDWR), radio7fd = open(RADIO7, O_RDWR);
  radio8fd = open(RADIO8, O_RDWR), radio9fd = open(RADIO9, O_RDWR);
  radio10fd = open(RADIO10, O_RDWR), radio11fd = open(RADIO11, O_RDWR);
  radio12fd = open(RADIO12, O_RDWR), radio13fd = open(RADIO13, O_RDWR);
  radio14fd = open(RADIO14, O_RDWR), radio15fd = open(RADIO15, O_RDWR);

/** If any found add to list of found Radios **/
  if (radio0fd > 0) {
    radiofd[radcnt] = radio0fd;
    radcnt++;
  }
  if (radio1fd > 0) {
    radiofd[radcnt] = radio1fd;
    radcnt++;
  }
  if (radio2fd > 0) {
    radiofd[radcnt] = radio2fd;
    radcnt++;
  }
  if (radio3fd > 0) {
    radiofd[radcnt] = radio3fd;
    radcnt++;
  }
  if (radio4fd > 0) {
    radiofd[radcnt] = radio4fd;
    radcnt++;
  }
  if (radio5fd > 0) {
    radiofd[radcnt] = radio5fd;
    radcnt++;
  }
  if (radio6fd > 0) {
    radiofd[radcnt] = radio6fd;
    radcnt++;
  }
  if (radio7fd > 0) {
    radiofd[radcnt] = radio7fd;
    radcnt++;
  }
  if (radio8fd > 0) {
    radiofd[radcnt] = radio8fd;
    radcnt++;
  }
  if (radio9fd > 0) {
    radiofd[radcnt] = radio9fd;
    radcnt++;
  }
  if (radio10fd > 0) {
    radiofd[radcnt] = radio10fd;
    radcnt++;
  }
  if (radio11fd > 0) {
    radiofd[radcnt] = radio11fd;
    radcnt++;
  }
  if (radio12fd > 0) {
    radiofd[radcnt] = radio12fd;
    radcnt++;
  }
  if (radio13fd > 0) {
    radiofd[radcnt] = radio13fd;
    radcnt++;
  }
  if (radio14fd > 0) {
    radiofd[radcnt] = radio14fd;
    radcnt++;
  }
  if (radio15fd > 0) {
    radiofd[radcnt] = radio15fd;
    radcnt++;
  }

/*** Find Connected Multi Panel *****/
  multifd = open(MULTI, O_RDWR);

/*** Find Connected Switch Panel *****/
  switchfd = open(SWITCH, O_RDWR);

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

/*** if open close that radio panel ****/
 
  if (radio0fd > 0) {
    wres = write(radio0fd, blankradiowbuf, sizeof(blankradiowbuf));
    close(radio0fd);
  }
  if (radio1fd > 0) {
    wres = write(radio1fd, blankradiowbuf, sizeof(blankradiowbuf));
    close(radio1fd);
  }
  if (radio2fd > 0) {
    wres = write(radio2fd, blankradiowbuf, sizeof(blankradiowbuf));
    close(radio2fd);
  }
  if (radio3fd > 0) {
    wres = write(radio3fd, blankradiowbuf, sizeof(blankradiowbuf));
    close(radio3fd);
  }
  if (radio4fd > 0) {
    wres = write(radio4fd, blankradiowbuf, sizeof(blankradiowbuf));
    close(radio4fd);
  }
  if (radio5fd > 0) {
    wres = write(radio5fd, blankradiowbuf, sizeof(blankradiowbuf));
    close(radio5fd);
  }
  if (radio6fd > 0) {
    wres = write(radio6fd, blankradiowbuf, sizeof(blankradiowbuf));
    close(radio6fd);
  }
  if (radio7fd > 0) {
    wres = write(radio7fd, blankradiowbuf, sizeof(blankradiowbuf));
    close(radio7fd);
  }
  if (radio8fd > 0) {
    wres = write(radio8fd, blankradiowbuf, sizeof(blankradiowbuf));
    close(radio8fd);
  }
  if (radio9fd > 0) {
    wres = write(radio9fd, blankradiowbuf, sizeof(blankradiowbuf));
    close(radio9fd);
  }
  if (radio10fd > 0) {
    wres = write(radio10fd, blankradiowbuf, sizeof(blankradiowbuf));
    close(radio10fd);
   }
  if (radio11fd > 0) {
    wres = write(radio11fd, blankradiowbuf, sizeof(blankradiowbuf));
    close(radio11fd);
  }
  if (radio12fd > 0) {
    wres = write(radio12fd, blankradiowbuf, sizeof(blankradiowbuf));
    close(radio12fd);
  }
  if (radio13fd > 0) {
    wres = write(radio13fd, blankradiowbuf, sizeof(blankradiowbuf));
    close(radio13fd);
   }
  if (radio14fd > 0) {
    wres = write(radio14fd, blankradiowbuf, sizeof(blankradiowbuf));
    close(radio14fd);
  }
  if (radio15fd > 0) {
    wres = write(radio15fd, blankradiowbuf, sizeof(blankradiowbuf));
    close(radio15fd);
  }

/*** if open close that multi panel ****/
  if (multifd > 0) {
    wres = write(multifd, blankmultiwbuf, sizeof(blankmultiwbuf));
    close(multifd);
  }

/*** if open close that switch panel ****/ 
  if (switchfd > 0) {
    blankswitchwbuf[0] = 1;
    blankswitchwbuf[1] = 0;
    wres = write(switchfd, blankswitchwbuf, sizeof(blankswitchwbuf));
    close(switchfd);
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
  if(multifd > 0){
    process_multi_panel();
  }
  if(switchfd > 0){
    process_switch_panel();
  }
  return interval;
}

