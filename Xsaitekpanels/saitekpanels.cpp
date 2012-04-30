// ****** saitekpanels.cpp ***********
// ****  William R. Good   ***********
// ******** ver 1.39    ***************
// ****** Apr 29 2012   **************

#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"
#include "XPLMPlugin.h"
#include "XPLMPlanes.h"
#include "XPLMMenus.h"
#include "XPWidgets.h"
#include "XPStandardWidgets.h"

#define MSG_ADD_DATAREF 0x01000000

#include "hidapi.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <string>
#include <string.h>

#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;



// ************* Radio Panel Command Ref ****************
XPLMCommandRef Com1StbyFineDn = NULL, Com1StbyFineUp = NULL, Com1StbyCorseDn = NULL, Com1StbyCorseUp = NULL;
XPLMCommandRef Com2StbyFineDn = NULL, Com2StbyFineUp = NULL, Com2StbyCorseDn = NULL, Com2StbyCorseUp = NULL;
XPLMCommandRef	Nav1StbyFineDn = NULL, Nav1StbyFineUp = NULL, Nav1StbyCorseDn = NULL, Nav1StbyCorseUp = NULL;
XPLMCommandRef	Nav2StbyFineDn = NULL, Nav2StbyFineUp = NULL, Nav2StbyCorseDn = NULL, Nav2StbyCorseUp = NULL;

XPLMCommandRef	Afd1StbyHunUp = NULL, Afd1StbyHunDn = NULL, Afd1StbyTensUp = NULL, Afd1StbyTensDn = NULL;
XPLMCommandRef	Afd1StbyOnesUp = NULL, Afd1StbyOnesDn = NULL;

XPLMCommandRef	Afd2StbyHunUp = NULL, Afd2StbyHunDn = NULL, Afd2StbyTensUp = NULL, Afd2StbyTensDn = NULL;
XPLMCommandRef	Afd2StbyOnesUp = NULL, Afd2StbyOnesDn = NULL;

XPLMCommandRef	XpdrThUp = NULL, XpdrThDn = NULL, XpdrHunUp = NULL, XpdrHunDn = NULL;
XPLMCommandRef	XpdrTensUp = NULL, XpdrTensDn = NULL, XpdrOnesUp = NULL, XpdrOnesDn = NULL;
XPLMCommandRef	BaroUp = NULL, BaroDn = NULL, BaroStd = NULL;


XPLMCommandRef Com1ActStby = NULL, Com2ActStby = NULL, Nav1ActStby = NULL, Nav2ActStby = NULL;
XPLMCommandRef Adf1ActStby = NULL, Adf2ActStby = NULL;

// ************* Radio Panel Data Ref ****************
XPLMDataRef Com1ActFreq = NULL, Com2ActFreq = NULL, Nav1ActFreq = NULL, Nav2ActFreq = NULL;
XPLMDataRef Com1StbyFreq = NULL, Com2StbyFreq = NULL, Nav1StbyFreq = NULL, Nav2StbyFreq = NULL;

XPLMDataRef Adf1StbyFreq = NULL, Adf2StbyFreq = NULL;
XPLMDataRef Adf1ActFreq = NULL, Adf2ActFreq = NULL;

XPLMDataRef XpdrCode = NULL, XpdrMode = NULL, BaroSetting = NULL;
XPLMDataRef MetricPress = NULL;

XPLMDataRef DmeMode = NULL, DmeSlvSource = NULL;
XPLMDataRef Nav1DmeNmDist = NULL, Nav1DmeSpeed = NULL;
XPLMDataRef Nav2DmeNmDist = NULL, Nav2DmeSpeed = NULL;
XPLMDataRef DmeFreq = NULL, DmeTime = NULL, DmeSpeed = NULL;

XPLMDataRef AvPwrOn = NULL, BatPwrOn = NULL;

XPLMDataRef Nav1PwrOn = NULL, Nav2PwrOn = NULL, Com1PwrOn = NULL, Com2PwrOn = NULL;
XPLMDataRef Afd1PwrOn = NULL, DmePwrOn = NULL;

XPLMMenuID      RadioMenu;
XPLMMenuID      RadioMenuId;

// ****************** Multi Panel Command Ref **********************
XPLMCommandRef ApAltDn = NULL, ApAltUp = NULL, ApVsDn = NULL, ApVsUp = NULL;
XPLMCommandRef ApAsDn = NULL, ApAsUp = NULL, ApHdgDn = NULL, ApHdgUp = NULL;
XPLMCommandRef ApCrsDn = NULL, ApCrsUp = NULL, ApCrsDn2 = NULL, ApCrsUp2 = NULL;

XPLMCommandRef ApMstrBtnUp = NULL, ApMstrBtnDn = NULL, ApMstrBtnOff = NULL;
XPLMCommandRef ApHdgBtn = NULL, ApNavBtn = NULL, ApAltBtn = NULL;
XPLMCommandRef ApIasBtn = NULL;

XPLMCommandRef ApVsBtn = NULL, ApAprBtn = NULL, ApRevBtn = NULL;

XPLMCommandRef FlapsDn = NULL, FlapsUp = NULL;
XPLMCommandRef PitchTrimDn = NULL, PitchTrimUp = NULL, PitchTrimTkOff = NULL;

XPLMCommandRef XpanelsFnButtonCommand = NULL, XpanelsCrsToggleCommand = NULL;

XPLMCommandRef x737mcp_cmd_a_toggle = NULL;
XPLMCommandRef x737mcp_hdg_up = NULL, x737mcp_hdg_down = NULL;
XPLMCommandRef x737mcp_hdg_up_fast = NULL, x737mcp_hdg_down_fast = NULL;
XPLMCommandRef x737mcp_spd_up = NULL, x737mcp_spd_down = NULL;
XPLMCommandRef x737mcp_spd_up_fast = NULL, x737mcp_spd_down_fast = NULL;
XPLMCommandRef x737mcp_alt_up = NULL, x737mcp_alt_down = NULL;
XPLMCommandRef x737mcp_alt_up_fast = NULL, x737mcp_alt_down_fast = NULL;
XPLMCommandRef x737mcp_vvi_up = NULL, x737mcp_vvi_down = NULL;
XPLMCommandRef x737mcp_vvi_up_fast = NULL, x737mcp_vvi_down_fast = NULL;

XPLMCommandRef x737mcp_hdg_toggle = NULL, x737mcp_vorloc_toggle = NULL;
XPLMCommandRef x737mcp_lvlchange_toggle = NULL, x737mcp_alt_toggle = NULL;
XPLMCommandRef x737mcp_vs_toggle = NULL, x737mcp_app_toggle = NULL;
XPLMCommandRef x737mcp_spd_changeover = NULL, x737mcp_lnav_toggle = NULL;

// ***************** Multi Panel Data Ref *********************
XPLMDataRef ApAlt = NULL, ApVs = NULL, ApAs = NULL, ApHdg = NULL, ApCrs = NULL, ApCrs2 = NULL;

XPLMDataRef ApMstrStat = NULL, ApHdgStat = NULL, ApNavStat = NULL, ApIasStat = NULL;
XPLMDataRef ApAltStat = NULL, ApVsStat = NULL, ApAprStat = NULL, ApRevStat = NULL;
XPLMDataRef x737athr_armed = NULL, x737swBatBus = NULL, x737stbyPwr = NULL;
XPLMDataRef ApState = NULL, ApAutThr;
XPLMDataRef Frp = NULL, MHdg = NULL;

XPLMDataRef AirspeedIsMach = NULL, Airspeed = NULL;

XPLMDataRef x737mcp_hdg = NULL, x737mcp_alt = NULL;
XPLMDataRef x737mcp_vs = NULL, x737mcp_vs_arm = NULL;

XPLMDataRef x737mcp_cmd_a_led = NULL;
XPLMDataRef x737mcp_hdg_led = NULL, x737mcp_vorloc_led = NULL, x737mcp_vorloc_armed = NULL;
XPLMDataRef x737mcp_lnav = NULL, x737mcp_lnav_armed = NULL;
XPLMDataRef x737mcp_lvlchange_led = NULL, x737mcp_alt_led = NULL;
XPLMDataRef x737mcp_vvi_led = NULL, x737mcp_vvi_armed_led = NULL, x737mcp_app_led = NULL;
XPLMDataRef HsiSelector = NULL;

XPLMMenuID      MultiMenu;
XPLMMenuID      MultiMenuId;

// *************** Switch Panel Command Ref *******************
XPLMCommandRef ClFlOpn = NULL, ClFlCls = NULL;
XPLMCommandRef PtHtOn = NULL, PtHtOff = NULL;
XPLMCommandRef PtHt0On = NULL, PtHt0Off = NULL, PtHt1On = NULL, PtHt1Off = NULL;

XPLMCommandRef AvLtOn = NULL, AvLtOff = NULL, BatOn = NULL, BatOff = NULL;
XPLMCommandRef LnLtOn = NULL, LnLtOff = NULL, TxLtOn = NULL, TxLtOff = NULL;
XPLMCommandRef StLtOn = NULL, StLtOff = NULL, NvLtOn = NULL, NvLtOff = NULL;
XPLMCommandRef BcLtOn = NULL, BcLtOff = NULL, GearUp = NULL, GearDn = NULL;

XPLMCommandRef FuelPumpOnCmd = NULL, FuelPumpOffCmd = NULL;
XPLMCommandRef DeiceOnCmd = NULL, DeiceOffCmd = NULL;
XPLMCommandRef CowlFlapsOpenCmd = NULL, CowlFlapsCloseCmd = NULL;
XPLMCommandRef PanelLightsOnCmd = NULL, PanelLightsOffCmd = NULL;
XPLMCommandRef GearUpCmd = NULL, GearDnCmd = NULL;


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

XPLMCommandRef x737gen1_on = NULL, x737gen2_on = NULL;
XPLMCommandRef x737gen1_off = NULL, x737gen2_off = NULL;
XPLMCommandRef x737ice_pitot1_on = NULL, x737ice_pitot2_on = NULL;
XPLMCommandRef x737ice_pitot1_off = NULL, x737ice_pitot2_off = NULL;
XPLMCommandRef x737ice_engine1_on = NULL, x737ice_engine2_on = NULL;
XPLMCommandRef x737ice_engine1_off = NULL, x737ice_engine2_off = NULL;
XPLMCommandRef x737ice_wing_on = NULL, x737ice_wing_off = NULL;

// ******************* Switch Panel Data Ref ********************
XPLMDataRef BatNum = NULL, GenNum = NULL, EngNum = NULL;
XPLMDataRef BatArrayOnDR = NULL;

XPLMDataRef CowlFlaps = NULL, CockpitLights = NULL, AntiIce = NULL;
XPLMDataRef GearRetract = NULL, OnGround = NULL, LandingGearStatus = {NULL};
XPLMDataRef Gear1Fail = NULL, Gear2Fail = NULL, Gear3Fail = NULL;

XPLMMenuID      SwitchMenu;
XPLMMenuID      SwitchMenuId;

// ****************** BIP Panel Command Ref **********************

//  ***************** BIP Panel Data Ref *********************
XPLMDataRef gTimeSimIsRunningXDataRef = NULL;

XPLMMenuID      XsaitekpanelsMenu;
XPLMMenuID      BipMenu;
XPLMMenuID      BipMenuId, Bip2MenuId, Bip3MenuId, Bip4MenuId;
XPLMMenuID      ConfigMenuId;

XPWidgetID      XsaitekpanelsWidgetID = NULL;
XPWidgetID      BipWidgetID = NULL;
XPWidgetID      Bip2WidgetID = NULL;
XPWidgetID      Bip3WidgetID = NULL;
XPWidgetID      Bip4WidgetID = NULL;

// ********************** Radio Panel variables ************************
int radcnt = 0, radiores, stopradcnt;
int radres, radnum = 0;
float interval = -1;
static unsigned char blankradiowbuf[4][23];
static unsigned char radiobuf[4][4], radiowbuf[4][23];

unsigned char radbuf[4], radwbuf[21];

int radspeed, numadf, metricpressenable;

hid_device *radiohandle[4];

// ********************** Multi Panel variables ***********************
int multicnt = 0, multires, stopmulticnt;
static unsigned char blankmultiwbuf[13];
unsigned char multibuf[4], multiwbuf[13];

int loaded737 = 0;

int trimspeed, multispeed, autothrottleswitchenable;

int xpanelsfnbutton = 0, xpanelscrstoggle = 0;

hid_device *multihandle;

// ****************** Switch Panel variables *******************************
int switchcnt = 0, switchres, stopswitchcnt;

long bataltinverse, cowlflapsenable, fuelpumpswitchenable;
long deiceswitchenable, panellightsenable, landinggearknobenable;

string fuel_pump_switch_on, fuel_pump_switch_off;
string deice_switch_on, deice_switch_off;
string cowl_flaps_open, cowl_flaps_close;
string panel_lights_switch_on, panel_lights_switch_off;
string gear_switch_up, gear_switch_down;


const char *GearTestStrUp;

static unsigned char blankswitchwbuf[2];
unsigned char switchbuf[4], switchwbuf[2];
float LandingGearDeployRatio[10];

hid_device *switchhandle;

// ****************** BIP Panel variables *******************************
int bipcnt = 0, bipres, biploop[4], stopbipcnt;
unsigned char bipwbuf[4][10];

string aircraft_path;

hid_device *biphandle[4];

// ****************** Saitek Panels variables *******************************
void            XsaitekpanelsMenuHandler(void *, void *);
void WriteCSVTableToDisk(void);

bool ReadConfigFile(std::string PlaneICAO);


int             XsaitekpanelsMenuItem;
int             BipMenuItem;

int Fps, multi_auto_mul;

int wrgXPlaneVersion = 0;
int wrgXPLMVersion = 0;
int wrgHostID = 0;

void process_radio_panel();
void process_multi_panel();
void process_switch_panel();
void process_bip_panel();
void process_pref_file();
void process_read_ini_file();

// ********************* MyPanelsFlightLoopCallback **************************
float	MyPanelsFlightLoopCallback(
                                   float                inElapsedSinceLastCall,    
                                   float                inElapsedTimeSinceLastFlightLoop,    
                                   int                  inCounter,    
                                   void *               inRefcon);

int    XpanelsFnButtonCommandHandler(XPLMCommandRef       inCommand,          //  custom command handler
                               XPLMCommandPhase     inPhase,
                               void *               inRefcon);

void WriteCSVTableToDisk(void);

bool ReadConfigFile(std::string PlaneICAO);
// ******************Plugin Calls ******************
PLUGIN_API int XPluginStart(char *		outName,
			    char *		outSig,
			    char *		outDesc)
{

  int ConfigSubMenuItem;
  int BipSubMenuItem, Bip2SubMenuItem;
  //Bip3SubMenuItem;
  int MultiSubMenuItem, RadioSubMenuItem;
  int SwitchSubMenuItem;

  XPLMGetVersions(&wrgXPlaneVersion, &wrgXPLMVersion, &wrgHostID);

  printf("gXPlaneVersion = %d gXPLMVersion = %d gHostID = %d\n", wrgXPlaneVersion, wrgXPLMVersion, wrgHostID);

	/* First set up our plugin info. */
  strcpy(outName, "Xsaitekpanels v1.39");
  strcpy(outSig, "saitekpanels.hardware uses hidapi interface");
  strcpy(outDesc, "A plugin allows use of Saitek Pro Flight Panels on all platforms");

// ************ Find Radio Panel Commands Ref ******************
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

  Afd1StbyHunUp         = XPLMFindCommand("sim/radios/stby_adf1_hundreds_up");
  Afd1StbyHunDn         = XPLMFindCommand("sim/radios/stby_adf1_hundreds_down");
  Afd1StbyTensUp	= XPLMFindCommand("sim/radios/stby_adf1_tens_up");
  Afd1StbyTensDn	= XPLMFindCommand("sim/radios/stby_adf1_tens_down");
  Afd1StbyOnesUp	= XPLMFindCommand("sim/radios/stby_adf1_ones_up");
  Afd1StbyOnesDn	= XPLMFindCommand("sim/radios/stby_adf1_ones_down");

  Afd2StbyHunUp         = XPLMFindCommand("sim/radios/stby_adf2_hundreds_up");
  Afd2StbyHunDn         = XPLMFindCommand("sim/radios/stby_adf2_hundreds_down");
  Afd2StbyTensUp	= XPLMFindCommand("sim/radios/stby_adf2_tens_up");
  Afd2StbyTensDn	= XPLMFindCommand("sim/radios/stby_adf2_tens_down");
  Afd2StbyOnesUp	= XPLMFindCommand("sim/radios/stby_adf2_ones_up");
  Afd2StbyOnesDn	= XPLMFindCommand("sim/radios/stby_adf2_ones_down");

  XpdrThUp	= XPLMFindCommand("sim/transponder/transponder_thousands_up");
  XpdrThDn	= XPLMFindCommand("sim/transponder/transponder_thousands_down");
  XpdrHunUp	= XPLMFindCommand("sim/transponder/transponder_hundreds_up");
  XpdrHunDn	= XPLMFindCommand("sim/transponder/transponder_hundreds_down");
  XpdrTensUp	= XPLMFindCommand("sim/transponder/transponder_tens_up");
  XpdrTensDn	= XPLMFindCommand("sim/transponder/transponder_tens_down");
  XpdrOnesUp	= XPLMFindCommand("sim/transponder/transponder_ones_up");
  XpdrOnesDn	= XPLMFindCommand("sim/transponder/transponder_ones_down");

  BaroUp	= XPLMFindCommand("sim/instruments/barometer_up");
  BaroDn	= XPLMFindCommand("sim/instruments/barometer_down");
  BaroStd	= XPLMFindCommand("sim/instruments/barometer_2992");

  Com1ActStby = XPLMFindCommand("sim/radios/com1_standy_flip");
  Com2ActStby = XPLMFindCommand("sim/radios/com2_standy_flip");
  Nav1ActStby = XPLMFindCommand("sim/radios/nav1_standy_flip");
  Nav2ActStby = XPLMFindCommand("sim/radios/nav2_standy_flip");
  Adf1ActStby = XPLMFindCommand("sim/radios/adf1_standy_flip");
  Adf2ActStby = XPLMFindCommand("sim/radios/adf2_standy_flip");


// ************ Find Radio Panel Data Ref ******************
  Com1ActFreq = XPLMFindDataRef("sim/cockpit/radios/com1_freq_hz");
  Com2ActFreq = XPLMFindDataRef("sim/cockpit/radios/com2_freq_hz");
  Nav1ActFreq = XPLMFindDataRef("sim/cockpit/radios/nav1_freq_hz");
  Nav2ActFreq = XPLMFindDataRef("sim/cockpit/radios/nav2_freq_hz");

  Com1StbyFreq = XPLMFindDataRef("sim/cockpit/radios/com1_stdby_freq_hz");
  Com2StbyFreq = XPLMFindDataRef("sim/cockpit/radios/com2_stdby_freq_hz");
  Nav1StbyFreq = XPLMFindDataRef("sim/cockpit/radios/nav1_stdby_freq_hz");
  Nav2StbyFreq = XPLMFindDataRef("sim/cockpit/radios/nav2_stdby_freq_hz");

  Adf1StbyFreq	= XPLMFindDataRef("sim/cockpit/radios/adf1_stdby_freq_hz");
  Adf2StbyFreq	= XPLMFindDataRef("sim/cockpit/radios/adf2_stdby_freq_hz");
  Adf1ActFreq	= XPLMFindDataRef("sim/cockpit/radios/adf1_freq_hz");
  Adf2ActFreq	= XPLMFindDataRef("sim/cockpit/radios/adf2_freq_hz");

  XpdrCode	= XPLMFindDataRef("sim/cockpit/radios/transponder_code");
  XpdrMode	= XPLMFindDataRef("sim/cockpit/radios/transponder_mode");
  BaroSetting	= XPLMFindDataRef("sim/cockpit/misc/barometer_setting");
  MetricPress   = XPLMFindDataRef("sim/physics/metric_press");

  DmeMode       = XPLMFindDataRef("sim/cockpit2/radios/actuators/DME_mode");
  DmeSlvSource  = XPLMFindDataRef("sim/cockpit2/radios/actuators/DME_slave_source");


  Nav1DmeNmDist	= XPLMFindDataRef("sim/cockpit2/radios/indicators/nav1_dme_distance_nm");
  Nav1DmeSpeed	= XPLMFindDataRef("sim/cockpit2/radios/indicators/nav1_dme_speed_kts");
  Nav2DmeNmDist	= XPLMFindDataRef("sim/cockpit2/radios/indicators/nav2_dme_distance_nm");
  Nav2DmeSpeed	= XPLMFindDataRef("sim/cockpit2/radios/indicators/nav2_dme_speed_kts");
  DmeSpeed	= XPLMFindDataRef("sim/cockpit2/radios/indicators/dme_dme_speed_kts");

  DmeFreq	= XPLMFindDataRef("sim/cockpit2/radios/actuators/dme_frequency_hz");
  DmeTime	= XPLMFindDataRef("sim/cockpit2/radios/indicators/dme_dme_time_min");

  AvPwrOn = XPLMFindDataRef("sim/cockpit/electrical/avionics_on");
  BatPwrOn = XPLMFindDataRef("sim/cockpit/electrical/battery_on");

  Nav1PwrOn = XPLMFindDataRef("sim/cockpit2/radios/actuators/nav1_power");
  Nav2PwrOn = XPLMFindDataRef("sim/cockpit2/radios/actuators/nav2_power");
  Com1PwrOn = XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_power");
  Com2PwrOn = XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_power");
  Afd1PwrOn = XPLMFindDataRef("sim/cockpit2/radios/actuators/adf1_power");
  DmePwrOn = XPLMFindDataRef("sim/cockpit2/radios/actuators/dme_power");


// ************ Find Multi Panel Commands Ref ***************
  ApAltDn = XPLMFindCommand("sim/autopilot/altitude_down");
  ApAltUp = XPLMFindCommand("sim/autopilot/altitude_up");
  ApVsDn = XPLMFindCommand("sim/autopilot/vertical_speed_down");
  ApVsUp = XPLMFindCommand("sim/autopilot/vertical_speed_up");
  ApAsDn = XPLMFindCommand("sim/autopilot/airspeed_down");
  ApAsUp = XPLMFindCommand("sim/autopilot/airspeed_up");
  ApHdgDn = XPLMFindCommand("sim/autopilot/heading_down");
  ApHdgUp = XPLMFindCommand("sim/autopilot/heading_up");

  ApCrsDn = XPLMFindCommand("sim/radios/obs1_down");
  ApCrsUp = XPLMFindCommand("sim/radios/obs1_up");
  ApCrsDn2 = XPLMFindCommand("sim/radios/obs2_down");
  ApCrsUp2 = XPLMFindCommand("sim/radios/obs2_up");

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

  PitchTrimDn = XPLMFindCommand("sim/flight_controls/pitch_trim_down");
  PitchTrimUp = XPLMFindCommand("sim/flight_controls/pitch_trim_up");
  PitchTrimTkOff = XPLMFindCommand("sim/flight_controls/pitch_trim_takeoff");
  FlapsDn = XPLMFindCommand("sim/flight_controls/flaps_down");
  FlapsUp = XPLMFindCommand("sim/flight_controls/flaps_up");

  XpanelsFnButtonCommand = XPLMCreateCommand("xplugins/xsaitekpanels/x_panels_fn_button","Xpanels Fn Button");


// **************** Find Multi Panel Data Ref ********************

  ApAlt = XPLMFindDataRef("sim/cockpit/autopilot/altitude");
  ApVs = XPLMFindDataRef("sim/cockpit/autopilot/vertical_velocity");
  ApAs = XPLMFindDataRef("sim/cockpit/autopilot/airspeed");
  ApHdg = XPLMFindDataRef("sim/cockpit/autopilot/heading_mag");

  ApCrs  = XPLMFindDataRef("sim/cockpit/radios/nav1_obs_degm");
  ApCrs2 = XPLMFindDataRef("sim/cockpit/radios/nav2_obs_degm");

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
  Frp = XPLMFindDataRef("sim/operation/misc/frame_rate_period");

  MHdg = XPLMFindDataRef("sim/flightmodel/position/magpsi");

  AirspeedIsMach = XPLMFindDataRef("sim/cockpit/autopilot/airspeed_is_mach");
  Airspeed = XPLMFindDataRef("sim/cockpit/autopilot/airspeed");


// **************** Find Switch Panel Commands Ref *******************
  ClFlOpn  = XPLMFindCommand("sim/flight_controls/cowl_flaps_open");
  ClFlCls  = XPLMFindCommand("sim/flight_controls/cowl_flaps_closed");
  PtHt0On   = XPLMFindCommand("sim/ice/pitot_heat0_on");
  PtHt0Off  = XPLMFindCommand("sim/ice/pitot_heat0_off");
  PtHt1On   = XPLMFindCommand("sim/ice/pitot_heat1_on");
  PtHt1Off  = XPLMFindCommand("sim/ice/pitot_heat1_off");

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

// **************** Find Switch Panel Data Ref *******************
  AntiIce           = XPLMFindDataRef("sim/cockpit/switches/anti_ice_on");
  CockpitLights     = XPLMFindDataRef("sim/cockpit/electrical/cockpit_lights");
  CowlFlaps         = XPLMFindDataRef("sim/flightmodel/engine/ENGN_cowl");
  GearRetract       = XPLMFindDataRef("sim/aircraft/gear/acf_gear_retract");
  LandingGearStatus = XPLMFindDataRef("sim/aircraft/parts/acf_gear_deploy");
  Gear1Fail         = XPLMFindDataRef("sim/operation/failures/rel_lagear1");
  Gear2Fail         = XPLMFindDataRef("sim/operation/failures/rel_lagear2");
  Gear3Fail         = XPLMFindDataRef("sim/operation/failures/rel_lagear3");

  OnGround          = XPLMFindDataRef("sim/flightmodel/failures/onground_any");

  BatNum            = XPLMFindDataRef("sim/aircraft/electrical/num_batteries");
  GenNum            = XPLMFindDataRef("sim/aircraft/electrical/num_generators");
  EngNum            = XPLMFindDataRef("sim/aircraft/engine/acf_num_engines");
  BatArrayOnDR      = XPLMFindDataRef("sim/cockpit/electrical/battery_array_on");



// ************* Open any Radio that is connected *****************

  struct hid_device_info *rad_devs, *rad_cur_dev;

  rad_devs = hid_enumerate(0x6a3, 0x0d05);
  rad_cur_dev = rad_devs;
  while (rad_cur_dev) {
        radiohandle[radcnt] = hid_open_path(rad_cur_dev->path);
        hid_set_nonblocking(radiohandle[radcnt], 1);
        radiores = hid_read(radiohandle[radcnt], radiobuf[radcnt], sizeof(radiobuf[radcnt]));
        hid_send_feature_report(radiohandle[radcnt], radiowbuf[radcnt], 23);
        radcnt++;
        rad_cur_dev = rad_cur_dev->next;
  }
  hid_free_enumeration(rad_devs);

// *** Find Connected Multi Panel *****

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

// *** Find Connected Switch Panel *****

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


  // *** Find Connected BIP Panel *****

  struct hid_device_info *bip_devs, *bip_cur_dev;

  bip_devs = hid_enumerate(0x6a3, 0xb4e);
  bip_cur_dev = bip_devs;
  while (bip_cur_dev) {
      biphandle[bipcnt] = hid_open_path(bip_cur_dev->path);
        //  If Found Set brightness to full (0 - 100)
        // 0xb2 Report ID for brightness
        // Next byte 0 - 100 brightness value
      if (biphandle[bipcnt] > 0) {
           bipwbuf[bipcnt][0] = 0xb2; // 0xb2 Report ID for brightness
           bipwbuf[bipcnt][1] = 100;  // Set brightness to 100%
           bipres = hid_send_feature_report(biphandle[bipcnt], bipwbuf[bipcnt], 10);
           biploop[bipcnt] = 1;
        }

      hid_send_feature_report(biphandle[bipcnt], bipwbuf[bipcnt], 10);
        bipcnt++;
        bip_cur_dev = bip_cur_dev->next;
    }
  hid_free_enumeration(bip_devs);


  // * Register our callback for every loop. Positive intervals
  // * are in seconds, negative are the negative of sim frames.  Zero
  // * registers but does not schedule a callback for time.
  XPLMRegisterFlightLoopCallback(		
                        MyPanelsFlightLoopCallback,	// * Callback *
                        interval,			// * Interval -1 every loop*
                        NULL);				// * refcon not used. *

  // Register our custom commands
  XPLMRegisterCommandHandler(XpanelsFnButtonCommand,           // in Command name
                             XpanelsFnButtonCommandHandler,    // in Handler
                             1,                          // Receive input before plugin windows.
                             (void *) 0);                // inRefcon.




  // Create our menu

   XsaitekpanelsMenuItem = XPLMAppendMenuItem(
              XPLMFindPluginsMenu(),
              "Xsaitekpanels",
               NULL,
               1);

   XsaitekpanelsMenu = XPLMCreateMenu(
               "Xsaitekpanels",
               XPLMFindPluginsMenu(),
               XsaitekpanelsMenuItem,
               XsaitekpanelsMenuHandler,
               (void *)0);

   ConfigSubMenuItem = XPLMAppendMenuItem(
           XsaitekpanelsMenu,
           "xsaitekpanels.ini",
           NULL,
           5);

   ConfigMenuId = XPLMCreateMenu(
           "xsaitekpanels.ini",
           XsaitekpanelsMenu,
           ConfigSubMenuItem,
           XsaitekpanelsMenuHandler,
           (void *)5);

    XPLMClearAllMenuItems(ConfigMenuId);
    XPLMAppendMenuItem(ConfigMenuId, "Reload xsaitekpanels.ini", (void *) "TRUE", 1);


   if (bipcnt > 0) {

     if(bipcnt > 0){
       BipSubMenuItem = XPLMAppendMenuItem(
               XsaitekpanelsMenu,
               "Bip",
               NULL,
               1);


       BipMenuId = XPLMCreateMenu(
               "Bip",
               XsaitekpanelsMenu,
               BipSubMenuItem,
               XsaitekpanelsMenuHandler,
               (void *)1);



       BipWidgetID = XPCreateWidget(XPLMGetDatai(XPLMFindDataRef("sim/graphics/view/window_width"))-150,
                                    XPLMGetDatai(XPLMFindDataRef("sim/graphics/view/window_height")),
                                    XPLMGetDatai(XPLMFindDataRef("sim/graphics/view/window_width"))-10,
                                    XPLMGetDatai(XPLMFindDataRef("sim/graphics/view/window_height"))-10,         // screen coordinates
                                    1,                            // visible
                                    "BIP is working!", // description
                                    1, NULL,                      // we want it root
                                    xpWidgetClass_Caption);
       XPSetWidgetProperty(BipWidgetID, xpProperty_CaptionLit, 0);
     }

     if(bipcnt > 1) {
       Bip2SubMenuItem = XPLMAppendMenuItem(
           XsaitekpanelsMenu,
           "Bip2",
           NULL,
           1);

       Bip2MenuId = XPLMCreateMenu(
           "Bip2",
           XsaitekpanelsMenu,
           Bip2SubMenuItem,
           XsaitekpanelsMenuHandler,
           (void *)1);



     }


   }

   if (multicnt > 0) {

       MultiSubMenuItem = XPLMAppendMenuItem(
               XsaitekpanelsMenu,
               "Multi",
               NULL,
               2);


       MultiMenuId = XPLMCreateMenu(
               "Multi",
               XsaitekpanelsMenu,
               MultiSubMenuItem,
               XsaitekpanelsMenuHandler,
               (void *)2);
  }

   if (radcnt > 0) {

       RadioSubMenuItem = XPLMAppendMenuItem(
               XsaitekpanelsMenu,
               "Radio",
               NULL,
               3);


       RadioMenuId = XPLMCreateMenu(
               "Radio",
               XsaitekpanelsMenu,
               RadioSubMenuItem,
               XsaitekpanelsMenuHandler,
               (void *)3);
  }

   if (switchcnt > 0) {

       SwitchSubMenuItem = XPLMAppendMenuItem(
               XsaitekpanelsMenu,
               "Switch",
               NULL,
               4);


       SwitchMenuId = XPLMCreateMenu(
               "Switch",
               XsaitekpanelsMenu,
               SwitchSubMenuItem,
               XsaitekpanelsMenuHandler,
               (void *)4);
  }

  return 1;
}

PLUGIN_API void	XPluginStop(void)
{

    //printf("XPluginStop has been called \n");
    XPLMDebugString("XPluginStop has been called \n");
  // ********** Unregitser the callback on quit. *************
  XPLMUnregisterFlightLoopCallback(MyPanelsFlightLoopCallback, NULL);
  XPLMUnregisterCommandHandler(XpanelsFnButtonCommand, XpanelsFnButtonCommandHandler, 1, NULL);
  XPDestroyWidget(BipWidgetID, 1);
  XPLMDestroyMenu(BipMenuId);
  XPLMDestroyMenu(Bip2MenuId);
  XPLMDestroyMenu(Bip3MenuId);
  XPLMDestroyMenu(MultiMenuId);
  XPLMDestroyMenu(RadioMenuId);
  XPLMDestroyMenu(SwitchMenuId);

  stopradcnt = radcnt - 1;

  // *** if open blank display and then close that radio panel ****

  if (stopradcnt == 3) {

      blankradiowbuf[3][0] = 0, blankradiowbuf[3][1] = 15, blankradiowbuf[3][2] = 15;
      blankradiowbuf[3][3] = 15, blankradiowbuf[3][4] = 15, blankradiowbuf[3][5] = 15;
      blankradiowbuf[3][6] = 15, blankradiowbuf[3][7] = 15, blankradiowbuf[3][8] = 15;
      blankradiowbuf[3][9] = 15, blankradiowbuf[3][10] = 15, blankradiowbuf[3][11] = 15;
      blankradiowbuf[3][12] = 15, blankradiowbuf[3][13] = 15, blankradiowbuf[3][14] = 15;
      blankradiowbuf[3][15] = 15, blankradiowbuf[3][16] = 15, blankradiowbuf[3][17] = 15;
      blankradiowbuf[3][18] = 15, blankradiowbuf[3][19] = 15, blankradiowbuf[3][20] = 15;

      radres = hid_send_feature_report(radiohandle[stopradcnt], blankradiowbuf[stopradcnt], 23);
      hid_close(radiohandle[stopradcnt]);
      stopradcnt--;
  }

  if (stopradcnt == 2) {

      blankradiowbuf[2][0] = 0, blankradiowbuf[2][1] = 15, blankradiowbuf[2][2] = 15;
      blankradiowbuf[2][3] = 15, blankradiowbuf[2][4] = 15, blankradiowbuf[2][5] = 15;
      blankradiowbuf[2][6] = 15, blankradiowbuf[2][7] = 15, blankradiowbuf[2][8] = 15;
      blankradiowbuf[2][9] = 15, blankradiowbuf[2][10] = 15, blankradiowbuf[2][11] = 15;
      blankradiowbuf[2][12] = 15, blankradiowbuf[2][13] = 15, blankradiowbuf[2][14] = 15;
      blankradiowbuf[2][15] = 15, blankradiowbuf[2][16] = 15, blankradiowbuf[2][17] = 15;
      blankradiowbuf[2][18] = 15, blankradiowbuf[2][19] = 15, blankradiowbuf[2][20] = 15;

      radres = hid_send_feature_report(radiohandle[stopradcnt], blankradiowbuf[stopradcnt], 23);
      hid_close(radiohandle[stopradcnt]);
      stopradcnt--;
  }

  if (stopradcnt == 1) {

      blankradiowbuf[1][0] = 0, blankradiowbuf[1][1] = 15, blankradiowbuf[1][2] = 15;
      blankradiowbuf[1][3] = 15, blankradiowbuf[1][4] = 15, blankradiowbuf[1][5] = 15;
      blankradiowbuf[1][6] = 15, blankradiowbuf[1][7] = 15, blankradiowbuf[1][8] = 15;
      blankradiowbuf[1][9] = 15, blankradiowbuf[1][10] = 15, blankradiowbuf[1][11] = 15;
      blankradiowbuf[1][12] = 15, blankradiowbuf[1][13] = 15, blankradiowbuf[1][14] = 15;
      blankradiowbuf[1][15] = 15, blankradiowbuf[1][16] = 15, blankradiowbuf[1][17] = 15;
      blankradiowbuf[1][18] = 15, blankradiowbuf[1][19] = 15, blankradiowbuf[1][20] = 15;

      radres = hid_send_feature_report(radiohandle[stopradcnt], blankradiowbuf[stopradcnt], 23);
      hid_close(radiohandle[stopradcnt]);
      stopradcnt--;
  }

  if (stopradcnt == 0) {

      blankradiowbuf[0][0] = 0, blankradiowbuf[0][1] = 15, blankradiowbuf[0][2] = 15;
      blankradiowbuf[0][3] = 15, blankradiowbuf[0][4] = 15, blankradiowbuf[0][5] = 15;
      blankradiowbuf[0][6] = 15, blankradiowbuf[0][7] = 15, blankradiowbuf[0][8] = 15;
      blankradiowbuf[0][9] = 15, blankradiowbuf[0][10] = 15, blankradiowbuf[0][11] = 15;
      blankradiowbuf[0][12] = 15, blankradiowbuf[0][13] = 15, blankradiowbuf[0][14] = 15;
      blankradiowbuf[0][15] = 15, blankradiowbuf[0][16] = 15, blankradiowbuf[0][17] = 15;
      blankradiowbuf[0][18] = 15, blankradiowbuf[0][19] = 15, blankradiowbuf[0][20] = 15;

      radres = hid_send_feature_report(radiohandle[stopradcnt], blankradiowbuf[stopradcnt], 23);
      hid_close(radiohandle[stopradcnt]);
  }



  // *** if open blank display and then close that multi panel ***
  if (multicnt > 0) {
        blankmultiwbuf[0] = 0, blankmultiwbuf[1] = 15, blankmultiwbuf[2] = 15;
        blankmultiwbuf[3] = 15, blankmultiwbuf[4] = 15, blankmultiwbuf[5] = 15;
        blankmultiwbuf[6] = 15, blankmultiwbuf[7] = 15, blankmultiwbuf[8] = 15;
        blankmultiwbuf[9] = 15, blankmultiwbuf[10] = 15, blankmultiwbuf[11] = 0;
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

// *** if open close that bip panel ***

  stopbipcnt = bipcnt - 1;


  if (stopbipcnt == 3) {

       bipwbuf[3][0] = 0xb8;  //0xb8 Report ID to display
       bipwbuf[3][1] = 0, bipwbuf[3][2] = 0, bipwbuf[3][3] = 0;
       bipwbuf[3][4] = 0, bipwbuf[3][5] = 0, bipwbuf[3][6] = 0;

       bipres = hid_send_feature_report(biphandle[stopbipcnt], bipwbuf[stopbipcnt], 10);
       hid_close(biphandle[stopbipcnt]);
       stopbipcnt--;
  }

  if (stopbipcnt == 2) {

       bipwbuf[2][0] = 0xb8;  //0xb8 Report ID to display
       bipwbuf[2][1] = 0, bipwbuf[2][2] = 0, bipwbuf[2][3] = 0;
       bipwbuf[2][4] = 0, bipwbuf[2][5] = 0, bipwbuf[2][6] = 0;

       bipres = hid_send_feature_report(biphandle[stopbipcnt], bipwbuf[stopbipcnt], 10);
       hid_close(biphandle[stopbipcnt]);
       stopbipcnt--;
  }

  if (stopbipcnt == 1) {

       bipwbuf[1][0] = 0xb8;  //0xb8 Report ID to display
       bipwbuf[1][1] = 0, bipwbuf[1][2] = 0, bipwbuf[1][3] = 0;
       bipwbuf[1][4] = 0, bipwbuf[1][5] = 0, bipwbuf[1][6] = 0;

       bipres = hid_send_feature_report(biphandle[stopbipcnt], bipwbuf[stopbipcnt], 10);
       hid_close(biphandle[stopbipcnt]);
       stopbipcnt--;
  }

  if (stopbipcnt == 0) {

       bipwbuf[0][0] = 0xb8;  //0xb8 Report ID to display
       bipwbuf[0][1] = 0, bipwbuf[0][2] = 0, bipwbuf[0][3] = 0;
       bipwbuf[0][4] = 0, bipwbuf[0][5] = 0, bipwbuf[0][6] = 0;

       bipres = hid_send_feature_report(biphandle[stopbipcnt], bipwbuf[stopbipcnt], 10);
       hid_close(biphandle[stopbipcnt]);
       stopbipcnt--;
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
    std::string          PlaneICAO = "[]";
    char            ICAOString[40];

    XPLMGetDatab(XPLMFindDataRef("sim/aircraft/view/acf_ICAO"), ICAOString, 0, 40);

    PlaneICAO.insert(1,ICAOString);

    if ((inMessage == XPLM_MSG_PLANE_LOADED) & ((long) inParam == 0)) {
      process_read_ini_file();
    }

    if(bipcnt > 0){

        if ((inMessage == XPLM_MSG_PLANE_LOADED) & ((long) inParam == 0)) {
          ReadConfigFile(PlaneICAO);
        }
        if (inMessage == XPLM_MSG_AIRPORT_LOADED) {
          ReadConfigFile(PlaneICAO);
        }

    }

}

void XsaitekpanelsMenuHandler(void * inMenuRef, void * inItemRef)
{
    (void) inMenuRef; // To get rid of warnings on unused variables
    if((long)inMenuRef == 1){
         if (strcmp((char *) inItemRef, "<<CSV>>") == 0) {
             WriteCSVTableToDisk();
         }
         if (strcmp((char *) inItemRef, "<<BIP2>>") == 0) {
               ReadConfigFile((char *) inItemRef);
         }
         else {
            ReadConfigFile((char *) inItemRef);

         }

    }
    if((long)inMenuRef == 2){
         if (strcmp((char *) inItemRef, "1") == 0) {
             multispeed = 1;
         }
         if (strcmp((char *) inItemRef, "2") == 0) {
             multispeed = 2;
         }
         if (strcmp((char *) inItemRef, "3") == 0) {
             multispeed = 3;
         }
         if (strcmp((char *) inItemRef, "4") == 0) {
             multispeed = 4;
         }
         if (strcmp((char *) inItemRef, "5") == 0) {
             multispeed = 5;
         }

         if (strcmp((char *) inItemRef, "TRIM X1") == 0) {
             trimspeed = 1;
         }
         if (strcmp((char *) inItemRef, "TRIM X2") == 0) {
             trimspeed = 2;
         }
         if (strcmp((char *) inItemRef, "TRIM X3") == 0) {
             trimspeed = 3;
         }

         if (strcmp((char *) inItemRef, "ENABLE_AUTO_THROTTLE") == 0) {
             autothrottleswitchenable = 1;
         }
         if (strcmp((char *) inItemRef, "DISABLE_AUTO_THROTTLE") == 0) {
             autothrottleswitchenable = 0;
         }

    }

    if((long)inMenuRef == 3){
         if (strcmp((char *) inItemRef, "1") == 0) {
             radspeed = 1;
         }
         if (strcmp((char *) inItemRef, "2") == 0) {
             radspeed = 2;
         }
         if (strcmp((char *) inItemRef, "3") == 0) {
             radspeed = 3;
         }
         if (strcmp((char *) inItemRef, "4") == 0) {
             radspeed = 4;
         }
         if (strcmp((char *) inItemRef, "5") == 0) {
             radspeed = 5;
         }
         if (strcmp((char *) inItemRef, "ADF1") == 0) {
             numadf = 1;
         }
         if (strcmp((char *) inItemRef, "ADF2") == 0) {
             numadf = 2;
         }
         if (strcmp((char *) inItemRef, "inHg") == 0) {
             metricpressenable = 0;
         }
         if (strcmp((char *) inItemRef, "hPa") == 0) {
             metricpressenable = 1;
         }


    }

    if((long)inMenuRef == 4){
         if (strcmp((char *) inItemRef, "NORMAL") == 0) {
             bataltinverse = 0;
         }
         if (strcmp((char *) inItemRef, "CESSNA") == 0) {
             bataltinverse = 1;
         }

         if (strcmp((char *) inItemRef, "ENABLE_FUEL_PUMP") == 0) {
             fuelpumpswitchenable = 1;
         }
         if (strcmp((char *) inItemRef, "DISABLE_FUEL_PUMP") == 0) {
             fuelpumpswitchenable = 0;
         }


         if (strcmp((char *) inItemRef, "ENABLE_DEICE") == 0) {
             deiceswitchenable = 1;
         }
         if (strcmp((char *) inItemRef, "DISABLE_DEICE") == 0) {
             deiceswitchenable = 0;
         }

         if (strcmp((char *) inItemRef, "ENABLE_COWL") == 0) {
             cowlflapsenable = 1;
         }
         if (strcmp((char *) inItemRef, "DISABLE_COWL") == 0) {
             cowlflapsenable = 0;
         }

         if (strcmp((char *) inItemRef, "ENABLE_PANEL_LIGHTS") == 0) {
             panellightsenable = 1;
         }
         if (strcmp((char *) inItemRef, "DISABLE_PANEL_LIGHTS") == 0) {
             panellightsenable = 0;
         }

         if (strcmp((char *) inItemRef, "ENABLE_GEAR") == 0) {
             landinggearknobenable = 1;
         }
         if (strcmp((char *) inItemRef, "DISABLE_GEAR") == 0) {
             landinggearknobenable = 0;
         }

     }

    if((long)inMenuRef == 5){
         if (strcmp((char *) inItemRef, "TRUE") == 0) {
             process_read_ini_file();
         }

    }

    return;
}

int    XpanelsFnButtonCommandHandler(XPLMCommandRef       inCommand,
                        XPLMCommandPhase     inPhase,
                        void *               inRefcon)
{
    (void) inCommand;
    (void) inRefcon;
//  If inPhase == 1 the command is executed continuously.
     if (inPhase == 1)
   {
          xpanelsfnbutton = 1;
    }
     if (inPhase == 2)
   {
          xpanelsfnbutton = 0;
    }

return 0;
}

// ************************* Panels Callback  *************************
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

  if(bipcnt > 0){
    process_bip_panel();

  }

  if (XPLMIsDataRefGood(XPLMFindDataRef("x737/systems/afds/plugin_status"))) {
     loaded737 = 1;

     x737athr_armed = XPLMFindDataRef("x737/systems/athr/athr_armed");

     x737gen1_on = XPLMFindCommand("x737/electrical/GEN_1_CONNECT");
     x737gen1_off = XPLMFindCommand("x737/electrical/GEN_1_DISCONNECT");
     x737gen2_on = XPLMFindCommand("x737/electrical/GEN_2_CONNECT");
     x737gen2_off = XPLMFindCommand("x737/electrical/GEN_2_DISCONNECT");
     x737ice_pitot1_on = XPLMFindCommand("x737/ice_and_rain/PITOTHEAT1_ON");
     x737ice_pitot1_off = XPLMFindCommand("x737/ice_and_rain/PITOTHEAT1_OFF");
     x737ice_pitot2_on = XPLMFindCommand("x737/ice_and_rain/PITOTHEAT2_ON");
     x737ice_pitot2_off = XPLMFindCommand("x737/ice_and_rain/PITOTHEAT2_OFF");
     x737ice_engine1_on = XPLMFindCommand("x737/ice_and_rain/EAI1_ON");
     x737ice_engine1_off = XPLMFindCommand("x737/ice_and_rain/EAI1_OFF");
     x737ice_engine2_on = XPLMFindCommand("x737/ice_and_rain/EAI2_ON");
     x737ice_engine2_off = XPLMFindCommand("x737/ice_and_rain/EAI2_OFF");
     x737ice_wing_on = XPLMFindCommand("x737/ice_and_rain/WAI_ON");
     x737ice_wing_off = XPLMFindCommand("x737/ice_and_rain/WAI_OFF");

     x737mcp_cmd_a_toggle = XPLMFindCommand("x737/mcp/CMDA_TOGGLE");
     x737mcp_cmd_a_led = XPLMFindDataRef("x737/systems/MCP/LED_CMDA_on");

     x737mcp_hdg = XPLMFindDataRef("x737/systems/afds/HDG");
     x737mcp_hdg_toggle = XPLMFindCommand("x737/mcp/HDGSEL_TOGGLE");
     x737mcp_hdg_up = XPLMFindCommand("x737/mcp/HDG+1");
     x737mcp_hdg_up_fast = XPLMFindCommand("x737/mcp/HDG+6");
     x737mcp_hdg_down = XPLMFindCommand("x737/mcp/HDG-1");
     x737mcp_hdg_down_fast = XPLMFindCommand("x737/mcp/HDG-6");

     x737mcp_lnav_armed = XPLMFindDataRef("x737/systems/afds/LNAV_arm");
     x737mcp_lnav = XPLMFindDataRef("x737/systems/afds/LNAV");
     x737mcp_lnav_toggle = XPLMFindCommand("x737/mcp/LNAV_TOGGLE");

     x737mcp_vorloc_armed = XPLMFindDataRef("x737/systems/afds/VORLOC_armed");
     x737mcp_vorloc_toggle = XPLMFindCommand("x737/mcp/VORLOC_TOGGLE");

     HsiSelector = XPLMFindDataRef("sim/cockpit/switches/HSI_selector");

     x737mcp_lvlchange_toggle = XPLMFindCommand("x737/mcp/LVLCHANGE_TOGGLE");
     x737mcp_spd_up = XPLMFindCommand("x737/mcp/MCPSPD+1");
     x737mcp_spd_up_fast = XPLMFindCommand("x737/mcp/MCPSPD+10");
     x737mcp_spd_down = XPLMFindCommand("x737/mcp/MCPSPD-1");
     x737mcp_spd_down_fast = XPLMFindCommand("x737/mcp/MCPSPD-10");
     x737mcp_spd_changeover = XPLMFindCommand("x737/mcp/CHANGEOVER");

     x737mcp_alt = XPLMFindDataRef("x737/systems/afds/ALTHLD_baroalt");
     x737mcp_alt_toggle = XPLMFindCommand("x737/mcp/ALTHLD_TOGGLE");
     x737mcp_alt_up = XPLMFindCommand("x737/mcp/ALTSEL+100");
     x737mcp_alt_down = XPLMFindCommand("x737/mcp/ALTSEL-100");
     x737mcp_alt_up_fast = XPLMFindCommand("x737/mcp/ALTSEL+1000");
     x737mcp_alt_down_fast = XPLMFindCommand("x737/mcp/ALTSEL-1000");

     x737mcp_vs = XPLMFindDataRef("x737/systems/afds/VS");
     x737mcp_vs_arm = XPLMFindDataRef("x737/systems/afds/VS_arm");
     x737mcp_vs_toggle = XPLMFindCommand("x737/mcp/VS_TOGGLE");
     x737mcp_vvi_up = XPLMFindCommand("x737/mcp/VVI+100");
     x737mcp_vvi_down = XPLMFindCommand("x737/mcp/VVI-100");
     x737mcp_vvi_up_fast = XPLMFindCommand("x737/mcp/VVI+500");
     x737mcp_vvi_down_fast = XPLMFindCommand("x737/mcp/VVI-500");

     x737mcp_app_toggle = XPLMFindCommand("x737/mcp/APP_TOGGLE");

     // x737 LEDs (match x737's custom glareshield and power source)
     x737mcp_hdg_led = XPLMFindDataRef("x737/systems/MCP/LED_HDG_on");
     x737mcp_vorloc_led = XPLMFindDataRef("x737/systems/afds/VORLOC");
     x737mcp_lvlchange_led = XPLMFindDataRef("x737/systems/MCP/LED_LVLCHG_on");
     x737mcp_alt_led = XPLMFindDataRef("x737/systems/MCP/LED_ALTHLD_on");
     x737mcp_vvi_led = XPLMFindDataRef("x737/systems/MCP/LED_VVI_on");
     x737mcp_vvi_armed_led = XPLMFindDataRef("x737/systems/MCP/LED_VVIARMED_on");
     x737mcp_app_led = XPLMFindDataRef("x737/systems/MCP/LED_APP_on");
  } else {
       loaded737 = 0;

       ApAutThr = XPLMFindDataRef("sim/cockpit2/autopilot/autothrottle_enabled");


  }


  return interval;
}


