#ifndef SAITEKPANELS_H
#define SAITEKPANELS_H

#include "XPLMUtilities.h"

#include "hidapi.h"

/****************  Start Radio Panel Section  ***************************/

/************** Radio Panel SDK Command Ref **********************/
extern XPLMCommandRef Com1StbyFineDn, Com1StbyFineUp, Com1StbyCorseDn, Com1StbyCorseUp;
extern XPLMCommandRef Com2StbyFineDn, Com2StbyFineUp, Com2StbyCorseDn, Com2StbyCorseUp;
extern XPLMCommandRef Nav1StbyFineDn, Nav1StbyFineUp, Nav1StbyCorseDn, Nav1StbyCorseUp;
extern XPLMCommandRef Nav2StbyFineDn, Nav2StbyFineUp, Nav2StbyCorseDn, Nav2StbyCorseUp;

extern XPLMCommandRef Afd1HunUp, Afd1HunDn, Afd1TensUp, Afd1TensDn;
extern XPLMCommandRef Afd1OnesUp, Afd1OnesDn;

extern XPLMCommandRef XpdrThUp, XpdrThDn, XpdrHunUp, XpdrHunDn;
extern XPLMCommandRef XpdrTensUp, XpdrTensDn, XpdrOnesUp, XpdrOnesDn;

extern XPLMCommandRef Com1ActStby, Com2ActStby, Nav1ActStby, Nav2ActStby;

/************** Radio Panel SDK Data Ref **********************/
extern XPLMDataRef Com1ActFreq, Com2ActFreq, Nav1ActFreq, Nav2ActFreq;
extern XPLMDataRef Com1StbyFreq, Com2StbyFreq, Nav1StbyFreq, Nav2StbyFreq;

extern XPLMDataRef XpdrCode, AdfFreq, Nav1DmeNmDist, Nav1DmeSpeed;
extern XPLMDataRef Nav2DmeNmDist, Nav2DmeSpeed, DmeSlvSource;

extern XPLMDataRef AvPwrOn, BatPwrOn;

extern XPLMDataRef Nav1PwrOn, Nav2PwrOn, Com1PwrOn, Com2PwrOn;
extern XPLMDataRef Afd1PwrOn, DmePwrOn;

/************************ Radio Panel variables **************************/
extern int radio0fd, radio1fd, radio2fd, radio3fd, radio4fd, radio5fd, radio6fd, radio7fd;
extern int radio8fd, radio9fd, radio10fd, radio11fd, radio12fd, radio13fd, radio14fd, radio15fd;
extern int radiofd[4], radcnt;
extern float interval;

/****************  End Radio Panel Section  ***************************/

/*************************************************************************/

/****************  Start Multi Panel Section  ***************************/

/********** Multi Panel Command Ref ***************/
extern XPLMCommandRef ApAltDn, ApAltUp, ApVsDn, ApVsUp;
extern XPLMCommandRef ApAsDn, ApAsUp, ApHdgDn, ApHdgUp;
extern XPLMCommandRef ApCrsDn, ApCrsUp;

extern XPLMCommandRef ApMstrBtnUp, ApMstrBtnDn, ApMstrBtnOff;
extern XPLMCommandRef ApHdgBtn, ApNavBtn, ApIasBtn, ApAltBtn;
extern XPLMCommandRef ApVsBtn, ApAprBtn, ApRevBtn;

extern XPLMCommandRef ApAutThrOn, ApAutThrOff, FlapsDn, FlapsUp;
extern XPLMCommandRef PitchTrimDn, PitchTrimUp, PitchTrimTkOff;


/************ Multi Panel Data Ref *****************/
extern XPLMDataRef ApAlt, ApVs, ApAs, ApHdg, ApCrs;

extern XPLMDataRef ApMstrStat, ApHdgStat, ApNavStat, ApIasStat;
extern XPLMDataRef ApAltStat, ApVsStat, ApAprStat, ApRevStat;
extern XPLMDataRef x737athr_armed;

/**************** Multi Panel variables *******************/
extern int multifd;
extern hid_device *multihandle;

/****************  End Multi Panel Section  ***************************/

/**************************************************************************/

/****************  Start Switch Panel Section  ***************************/

/************ Switch Panel Command Ref ***************/
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

/************** Switch Panel Data Ref ******************/
extern XPLMDataRef BatNum, GenNum, EngNum;
extern XPLMDataRef BatArrayOnDR;

extern XPLMDataRef CowlFlaps, CockpitLights, AntiIce;
extern XPLMDataRef GearRetract, OnGround, x737swBatBus;

/***************** Switch Panel variables ********************/
extern int switchfd;

extern int loaded737;


#endif
