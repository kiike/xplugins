#ifndef SAITEKPANELS_H
#define SAITEKPANELS_H

#include "XPLMUtilities.h"
#include "XPLMMenus.h"
#include "XPWidgets.h"
#include "XPStandardWidgets.h"

#include "hidapi.h"

#include <string>

using namespace std;

// ****************  Start Radio Panel Section  ***************************

// ************** Radio Panel SDK Command Ref **********************
extern XPLMCommandRef Com1StbyFineDn, Com1StbyFineUp, Com1StbyCorseDn, Com1StbyCorseUp;
extern XPLMCommandRef Com2StbyFineDn, Com2StbyFineUp, Com2StbyCorseDn, Com2StbyCorseUp;
extern XPLMCommandRef Nav1StbyFineDn, Nav1StbyFineUp, Nav1StbyCorseDn, Nav1StbyCorseUp;
extern XPLMCommandRef Nav2StbyFineDn, Nav2StbyFineUp, Nav2StbyCorseDn, Nav2StbyCorseUp;

extern XPLMCommandRef Afd1StbyHunUp, Afd1StbyHunDn, Afd1StbyTensUp, Afd1StbyTensDn;
extern XPLMCommandRef Afd1StbyOnesUp, Afd1StbyOnesDn;

extern XPLMCommandRef Afd2StbyHunUp, Afd2StbyHunDn, Afd2StbyTensUp, Afd2StbyTensDn;
extern XPLMCommandRef Afd2StbyOnesUp, Afd2StbyOnesDn;

extern XPLMCommandRef XpdrThUp, XpdrThDn, XpdrHunUp, XpdrHunDn;
extern XPLMCommandRef XpdrTensUp, XpdrTensDn, XpdrOnesUp, XpdrOnesDn;
extern XPLMCommandRef BaroUp, BaroDn, BaroStd;

extern XPLMCommandRef Com1ActStby, Com2ActStby, Nav1ActStby, Nav2ActStby;
extern XPLMCommandRef Adf1ActStby, Adf2ActStby;

// ************** Radio Panel SDK Data Ref **********************
extern XPLMDataRef Com1ActFreq, Com2ActFreq, Nav1ActFreq, Nav2ActFreq;
extern XPLMDataRef Com1StbyFreq, Com2StbyFreq, Nav1StbyFreq, Nav2StbyFreq;

extern XPLMDataRef Adf1StbyFreq, Adf2StbyFreq;
extern XPLMDataRef Adf1ActFreq, Adf2ActFreq;

extern XPLMDataRef XpdrCode, XpdrMode, BaroSetting;
extern XPLMDataRef DmeMode, DmeSlvSource;
extern XPLMDataRef Nav1DmeNmDist, Nav1DmeSpeed;
extern XPLMDataRef Nav2DmeNmDist, Nav2DmeSpeed;
extern XPLMDataRef DmeFreq, DmeTime, DmeSpeed;

extern XPLMDataRef AvPwrOn, BatPwrOn;

extern XPLMDataRef Nav1PwrOn, Nav2PwrOn, Com1PwrOn, Com2PwrOn;
extern XPLMDataRef Afd1PwrOn, DmePwrOn;

extern XPLMMenuID      RadioMenu;
extern XPLMMenuID      RadioMenuId;


// ************************ Radio Panel variables **************************
extern int radio0fd, radio1fd, radio2fd, radio3fd, radio4fd, radio5fd, radio6fd, radio7fd;
extern int radio8fd, radio9fd, radio10fd, radio11fd, radio12fd, radio13fd, radio14fd, radio15fd;
extern int radiofd[4], radcnt, radres;
extern int radspeed;
extern float interval;
extern unsigned char radbuf[4], radwbuf[21];

extern hid_device *radiohandle[4];

// ****************  End Radio Panel Section  ***************************

// *************************************************************************

// ****************  Start Multi Panel Section  ***************************

// ********** Multi Panel Command Ref ***************
extern XPLMCommandRef ApAltDn, ApAltUp, ApVsDn, ApVsUp;
extern XPLMCommandRef ApAsDn, ApAsUp, ApHdgDn, ApHdgUp;
extern XPLMCommandRef ApCrsDn, ApCrsUp, ApCrsDn2, ApCrsUp2;

extern XPLMCommandRef ApMstrBtnUp, ApMstrBtnDn, ApMstrBtnOff;
extern XPLMCommandRef ApHdgBtn, ApNavBtn, ApAltBtn;
extern XPLMCommandRef ApIasBtn;
extern XPLMCommandRef ApVsBtn, ApAprBtn, ApRevBtn;

extern XPLMCommandRef FlapsDn, FlapsUp;
extern XPLMCommandRef PitchTrimDn, PitchTrimUp, PitchTrimTkOff;

extern XPLMCommandRef x737mcp_cmd_a_toggle;
extern XPLMCommandRef x737mcp_hdg_up, x737mcp_hdg_down;
extern XPLMCommandRef x737mcp_hdg_up_fast, x737mcp_hdg_down_fast;
extern XPLMCommandRef x737mcp_spd_up, x737mcp_spd_down;
extern XPLMCommandRef x737mcp_spd_up_fast, x737mcp_spd_down_fast;
extern XPLMCommandRef x737mcp_alt_up, x737mcp_alt_down;
extern XPLMCommandRef x737mcp_alt_up_fast, x737mcp_alt_down_fast;
extern XPLMCommandRef x737mcp_vvi_up, x737mcp_vvi_down;
extern XPLMCommandRef x737mcp_vvi_up_fast, x737mcp_vvi_down_fast;

extern XPLMCommandRef x737mcp_hdg_toggle, x737mcp_vorloc_toggle;
extern XPLMCommandRef x737mcp_lvlchange_toggle, x737mcp_alt_toggle;
extern XPLMCommandRef x737mcp_vs_toggle, x737mcp_app_toggle;
extern XPLMCommandRef x737mcp_spd_changeover, x737mcp_lnav_toggle;

// ************ Multi Panel Data Ref *****************
extern XPLMDataRef ApAlt, ApVs, ApAs, ApHdg, ApCrs, ApCrs2;

extern XPLMDataRef ApMstrStat, ApHdgStat, ApNavStat, ApIasStat;
extern XPLMDataRef ApAltStat, ApVsStat, ApAprStat, ApRevStat;
extern XPLMDataRef x737athr_armed ,x737swBatBus ,x737stbyPwr ;
extern XPLMDataRef ApState, ApAutThr;
extern XPLMDataRef AirspeedIsMach, Airspeed;

extern XPLMDataRef x737mcp_hdg, x737mcp_alt, x737mcp_vs, x737mcp_vs_arm;

extern XPLMDataRef x737mcp_cmd_a_led;
extern XPLMDataRef x737mcp_hdg_led, x737mcp_vorloc_led, x737mcp_vorloc_armed;
extern XPLMDataRef x737mcp_lnav, x737mcp_lnav_armed;
extern XPLMDataRef x737mcp_lvlchange_led, x737mcp_alt_led;
extern XPLMDataRef x737mcp_vvi_led, x737mcp_vvi_armed_led, x737mcp_app_led;
extern XPLMDataRef HsiSelector;

extern XPLMMenuID      MultiMenu;
extern XPLMMenuID      MultiMenuId;
extern XPLMMenuID      Frp;
extern XPLMMenuID      MHdg;

// **************** Multi Panel variables *******************
extern int multifd;
extern int numadf;
extern int loaded737;
extern int trimspeed, multispeed, autothrottleswitchenable;
extern int xpanelsfnbutton;
extern int xpanelscrstoggle;
extern int multi_auto_mul;
extern hid_device *multihandle;

// ****************  End Multi Panel Section  ***************************

// **************************************************************************

// ****************  Start Switch Panel Section  ***************************

// ************ Switch Panel Command Ref ***************
extern XPLMCommandRef ClFlOpn, ClFlCls, PtHtOn, PtHtOff;
extern XPLMCommandRef PtHt0On, PtHt0Off, PtHt1On, PtHt1Off;

extern XPLMCommandRef AvLtOn, AvLtOff, BatOn, BatOff;
extern XPLMCommandRef LnLtOn, LnLtOff, TxLtOn, TxLtOff;
extern XPLMCommandRef StLtOn, StLtOff, NvLtOn, NvLtOff;
extern XPLMCommandRef BcLtOn, BcLtOff, GearUp, GearDn;

extern XPLMCommandRef MagOff1, MagOff2, MagOff3, MagOff4;
extern XPLMCommandRef MagOff5, MagOff6, MagOff7, MagOff8;

extern XPLMCommandRef MagLeft1, MagLeft2, MagLeft3, MagLeft4;
extern XPLMCommandRef MagLeft5, MagLeft6, MagLeft7, MagLeft8;

extern XPLMCommandRef MagRight1, MagRight2, MagRight3, MagRight4;
extern XPLMCommandRef MagRight5, MagRight6, MagRight7, MagRight8;

extern XPLMCommandRef MagBoth1, MagBoth2, MagBoth3, MagBoth4;
extern XPLMCommandRef MagBoth5, MagBoth6, MagBoth7, MagBoth8;

extern XPLMCommandRef EngStart1, EngStart2, EngStart3, EngStart4;
extern XPLMCommandRef EngStart5, EngStart6, EngStart7, EngStart8;

extern XPLMCommandRef BatOn1, BatOn2, BatOff1, BatOff2;

extern XPLMCommandRef GenOn1, GenOn2, GenOn3, GenOn4;
extern XPLMCommandRef GenOn5, GenOn6, GenOn7, GenOn8;

extern XPLMCommandRef GenOff1, GenOff2, GenOff3, GenOff4;
extern XPLMCommandRef GenOff5, GenOff6, GenOff7, GenOff8;

extern XPLMCommandRef FuelPumpOn1, FuelPumpOn2, FuelPumpOn3, FuelPumpOn4;
extern XPLMCommandRef FuelPumpOn5, FuelPumpOn6, FuelPumpOn7, FuelPumpOn8;

extern XPLMCommandRef FuelPumpOff1, FuelPumpOff2, FuelPumpOff3, FuelPumpOff4;
extern XPLMCommandRef FuelPumpOff5, FuelPumpOff6, FuelPumpOff7, FuelPumpOff8;

extern XPLMCommandRef x737gen1_on, x737gen2_on;
extern XPLMCommandRef x737gen1_off, x737gen2_off;

extern XPLMCommandRef x737ice_pitot1_on, x737ice_pitot2_on;
extern XPLMCommandRef x737ice_pitot1_off, x737ice_pitot2_off;

extern XPLMCommandRef x737ice_engine1_on, x737ice_engine2_on;
extern XPLMCommandRef x737ice_engine1_off, x737ice_engine2_off;
extern XPLMCommandRef x737ice_wing_on, x737ice_wing_off;

// ************** Switch Panel Data Ref ******************
extern XPLMDataRef BatNum, GenNum, EngNum;
extern XPLMDataRef BatArrayOnDR;

extern XPLMDataRef CowlFlaps, CockpitLights, AntiIce;
extern XPLMDataRef GearRetract, OnGround, LandingGearStatus;
extern XPLMDataRef Gear1Fail, Gear2Fail, Gear3Fail;

extern XPLMMenuID      SwitchMenu;
extern XPLMMenuID      SwitchMenuId;

// ***************** Switch Panel variables ********************
extern int switchfd;
extern int bataltinverse, cowlflapsenable, landinggearknobenable;
extern float LandingGearDeployRatio[10];
extern hid_device *switchhandle;



// ************** Bip Panel Data Ref ******************
extern XPLMDataRef gTimeSimIsRunningXDataRef;

extern XPLMMenuID      BipMenu;
extern XPLMMenuID      BipMenuId, Bip2MenuId, Bip3MenuId, Bip4MenuId;
extern XPWidgetID      BipWidgetID, Bip2WidgetID, Bip3WidgetID, Bip4WidgetID;

// ***************** Bip Panel variables ********************
extern hid_device *biphandle[4];
extern int bipcnt;

void WriteCSVTableToDisk(void);
bool ReadConfigFile(string PlaneICAO);

// ***************** Saitek Panel variables ********************

extern int wrgXPlaneVersion;
extern int wrgXPLMVersion;
extern int wrgHostID;


#endif
