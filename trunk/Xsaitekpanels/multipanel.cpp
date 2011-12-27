// ****** multipanel.cpp **********
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

// ********************** Multi Panel variables ***********************
static int multinowrite = 0, lastmultiseldis = 0;
static int mulres, multires;

static int multimul = 10;

static int n = 5;

static int appushed = 0;
static int lastappos = 0;
static int aploop = 0;

static int upapalt, upapvs, upapas, upaphdg, upapcrs, neg;
static int flashcnt = 0, flashon = 0;

static int altdbncinc = 0, altdbncdec = 0, vsdbncinc = 0, vsdbncdec = 0;
static int iasdbncinc = 0, iasdbncdec = 0, hdgdbncinc = 0, hdgdbncdec = 0;
static int crsdbncinc = 0, crsdbncdec = 0; 

static float upapaltf, upapvsf, upapasf, upaphdgf, upapcrsf, rhdgf;

static int multiaactv, multiadig1, multiarem1, multiadig2, multiarem2;
static int multiadig3, multiarem3, multiadig4, multiarem4, multiadig5;
static int multibstby, multibdig1, multibdig2, multibrem2;
static int multibdig3, multibrem3, multibdig4, multibrem4, multibdig5;  
static int btnleds = 0, lastbtnleds = 0, multiseldis = 1;

static int ALT_SWITCH = 7, VS_SWITCH = 6;
static int IAS_SWITCH = 5, HDG_SWITCH = 4;
static int CRS_SWITCH = 3, AUTO_THROTTLE_SWITCH = 8;
static int AP_MASTER_BUTTON = 0, HDG_BUTTON = 15;
static int NAV_BUTTON = 14, IAS_BUTTON = 13;
static int ALT_BUTTON = 12, VS_BUTTON = 11;
static int APR_BUTTON = 10, REV_BUTTON = 9;
static int FLAPS_UP_SWITCH = 23, FLAPS_DN_SWITCH = 22;
static int TRIM_WHEEL_UP = 20, TRIM_WHEEL_DN = 21;
static int ADJUSTMENT_UP = 2, ADJUSTMENT_DN = 1;

static unsigned char multibuf[4];
static unsigned char multiwbuf[13];


void process_multi_menu()
{
   //!!! I'm pretty sure this would be much better placed in initialization function of the plugin
    XPLMClearAllMenuItems(MultiMenuId);
    XPLMAppendMenuItem(MultiMenuId, "# OF SWITCH PULSES PER COMMAND", (void *) "VOID", 2);
    XPLMAppendMenuSeparator(MultiMenuId);
    XPLMAppendMenuItem(MultiMenuId, "MULTI KNOB  1 PULSE PER COMMAND", (void *) "1", 2);
    XPLMAppendMenuItem(MultiMenuId, "MULTI KNOB  2 PULSES PER COMMAND", (void *) "2", 2);
    XPLMAppendMenuItem(MultiMenuId, "MULTI KNOB  3 PULSES PER COMMAND", (void *) "3", 2);
    XPLMAppendMenuItem(MultiMenuId, "MULTI KNOB  4 PULSES PER COMMAND", (void *) "4", 2);
    XPLMAppendMenuItem(MultiMenuId, "MULTI KNOB  5 PULSES PER COMMAND", (void *) "5", 2);
    XPLMAppendMenuSeparator(MultiMenuId);
    XPLMAppendMenuItem(MultiMenuId, "TRIM X1", (void *) "TRIM X1", 2);
    XPLMAppendMenuItem(MultiMenuId, "TRIM X2", (void *) "TRIM X2", 2);
    XPLMAppendMenuItem(MultiMenuId, "TRIM X3", (void *) "TRIM X3", 2);

  //!!! I'm adding memory of old setup, so the change is easily detected...
  //!!! Although I'm pretty sure it would be much better to handle this in the menu callback
  //static int last_multispeed = -1;
  //static int last_trimspeed = -1;
  int i;
  
  const int MMENU_BASE = 1;
  const int TMENU_BASE = 7;
  
  //if(multispeed != last_multispeed){
    //last_multispeed = multispeed;
   // XPLMCheckMenuItem(MultiMenuId, multispeed + MMENU_BASE, xplm_Menu_Checked);
    for(i = 1; i <= 5; ++i){
      if(i != multispeed){
        XPLMCheckMenuItem(MultiMenuId, i + MMENU_BASE, xplm_Menu_Unchecked);
      }
      if(i == multispeed){
        XPLMCheckMenuItem(MultiMenuId, i + MMENU_BASE, xplm_Menu_Checked);
      }
    }
  //}
  
  //if(trimspeed != last_trimspeed){
    //last_trimspeed = trimspeed;
    //XPLMCheckMenuItem(MultiMenuId, trimspeed + TMENU_BASE, xplm_Menu_Checked);
    for(i = 1; i <= 3; ++i){
      if(i != trimspeed){
        XPLMCheckMenuItem(MultiMenuId, i + TMENU_BASE, xplm_Menu_Unchecked);
      }
      if(i == trimspeed){
        XPLMCheckMenuItem(MultiMenuId, i + TMENU_BASE, xplm_Menu_Checked);
      }
    }
  //}
}


void process_multi_display()
{
  switch(multiseldis){
    case 1:
    // ***** Setup Display for ALT or VS Switch Position *********
      multiaactv = upapalt;
      multiadig1 = multiaactv/10000, multiarem1 = multiaactv%10000;
      multiadig2 = multiarem1 /1000, multiarem2 = multiarem1%1000;
      multiadig3 = multiarem2/100, multiarem3 = multiarem2%100;
      multiadig4 = multiarem3/10, multiarem4 = multiarem3%10;
      multiadig5 = multiarem4;
      multibstby = upapvs;
      
      if (neg == 1) {
        multibdig1 = 254;
      } else {
        multibdig1 = 15;
      }
      
      multibdig2 = multibstby /1000, multibrem2 = multibstby%1000;
      multibdig3 = multibrem2/100, multibrem3 = multibrem2%100;
      multibdig4 = multibrem3/10, multibrem4 = multibrem3%10;
      multibdig5 = multibrem4;
      break;
    case 2:
    // ***** Setup Display for IAS Switch Position *********
      multiaactv = upapas;
      multiadig1 = 15, multiadig2 = 15 ;
      multiadig3 = multiaactv/100, multiarem3 = multiaactv%100;
      multiadig4 = multiarem3/10, multiarem4 = multiarem3%10;
      multiadig5 = multiarem4;
      break;
    case 3:
    // ***** Setup Display for HDG Switch Position *********
      multiaactv = upaphdg;
      multiadig1 = 15, multiadig2 = 15 ;
      multiadig3 = multiaactv/100, multiarem3 = multiaactv%100;
      multiadig4 = multiarem3/10, multiarem4 = multiarem3%10;
      multiadig5 = multiarem4;
      break;
    case 4:
    // ***** Setup Display for CRS Switch Position *********
      multiaactv = upapcrs;
      multiadig1 = 15, multiadig2 = 15 ;
      multiadig3 = multiaactv/100, multiarem3 = multiaactv%100;
      multiadig4 = multiarem3/10, multiarem4 = multiarem3%10;
      multiadig5 = multiarem4;
      break;
    case 5:
    default:
    // ********** Setup the Display to be Blank *******************
      multiadig1 = 15, multiadig2 = 15, multiadig3 = 15, multiadig4 = 15, multiadig5 = 15;
      multibdig1 = 15, multibdig2 = 15, multibdig3 = 15, multibdig4 = 15, multibdig5 = 15;
      btnleds = 0;
      break;
  }
// ****** Make Message One Digit at A Time and Turn on Button LEDS  *******
  char multiadigit1 = multiadig1, multiadigit2 = multiadig2, multiadigit3 = multiadig3;
  char multiadigit4 = multiadig4, multiadigit5 = multiadig5;
  char multibdigit1 = multibdig1, multibdigit2 = multibdig2, multibdigit3 = multibdig3;
  char multibdigit4 = multibdig4, multibdigit5 = multibdig5;	
  char cdigit1 = btnleds; 

// ****** Load Array with Message of Digits and Button LEDS *************
  multiwbuf[0] = 0;
  multiwbuf[1] = multiadigit1, multiwbuf[2] = multiadigit2, multiwbuf[3] = multiadigit3;
  multiwbuf[4] = multiadigit4, multiwbuf[5] = multiadigit5;
  multiwbuf[6] = multibdigit1, multiwbuf[7] = multibdigit2, multiwbuf[8] = multibdigit3;
  multiwbuf[9] = multibdigit4, multiwbuf[10] = multibdigit5, multiwbuf[11] = cdigit1;

}

void process_alt_switch()
{
// ***************** ALT Switch Position *******************

	if(testbit(multibuf,ALT_SWITCH)) {
          multiseldis = 1;
          upapaltf = XPLMGetDataf(ApAlt);
          upapalt = (int)(upapaltf);
	  if(testbit(multibuf,ADJUSTMENT_UP)) {
            altdbncinc++;
            if (altdbncinc > multispeed) {
                 if(xpanelsfnbutton == 1) {
                    upapalt = upapalt + 1000;
                    upapalt = (upapalt / 1000);
                    upapalt = (upapalt * 1000);
                    altdbncinc = 0;
                }
                if (xpanelsfnbutton == 0) {
                    upapalt = upapalt + 100;
                    upapalt = (upapalt / 100);
                    upapalt = (upapalt * 100);
                    altdbncinc = 0;
               }
            }

          }

          if(testbit(multibuf,ADJUSTMENT_DN)) {
             altdbncdec++;
             if (altdbncdec > multispeed) {
                 if(xpanelsfnbutton == 1) {
                     if (upapalt >= 1000){
                         upapalt = upapalt - 1000;
                     }
                     if(upapalt > 100){
                        upapalt = (upapalt / 100);
                        upapalt = (upapalt * 100);
                     }
                     altdbncdec = 0;
                 }

                 if (xpanelsfnbutton == 0) {
                     if (upapalt >= 100){
                         upapalt = upapalt - 100;
                     }
                     if(upapalt > 100){
                        upapalt = (upapalt / 100);
                        upapalt = (upapalt * 100);
                     }
                     altdbncdec = 0;
                 }

             }
          }

          upapaltf = upapalt;
          XPLMSetDataf(ApAlt, upapaltf);

          upapvsf = XPLMGetDataf(ApVs);
          upapvs = (int)(upapvsf);
          if (upapvs < 0){
              upapvs = (upapvs * -1);
              neg = 1;
          }
          else {
              neg = 0;
          }

	}
}

void process_vs_switch()
{
// ***************** VS Switch Position *******************

	if(testbit(multibuf,VS_SWITCH)) {
          multiseldis = 1;
	  if(testbit(multibuf,ADJUSTMENT_UP)) {
	    vsdbncinc++;
            if (vsdbncinc > multispeed) {
                n = multimul;
                if(xpanelsfnbutton == 1) {
                    while (n>0) {
                        XPLMCommandOnce(ApVsUp);
                        --n;
                    }
                    vsdbncinc = 0;
                }

                if(xpanelsfnbutton == 0) {
                    XPLMCommandOnce(ApVsUp);
                    vsdbncinc = 0;
                }
            }
	  }
	  if(testbit(multibuf,ADJUSTMENT_DN)) {
	    vsdbncdec++;
            if (vsdbncdec > multispeed) {
                n = multimul;
                if(xpanelsfnbutton == 1) {
                    while (n>0) {
                        XPLMCommandOnce(ApVsDn);
                        --n;
                    }
                }
                vsdbncdec = 0;
                if(xpanelsfnbutton == 0) {
                    XPLMCommandOnce(ApVsDn);
                    vsdbncdec = 0;
                }

	    }
	  }
          upapaltf = XPLMGetDataf(ApAlt);
	  upapvsf = XPLMGetDataf(ApVs);
	  upapalt = (int)(upapaltf);
	  upapvs = (int)(upapvsf);
	  if (upapvs < 0){
	    upapvs = (upapvs * -1);
	    neg = 1;
	  }
	  else {
	    neg = 0;
	  }
	}
}

void process_ias_switch()
{
// ***************** IAS Switch Position *******************

        if(testbit(multibuf,IAS_SWITCH)) {
          multiseldis = 2;
          if(testbit(multibuf,ADJUSTMENT_UP)) {
            iasdbncinc++;
            if (iasdbncinc > multispeed) {
                n = multimul;
                if(xpanelsfnbutton == 1) {
                    while (n>0) {
                       XPLMCommandOnce(ApAsUp);
                       --n;
                    }
                    iasdbncinc = 0;
                }
                if(xpanelsfnbutton == 0) {
                    XPLMCommandOnce(ApAsUp);
                    iasdbncinc = 0;
                }
            }
          }
          if(testbit(multibuf,ADJUSTMENT_DN)) {
            iasdbncdec++;
            if (iasdbncdec > multispeed) {
                n = multimul;
                if(xpanelsfnbutton == 1) {
                    while (n>0) {
                       XPLMCommandOnce(ApAsDn);
                       --n;
                    }
                    iasdbncdec = 0;
                 }
                 if(xpanelsfnbutton == 0) {
                     XPLMCommandOnce(ApAsDn);
                     iasdbncdec = 0;
                 }


            }
          }
          upapasf = XPLMGetDataf(ApAs);
          upapas = (int)(upapasf);
        }
}

void process_hdg_switch()
{
// ***************** HDG Switch Position *******************

	if(testbit(multibuf,HDG_SWITCH)) {
          multiseldis = 3;
          upaphdgf = XPLMGetDataf(ApHdg);
          upaphdg = (int)(upaphdgf);
	  if(testbit(multibuf,ADJUSTMENT_UP)) {
	    hdgdbncinc++;
            if (hdgdbncinc > multispeed) {
                 if(xpanelsfnbutton == 1) {
                    upaphdg = upaphdg + multimul;
                    hdgdbncinc = 0;
                 }
                 if(xpanelsfnbutton == 0) {
                    upaphdg = upaphdg + 1;
                    hdgdbncinc = 0;
                 }
	    }
	  }
	  if(testbit(multibuf,ADJUSTMENT_DN)) {
	    hdgdbncdec++;
            if (hdgdbncdec > multispeed) {
                if(xpanelsfnbutton == 1) {
                    upaphdg = upaphdg - multimul;
                    hdgdbncdec = 0;
                }
                if(xpanelsfnbutton == 0) {
                    upaphdg = upaphdg - 1;
                    hdgdbncdec = 0;
                }
	    }
	  }
          if(upaphdg > 360){
              upaphdg = 1;
          }
          if(upaphdg < 0){
              upaphdg = 359;
          }
          upaphdgf = upaphdg;
          XPLMSetDataf(ApHdg, upaphdgf);

	}
}

void process_crs_switch()
{
// ***************** CRS Switch Position *******************

	if(testbit(multibuf,CRS_SWITCH)) {
          multiseldis = 4;
          upapcrsf = XPLMGetDataf(ApCrs);
          upapcrs = (int)(upapcrsf);
	  if(testbit(multibuf,ADJUSTMENT_UP)) {
	    crsdbncinc++;
            if (crsdbncinc > multispeed) {
                 if (xpanelsfnbutton == 1) {
                    upapcrs = upapcrs + multimul;
                    crsdbncinc = 0;
                }
                if(xpanelsfnbutton == 0) {
                    upapcrs = upapcrs + 1;
                    crsdbncinc = 0;
                }
             }
	  }
	  if(testbit(multibuf,ADJUSTMENT_DN)) {
	    crsdbncdec++;
            if (crsdbncdec > multispeed) {
                if (xpanelsfnbutton == 1) {
                    upapcrs = upapcrs - multimul;
                    crsdbncdec = 0;
                }
                if(xpanelsfnbutton == 0) {
                    upapcrs = upapcrs - 1;
                    crsdbncdec = 0;
                }
            }
          }
          if(upapcrs > 360){
              upapcrs = 1;
          }
          if(upapcrs < 0){
              upapcrs = 359;
          }
          upapcrsf = upapcrs;
          XPLMSetDataf(ApCrs, upapcrsf);

	}
}

void process_autothrottle_switch()
{
// ***************** Auto Throttle Switch Position *******************

        if(testbit(multibuf,AUTO_THROTTLE_SWITCH)) {
          if (loaded737 == 1){
             XPLMSetDatai(x737athr_armed, 1);
          }else{
             XPLMCommandOnce(ApAutThrOn);
          }
        }
        else {
           if (loaded737 == 1){
             XPLMSetDatai(x737athr_armed, 0);
           }else{
             XPLMCommandOnce(ApAutThrOff);
           }
         }
}

void process_ap_master_switch()
{
// ***************** AP Master Button and light *******************

        if (appushed == 0) {
          switch(XPLMGetDatai(ApMstrStat)){
            case 0:
             if(testbit(multibuf, AP_MASTER_BUTTON)) {
                XPLMSetDatai(ApMstrStat, 1);
                appushed = 1;
                lastappos = 1;
              }
              break;
            case 1:
             if(testbit(multibuf, AP_MASTER_BUTTON)) {
                 if (lastappos == 1){
                   XPLMSetDatai(ApMstrStat, 2);
                   appushed = 1;
                 }
                 if (lastappos == 2){
                     XPLMSetDatai(ApMstrStat, 0);
                     appushed = 1;
                 }
              }
              break;
            case 2:
              if(testbit(multibuf, AP_MASTER_BUTTON)) {
                 XPLMSetDatai(ApMstrStat, 1);
                 appushed = 1;
                 lastappos = 2;
              }
              break;
          }
        }

        if (appushed == 1){
           aploop++;
            if (aploop == 50){
               appushed = 0;
               aploop = 0;
            }

        }
        
        switch(XPLMGetDatai(ApMstrStat)){
          case 0:
            btnleds &= ~(1<<0);   // * clear bit 0 in btnleds to 0 *
            break;
          case 1:
	    if (flashon == 1) {
              btnleds |= (1<<0);   // * set bit 0 in btnleds to 1 *
	    }else{   
              btnleds &= ~(1<<0);   // * clear bit 0 in btnleds to 0 *
	    }
            break;
          case 2:
            btnleds |= (1<<0);   // * set bit 0 in btnleds to 1 *
            break;
        }
}

void process_hdg_button()
{
// ***************** HDG Button and light *******************

        if (multires > 0) {
          if(testbit(multibuf,HDG_BUTTON)) {

            if(xpanelsfnbutton == 1) {
                rhdgf = XPLMGetDataf(MHdg);
                XPLMSetDataf(ApHdg, rhdgf);
            }
            if(xpanelsfnbutton == 0) {
                XPLMCommandOnce(ApHdgBtn);
                lastappos = 1;
            }
          }
        }
        
        switch(XPLMGetDatai(ApHdgStat)){
          case 2:
            btnleds |= (1<<1);   // * set bit 1 in btnleds to 1 *
            break;
          case 1:
            if (flashon == 1) {
              btnleds |= (1<<1);   // * set bit 1 in btnleds to 1 *
	    }else{   
              btnleds &= ~(1<<1);   // * clear bit 1 in btnleds to 0 *
	    }
            break;
          case 0:
            btnleds &= ~(1<<1);   // * clear bit 1 in btnleds to 0 *
            break;
        }
	if (XPLMGetDatai(ApMstrStat) == 0) {
          btnleds &= ~(1<<1);   // * clear bit 1 in btnleds to 0 *
	}
}

void process_nav_button()
{
// ***************** NAV Button and light *******************

        if (multires > 0) {
          if(testbit(multibuf,NAV_BUTTON)) {
            XPLMCommandOnce(ApNavBtn);
            lastappos = 1;
          }
        }

	switch(XPLMGetDatai(ApNavStat)){
	  case 2:
            btnleds |= (1<<2);   // * set bit 2 in btnleds to 1 *
            break;
          case 1:
	    if (flashon == 1) {
              btnleds |= (1<<2);   // * set bit 1 in btnleds to 1 *
	    }else{
              btnleds &= ~(1<<2);   // * clear bit 2 in btnleds to 0 *
	    }
	    break;
	  case 0:
            btnleds &= ~(1<<2);   // * clear bit 2 in btnleds to 0 *
	    break;
	}
}

void process_ias_button()
{
// ***************** IAS Button and light *******************

        if (multires > 0) {
          if(testbit(multibuf,IAS_BUTTON)) {
            XPLMCommandOnce(ApIasBtn);
            lastappos = 1;
          }
        }
        switch(XPLMGetDatai(ApIasStat)){
          case 2:
            btnleds |= (1<<3);   // * set bit 3 in btnleds to 1 *
            break;
          case 1:
            if (flashon == 1) {
              btnleds |= (1<<3);   // * set bit 3 in btnleds to 1 *
            }
            if (flashon == 0) {
              btnleds &= ~(1<<3);   // * clear bit 3 in btnleds to 0 *
            }
            break;
          case 0:
            btnleds &= ~(1<<3);   // * clear bit 3 in btnleds to 0 *
            break;
        }
}

void process_alt_button()
{
// ***************** ALT Button and light *******************

        if (multires > 0) {
          if(testbit(multibuf,ALT_BUTTON)) {
           XPLMCommandOnce(ApAltBtn);
           lastappos = 1;
          }
        }
	switch(XPLMGetDatai(ApAltStat)){
	  case 2:
            btnleds |= (1<<4);   // * set bit 4 in btnleds to 1 *
            break;
          case 1:
	    if (flashon == 1) {
              btnleds |= (1<<4);   // * set bit 4 in btnleds to 1 *
	    }else{    
              btnleds &= ~(1<<4);   // * clear bit 4 in btnleds to 0 *
	    }
            break;
          case 0:
            btnleds &= ~(1<<4);   // * clear bit 4 in btnleds to 0 *
            break;
	} 
	
	if (XPLMGetDatai(ApMstrStat) == 0) {
          btnleds &= ~(1<<4);   // * clear bit 4 in btnleds to 0 *
	}
}

void process_vs_button()
{
// ***************** VS Button and light *******************

        if (multires > 0) {
          if(testbit(multibuf,VS_BUTTON)) {
            XPLMCommandOnce(ApVsBtn);
            lastappos = 1;
          }
        }
	switch(XPLMGetDatai(ApVsStat)){
	  case 2:
            btnleds |= (1<<5);   // * set bit 5 in btnleds to 1 *
            break;
          case 1:
	    if (flashon == 0) {
              btnleds |= (1<<5);   // * set bit 5 in btnleds to 1 *
	    }else{   
              btnleds &= ~(1<<5);   // * clear bit 5 in btnleds to 0 *
	    }
            break;
          case 0:
            btnleds &= ~(1<<5);   // * clear bit 5 in btnleds to 0 *
            break;
	} 
}

void process_apr_button()
{
// ***************** APR Button and light *******************

        if (multires > 0) {
          if(testbit(multibuf,APR_BUTTON)) {
            if(xpanelsfnbutton == 1) {
                rhdgf = XPLMGetDataf(MHdg);
                XPLMSetDataf(ApCrs, rhdgf);
            }
            if(xpanelsfnbutton == 0) {
                XPLMCommandOnce(ApAprBtn);
                lastappos = 1;
            }
          }
        }
	switch(XPLMGetDatai(ApAprStat)){
	  case 2:
            btnleds |= (1<<6);   // * set bit 6 in btnleds to 1 *
            break;
          case 1:
	    if (flashon == 1) {
              btnleds |= (1<<6);   // * set bit 6 in btnleds to 1 *
	    }else{   
              btnleds &= ~(1<<6);   // * clear bit 6 in btnleds to 0 *
	    }
            break;
          case 0:
            btnleds &= ~(1<<6);   // * clear bit 6 in btnleds to 0 *
            break;
	} 
}

void process_rev_button()
{
// ***************** REV Button and light *******************

        if (multires > 0) {
          if(testbit(multibuf,REV_BUTTON)) {
            XPLMCommandOnce(ApRevBtn);
            lastappos = 1;
          }
        }
	switch(XPLMGetDatai(ApRevStat)){
	  case 2:
            btnleds |= (1<<7);   // * set bit 7 in btnleds to 1 *
            break;
          case 1:
	    if (flashon == 1) {
              btnleds |= (1<<7);   // * set bit 7 in btnleds to 1 *
	    }else{    
              btnleds &= ~(1<<7);   // * clear bit 7 in btnleds to 0 *
	    }
            break;
          case 0:
            btnleds &= ~(1<<7);   // * clear bit 7 in btnleds to 0 *
            break;
	} 
}

void process_flaps_switch()
{
// ***************** Flaps Switch *******************

        if (multires > 0) {
	  if(testbit(multibuf,FLAPS_UP_SWITCH)) {
	    XPLMCommandOnce(FlapsUp);	 
          }
	  if(testbit(multibuf,FLAPS_DN_SWITCH)) {
	    XPLMCommandOnce(FlapsDn);	 
	  }
        }
}

void process_trim_wheel()
{
// *************** Trim Wheel *********************
  int i;
        if (multires > 0) {
	  if(testbit(multibuf,TRIM_WHEEL_UP)) {
	    for(i = 0; i < trimspeed; ++i){
              XPLMCommandOnce(PitchTrimUp);
	    }
	  }	
	  if(testbit(multibuf,TRIM_WHEEL_DN)) {
	    for(i = 0; i < trimspeed; ++i){
              XPLMCommandOnce(PitchTrimDn);
            }
	  }
	}
}

void process_multi_flash()
{
// ***************** Flasher for Button LEDS *******************

flashcnt++;
if (flashcnt < 50) {
  flashon = 0;
}
if (flashcnt > 50) {
  flashon = 1;
}
if (flashcnt == 100) {
  flashcnt = 0;
}
}

void process_multi_blank_display()
{
// ***************** Blank Display *******************

if (XPLMGetDatai(AvPwrOn) == 0) {
  multiseldis = 5;
}
if (XPLMGetDatai(BatPwrOn) == 0) {
  multiseldis = 5;
}

}


// ***** Multi Panel Process ******
void process_multi_panel()

{
  process_multi_menu();

// ******* Only do a read if something new to be read ********
  hid_set_nonblocking(multihandle, 1);
  int multi_safety_cntr = 30;
  do{
    multires = hid_read(multihandle, multibuf, sizeof(multibuf));
    
    process_alt_switch();
    process_vs_switch();
    process_ias_switch();
    process_hdg_switch();
    process_crs_switch();
    process_autothrottle_switch();
    process_ap_master_switch();
    process_hdg_button();
    process_nav_button();
    process_ias_button();
    process_alt_button();
    process_vs_button();
    process_apr_button();
    process_rev_button();
    process_flaps_switch();
    process_trim_wheel();
    if(multires > 0){
       process_multi_flash();
       process_multi_blank_display();
       process_multi_display();
       hid_send_feature_report(multihandle, multiwbuf, sizeof(multiwbuf));
    }
    --multi_safety_cntr;
  }while((multires > 0) && (multi_safety_cntr > 0));

  process_multi_flash();
  process_multi_blank_display();
  process_multi_display();
  
// ******* Write on changes or timeout ********
  if ((lastmultiseldis != multiseldis) || (lastbtnleds != btnleds) || (multinowrite > 50)) {
      mulres = hid_send_feature_report(multihandle, multiwbuf, sizeof(multiwbuf));
      multinowrite = 1;
      lastmultiseldis = multiseldis;
      lastbtnleds = btnleds;
  }else{
      multinowrite++;
  }
  return;
}
