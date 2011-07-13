/****** multipanel.cpp **********/
/****  William R. Good  ********/ 


#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"

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

/********************** Multi Panel variables ***********************/
static int multinowrite = 0, lastmultiseldis = 0;
static int mulres, multires;

static int upapalt, upapvs, upapas, upaphdg, upapcrs, neg;
static int flashcnt = 0, flashon = 0, apbtncnt = 0;

static int altdbncinc = 0, altdbncdec = 0, vsdbncinc = 0, vsdbncdec = 0;
static int iasdbncinc = 0, iasdbncdec = 0, hdgdbncinc = 0, hdgdbncdec = 0;
static int crsdbncinc = 0, crsdbncdec = 0; 

static float upapaltf, upapvsf, upapasf, upaphdgf, upapcrsf;  

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

static unsigned char multibuf[3];
static unsigned char multiwbuf[12];


/***** Multi Panel Process ******/
void process_multi_panel()

{

/***** Setup Display for ALT or VS Switch Position *********/
if (multiseldis == 1) { 

  multiaactv = upapalt;
  multiadig1 = multiaactv/10000, multiarem1 = multiaactv%10000;
  multiadig2 = multiarem1 /1000, multiarem2 = multiarem1%1000;
  multiadig3 = multiarem2/100, multiarem3 = multiarem2%100;
  multiadig4 = multiarem3/10, multiarem4 = multiarem3%10;
  multiadig5 = multiarem4;
  multibstby = upapvs;

  if (neg == 1) {
    multibdig1 = 254;
  }
  else {
    multibdig1 = 11;
  }

  multibdig2 = multibstby /1000, multibrem2 = multibstby%1000;
  multibdig3 = multibrem2/100, multibrem3 = multibrem2%100;
  multibdig4 = multibrem3/10, multibrem4 = multibrem3%10;
  multibdig5 = multibrem4;

  }

/***** Setup Display for IAS Switch Position *********/
if (multiseldis == 2) { 
  multiaactv = upapas;
  multiadig1 = 11, multiadig2 = 11 ;
  multiadig3 = multiaactv/100, multiarem3 = multiaactv%100;
  multiadig4 = multiarem3/10, multiarem4 = multiarem3%10;
  multiadig5 = multiarem4;

  }

/***** Setup Display for HDG Switch Position *********/
if (multiseldis == 3) { 
  multiaactv = upaphdg;
  multiadig1 = 11, multiadig2 = 11 ;
  multiadig3 = multiaactv/100, multiarem3 = multiaactv%100;
  multiadig4 = multiarem3/10, multiarem4 = multiarem3%10;
  multiadig5 = multiarem4;

  }

/***** Setup Display for CRS Switch Position *********/
if (multiseldis == 4) { 
  multiaactv = upapcrs;
  multiadig1 = 11, multiadig2 = 11 ;
  multiadig3 = multiaactv/100, multiarem3 = multiaactv%100;
  multiadig4 = multiarem3/10, multiarem4 = multiarem3%10;
  multiadig5 = multiarem4;

  }

/********** Setup the Display to be Blank *******************/
if (multiseldis == 5) { 
  
  multiadig1 = 11, multiadig2 = 11, multiadig3 = 11, multiadig4 = 11, multiadig5 = 11;
  multibdig1 = 11, multibdig2 = 11, multibdig3 = 11, multibdig4 = 11, multibdig5 = 11;
  btnleds = 0;

  }

/****** Make Message One Digit at A Time and Turn on Button LEDS  *******/ 
  char multiadigit1 = multiadig1, multiadigit2 = multiadig2, multiadigit3 = multiadig3;
  char multiadigit4 = multiadig4, multiadigit5 = multiadig5;
  char multibdigit1 = multibdig1, multibdigit2 = multibdig2, multibdigit3 = multibdig3;
  char multibdigit4 = multibdig4, multibdigit5 = multibdig5;	
  char cdigit1 = btnleds; 

/****** Load Array with Message of Digits and Button LEDS *************/
  multiwbuf[0] = 0;
  multiwbuf[1] = multiadigit1, multiwbuf[2] = multiadigit2, multiwbuf[3] = multiadigit3;
  multiwbuf[4] = multiadigit4, multiwbuf[5] = multiadigit5;
  multiwbuf[6] = multibdigit1, multiwbuf[7] = multibdigit2, multiwbuf[8] = multibdigit3;
  multiwbuf[9] = multibdigit4, multiwbuf[10] = multibdigit5, multiwbuf[11] = cdigit1;


/******* Only do a read if something new to be read ********/

  hid_set_nonblocking(multihandle, 1);
  multires = hid_read(multihandle, multibuf, sizeof(multibuf));
  if (multires > 0) {
      mulres = hid_send_feature_report(multihandle, multiwbuf, 12);
      multinowrite = 1;
  }



 /**** Trying to only write on changes ****/
  if (lastmultiseldis == multiseldis) {
  }
  else {
    if (multinowrite == 1) {
    }
    else {
      mulres = hid_send_feature_report(multihandle, multiwbuf, 12);
      multinowrite = 1;
      lastmultiseldis = multiseldis;
    }
  }

  if (lastbtnleds == btnleds) {
  }
  else {
    if (multinowrite == 1) {
    }
    else {
      mulres = hid_send_feature_report(multihandle, multiwbuf, 12);
      multinowrite = 1;
      lastbtnleds = btnleds;
    }
  }


  if (multinowrite == 50) {
      mulres = hid_send_feature_report(multihandle, multiwbuf, 12);
      multinowrite = 0;
  }
  else {
    multinowrite++;
  }


/***************** ALT Switch Position *******************/

	if(testbit(multibuf,ALT_SWITCH)) {
          multiseldis = 1;
	  if(testbit(multibuf,ADJUSTMENT_UP)) {
	    altdbncinc++;
	    if (altdbncinc == 3) {
	      XPLMCommandOnce(ApAltUp);
	      altdbncinc = 0;
	    }
	  }
	  if(testbit(multibuf,ADJUSTMENT_DN)) {
	    altdbncdec++;
	    if (altdbncdec == 3) {	    
	      XPLMCommandOnce(ApAltDn);
	      altdbncdec = 0;
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

/***************** VS Switch Position *******************/

	if(testbit(multibuf,VS_SWITCH)) {
          multiseldis = 1;
	  if(testbit(multibuf,ADJUSTMENT_UP)) {
	    vsdbncinc++;
	    if (vsdbncinc == 3) {
	      XPLMCommandOnce(ApVsUp); 
	      vsdbncinc = 0;
	    }
	  }
	  if(testbit(multibuf,ADJUSTMENT_DN)) {
	    vsdbncdec++;
	    if (vsdbncdec == 3) {
	      XPLMCommandOnce(ApVsDn); 
	      vsdbncdec = 0;
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

/***************** IAS Switch Position *******************/

	if(testbit(multibuf,IAS_SWITCH)) {
          multiseldis = 2;
	  if(testbit(multibuf,ADJUSTMENT_UP)) {
	    iasdbncinc++;
	    if (iasdbncinc == 3) {
	      XPLMCommandOnce(ApAsUp); 
	      iasdbncinc = 0;
	    }
	  }
	  if(testbit(multibuf,ADJUSTMENT_DN)) {
	    iasdbncdec++;
	    if (iasdbncdec == 3) {
	      XPLMCommandOnce(ApAsDn); 
	      iasdbncdec = 0;
	    }
	  }
	  upapasf = XPLMGetDataf(ApAs);
	  upapas = (int)(upapasf);
	}

/***************** HDG Switch Position *******************/

	if(testbit(multibuf,HDG_SWITCH)) {
          multiseldis = 3;
	  if(testbit(multibuf,ADJUSTMENT_UP)) {
	    hdgdbncinc++;
	    if (hdgdbncinc == 3) {
	      XPLMCommandOnce(ApHdgUp); 
	      hdgdbncinc = 0;
	    }
	  }
	  if(testbit(multibuf,ADJUSTMENT_DN)) {
	    hdgdbncdec++;
	    if (hdgdbncdec == 3) {
	      XPLMCommandOnce(ApHdgDn); 
	      hdgdbncdec = 0;
	    }
	  }
	  upaphdgf = XPLMGetDataf(ApHdg);
	  upaphdg = (int)(upaphdgf);
	}

/***************** CRS Switch Position *******************/

	if(testbit(multibuf,CRS_SWITCH)) {
          multiseldis = 4;
	  if(testbit(multibuf,ADJUSTMENT_UP)) {
	    crsdbncinc++;
	    if (crsdbncinc == 1) {
	      XPLMCommandOnce(ApCrsUp); 
	      crsdbncinc = 0;
	    }	 
	  }
	  if(testbit(multibuf,ADJUSTMENT_DN)) {
	    crsdbncdec++;
	    if (crsdbncdec == 1) {
	      XPLMCommandOnce(ApCrsDn); 
	      crsdbncdec = 0;
	    }	 
	  }
	  upapcrsf = XPLMGetDataf(ApCrs);
	  upapcrs = (int)(upapcrsf);
	}

/***************** Auto Throttle Switch Position *******************/

	if(testbit(multibuf,AUTO_THROTTLE_SWITCH)) {
          if (loaded737 == 0){
            XPLMCommandOnce(ApAutThrOn);
          }
          if (loaded737 == 1){
            XPLMSetDatai(x737athr_armed, 1);
          }
	}
	else {
          if (loaded737 == 0){
            XPLMCommandOnce(ApAutThrOff);
          }
          if (loaded737 == 1){
            XPLMSetDatai(x737athr_armed, 0);
          }
        }

/***************** AP Master Button and light *******************/

	if (XPLMGetDatai(ApMstrStat) == 0) {
          if (multires > 0) {
	    if(testbit(multibuf,AP_MASTER_BUTTON)) {
	      XPLMCommandOnce(ApMstrBtnUp);
	    }
	  }
	}

	if (XPLMGetDatai(ApMstrStat) == 1) {
          if (multires > 0) {
	    if(testbit(multibuf,AP_MASTER_BUTTON)) {
	      apbtncnt++;
	    }
              if (apbtncnt == 1) {
	        XPLMCommandOnce(ApMstrBtnUp);
	        apbtncnt = 0;
	      }	 
	  }	
	}

	if (XPLMGetDatai(ApMstrStat) == 2) {
          if (multires > 0) {
	    if(testbit(multibuf,AP_MASTER_BUTTON)) {
	      apbtncnt++;
	    }
	    if (apbtncnt == 2) {
	      XPLMSetDatai(ApMstrStat, 0);
	      apbtncnt = 0;
	    }
	  }	
	}

	if (XPLMGetDatai(ApMstrStat) == 0) {
	  btnleds &= ~(1<<0);   /* clear bit 0 in btnleds to 0 */   
	}
	if (XPLMGetDatai(ApMstrStat) == 1) {
	  if (flashon == 1) {
	    btnleds |= (1<<0);   /* set bit 0 in btnleds to 1 */ 
	  }
	  if (flashon == 0) {   
	    btnleds &= ~(1<<0);   /* clear bit 0 in btnleds to 0 */
	  }
	}
	if (XPLMGetDatai(ApMstrStat) == 2) {
	  btnleds |= (1<<0);   /* set bit 0 in btnleds to 1 */ 
	}

/***************** HDG Button and light *******************/

        if (multires > 0) {
          if(testbit(multibuf,HDG_BUTTON)) {
	    XPLMCommandOnce(ApHdgBtn);
	  }
	}
	if (XPLMGetDatai(ApHdgStat) == 2) {
	  btnleds |= (1<<1);   /* set bit 1 in btnleds to 1 */    
	}
	if (XPLMGetDatai(ApHdgStat) == 1) {
	  if (flashon == 1) {
	    btnleds |= (1<<1);   /* set bit 1 in btnleds to 1 */
	  }
	  if (flashon == 0) {   
	    btnleds &= ~(1<<1);   /* clear bit 1 in btnleds to 0 */
	  }
	}
	if (XPLMGetDatai(ApHdgStat) == 0) {
	  btnleds &= ~(1<<1);   /* clear bit 1 in btnleds to 0 */   
	}
	if (XPLMGetDatai(ApMstrStat) == 0) {
	  btnleds &= ~(1<<1);   /* clear bit 1 in btnleds to 0 */   
	}
/***************** NAV Button and light *******************/

        if (multires > 0) {
          if(testbit(multibuf,NAV_BUTTON)) {
	    XPLMCommandOnce(ApNavBtn);	 
	  }
	}
	if (XPLMGetDatai(ApNavStat) == 2) {
	  btnleds |= (1<<2);   /* set bit 2 in btnleds to 1 */    
	}
	if (XPLMGetDatai(ApNavStat) == 1) {
	  if (flashon == 1) {
	    btnleds |= (1<<2);   /* set bit 1 in btnleds to 1 */ 
	  }
	  if (flashon == 0) {   
	    btnleds &= ~(1<<2);   /* clear bit 2 in btnleds to 0 */
	  }
	}
	if (XPLMGetDatai(ApNavStat) == 0) {
	  btnleds &= ~(1<<2);   /* clear bit 2 in btnleds to 0 */   
	}

/***************** IAS Button and light *******************/

	if(testbit(multibuf,IAS_BUTTON)) {
	  XPLMCommandOnce(ApIasBtn);
	}
	if (XPLMGetDatai(ApIasStat) == 2) {
	  btnleds |= (1<<3);   /* set bit 3 in btnleds to 1 */    
	}
	if (XPLMGetDatai(ApIasStat) == 1) {
	  if (flashon == 1) {
	    btnleds |= (1<<3);   /* set bit 3 in btnleds to 1 */    
	  }  
	  if (flashon == 0) {    
	    btnleds &= ~(1<<3);   /* clear bit 3 in btnleds to 0 */
	  }
	}
	if (XPLMGetDatai(ApIasStat) == 0) {
	  btnleds &= ~(1<<3);   /* clear bit 3 in btnleds to 0 */   
	}

/***************** ALT Button and light *******************/

        if (multires > 0) {
	  if(testbit(multibuf,ALT_BUTTON)) {
	    XPLMCommandOnce(ApAltBtn);	 
	  }
	}
	if (XPLMGetDatai(ApAltStat) == 2) {
	  btnleds |= (1<<4);   /* set bit 4 in btnleds to 1 */ 
	} 
	if (XPLMGetDatai(ApAltStat) == 1) {
	  if (flashon == 1) {
	    btnleds |= (1<<4);   /* set bit 4 in btnleds to 1 */ 
	  }  
	  if (flashon == 0) {    
	    btnleds &= ~(1<<4);   /* clear bit 4 in btnleds to 0 */
	  }
	}
	if (XPLMGetDatai(ApAltStat) == 0) {
	  btnleds &= ~(1<<4);   /* clear bit 4 in btnleds to 0 */   
	}
	if (XPLMGetDatai(ApMstrStat) == 0) {
	  btnleds &= ~(1<<4);   /* clear bit 4 in btnleds to 0 */   
	}
/***************** VS Button and light *******************/

        if (multires > 0) {
	  if(testbit(multibuf,VS_BUTTON)) {
 	    XPLMCommandOnce(ApVsBtn);	 
	  }
	}
	if (XPLMGetDatai(ApVsStat) == 2) {
	  btnleds |= (1<<5);   /* set bit 5 in btnleds to 1 */ 
	} 
	if (XPLMGetDatai(ApVsStat) == 1) {
	  if (flashon == 0) {
	    btnleds |= (1<<5);   /* set bit 5 in btnleds to 1 */
	  } 
	  if (flashon == 1) {   
	    btnleds &= ~(1<<5);   /* clear bit 5 in btnleds to 0 */
	  }
	}
	if (XPLMGetDatai(ApVsStat) == 0) {
	  btnleds &= ~(1<<5);   /* clear bit 5 in btnleds to 0 */   
	}

/***************** APR Button and light *******************/

        if (multires > 0) {
	  if(testbit(multibuf,APR_BUTTON)) {
	    XPLMCommandOnce(ApAprBtn);	 
	  }
	}
	if (XPLMGetDatai(ApAprStat) == 2) {
	  btnleds |= (1<<6);   /* set bit 6 in btnleds to 1 */ 
	} 
	if (XPLMGetDatai(ApAprStat) == 1) {
	  if (flashon == 1) {
	    btnleds |= (1<<6);   /* set bit 6 in btnleds to 1 */
	  } 
	  if (flashon == 0) {   
	    btnleds &= ~(1<<6);   /* clear bit 6 in btnleds to 0 */
	  }
	}
	if (XPLMGetDatai(ApAprStat) == 0) {
	  btnleds &= ~(1<<6);   /* clear bit 6 in btnleds to 0 */   
	}

/***************** REV Button and light *******************/

        if (multires > 0) {
	  if(testbit(multibuf,REV_BUTTON)) {
	    XPLMCommandOnce(ApRevBtn);	 
	  }
	}
	if (XPLMGetDatai(ApRevStat) == 2) {
	  btnleds |= (1<<7);   /* set bit 7 in btnleds to 1 */ 
	} 
	if (XPLMGetDatai(ApRevStat) == 1) {
	  if (flashon == 1) {
	    btnleds |= (1<<7);   /* set bit 7 in btnleds to 1 */
	  } 
	  if (flashon == 0) {    
	    btnleds &= ~(1<<7);   /* clear bit 7 in btnleds to 0 */
	  }
	}
	if (XPLMGetDatai(ApRevStat) == 0) {
	  btnleds &= ~(1<<7);   /* clear bit 7 in btnleds to 0 */   
	}

/***************** Flaps Switch *******************/

        if (multires > 0) {
	  if(testbit(multibuf,FLAPS_UP_SWITCH)) {
	    XPLMCommandOnce(FlapsUp);	 
          }
	}
        if (multires > 0) {
	  if(testbit(multibuf,FLAPS_DN_SWITCH)) {
	    XPLMCommandOnce(FlapsDn);	 
	  }
        }

/*************** Trim Wheel *********************/

        if (multires > 0) {
	  if(testbit(multibuf,TRIM_WHEEL_UP)) {
	    XPLMCommandOnce(PitchTrimUp);		 
	  }	
	}
        if (multires > 0) {

	  if(testbit(multibuf,TRIM_WHEEL_DN)) {
	    XPLMCommandOnce(PitchTrimDn);	 
	  }
	}

/***************** Flasher for Button LEDS *******************/

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

/***************** Blank Display *******************/

	if (XPLMGetDatai(AvPwrOn) == 0) {
          multiseldis = 5;	  
	}
	if (XPLMGetDatai(BatPwrOn) == 0) {
          multiseldis = 5;	  
	}

  return;
}
