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

extern XPLMCommandRef Com1ActStby, Com2ActStby, Nav1ActStby, Nav2ActStby;
extern XPLMCommandRef Adf1ActStby, Adf2ActStby;

// ************** Radio Panel SDK Data Ref **********************
extern XPLMDataRef Com1ActFreq, Com2ActFreq, Nav1ActFreq, Nav2ActFreq;
extern XPLMDataRef Com1StbyFreq, Com2StbyFreq, Nav1StbyFreq, Nav2StbyFreq;

extern XPLMDataRef Adf1StbyFreq, Adf2StbyFreq;
extern XPLMDataRef Adf1ActFreq, Adf2ActFreq;

extern XPLMDataRef XpdrCode;
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
extern int freqspeed;
extern float interval;
extern unsigned char radbuf[4], radwbuf[21];

extern hid_device *radhandle[4];

// ****************  End Radio Panel Section  ***************************

// *************************************************************************

// ****************  Start Multi Panel Section  ***************************

// ********** Multi Panel Command Ref ***************
extern XPLMCommandRef ApAltDn, ApAltUp, ApVsDn, ApVsUp;
extern XPLMCommandRef ApAsDn, ApAsUp, ApHdgDn, ApHdgUp;
extern XPLMCommandRef ApCrsDn, ApCrsUp;

extern XPLMCommandRef ApMstrBtnUp, ApMstrBtnDn, ApMstrBtnOff;
extern XPLMCommandRef ApHdgBtn, ApNavBtn, ApAltBtn;
extern XPLMCommandRef ApIasBtn;
extern XPLMCommandRef ApVsBtn, ApAprBtn, ApRevBtn;

extern XPLMCommandRef ApAutThrOn, ApAutThrOff, FlapsDn, FlapsUp;
extern XPLMCommandRef PitchTrimDn, PitchTrimUp, PitchTrimTkOff;


// ************ Multi Panel Data Ref *****************
extern XPLMDataRef ApAlt, ApVs, ApAs, ApHdg, ApCrs;

extern XPLMDataRef ApMstrStat, ApHdgStat, ApNavStat, ApIasStat;
extern XPLMDataRef ApAltStat, ApVsStat, ApAprStat, ApRevStat;
extern XPLMDataRef x737athr_armed ,x737swBatBus ,x737stbyPwr ;
extern XPLMDataRef ApState;
extern XPLMMenuID      MultiMenu;
extern XPLMMenuID      MultiMenuId;

// **************** Multi Panel variables *******************
extern int multifd;
extern int trimspeed, numadf;
extern int loaded737;
extern hid_device *multihandle;

// ****************  End Multi Panel Section  ***************************

// **************************************************************************

// ****************  Start Switch Panel Section  ***************************

// ************ Switch Panel Command Ref ***************
extern XPLMCommandRef ClFlOpn, ClFlCls, PtHtOn, PtHtOff;
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

// ************** Switch Panel Data Ref ******************
extern XPLMDataRef BatNum, GenNum, EngNum;
extern XPLMDataRef BatArrayOnDR;

extern XPLMDataRef CowlFlaps, CockpitLights, AntiIce;
extern XPLMDataRef GearRetract, OnGround, LandingGearStatus;
extern XPLMDataRef Gear1Fail, Gear2Fail, Gear3Fail;

// ***************** Switch Panel variables ********************
extern int switchfd;
extern float LandingGearDeployRatio[10];
extern hid_device *switchhandle;



// ************** Bip Panel Data Ref ******************
extern XPLMDataRef gTimeSimIsRunningXDataRef;

extern XPLMMenuID      BipMenu;
extern XPLMMenuID      BipMenuId;
extern XPWidgetID      BipWidgetID;

// ***************** Bip Panel variables ********************
extern hid_device *biphandle;

void WriteCSVTableToDisk(void);
bool ReadConfigFile(string PlaneICAO);


#endif
