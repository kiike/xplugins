// ****** radiopanels.cpp **********
// *****  William R. Good **********

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

// ********************** Radio Panel variables ***********************
static int radnum = 0, radionowrite[4] = {0, 0, 0, 0};
static int radiores = 0;

static int updmepushed = 0, lodmepushed = 0;
static int updmeloop = 0, lodmeloop = 0;
static int uplastdmepos = 0, lolastdmepos = 0;

static int upactcomnavfreq[4], upstbycomnavfreq[4], loactcomnavfreq[4], lostbycomnavfreq[4];
static int upstbyadffreq[4], lostbyadffreq[4];
static int upactadffreq[4], loactadffreq[4];
static int updmedist[4], lodmedist[4];
static int updmenavspeed[4], lodmenavspeed[4];
static int updmespeed[4], lodmespeed[4];
static int updmedbncfninc[4], lodmedbncfninc[4];
static int updmedbncfndec[4], lodmedbncfndec[4];
static int updmefreqhnd, updmefreqfrc;
static int lodmefreqhnd, lodmefreqfrc;

static int upxpdrcode[4], loxpdrcode[4];
static int updmemode[4], lodmemode[4];
static int updmesource[4], lodmesource[4];
static int updmefreq[4], lodmefreq[4];

static int upcom1[4] = {0, 0, 0, 0}, upcom2[4] = {0, 0, 0, 0};
static int upnav1[4] = {0, 0, 0, 0}, upnav2[4] = {0, 0, 0, 0};
static int locom1[4] = {0, 0, 0, 0}, locom2[4] = {0, 0, 0, 0}; 
static int lonav1[4] = {0, 0, 0, 0}, lonav2[4] = {0, 0, 0, 0};

static int upcom1dbncfninc[4] = {0, 0, 0, 0}, upcom1dbncfndec[4] = {0, 0, 0, 0};
static int upcom1dbnccorinc[4] = {0, 0, 0, 0}, upcom1dbnccordec[4] = {0, 0, 0, 0};
static int upcom2dbncfninc[4] = {0, 0, 0, 0}, upcom2dbncfndec[4] = {0, 0, 0, 0,};
static int upcom2dbnccorinc[4] = {0, 0, 0, 0}, upcom2dbnccordec[4] = {0, 0, 0, 0};
static int upnav1dbncfninc[4] = {0, 0, 0, 0}, upnav1dbncfndec[4] = {0, 0, 0, 0};
static int upnav1dbnccorinc[4] = {0, 0, 0, 0}, upnav1dbnccordec[4] = {0, 0, 0, 0};
static int upnav2dbncfninc[4] = {0, 0, 0, 0}, upnav2dbncfndec[4] = {0, 0, 0, 0};
static int upnav2dbnccorinc[4] = {0, 0, 0, 0}, upnav2dbnccordec[4] = {0, 0, 0, 0};
static int upadfdbncfninc[4] = {0, 0, 0, 0}, upadfdbncfndec[4] = {0, 0, 0, 0};
static int upadfdbnccorinc[4] = {0, 0, 0, 0}, upadfdbnccordec[4] = {0, 0, 0, 0};
static int upxpdrdbncfninc[4] = {0, 0, 0, 0}, upxpdrdbncfndec[4] = {0, 0, 0, 0};
static int upxpdrdbnccorinc[4] = {0, 0, 0, 0}, upxpdrdbnccordec[4] = {0, 0, 0, 0};

static int locom1dbncfninc[4] = {0, 0, 0, 0}, locom1dbncfndec[4] = {0, 0, 0, 0};
static int locom1dbnccorinc[4] = {0, 0, 0, 0}, locom1dbnccordec[4] = {0, 0, 0, 0};
static int locom2dbncfninc[4] = {0, 0, 0, 0}, locom2dbncfndec[4] = {0, 0, 0, 0};
static int locom2dbnccorinc[4] = {0, 0, 0, 0}, locom2dbnccordec[4] = {0, 0, 0, 0};
static int lonav1dbncfninc[4] = {0, 0, 0, 0}, lonav1dbncfndec[4] = {0, 0, 0, 0};
static int lonav1dbnccorinc[4] = {0, 0, 0, 0}, lonav1dbnccordec[4] = {0, 0, 0, 0};
static int lonav2dbncfninc[4] = {0, 0, 0, 0}, lonav2dbncfndec[4] = {0, 0, 0, 0};
static int lonav2dbnccorinc[4] = {0, 0, 0, 0}, lonav2dbnccordec[4] = {0, 0, 0, 0};
static int loadfdbncfninc[4] = {0, 0, 0, 0}, loadfdbncfndec[4] = {0, 0, 0, 0};
static int loadfdbnccorinc[4] = {0, 0, 0, 0}, loadfdbnccordec[4] = {0, 0, 0, 0};
static int loxpdrdbncfninc[4] = {0, 0, 0, 0}, loxpdrdbncfndec[4] = {0, 0, 0, 0};
static int loxpdrdbnccorinc[4] = {0, 0, 0, 0}, loxpdrdbnccordec[4] = {0, 0, 0, 0};

static float updmedistf[4], lodmedistf[4];
static float updmenav1speedf[4], updmenav2speedf[4], lodmenav1speedf[4], lodmenav2speedf[4];
static float updmespeedf[4], lodmespeedf[4];

static float updmetime[4], lodmetime[4];

static int radioaactv, radioadig1, radioarem1, radioadig2, radioarem2, radioadig3, radioarem3;
static int radioadig4, radioarem4, radioadig5;
static int radiobstby, radiobdig1, radiobrem1, radiobdig2, radiobrem2, radiobdig3, radiobrem3;
static int radiobdig4, radiobrem4, radiobdig5;  
static int radiocactv, radiocdig1, radiocrem1, radiocdig2, radiocrem2, radiocdig3, radiocrem3;
static int radiocdig4, radiocrem4, radiocdig5;
static int radiodstby, radioddig1, radiodrem1, radioddig2, radiodrem2, radioddig3, radiodrem3;
static int radioddig4, radiodrem4, radioddig5;

static int upxpdrsel[4] = {1, 1, 1, 1}, loxpdrsel[4] = {1, 1, 1, 1};
static int upadfsel[4] = {1, 1, 1, 1}, loadfsel[4] = {1, 1, 1, 1};

static int upseldis[4] = {1, 1, 1, 1}, loseldis[4] = {1, 1, 1, 1};
static int lastupseldis[4] = {1, 1, 1, 1}, lastloseldis[4] = {1, 1, 1, 1};

static int UPPER_FINE_UP = 23, UPPER_FINE_DN = 22; 
static int UPPER_COARSE_UP = 21, UPPER_COARSE_DN = 20;
static int UPPER_COM1 = 7, UPPER_COM2 = 6;
static int UPPER_NAV1 = 5, UPPER_NAV2 = 4; 
static int UPPER_ADF = 3, UPPER_DME = 2;
static int UPPER_XPDR = 1, UPPER_ACT_STBY = 9;

static int LOWER_FINE_UP = 19, LOWER_FINE_DN = 18; 
static int LOWER_COARSE_UP = 17, LOWER_COARSE_DN = 16;
static int LOWER_COM1 = 0, LOWER_COM2 = 15;
static int LOWER_NAV1 = 14, LOWER_NAV2 = 13; 
static int LOWER_ADF = 12, LOWER_DME = 11;
static int LOWER_XPDR = 10, LOWER_ACT_STBY = 8; 

static unsigned char radiobuf[4][4];
static unsigned char radiowbuf[4][23];

void process_upper_nav_com_freq();
void process_lower_nav_com_freq();

// ***** Radio Panel Process  *******
void process_radio_panel()

{

    XPLMClearAllMenuItems(RadioMenuId);
    XPLMAppendMenuItem(RadioMenuId, "# OF SWITCH PULSES PER COMMAND", (void *) "VOID", 1);
    XPLMAppendMenuSeparator(RadioMenuId);
    XPLMAppendMenuItem(RadioMenuId, "FREQ KNOB  1 PULSE PER COMMAND", (void *) "1", 1);
    XPLMAppendMenuItem(RadioMenuId, "FREQ KNOB  2 PULSES PER COMMAND", (void *) "2", 1);
    XPLMAppendMenuItem(RadioMenuId, "FREQ KNOB  3 PULSES PER COMMAND", (void *) "3", 1);
    XPLMAppendMenuItem(RadioMenuId, "FREQ KNOB  4 PULSES PER COMMAND", (void *) "4", 1);
    XPLMAppendMenuItem(RadioMenuId, "FREQ KNOB  5 PULSES PER COMMAND", (void *) "5", 1);
    XPLMAppendMenuSeparator(RadioMenuId);
    XPLMAppendMenuItem(RadioMenuId, "ONE ADF TUNER", (void *) "ADF1", 1);
    XPLMAppendMenuItem(RadioMenuId, "TWO ADF TUNERS", (void *) "ADF2", 1);

    if (radspeed == 1) {
      XPLMCheckMenuItem(RadioMenuId, 2, xplm_Menu_Checked);
    }
    if (radspeed != 1) {
      XPLMCheckMenuItem(RadioMenuId, 2, xplm_Menu_Unchecked);
    }
    if (radspeed == 2) {
      XPLMCheckMenuItem(RadioMenuId, 3, xplm_Menu_Checked);
    }
    if (radspeed != 2) {
      XPLMCheckMenuItem(RadioMenuId, 3, xplm_Menu_Unchecked);
    }
    if (radspeed == 3) {
      XPLMCheckMenuItem(RadioMenuId, 4, xplm_Menu_Checked);
    }
    if (radspeed != 3) {
      XPLMCheckMenuItem(RadioMenuId, 4, xplm_Menu_Unchecked);
    }
    if (radspeed == 4) {
      XPLMCheckMenuItem(RadioMenuId, 5, xplm_Menu_Checked);
    }
    if (radspeed != 4) {
      XPLMCheckMenuItem(RadioMenuId, 5, xplm_Menu_Unchecked);
    }
    if (radspeed == 5) {
      XPLMCheckMenuItem(RadioMenuId, 6, xplm_Menu_Checked);
    }
    if (radspeed != 5) {
      XPLMCheckMenuItem(RadioMenuId, 6, xplm_Menu_Unchecked);
    }
    if (numadf == 1) {
      XPLMCheckMenuItem(RadioMenuId, 8, xplm_Menu_Checked);
    }
    if (numadf != 1) {
      XPLMCheckMenuItem(RadioMenuId, 8, xplm_Menu_Unchecked);
    }
    if (numadf == 2) {
      XPLMCheckMenuItem(RadioMenuId, 9, xplm_Menu_Checked);
    }
    if (numadf != 2) {
      XPLMCheckMenuItem(RadioMenuId, 9, xplm_Menu_Unchecked);
    }

  // *************** Upper Display info **********************

  if (upseldis[radnum] == 1) { 
    process_upper_nav_com_freq();
  } 

  else if (upseldis[radnum] == 2) { 
    process_upper_nav_com_freq();
  } 

  else if (upseldis[radnum] == 3) { 
    process_upper_nav_com_freq();
  } 

  else if (upseldis[radnum] == 4) { 
    process_upper_nav_com_freq();
  } 

  else if (upseldis[radnum] == 5) {
    if (upadfsel[radnum] == 1) {
      radioaactv = upactadffreq[radnum];
      radioadig1 = 15, radioadig2 = 15;
      radioadig3 = radioaactv/100, radioarem3 =radioaactv%100;
      radioadig4 = radioarem3/10, radioarem4 = radioarem3%10;
      radioadig5 = radioarem4;

      radiobstby = upstbyadffreq[radnum];
      radiobdig1 = 15, radiobdig2 = 15;
      radiobdig3 = radiobstby/100, radiobrem3 =radiobstby%100;
      radiobdig4 = radiobrem3/10, radiobrem4 = radiobrem3%10;
      radiobdig5 = radiobrem4;
      radiobdig5 = radiobdig5+208;
    }

    if (upadfsel[radnum] == 2) {
      radioaactv = upactadffreq[radnum];
      radioadig1 = 15, radioadig2 = 15;
      radioadig3 = radioaactv/100, radioarem3 =radioaactv%100;
      radioadig4 = radioarem3/10, radioarem4 = radioarem3%10;
      radioadig5 = radioarem4;

      radiobstby = upstbyadffreq[radnum];
      radiobdig1 = 15, radiobdig2 = 15;
      radiobdig3 = radiobstby/100, radiobrem3 =radiobstby%100;
      radiobdig4 = radiobrem3/10, radiobrem4 = radiobrem3%10;
      radiobdig4 = radiobdig4+208, radiobdig5 = radiobrem4;
    }

    if (upadfsel[radnum] == 3) {
      radioaactv = upactadffreq[radnum];
      radioadig1 = 15, radioadig2 = 15;
      radioadig3 = radioaactv/100, radioarem3 =radioaactv%100;
      radioadig4 = radioarem3/10, radioarem4 = radioarem3%10;
      radioadig5 = radioarem4;

      radiobstby = upstbyadffreq[radnum];
      radiobdig1 = 15, radiobdig2 = 15;
      radiobdig3 = radiobstby/100, radiobrem3 =radiobstby%100;
      radiobdig3 = radiobdig3+208;
      radiobdig4 = radiobrem3/10, radiobrem4 = radiobrem3%10;
      radiobdig5 = radiobrem4;
    }

    if (upadfsel[radnum] > 3) {
      radioaactv = upactadffreq[radnum];
      radioadig1 = 15, radioadig2 = 15;
      radioadig3 = radioaactv/100, radioarem3 =radioaactv%100;
      radioadig4 = radioarem3/10, radioarem4 = radioarem3%10;
      radioadig5 = radioarem4;

      radiobstby = upstbyadffreq[radnum];
      radiobdig1 = 15, radiobdig2 = 15;
      radiobdig3 = radiobstby/100, radiobrem3 =radiobstby%100;
      radiobdig4 = radiobrem3/10, radiobrem4 = radiobrem3%10;
      radiobdig5 = radiobrem4;
    }

  }

  else if (upseldis[radnum] == 6) {
    radioaactv = updmenavspeed[radnum];
    radioadig1 = 15, radioadig2 = 15;
    radioadig3 = radioaactv/100, radioarem3 =radioaactv%100;
    radioadig4 = radioarem3/10, radioarem4 = radioarem3%10;
    radioadig5 = radioarem4;

    radiobstby = updmedist[radnum];
    radiobdig1 = radiobstby/10000, radiobrem1 = radiobstby%10000;
    radiobdig2 = radiobrem1 /1000, radiobrem2 = radiobrem1%1000;
    radiobdig3 = radiobrem2/100, radiobrem3 = radiobrem2%100;
    radiobdig4 = radiobrem3/10, radiobrem4 = radiobrem3%10;
    radiobdig4 = radiobdig4+208, radiobdig5 = radiobrem4;

  }

  else if (upseldis[radnum] == 7) {
    radioaactv = updmefreq[radnum];
    radioadig1 = radioaactv/10000, radioarem1 = radioaactv%10000;
    radioadig2 = radioarem1 /1000, radioarem2 = radioarem1%1000;
    radioadig3 = radioarem2/100, radioarem3 = radioarem2%100;
    radioadig4 = radioarem3/10, radioarem4 = radioarem3%10;
    radioadig3 = radioadig3+208, radioadig5 = radioarem4;

    radiobstby = updmetime[radnum];
    radiobdig1 = 15,radiobdig2 = 15;
    radiobdig3 = radiobstby/100, radiobrem3 = radiobstby%100;
    radiobdig4 = radiobrem3/10, radiobrem4 = radiobrem3%10;
    radiobdig4 = radiobdig4, radiobdig5 = radiobrem4;

  }

  else if (upseldis[radnum] == 8) {
    radioaactv = updmenavspeed[radnum];
    radioadig1 = 15, radioadig2 = 15;
    radioadig3 = radioaactv/100, radioarem3 =radioaactv%100;
    radioadig4 = radioarem3/10, radioarem4 = radioarem3%10;
    radioadig5 = radioarem4;

    radiobstby = updmetime[radnum];
    radiobdig1 = 15,radiobdig2 = 15;
    radiobdig3 = radiobstby/100, radiobrem3 = radiobstby%100;
    radiobdig4 = radiobrem3/10, radiobrem4 = radiobrem3%10;
    radiobdig4 = radiobdig4, radiobdig5 = radiobrem4;

  }

  else if (upseldis[radnum] == 9) {

    if (upxpdrsel[radnum] == 1) {
      radioadig1 = 15, radioadig2 = 15, radioadig3 = 15, radioadig4 = 15, radioadig5 = 15;
      radiobstby = upxpdrcode[radnum];
      radiobdig1 = 15, radiobdig2 = radiobstby/1000, radiobrem2 = radiobstby%1000;
      radiobdig3 = radiobrem2/100, radiobrem3 = radiobrem2%100;
      radiobdig4 = radiobrem3/10, radiobrem4 = radiobrem3%10;
      radiobdig5 = radiobrem4,  radiobdig5 = radiobdig5+208;
    }

    if (upxpdrsel[radnum] == 2) {
      radioadig1 = 15, radioadig2 = 15, radioadig3 = 15, radioadig4 = 15, radioadig5 = 15;
      radiobstby = upxpdrcode[radnum];
      radiobdig1 = 15, radiobdig2 = radiobstby/1000, radiobrem2 = radiobstby%1000;
      radiobdig3 = radiobrem2/100, radiobrem3 = radiobrem2%100;
      radiobdig4 = radiobrem3/10, radiobrem4 = radiobrem3%10;
      radiobdig4 = radiobdig4+208, radiobdig5 = radiobrem4;
    }

    if (upxpdrsel[radnum] == 3) {
      radioadig1 = 15, radioadig2 = 15, radioadig3 = 15, radioadig4 = 15, radioadig5 = 15;
      radiobstby = upxpdrcode[radnum];
      radiobdig1 = 15, radiobdig2 = radiobstby/1000, radiobrem2 = radiobstby%1000;
      radiobdig3 = radiobrem2/100, radiobrem3 = radiobrem2%100;
      radiobdig3 = radiobdig3+208;
      radiobdig4 = radiobrem3/10, radiobrem4 = radiobrem3%10;
      radiobdig5 = radiobrem4;
    }

    if (upxpdrsel[radnum] == 4) {
      radioadig1 = 15, radioadig2 = 15, radioadig3 = 15, radioadig4 = 15, radioadig5 = 15;
      radiobstby = upxpdrcode[radnum];
      radiobdig1 = 15, radiobdig2 = radiobstby/1000, radiobrem2 = radiobstby%1000;
      radiobdig2 = radiobdig2+208;
      radiobdig3 = radiobrem2/100, radiobrem3 = radiobrem2%100;
      radiobdig4 = radiobrem3/10, radiobrem4 = radiobrem3%10;
      radiobdig5 = radiobrem4;
    }

  }

  else if (upseldis[radnum] == 10) {
    radioadig1 = 15, radioadig2 = 15, radioadig3 = 15, radioadig4 = 15, radioadig5 = 15;
    radiobdig1 = 15, radiobdig2 = 15, radiobdig3 = 15, radiobdig4 = 15, radiobdig5 = 15;
  }

  // ************************ Lower Display info ********************
  
  if (loseldis[radnum] == 1) { 
    process_lower_nav_com_freq();
  } 

  else if (loseldis[radnum] == 2) { 
    process_lower_nav_com_freq();
  } 

  else if (loseldis[radnum] == 3) { 
    process_lower_nav_com_freq();
  } 

  else if (loseldis[radnum] == 4) {
    process_lower_nav_com_freq(); 
  } 
  
  else if (loseldis[radnum] == 5) {
    if (loadfsel[radnum] == 1) {
      radiocactv = loactadffreq[radnum];
      radiocdig1 = 15, radiocdig2 = 15;
      radiocdig3 = radiocactv/100, radiocrem3 =radiocactv%100;
      radiocdig4 = radiocrem3/10, radiocrem4 = radiocrem3%10;
      radiocdig5 = radiocrem4;

      radiodstby = lostbyadffreq[radnum];
      radioddig1 = 15, radioddig2 = 15;
      radioddig3 = radiodstby/100, radiodrem3 = radiodstby%100;
      radioddig4 = radiodrem3/10, radiodrem4 = radiodrem3%10;
      radioddig5 = radiodrem4;
      radioddig5 = radioddig5+208;
    }

    if (loadfsel[radnum] == 2) {
      radiocactv = loactadffreq[radnum];
      radiocdig1 = 15, radiocdig2 = 15;
      radiocdig3 = radiocactv/100, radiocrem3 =radiocactv%100;
      radiocdig4 = radiocrem3/10, radiocrem4 = radiocrem3%10;
      radiocdig5 = radiocrem4;

      radiodstby = lostbyadffreq[radnum];
      radioddig1 = 15, radioddig2 = 15;
      radioddig3 = radiodstby/100, radiodrem3 = radiodstby%100;
      radioddig4 = radiodrem3/10, radiodrem4 = radiodrem3%10;
      radioddig4 = radioddig4+208;
      radioddig5 = radiodrem4;
    }

    if (loadfsel[radnum] == 3) {
      radiocactv = loactadffreq[radnum];
      radiocdig1 = 15, radiocdig2 = 15;
      radiocdig3 = radiocactv/100, radiocrem3 =radiocactv%100;
      radiocdig4 = radiocrem3/10, radiocrem4 = radiocrem3%10;
      radiocdig5 = radiocrem4;

      radiodstby = lostbyadffreq[radnum];
      radioddig1 = 15, radioddig2 = 15;
      radioddig3 = radiodstby/100, radiodrem3 = radiodstby%100;
      radioddig3 = radioddig3+208;
      radioddig4 = radiodrem3/10, radiodrem4 = radiodrem3%10;
      radioddig5 = radiodrem4;
    }

    if (loadfsel[radnum] > 3) {
      radiocactv = loactadffreq[radnum];
      radiocdig1 = 15, radiocdig2 = 15;
      radiocdig3 = radiocactv/100, radiocrem3 =radiocactv%100;
      radiocdig4 = radiocrem3/10, radiocrem4 = radiocrem3%10;
      radiocdig5 = radiocrem4;

      radiodstby = lostbyadffreq[radnum];
      radioddig1 = 15, radioddig2 = 15;
      radioddig3 = radiodstby/100, radiodrem3 = radiodstby%100;
      radioddig4 = radiodrem3/10, radiodrem4 = radiodrem3%10;
      radioddig5 = radiodrem4;
    }

  }


  else if (loseldis[radnum] == 6) {
    radiocactv = lodmenavspeed[radnum];
    radiocdig1 = 15, radiocdig2 = 15;
    radiocdig3 = radiocactv/100, radiocrem3 =radiocactv%100;
    radiocdig4 = radiocrem3/10, radiocrem4 = radiocrem3%10;
    radiocdig5 = radiocrem4;

    radiodstby = lodmedist[radnum];
    radioddig1 = radiodstby/10000, radiodrem1 = radiodstby%10000;
    radioddig2 = radiodrem1 /1000, radiodrem2 = radiodrem1%1000;
    radioddig3 = radiodrem2/100, radiodrem3 = radiodrem2%100;
    radioddig4 = radiodrem3/10, radiodrem4 = radiodrem3%10;
    radioddig4 = radioddig4+208, radioddig5 = radiodrem4;

  }

  else if (loseldis[radnum] == 7) {
    radiocactv = lodmefreq[radnum];
    radiocdig1 = radiocactv/10000, radiocrem1 = radiocactv%10000;
    radiocdig2 = radiocrem1 /1000, radiocrem2 = radiocrem1%1000;
    radiocdig3 = radiocrem2/100, radiocrem3 = radiocrem2%100;
    radiocdig4 = radiocrem3/10, radiocrem4 = radiocrem3%10;
    radiocdig3 = radiocdig3+208;
    radiocdig5 = radiocrem4;

    radiodstby = lodmetime[radnum];
    radioddig1 = 15, radioddig2 = 15;
    radioddig3 = radiodstby/100, radiodrem3 = radiodstby%100;
    radioddig4 = radiodrem3/10, radiodrem4 = radiodrem3%10;
    radioddig4 = radioddig4, radioddig5 = radiodrem4;

  }

  else if (loseldis[radnum] == 8) {
    radiocactv = lodmespeed[radnum];
    radiocdig1 = 15, radiocdig2 = 15;
    radiocdig3 = radiocactv/100, radiocrem3 =radiocactv%100;
    radiocdig4 = radiocrem3/10, radiocrem4 = radiocrem3%10;
    radiocdig5 = radiocrem4;

    radiodstby = lodmetime[radnum];
    radioddig1 = 15, radioddig2 = 15;
    radioddig3 = radiodstby/100, radiodrem3 = radiodstby%100;
    radioddig4 = radiodrem3/10, radiodrem4 = radiodrem3%10;
    radioddig4 = radioddig4, radioddig5 = radiodrem4;

  }


  else if (loseldis[radnum] == 9) {

    if (loxpdrsel[radnum] == 1) {
      radiocdig1 = 15, radiocdig2 = 15, radiocdig3 = 15, radiocdig4 = 15, radiocdig5 = 15;
      radiodstby = loxpdrcode[radnum];
      radioddig1 = 15, radioddig2 = radiodstby/1000, radiodrem2 = radiodstby%1000;
      radioddig3 = radiodrem2/100, radiodrem3 = radiodrem2%100;
      radioddig4 = radiodrem3/10, radiodrem4 = radiodrem3%10;
      radioddig5 = radiodrem4, radioddig5 = radioddig5+208;
    }

    if (loxpdrsel[radnum] == 2) {
      radiocdig1 = 15, radiocdig2 = 15, radiocdig3 = 15, radiocdig4 = 15, radiocdig5 = 15;
      radiodstby = loxpdrcode[radnum];
      radioddig1 = 15, radioddig2 = radiodstby/1000, radiodrem2 = radiodstby%1000;
      radioddig3 = radiodrem2/100, radiodrem3 = radiodrem2%100;
      radioddig4 = radiodrem3/10, radiodrem4 = radiodrem3%10;
      radioddig4 = radioddig4+208;
      radioddig5 = radiodrem4;
    }

    if (loxpdrsel[radnum] == 3) {
      radiocdig1 = 15, radiocdig2 = 15, radiocdig3 = 15, radiocdig4 = 15, radiocdig5 = 15;
      radiodstby = loxpdrcode[radnum];
      radioddig1 = 15, radioddig2 = radiodstby/1000, radiodrem2 = radiodstby%1000;
      radioddig3 = radiodrem2/100, radiodrem3 = radiodrem2%100;
      radioddig3 = radioddig3+208;
      radioddig4 = radiodrem3/10, radiodrem4 = radiodrem3%10;
      radioddig5 = radiodrem4;
    }

    if (loxpdrsel[radnum] == 4) {
      radiocdig1 = 15, radiocdig2 = 15, radiocdig3 = 15, radiocdig4 = 15, radiocdig5 = 15;
      radiodstby = loxpdrcode[radnum];
      radioddig1 = 15, radioddig2 = radiodstby/1000, radiodrem2 = radiodstby%1000;
      radioddig2 = radioddig2+208;
      radioddig3 = radiodrem2/100, radiodrem3 = radiodrem2%100;
      radioddig4 = radiodrem3/10, radiodrem4 = radiodrem3%10;
      radioddig5 = radiodrem4;
    }
  
  }

  else if (loseldis[radnum] == 10) {
    radiocdig1 = 15, radiocdig2 = 15, radiocdig3 = 15, radiocdig4 = 15, radiocdig5 = 15;
    radioddig1 = 15, radioddig2 = 15, radioddig3 = 15, radioddig4 = 15, radioddig5 = 15;
  } 

// ****************** Make Message One Digit at A Time ************************
  char radioadigit1 = radioadig1, radioadigit2 = radioadig2, radioadigit3 = radioadig3;
  char radioadigit4 = radioadig4, radioadigit5 = radioadig5;
  char radiobdigit1 = radiobdig1, radiobdigit2 = radiobdig2, radiobdigit3 = radiobdig3;
  char radiobdigit4 = radiobdig4, radiobdigit5 = radiobdig5;	
  char radiocdigit1 = radiocdig1, radiocdigit2 = radiocdig2, radiocdigit3 = radiocdig3;
  char radiocdigit4 = radiocdig4, radiocdigit5 = radiocdig5;
  char radioddigit1 = radioddig1, radioddigit2 = radioddig2, radioddigit3 = radioddig3;
  char radioddigit4 = radioddig4, radioddigit5 = radioddig5;

// ******************* Load Array with Message of Digits *********************
  radiowbuf[radnum][0] = 0; 
  radiowbuf[radnum][1] = radioadigit1, radiowbuf[radnum][2] = radioadigit2, radiowbuf[radnum][3] = radioadigit3; 
  radiowbuf[radnum][4] = radioadigit4, radiowbuf[radnum][5] = radioadigit5;
  radiowbuf[radnum][6] = radiobdigit1, radiowbuf[radnum][7] = radiobdigit2, radiowbuf[radnum][8] = radiobdigit3; 
  radiowbuf[radnum][9] = radiobdigit4, radiowbuf[radnum][10] = radiobdigit5;
  radiowbuf[radnum][11] = radiocdigit1, radiowbuf[radnum][12] = radiocdigit2, radiowbuf[radnum][13] = radiocdigit3; 
  radiowbuf[radnum][14] = radiocdigit4, radiowbuf[radnum][15] = radiocdigit5;
  radiowbuf[radnum][16] = radioddigit1, radiowbuf[radnum][17] = radioddigit2, radiowbuf[radnum][18] = radioddigit3; 
  radiowbuf[radnum][19] = radioddigit4, radiowbuf[radnum][20] = radioddigit5;

// ******* Only do a read if something new to be read ********

  hid_set_nonblocking(radiohandle[radnum], 1);
  radiores = hid_read(radiohandle[radnum], radiobuf[radnum], sizeof(radiobuf[radnum]));
  if (radiores > 0) {
    radres = hid_send_feature_report(radiohandle[radnum], radiowbuf[radnum], 23);
    radionowrite[radnum] = 1;
  }

  // ***** Trying to only write on changes ********

  if (lastupseldis[radnum] == upseldis[radnum]) {
  }
  else {
      if (radionowrite[radnum] == 1) {

      }
      else {
          radres = hid_send_feature_report(radiohandle[radnum], radiowbuf[radnum], 23);
          radionowrite[radnum] = 1;
          lastupseldis[radnum] = upseldis[radnum];
      }
  }

  if (lastloseldis[radnum] == loseldis[radnum]) {
  }
  else {
      if (radionowrite[radnum] == 1) {

      }
      else {
          radres = hid_send_feature_report(radiohandle[radnum], radiowbuf[radnum], 23);
          radionowrite[radnum] = 1;
          lastloseldis[radnum] = loseldis[radnum];
      }
  }

  if (radionowrite[radnum] == 50) {
      radres = hid_send_feature_report(radiohandle[radnum], radiowbuf[radnum], 23);
      radionowrite[radnum] = 0;
  }
  else {
      radionowrite[radnum]++;
  }


// ***************** Upper COM1 Switch Position *******************

    if(testbit(radiobuf[radnum],UPPER_COM1)) {
      upseldis[radnum] = 1;
      if (radiores > 0) {
        if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
          upcom1dbncfninc[radnum]++;
          if (upcom1dbncfninc[radnum] > radspeed) {
	    XPLMCommandOnce(Com1StbyFineUp);
	    upcom1dbncfninc[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
	  upcom1dbncfndec[radnum]++;
          if (upcom1dbncfndec[radnum] > radspeed) {
	    XPLMCommandOnce(Com1StbyFineDn);
	    upcom1dbncfndec[radnum] = 0;
	  }	
        }
        if(testbit(radiobuf[radnum],UPPER_COARSE_UP)) {
	  upcom1dbnccorinc[radnum]++;
          if (upcom1dbnccorinc[radnum] > radspeed) {
	    XPLMCommandOnce(Com1StbyCorseUp);
	    upcom1dbnccorinc[radnum] = 0;
	  }
        }  
        if(testbit(radiobuf[radnum],UPPER_COARSE_DN)) {
	  upcom1dbnccordec[radnum]++;
          if (upcom1dbnccordec[radnum] > radspeed) {
	    XPLMCommandOnce(Com1StbyCorseDn);
	    upcom1dbnccordec[radnum] = 0;
	  }
        }
	if(testbit(radiobuf[radnum],UPPER_ACT_STBY)) {
          XPLMCommandOnce(Com1ActStby);
        }
      }   
    upactcomnavfreq[radnum] = XPLMGetDatai(Com1ActFreq);
    upstbycomnavfreq[radnum] = XPLMGetDatai(Com1StbyFreq);
    upcom1[radnum] = 1;
    }

// ***************** Upper COM2 Switch Position *******************

    if(testbit(radiobuf[radnum],UPPER_COM2)) {
      upseldis[radnum] = 2;
      if (radiores > 0) {
        if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
          upcom2dbncfninc[radnum]++;
          if (upcom2dbncfninc[radnum] > radspeed) {
	    XPLMCommandOnce(Com2StbyFineUp);
	    upcom2dbncfninc[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
	  upcom2dbncfndec[radnum]++;
          if (upcom2dbncfndec[radnum] > radspeed) {
	    XPLMCommandOnce(Com2StbyFineDn);
	    upcom2dbncfndec[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],UPPER_COARSE_UP)) {
	  upcom2dbnccorinc[radnum]++;
          if (upcom2dbnccorinc[radnum] > radspeed) {
	    XPLMCommandOnce(Com2StbyCorseUp);
	    upcom2dbnccorinc[radnum] = 0;
	  }   
        }  
        if(testbit(radiobuf[radnum],UPPER_COARSE_DN)) {
	  upcom2dbnccordec[radnum]++;
          if (upcom2dbnccordec[radnum] > radspeed) {
	    XPLMCommandOnce(Com2StbyCorseDn);
	    upcom2dbnccordec[radnum] = 0;
	  }
        }
	if(testbit(radiobuf[radnum],UPPER_ACT_STBY)) {
	  XPLMCommandOnce(Com2ActStby);
        }
      }
    upactcomnavfreq[radnum] = XPLMGetDatai(Com2ActFreq);
    upstbycomnavfreq[radnum] = XPLMGetDatai(Com2StbyFreq);
    upcom2[radnum] = 1;	 
    }

// ***************** Upper NAV1 Switch Position *******************

    if(testbit(radiobuf[radnum],UPPER_NAV1)) {
      upseldis[radnum] = 3;
      if (radiores > 0) {
        if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
          upnav1dbncfninc[radnum]++;
          if (upnav1dbncfninc[radnum] > radspeed) {
	    XPLMCommandOnce(Nav1StbyFineUp);
	    upnav1dbncfninc[radnum] = 0;
	  } 
        }
        if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
	  upnav1dbncfndec[radnum]++;
          if (upnav1dbncfndec[radnum] > radspeed) {
	    XPLMCommandOnce(Nav1StbyFineDn);
	    upnav1dbncfndec[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],UPPER_COARSE_UP)) {
	  upnav1dbnccorinc[radnum]++;
          if (upnav1dbnccorinc[radnum] > radspeed) {
	    XPLMCommandOnce(Nav1StbyCorseUp);
	    upnav1dbnccorinc[radnum] = 0;
	  }
        }  
        if(testbit(radiobuf[radnum],UPPER_COARSE_DN)) {
	  upnav1dbnccordec[radnum]++;
          if (upnav1dbnccordec[radnum] > radspeed) {
	    XPLMCommandOnce(Nav1StbyCorseDn);
	    upnav1dbnccordec[radnum] = 0;
	  }
        }
	if(testbit(radiobuf[radnum],UPPER_ACT_STBY)) {
	  XPLMCommandOnce(Nav1ActStby);
        }
      } 

    upactcomnavfreq[radnum] = XPLMGetDatai(Nav1ActFreq);
    upstbycomnavfreq[radnum] = XPLMGetDatai(Nav1StbyFreq);
    upnav1[radnum] = 1;	 
    }

// ***************** Upper NAV2 Switch Position *******************

    if(testbit(radiobuf[radnum],UPPER_NAV2)) {
      upseldis[radnum] = 4;
      if (radiores > 0) {
        if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
          upnav2dbncfninc[radnum]++;
          if (upnav2dbncfninc[radnum] > radspeed) {
	    XPLMCommandOnce(Nav2StbyFineUp);
	    upnav2dbncfninc[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
	  upnav2dbncfndec[radnum]++;
          if (upnav2dbncfndec[radnum] > radspeed) {
	    XPLMCommandOnce(Nav2StbyFineDn);
	    upnav2dbncfndec[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],UPPER_COARSE_UP)) {
	  upnav2dbnccorinc[radnum]++;
          if (upnav2dbnccorinc[radnum] > radspeed) {
	    XPLMCommandOnce(Nav2StbyCorseUp);
	    upnav2dbnccorinc[radnum] = 0;
	  }
        }  
        if(testbit(radiobuf[radnum],UPPER_COARSE_DN)) {
	  upnav2dbnccordec[radnum]++;
          if (upnav2dbnccordec[radnum] > radspeed) {
	    XPLMCommandOnce(Nav2StbyCorseDn);
	    upnav2dbnccordec[radnum] = 0;
	  }
        }

	if(testbit(radiobuf[radnum],UPPER_ACT_STBY)) {
	  XPLMCommandOnce(Nav2ActStby);
        }
      }

    upactcomnavfreq[radnum] = XPLMGetDatai(Nav2ActFreq);
    upstbycomnavfreq[radnum] = XPLMGetDatai(Nav2StbyFreq);
    upnav2[radnum] = 1;	 
    }

// ***************** Upper AFD Switch Position *******************

    if(testbit(radiobuf[radnum],UPPER_ADF)) {
      upseldis[radnum] = 5;
      if (radiores > 0) {
        if (upadfsel[radnum] == 1) {
	  if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
	    upadfdbncfninc[radnum]++;
            if (upadfdbncfninc[radnum] > radspeed) {
              XPLMCommandOnce(Afd1StbyOnesUp);
	      upadfdbncfninc[radnum] = 0;
	    }
          }
          if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
	    upadfdbncfndec[radnum]++;
            if (upadfdbncfndec[radnum] > radspeed) {
              XPLMCommandOnce(Afd1StbyOnesDn);
	      upadfdbncfndec[radnum] = 0;
	    }
          }
	}
          if (upadfsel[radnum] == 2) {
            if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
	      upadfdbncfninc[radnum]++;
              if (upadfdbncfninc[radnum] > radspeed) {
                XPLMCommandOnce(Afd1StbyTensUp);
	        upadfdbncfninc[radnum] = 0;
	      }		
            }
            if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
	      upadfdbncfndec[radnum]++;
              if (upadfdbncfndec[radnum] > radspeed) {
                XPLMCommandOnce(Afd1StbyTensDn);
	        upadfdbncfndec[radnum] = 0;
	      } 
            }
	  }
          if (upadfsel[radnum] == 3) {
            if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
              upadfdbncfninc[radnum]++;
              if (upadfdbncfninc[radnum] > radspeed) {
                XPLMCommandOnce(Afd1StbyHunUp);
                upadfdbncfninc[radnum] = 0;
              }
            }
            if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
              upadfdbncfndec[radnum]++;
              if (upadfdbncfndec[radnum] > radspeed) {
                XPLMCommandOnce(Afd1StbyHunDn);
                upadfdbncfndec[radnum] = 0;
              }
            }
          }

  // Use the Coarse knob to select digit in the up direction
          if(testbit(radiobuf[radnum],UPPER_COARSE_UP)) {
            upadfdbnccorinc[radnum] ++;
            if(upadfdbnccorinc[radnum] == 3) {
              upadfsel[radnum] ++;
              upadfdbnccorinc[radnum] = 0;
            }
            if (upadfsel[radnum] == 4) {
              upadfsel[radnum] = 1;
            }
          }

  // Use the Coarse knob to select digit in the down direction
          if(testbit(radiobuf[radnum],UPPER_COARSE_DN)) {
            upadfdbnccordec[radnum] ++;
            if(upadfdbnccordec[radnum] == 3) {
              upadfsel[radnum] --;
              upadfdbnccordec[radnum] = 0;
            }
            if (upadfsel[radnum] == 0) {
              upadfsel[radnum] = 3;
            }
          }

          if(testbit(radiobuf[radnum],UPPER_ACT_STBY)) {
            XPLMCommandOnce(Adf1ActStby);
          }

        }
        upactadffreq[radnum] = XPLMGetDatai(Adf1ActFreq);
        upstbyadffreq[radnum] = XPLMGetDatai(Adf1StbyFreq);
    }

// ***************** Upper DME Switch Position *******************

    if(testbit(radiobuf[radnum],UPPER_DME)) {

        if (updmepushed == 0) {
          if (XPLMGetDatai(DmeMode) == 0) {
             if(testbit(radiobuf[radnum], UPPER_ACT_STBY)) {
                XPLMSetDatai(DmeMode, 1);
                updmepushed = 1;
                uplastdmepos = 1;
              }
          }
        }

        if (updmepushed == 0) {
          if (XPLMGetDatai(DmeMode) == 1) {
             if(testbit(radiobuf[radnum], UPPER_ACT_STBY)) {
                 if (uplastdmepos == 1){
                   XPLMSetDatai(DmeMode, 2);
                   updmepushed = 1;
                 }
                 if (uplastdmepos == 2){
                     XPLMSetDatai(DmeMode, 0);
                     updmepushed = 1;
                 }
              }
          }
        }

        if (updmepushed == 0) {
          if (XPLMGetDatai(DmeMode) == 2) {
              if(testbit(radiobuf[radnum], UPPER_ACT_STBY)) {
                 XPLMSetDatai(DmeMode, 1);
                 updmepushed = 1;
                 uplastdmepos = 2;
              }
          }
        }

        if (updmepushed == 1){
           updmeloop++;
            if (updmeloop == 50){
               updmepushed = 0;
               updmeloop = 0;
            }

        }


      updmemode[radnum] = XPLMGetDatai(DmeMode);
      updmesource[radnum] = XPLMGetDatai(DmeSlvSource);
      if (updmemode[radnum] == 0) {
          upseldis[radnum] = 6;
          if (updmesource[radnum] == 0){
            updmenav1speedf[radnum] = XPLMGetDataf(Nav1DmeSpeed);
            updmenavspeed[radnum] = (int)(updmenav1speedf[radnum]);
            updmedistf[radnum] = XPLMGetDataf(Nav1DmeNmDist);
            updmedist[radnum] = (int)(updmedistf[radnum] * 10.0f);

          }
          else if (updmesource[radnum] == 1){
            updmenav2speedf[radnum] = XPLMGetDataf(Nav2DmeSpeed);
            updmenavspeed[radnum] = (int)(updmenav2speedf[radnum]);
            updmedistf[radnum] = XPLMGetDataf(Nav2DmeNmDist);
            updmedist[radnum] = (int)(updmedistf[radnum] * 10.0f);

           }

      }
      if (updmemode[radnum] == 1) {
          upseldis[radnum] = 7;

          updmefreq[radnum] = XPLMGetDatai(DmeFreq);
          updmefreqhnd = updmefreq[radnum]/100;
          updmefreqfrc = updmefreq[radnum]%100;
          if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
            if (updmefreqfrc == 95) {
                updmefreqfrc = 0;
            }
            updmedbncfninc[radnum]++;
            if (updmedbncfninc[radnum] == 3) {
              updmefreqfrc = updmefreqfrc + 5;
              updmedbncfninc[radnum] = 0;
            }
          }
          if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
            if (updmefreqfrc == 0) {
                updmefreqfrc = 95;
            }
            updmedbncfndec[radnum]++;
            if (updmedbncfndec[radnum] == 3) {
              updmefreqfrc = updmefreqfrc - 5;
              updmedbncfndec[radnum] = 0;
            }
          }

          if(testbit(radiobuf[radnum],UPPER_COARSE_UP)) {
            if (updmefreqhnd == 117) {
                updmefreqhnd = 108;
            }
            updmedbncfninc[radnum]++;
            if (updmedbncfninc[radnum] == 3) {
              updmefreqhnd = updmefreqhnd + 1;
              updmedbncfninc[radnum] = 0;
            }
          }
          if(testbit(radiobuf[radnum],UPPER_COARSE_DN)) {
            if (updmefreqhnd == 108) {
                updmefreqhnd = 117;
            }
            updmedbncfndec[radnum]++;
            if (updmedbncfndec[radnum] == 3) {
              updmefreqhnd = updmefreqhnd - 1;
              updmedbncfndec[radnum] = 0;
            }
          }


          updmefreq[radnum] = (updmefreqhnd * 100) + updmefreqfrc;
          XPLMSetDatai(DmeFreq, updmefreq[radnum]);
          updmetime[radnum] = XPLMGetDataf(DmeTime);

      }

      if (updmemode[radnum] == 2) {
          upseldis[radnum] = 8;
          updmespeedf[radnum] = XPLMGetDataf(DmeSpeed);
          updmespeed[radnum] = (int)(updmespeedf[radnum]);
          updmetime[radnum] = XPLMGetDataf(DmeTime);
      }

    }

// ***************** Upper Transponder Switch Position *******************

    if(testbit(radiobuf[radnum],UPPER_XPDR)) {
      upseldis[radnum] = 9;
      if (radiores > 0) {
        if (upxpdrsel[radnum] == 1) {	
          if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
            upxpdrdbncfninc[radnum]++;
            if (upxpdrdbncfninc[radnum] > radspeed) {
	      XPLMCommandOnce(XpdrOnesUp);
	      upxpdrdbncfninc[radnum] = 0;
	    }
          }
          if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
	    upxpdrdbncfndec[radnum]++;
            if (upxpdrdbncfndec[radnum] > radspeed) {
	      XPLMCommandOnce(XpdrOnesDn);
	      upxpdrdbncfndec[radnum] = 0;
	    }
          }

	}

        else if (upxpdrsel[radnum] == 2) { 
	  if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
	    upxpdrdbncfninc[radnum]++;
            if (upxpdrdbncfninc[radnum] > radspeed) {
              XPLMCommandOnce(XpdrTensUp);
	      upxpdrdbncfninc[radnum] = 0;
	    }
          }
          if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
	    upxpdrdbncfndec[radnum]++;
            if (upxpdrdbncfndec[radnum] > radspeed) {
              XPLMCommandOnce(XpdrTensDn);
	      upxpdrdbncfndec[radnum] = 0;
	    }
          }

        }

        else if (upxpdrsel[radnum] == 3) {
          if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
            upxpdrdbncfninc[radnum]++;
            if (upxpdrdbncfninc[radnum] > radspeed) {
              XPLMCommandOnce(XpdrHunUp);
              upxpdrdbncfninc[radnum] = 0;
            }
          }
          if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
            upxpdrdbncfndec[radnum]++;
            if (upxpdrdbncfndec[radnum] > radspeed) {
              XPLMCommandOnce(XpdrHunDn);
              upxpdrdbncfndec[radnum] = 0;
            }
          }

        }

        else if (upxpdrsel[radnum] == 4) {
          if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
            upxpdrdbncfninc[radnum]++;
            if (upxpdrdbncfninc[radnum] > radspeed) {
              XPLMCommandOnce(XpdrThUp);
              upxpdrdbncfninc[radnum] = 0;
            }
          }
          if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
            upxpdrdbncfndec[radnum]++;
            if (upxpdrdbncfndec[radnum] > radspeed) {
              XPLMCommandOnce(XpdrThDn);
              upxpdrdbncfndec[radnum] = 0;
            }
          }

        }

  // Use the Coarse knob to select digit in the up direction
          if(testbit(radiobuf[radnum],UPPER_COARSE_UP)) {
            upxpdrdbnccorinc[radnum]++;
            if(upxpdrdbnccorinc[radnum] == 3) {
               upxpdrsel[radnum] ++;
               upxpdrdbnccorinc[radnum] = 0;
            }
            if (upxpdrsel[radnum] == 5) {
	      upxpdrsel[radnum] = 1;
	    }
	  }
  // Use the Coarse knob to select digit in the down direction
          if(testbit(radiobuf[radnum],UPPER_COARSE_DN)) {
            upxpdrdbnccordec[radnum]++;
            if(upxpdrdbnccordec[radnum] == 3) {
               upxpdrsel[radnum] --;
               upxpdrdbnccordec[radnum] = 0;
            }
            if (upxpdrsel[radnum] == 0) {
              upxpdrsel[radnum] = 4;
            }
          }

      }
    upxpdrcode[radnum] = XPLMGetDatai(XpdrCode);
    }

// ***************** Lower COM1 Switch Position *******************

    if(testbit(radiobuf[radnum],LOWER_COM1)) {
      loseldis[radnum] = 1;
      if (radiores > 0) {
        if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
          locom1dbncfninc[radnum]++;
          if (locom1dbncfninc[radnum] > radspeed) {
            XPLMCommandOnce(Com1StbyFineUp);
            locom1dbncfninc[radnum] = 0;
          }
        }
        if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
	  locom1dbncfndec[radnum]++;
          if (locom1dbncfndec[radnum] > radspeed) {
	    XPLMCommandOnce(Com1StbyFineDn);
	    locom1dbncfndec[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],LOWER_COARSE_UP)) {
	  locom1dbnccorinc[radnum]++;
          if (locom1dbnccorinc[radnum] > radspeed) {
	    XPLMCommandOnce(Com1StbyCorseUp);
	    locom1dbnccorinc[radnum] = 0;
	  }
        }  
        if(testbit(radiobuf[radnum],LOWER_COARSE_DN)) {
	  locom1dbnccordec[radnum]++;
          if (locom1dbnccordec[radnum] > radspeed) {
	    XPLMCommandOnce(Com1StbyCorseDn);
	    locom1dbnccordec[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],LOWER_ACT_STBY)) {
	  XPLMCommandOnce(Com1ActStby);
        }
      }
    loactcomnavfreq[radnum] = XPLMGetDatai(Com1ActFreq);
    lostbycomnavfreq[radnum] = XPLMGetDatai(Com1StbyFreq);
    locom1[radnum] = 1;	 
    }

// ***************** Lower COM2 Switch Position *******************

   if(testbit(radiobuf[radnum],LOWER_COM2)) {
     loseldis[radnum] = 2;
     if (radiores > 0) {
       if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
         locom2dbncfninc[radnum]++;
         if (locom2dbncfninc[radnum] > radspeed) {
           XPLMCommandOnce(Com2StbyFineUp);
	   locom2dbncfninc[radnum] = 0;
	 }
       }
       if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
         locom2dbncfndec[radnum]++;
         if (locom2dbncfndec[radnum] > radspeed) {
	   XPLMCommandOnce(Com2StbyFineDn);
	   locom2dbncfndec[radnum] = 0;
	 }
       }
       if(testbit(radiobuf[radnum],LOWER_COARSE_UP)) {
         locom2dbnccorinc[radnum]++;
         if (locom2dbnccorinc[radnum] > radspeed) {
	   XPLMCommandOnce(Com2StbyCorseUp);
	   locom2dbnccorinc[radnum] = 0;
	 }
       }  
       if(testbit(radiobuf[radnum],LOWER_COARSE_DN)) {
         locom2dbnccordec[radnum]++;
         if (locom2dbnccordec[radnum] > radspeed) {
	   XPLMCommandOnce(Com2StbyCorseDn);
	   locom2dbnccordec[radnum] = 0;
	 }
       }
       if(testbit(radiobuf[radnum],LOWER_ACT_STBY)) {
         XPLMCommandOnce(Com2ActStby);
       }
     }
    loactcomnavfreq[radnum] = XPLMGetDatai(Com2ActFreq);
    lostbycomnavfreq[radnum] = XPLMGetDatai(Com2StbyFreq);
    locom2[radnum] = 1;
    }

// ***************** Lower NAV1 Switch Position *******************

    if(testbit(radiobuf[radnum],LOWER_NAV1)) {
      loseldis[radnum] = 3;
      if (radiores > 0) {
        if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
          lonav1dbncfninc[radnum]++;
          if (lonav1dbncfninc[radnum] > radspeed) {
	    XPLMCommandOnce(Nav1StbyFineUp);
	    lonav1dbncfninc[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
	  lonav1dbncfndec[radnum]++;
          if (lonav1dbncfndec[radnum] > radspeed) {
	    XPLMCommandOnce(Nav1StbyFineDn);
	    lonav1dbncfndec[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],LOWER_COARSE_UP)) {
	  lonav1dbnccorinc[radnum]++;
          if (lonav1dbnccorinc[radnum] > radspeed) {
	    XPLMCommandOnce(Nav1StbyCorseUp);
	    lonav1dbnccorinc[radnum] = 0;
	  }
        }  
        if(testbit(radiobuf[radnum],LOWER_COARSE_DN)) {
	  lonav1dbnccordec[radnum]++;
          if (lonav1dbnccordec[radnum] > radspeed) {
	    XPLMCommandOnce(Nav1StbyCorseDn);
	    lonav1dbnccordec[radnum] = 0;
	  }
        }

	if(testbit(radiobuf[radnum],LOWER_ACT_STBY)) {
	  XPLMCommandOnce(Nav1ActStby);
        }
      } 
    loactcomnavfreq[radnum] = XPLMGetDatai(Nav1ActFreq);
    lostbycomnavfreq[radnum] = XPLMGetDatai(Nav1StbyFreq);
    lonav1[radnum] = 0;	 
    }

// ***************** Lower NAV2 Switch Position *******************

    if(testbit(radiobuf[radnum],LOWER_NAV2)) {
      loseldis[radnum] = 4;
      if (radiores > 0) {
        if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
          lonav2dbncfninc[radnum]++;
          if (lonav2dbncfninc[radnum] > radspeed) {
	    XPLMCommandOnce(Nav2StbyFineUp);
	    lonav2dbncfninc[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
	  lonav2dbncfndec[radnum]++;
          if (lonav2dbncfndec[radnum] > radspeed) {
	    XPLMCommandOnce(Nav2StbyFineDn);
	    lonav2dbncfndec[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],LOWER_COARSE_UP)) {
	  lonav2dbnccorinc[radnum]++;
          if (lonav2dbnccorinc[radnum] > radspeed) {
	    XPLMCommandOnce(Nav2StbyCorseUp);
	    lonav2dbnccorinc[radnum] = 0;
	  }
        }  
        if(testbit(radiobuf[radnum],LOWER_COARSE_DN)) {
	  lonav2dbnccordec[radnum]++;
          if (lonav2dbnccordec[radnum] > radspeed) {
	    XPLMCommandOnce(Nav2StbyCorseDn);
	    lonav2dbnccordec[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],LOWER_ACT_STBY)) {
	  XPLMCommandOnce(Nav2ActStby);
        }
      }
    loactcomnavfreq[radnum] = XPLMGetDatai(Nav2ActFreq);
    lostbycomnavfreq[radnum] = XPLMGetDatai(Nav2StbyFreq);
    lonav2[radnum] = 0;	 
    }

// ***************** Lower ADF Switch Position *******************

    if(testbit(radiobuf[radnum],LOWER_ADF)) {
      loseldis[radnum] = 5;
      if (numadf == 1) {
         if (radiores > 0) {
          if (loadfsel[radnum] == 1) {
            if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
              loadfdbncfninc[radnum]++;
              if (loadfdbncfninc[radnum] > radspeed) {
                XPLMCommandOnce(Afd1StbyOnesUp);
                loadfdbncfninc[radnum] = 0;
              }
            }
            if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
              loadfdbncfndec[radnum]++;
              if (loadfdbncfndec[radnum] > radspeed) {
                XPLMCommandOnce(Afd1StbyOnesDn);
                loadfdbncfndec[radnum] = 0;
              }
            }


          }
          if (loadfsel[radnum] == 2) {
	    if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
	      loadfdbncfninc[radnum]++;
              if (loadfdbncfninc[radnum] > radspeed) {
                XPLMCommandOnce(Afd1StbyTensUp);
	        loadfdbncfninc[radnum] = 0;
	      }
            }
            if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
              loadfdbncfndec[radnum]++;
              if (loadfdbncfndec[radnum] > radspeed) {
                XPLMCommandOnce(Afd1StbyTensDn);
                loadfdbncfndec[radnum] = 0;
              }
            }

          }
            if (loadfsel[radnum] == 3) {
              if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
                loadfdbncfninc[radnum]++;
                if (loadfdbncfninc[radnum] > radspeed) {
                  XPLMCommandOnce(Afd1StbyHunUp);
                  loadfdbncfninc[radnum] = 0;
                }
              }
              if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
                loadfdbncfndec[radnum]++;
                if (loadfdbncfndec[radnum] > radspeed) {
                  XPLMCommandOnce(Afd1StbyHunDn);
                  loadfdbncfndec[radnum] = 0;
                }
              }
          }

  // Use the Coarse knob to select digit in the up direction
          if(testbit(radiobuf[radnum],LOWER_COARSE_UP)) {
            loadfdbnccorinc[radnum] ++;
            if(loadfdbnccorinc[radnum] == 3) {
              loadfsel[radnum] ++;
              loadfdbnccorinc[radnum] = 0;
            }
            if (loadfsel[radnum] == 4) {
              loadfsel[radnum] = 1;
            }
          }
  // Use the Coarse knob to select digit in the down direction
          if(testbit(radiobuf[radnum],LOWER_COARSE_DN)) {
            loadfdbnccordec[radnum] ++;
            if(loadfdbnccordec[radnum] == 3) {
              loadfsel[radnum] --;
              loadfdbnccordec[radnum] = 0;
            }
            if (loadfsel[radnum] == 0) {
              loadfsel[radnum] = 3;
            }
          }

          if(testbit(radiobuf[radnum],LOWER_ACT_STBY)) {
            XPLMCommandOnce(Adf1ActStby);
          }
         }
        loactadffreq[radnum] = XPLMGetDatai(Adf1ActFreq);
        lostbyadffreq[radnum] = XPLMGetDatai(Adf1StbyFreq);

      }

// Second ADF on the lower position
      if (numadf == 2) {
        if (radiores > 0) {
          if (loadfsel[radnum] == 1) {
            if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
              loadfdbncfninc[radnum]++;
              if (loadfdbncfninc[radnum] > radspeed) {
                XPLMCommandOnce(Afd2StbyOnesUp);
                loadfdbncfninc[radnum] = 0;
              }
            }
            if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
              loadfdbncfndec[radnum]++;
              if (loadfdbncfndec[radnum] > radspeed) {
                XPLMCommandOnce(Afd2StbyOnesDn);
                loadfdbncfndec[radnum] = 0;
              }
            }


          }
          if (loadfsel[radnum] == 2) {
            if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
              loadfdbncfninc[radnum]++;
              if (loadfdbncfninc[radnum] > radspeed) {
                XPLMCommandOnce(Afd2StbyTensUp);
                loadfdbncfninc[radnum] = 0;
              }
            }
            if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
              loadfdbncfndec[radnum]++;
              if (loadfdbncfndec[radnum] > radspeed) {
                XPLMCommandOnce(Afd2StbyTensDn);
                loadfdbncfndec[radnum] = 0;
              }
            }
          }
            if (loadfsel[radnum] == 3) {
              if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
                loadfdbncfninc[radnum]++;
                if (loadfdbncfninc[radnum] > radspeed) {
                  XPLMCommandOnce(Afd2StbyHunUp);
                  loadfdbncfninc[radnum] = 0;
                }
              }
              if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
                loadfdbncfndec[radnum]++;
                if (loadfdbncfndec[radnum] > radspeed) {
                  XPLMCommandOnce(Afd2StbyHunDn);
                  loadfdbncfndec[radnum] = 0;
                }
              }

          }

  // Use the Coarse knob to select digit in the up direction
            if(testbit(radiobuf[radnum],LOWER_COARSE_UP)) {
              loadfdbnccorinc[radnum] ++;
              if(loadfdbnccorinc[radnum] == 3) {
                loadfsel[radnum] ++;
                loadfdbnccorinc[radnum] = 0;
              }
              if (loadfsel[radnum] == 4) {
                loadfsel[radnum] = 1;
              }
            }

   // Use the Coarse knob to select digit in the down direction
            if(testbit(radiobuf[radnum],LOWER_COARSE_DN)) {
              loadfdbnccordec[radnum] ++;
              if(loadfdbnccordec[radnum] == 3) {
                loadfsel[radnum] --;
                loadfdbnccordec[radnum] = 0;
              }
              if (loadfsel[radnum] == 0) {
                loadfsel[radnum] = 3;
              }
            }

            if(testbit(radiobuf[radnum],LOWER_ACT_STBY)) {
              XPLMCommandOnce(Adf2ActStby);
            }

        }
        loactadffreq[radnum] = XPLMGetDatai(Adf2ActFreq);
        lostbyadffreq[radnum] = XPLMGetDatai(Adf2StbyFreq);
      }

    }

// ***************** Lower DME Switch Position *******************

    if(testbit(radiobuf[radnum],LOWER_DME)) {

        if (lodmepushed == 0) {
          if (XPLMGetDatai(DmeMode) == 0) {
             if(testbit(radiobuf[radnum], LOWER_ACT_STBY)) {
                XPLMSetDatai(DmeMode, 1);
                lodmepushed = 1;
                lolastdmepos = 1;
              }
          }
        }

        if (lodmepushed == 0) {
          if (XPLMGetDatai(DmeMode) == 1) {
             if(testbit(radiobuf[radnum], LOWER_ACT_STBY)) {
                 if (lolastdmepos == 1){
                   XPLMSetDatai(DmeMode, 2);
                   lodmepushed = 1;
                 }
                 if (lolastdmepos == 2){
                     XPLMSetDatai(DmeMode, 0);
                     lodmepushed = 1;
                 }
              }
          }
        }

        if (lodmepushed == 0) {
          if (XPLMGetDatai(DmeMode) == 2) {
              if(testbit(radiobuf[radnum], LOWER_ACT_STBY)) {
                 XPLMSetDatai(DmeMode, 1);
                 lodmepushed = 1;
                 lolastdmepos = 2;
              }
          }
        }

        if (lodmepushed == 1){
           lodmeloop++;
            if (lodmeloop == 50){
               lodmepushed = 0;
               lodmeloop = 0;
            }

        }

        lodmemode[radnum] = XPLMGetDatai(DmeMode);
        lodmesource[radnum] = XPLMGetDatai(DmeSlvSource);
        if (lodmemode[radnum] == 0) {
            loseldis[radnum] = 6;
            if (lodmesource[radnum] == 0){
              lodmenav1speedf[radnum] = XPLMGetDataf(Nav1DmeSpeed);
              lodmenavspeed[radnum] = (int)(lodmenav1speedf[radnum]);
              lodmedistf[radnum] = XPLMGetDataf(Nav1DmeNmDist);
              lodmedist[radnum] = (int)(lodmedistf[radnum] * 10.0f);

            }
            else if (lodmesource[radnum] == 1){
              lodmenav2speedf[radnum] = XPLMGetDataf(Nav2DmeSpeed);
              lodmenavspeed[radnum] = (int)(lodmenav2speedf[radnum]);
              lodmedistf[radnum] = XPLMGetDataf(Nav2DmeNmDist);
              lodmedist[radnum] = (int)(lodmedistf[radnum] * 10.0f);

             }

        }
        if (lodmemode[radnum] == 1) {
            loseldis[radnum] = 7;

            lodmefreq[radnum] = XPLMGetDatai(DmeFreq);
            lodmefreqhnd = lodmefreq[radnum]/100;
            lodmefreqfrc = lodmefreq[radnum]%100;
            if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
              if (lodmefreqfrc == 95) {
                  lodmefreqfrc = 0;
              }
              lodmedbncfninc[radnum]++;
              if (lodmedbncfninc[radnum] == 3) {
                lodmefreqfrc = lodmefreqfrc + 5;
                lodmedbncfninc[radnum] = 0;
              }
            }
            if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
              if (lodmefreqfrc == 0) {
                  lodmefreqfrc = 95;
              }
              lodmedbncfndec[radnum]++;
              if (lodmedbncfndec[radnum] == 3) {
                lodmefreqfrc = lodmefreqfrc - 5;
                lodmedbncfndec[radnum] = 0;
              }
            }

            if(testbit(radiobuf[radnum],LOWER_COARSE_UP)) {
              if (lodmefreqhnd == 117) {
                  lodmefreqhnd = 108;
              }
              lodmedbncfninc[radnum]++;
              if (lodmedbncfninc[radnum] == 3) {
                lodmefreqhnd = lodmefreqhnd + 1;
                lodmedbncfninc[radnum] = 0;
              }
            }
            if(testbit(radiobuf[radnum],LOWER_COARSE_DN)) {
              if (lodmefreqhnd == 108) {
                  lodmefreqhnd = 117;
              }
              lodmedbncfndec[radnum]++;
              if (lodmedbncfndec[radnum] == 3) {
                lodmefreqhnd = lodmefreqhnd - 1;
                lodmedbncfndec[radnum] = 0;
              }
            }


            lodmefreq[radnum] = (lodmefreqhnd * 100) + lodmefreqfrc;
            XPLMSetDatai(DmeFreq, lodmefreq[radnum]);
            lodmetime[radnum] = XPLMGetDataf(DmeTime);
        }

        if (lodmemode[radnum] == 2) {
            loseldis[radnum] = 8;
            lodmespeedf[radnum] = XPLMGetDataf(DmeSpeed);
            lodmespeed[radnum] = (int)(lodmespeedf[radnum]);
            lodmetime[radnum] = XPLMGetDataf(DmeTime);

        }

    }

// ***************** Lower Transponder Switch Position *******************
 
   if(testbit(radiobuf[radnum],LOWER_XPDR)) {
     loseldis[radnum] = 9;
     if (radiores > 0) {
       if (loxpdrsel[radnum] == 1) {	
         if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
	   loxpdrdbncfninc[radnum]++;
           if (loxpdrdbncfninc[radnum] > radspeed) {
	     XPLMCommandOnce(XpdrOnesUp);
	     loxpdrdbncfninc[radnum] = 0;
	   }
         }
         if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
	   loxpdrdbncfndec[radnum]++;
           if (loxpdrdbncfndec[radnum] > radspeed) {
	     XPLMCommandOnce(XpdrOnesDn);
	     loxpdrdbncfndec[radnum] = 0;
	   }
         }
        }
         if (loxpdrsel[radnum] == 2) {	
           if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
	     loxpdrdbncfninc[radnum]++;
             if (loxpdrdbncfninc[radnum] > radspeed) {
               XPLMCommandOnce(XpdrTensUp);
	       loxpdrdbncfninc[radnum] = 0;
	     }
           }
           if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
	     loxpdrdbncfndec[radnum]++;
             if (loxpdrdbncfndec[radnum] > radspeed) {
               XPLMCommandOnce(XpdrTensDn);
	       loxpdrdbncfndec[radnum] = 0;
	     }
           }

	 }

         if (loxpdrsel[radnum] == 3) {
           if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
             loxpdrdbncfninc[radnum]++;
             if (loxpdrdbncfninc[radnum] > radspeed) {
               XPLMCommandOnce(XpdrHunUp);
               loxpdrdbncfninc[radnum] = 0;
             }
           }
           if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
             loxpdrdbncfndec[radnum]++;
             if (loxpdrdbncfndec[radnum] > radspeed) {
               XPLMCommandOnce(XpdrHunDn);
               loxpdrdbncfndec[radnum] = 0;
             }
           }

         }

         if (loxpdrsel[radnum] == 4) {
           if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
             loxpdrdbncfninc[radnum]++;
             if (loxpdrdbncfninc[radnum] > radspeed) {
               XPLMCommandOnce(XpdrThUp);
               loxpdrdbncfninc[radnum] = 0;
             }
           }
           if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
             loxpdrdbncfndec[radnum]++;
             if (loxpdrdbncfndec[radnum] > radspeed) {
               XPLMCommandOnce(XpdrThDn);
               loxpdrdbncfndec[radnum] = 0;
             }
           }

         }

// Use the Coarse knob to select digit in the up direction
           if(testbit(radiobuf[radnum],LOWER_COARSE_UP)) {
             loxpdrdbnccorinc[radnum]++;
             if(loxpdrdbnccorinc[radnum] == 3) {
                loxpdrsel[radnum] ++;
                loxpdrdbnccorinc[radnum] = 0;
             }

             if (loxpdrsel[radnum] == 5) {
               loxpdrsel[radnum] = 1;
             }
           }
// Use the Coarse knob to select digit in the up direction
           if(testbit(radiobuf[radnum],LOWER_COARSE_DN)) {
             loxpdrdbnccordec[radnum]++;
             if(loxpdrdbnccordec[radnum] == 3) {
                loxpdrsel[radnum] --;
                loxpdrdbnccordec[radnum] = 0;
             }

             if (loxpdrsel[radnum] == 0) {
               loxpdrsel[radnum] = 4;
             }
           }



         }
    loxpdrcode[radnum] = XPLMGetDatai(XpdrCode);
    }

// ***************** Blank Display *******************

	if (XPLMGetDatai(AvPwrOn) == 0) {
          upseldis[radnum] = 10;
          loseldis[radnum] = 10;
	}

	if (XPLMGetDatai(BatPwrOn) == 0) {
          upseldis[radnum] = 10;
          loseldis[radnum] = 10;
	}

	if (XPLMGetDatai(Com1PwrOn) == 0) {
          if (upcom1[radnum] == 1) {
            upseldis[radnum] = 10;
          }
          if (locom1[radnum] == 1) {
            loseldis[radnum] = 10;
          }
	}

	if (XPLMGetDatai(Com2PwrOn) == 0) {
          if (upcom2[radnum] == 2) {
            upseldis[radnum] = 8;
          }
          if (locom2[radnum] == 2) {
            loseldis[radnum] = 10;
          }
	}

	if (XPLMGetDatai(Nav1PwrOn) == 0) {
          if (upnav1[radnum] == 3) {
            upseldis[radnum] = 10;
          }
          if (lonav1[radnum] == 3) {
            loseldis[radnum] = 10;
          }
	}

	if (XPLMGetDatai(Nav2PwrOn) == 0) {
          if (upnav2[radnum] == 4) {
            upseldis[radnum] = 10;
          }
          if (lonav2[radnum] == 4) {
            loseldis[radnum] = 10;
          }
	}

// *********** loop untill all radios serviced *************
// **************   then start again    *******************

  radnum++;
  if (radnum == radcnt) {
    radnum = 0;
  }
  
  return;
}

void process_upper_nav_com_freq()

{
  radioaactv = upactcomnavfreq[radnum];
  radioadig1 = radioaactv/10000, radioarem1 = radioaactv%10000;
  radioadig2 = radioarem1 /1000, radioarem2 = radioarem1%1000;
  radioadig3 = radioarem2/100, radioarem3 = radioarem2%100;
  radioadig4 = radioarem3/10, radioarem4 = radioarem3%10;
  radioadig3 = radioadig3+208, radioadig5 = radioarem4;

  radiobstby = upstbycomnavfreq[radnum];
  radiobdig1 = radiobstby/10000, radiobrem1 = radiobstby%10000;
  radiobdig2 = radiobrem1 /1000, radiobrem2 = radiobrem1%1000;
  radiobdig3 = radiobrem2/100, radiobrem3 = radiobrem2%100;
  radiobdig4 = radiobrem3/10, radiobrem4 = radiobrem3%10;
  radiobdig3 = radiobdig3+208, radiobdig5 = radiobrem4;

  return;
}

void process_lower_nav_com_freq()

{
  radiocactv = loactcomnavfreq[radnum];
  radiocdig1 = radiocactv/10000, radiocrem1 = radiocactv%10000;
  radiocdig2 = radiocrem1 /1000, radiocrem2 = radiocrem1%1000;
  radiocdig3 = radiocrem2/100, radiocrem3 = radiocrem2%100;
  radiocdig4 = radiocrem3/10, radiocrem4 = radiocrem3%10;
  radiocdig3 = radiocdig3+208;
  radiocdig5 = radiocrem4;

  radiodstby = lostbycomnavfreq[radnum];
  radioddig1 = radiodstby/10000, radiodrem1 = radiodstby%10000;
  radioddig2 = radiodrem1 /1000, radiodrem2 = radiodrem1%1000;
  radioddig3 = radiodrem2/100, radiodrem3 = radiodrem2%100;
  radioddig4 = radiodrem3/10, radiodrem4 = radiodrem3%10;
  radioddig3 = radioddig3+208, radioddig5 = radiodrem4;

  return;
}
