#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"

#include "SaitekPanels.h"

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

#define testbit(x, y)  ( ( ((const char*)&(x))[(y)>>3] & 0x80 >> ((y)&0x07)) >> (7-((y)&0x07) ) )

static int radnum = 0, radionowrite[4] = {0, 0, 0, 0};

static int upactcomnavfreq[4], upstbycomnavfreq[4], loactcomnavfreq[4], lostbycomnavfreq[4];
static int upadffreq[4], loadffreq[4], updmedist[4], updmespeed[4], lodmedist[4], lodmespeed[4]; 
static int upxpdrcode[4], loxpdrcode[4], updmesource[4], lodmesource[4];
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

static float updmedistf[4], updmespeedf[4], lodmedistf[4], lodmespeedf[4]; 

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

static int res, wres;

static unsigned char radiobuf[4][3];
static char radiowbuf[4][24];

void process_upper_nav_com_freq();
void process_lower_nav_com_freq();

/***** Radio Panel Process  *******/
void process_radio_panel()

{

  /*************** Upper Display info **********************/ 

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
    radiobstby = upadffreq[radnum];
    radioadig1 = 11, radioadig2 = 11, radioadig3 = 11, radioadig4 = 11, radioadig5 = 11;
    radiobdig1 = 11, radiobdig2 = 11;
    radiobdig3 = radiobstby/100, radiobrem3 =radiobstby%100;
    radiobdig4 = radiobrem3/10, radiobrem4 = radiobrem3%10;
    radiobdig5 = radiobrem4;
  }

  else if (upseldis[radnum] == 6) {
    radioaactv = updmedist[radnum];
    radioadig1 = radioaactv/10000, radioarem1 = radioaactv%10000;
    radioadig2 = radioarem1 /1000, radioarem2 = radioarem1%1000;
    radioadig3 = radioarem2/100, radioarem3 = radioarem2%100;
    radioadig4 = radioarem3/10, radioarem4 = radioarem3%10;
    radioadig4 = radioadig4+208, radioadig5 = radioarem4;

    radiobstby = updmespeed[radnum];
    radiobdig1 = 11, radiobdig2 = 11;
    radiobdig3 = radiobstby/100, radiobrem3 =radiobstby%100;
    radiobdig4 = radiobrem3/10, radiobrem4 = radiobrem3%10;
    radiobdig5 = radiobrem4;
  }

  else if (upseldis[radnum] == 7) {
    radioadig1 = 11, radioadig2 = 11, radioadig3 = 11, radioadig4 = 11, radioadig5 = 11;
    radiobstby = upxpdrcode[radnum];
    radiobdig1 = 11, radiobdig2 = radiobstby/1000, radiobrem2 = radiobstby%1000;
    radiobdig3 = radiobrem2/100, radiobrem3 = radiobrem2%100;
    radiobdig4 = radiobrem3/10, radiobrem4 = radiobrem3%10;
    radiobdig5 = radiobrem4; 
  }

  else if (upseldis[radnum] == 8) {
    radioadig1 = 11, radioadig2 = 11, radioadig3 = 11, radioadig4 = 11, radioadig5 = 11;
    radiobdig1 = 11, radiobdig2 = 11, radiobdig3 = 11, radiobdig4 = 11, radiobdig5 = 11; 
  }

  /************************ Lower Display info ********************/
  
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
    radiodstby = loadffreq[radnum]; 
    radiocdig1 = 11, radiocdig2 = 11, radiocdig3 = 11, radiocdig4 = 11, radiocdig5 = 11;
    radioddig1 = 11, radioddig2 = 11;
    radioddig3 = radiodstby/100, radiodrem3 = radiodstby%100;
    radioddig4 = radiodrem3/10, radiodrem4 = radiodrem3%10;
    radioddig5 = radiodrem4;
  }

  else if (loseldis[radnum] == 6) {
    radiocactv = lodmedist[radnum];
    radiocdig1 = radiocactv/10000, radiocrem1 = radiocactv%10000;
    radiocdig2 = radiocrem1 /1000, radiocrem2 = radiocrem1%1000;
    radiocdig3 = radiocrem2/100, radiocrem3 = radiocrem2%100;
    radiocdig4 = radiocrem3/10, radiocrem4 = radiocrem3%10;
    radiocdig4 = radiocdig4+208, radiocdig5 = radiocrem4;

    radiodstby = lodmespeed[radnum];
    radioddig1 = 11, radioddig2 = 11;
    radioddig3 = radiodstby/100, radiodrem3 = radiodstby%100;
    radioddig4 = radiodrem3/10, radiodrem4 = radiodrem3%10;
    radioddig5 = radiodrem4;
  }

  else if (loseldis[radnum] == 7) { 
    radiocdig1 = 11, radiocdig2 = 11, radiocdig3 = 11, radiocdig4 = 11, radiocdig5 = 11;

    radiodstby = loxpdrcode[radnum];
    radioddig1 = 11, radioddig2 = radiodstby/1000, radiodrem2 = radiodstby%1000;
    radioddig3 = radiodrem2/100, radiodrem3 = radiodrem2%100;
    radioddig4 = radiodrem3/10, radiodrem4 = radiodrem3%10;
    radioddig5 = radiodrem4; 
  
  }

  else if (loseldis[radnum] == 8) {
    radiocdig1 = 11, radiocdig2 = 11, radiocdig3 = 11, radiocdig4 = 11, radiocdig5 = 11;
    radioddig1 = 11, radioddig2 = 11, radioddig3 = 11, radioddig4 = 11, radioddig5 = 11;
  } 


/****************** Make Message One Digit at A Time ************************/ 
  //char radioadigit1 = radioadig1,
  char radioadigit2 = radioadig2, radioadigit3 = radioadig3;
  char radioadigit4 = radioadig4, radioadigit5 = radioadig5;
  char radiobdigit1 = radiobdig1, radiobdigit2 = radiobdig2, radiobdigit3 = radiobdig3;
  char radiobdigit4 = radiobdig4, radiobdigit5 = radiobdig5;	
  char radiocdigit1 = radiocdig1, radiocdigit2 = radiocdig2, radiocdigit3 = radiocdig3;
  char radiocdigit4 = radiocdig4, radiocdigit5 = radiocdig5;
  char radioddigit1 = radioddig1, radioddigit2 = radioddig2, radioddigit3 = radioddig3;
  char radioddigit4 = radioddig4, radioddigit5 = radioddig5;

/******************* Load Array with Message of Digits *********************/
  radiowbuf[radnum][0] = 1; 
  /*radiowbuf[radnum][1] = radioadigit1,*/ 
  radiowbuf[radnum][1] = radioadigit2, radiowbuf[radnum][2] = radioadigit3; 
  radiowbuf[radnum][3] = radioadigit4, radiowbuf[radnum][4] = radioadigit5;
  radiowbuf[radnum][5] = radiobdigit1, radiowbuf[radnum][6] = radiobdigit2, radiowbuf[radnum][7] = radiobdigit3; 
  radiowbuf[radnum][8] = radiobdigit4, radiowbuf[radnum][9] = radiobdigit5;
  radiowbuf[radnum][10] = radiocdigit1, radiowbuf[radnum][11] = radiocdigit2, radiowbuf[radnum][12] = radiocdigit3; 
  radiowbuf[radnum][13] = radiocdigit4, radiowbuf[radnum][14] = radiocdigit5;
  radiowbuf[radnum][15] = radioddigit1, radiowbuf[radnum][16] = radioddigit2, radiowbuf[radnum][17] = radioddigit3; 
  radiowbuf[radnum][18] = radioddigit4, radiowbuf[radnum][19] = radioddigit5;


/******* Only do a read if something new to be read ********/
  int             radiores;
  fd_set          radiosready;
  struct timeval  radionowait;

  FD_ZERO(&radiosready);
  FD_SET((unsigned int)radiofd[radnum],&radiosready);
  radionowait.tv_sec = 0;    // specify how many seconds you would like to wait for timeout
  radionowait.tv_usec = 0;   // how many microseconds? If both is zero, select will return immediately

  radiores = select(radiofd[radnum]+1,&radiosready,NULL,NULL,&radionowait);
  if( FD_ISSET(radiofd[radnum],&radiosready) ) {
    res = read(radiofd[radnum], radiobuf[radnum], sizeof(radiobuf[radnum]));
    wres = write(radiofd[radnum], radiowbuf[radnum], sizeof(radiowbuf[radnum]));
    radionowrite[radnum] = 1;
  }
  else {
  }

  /***** Trying to only write on changes ********/

  if (lastupseldis[radnum] == upseldis[radnum]) {
  }
  else {
    if (radionowrite[radnum] == 1) {
    }
    else {
      wres = write(radiofd[radnum], radiowbuf[radnum], sizeof(radiowbuf[radnum]));
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
      wres = write(radiofd[radnum], radiowbuf[radnum], sizeof(radiowbuf[radnum]));
      radionowrite[radnum] = 1;
      lastloseldis[radnum] = loseldis[radnum];
    }
  }

  if (radionowrite[radnum] == 50) {
    wres = write(radiofd[radnum], radiowbuf[radnum], sizeof(radiowbuf[radnum]));
    radionowrite[radnum] = 0;
  }
  else {
    radionowrite[radnum]++;
  } 

/***************** Upper COM1 Switch Position *******************/

    if(testbit(radiobuf[radnum],UPPER_COM1)) {
      upseldis[radnum] = 1;
      if (radiores == 1) {	    
        if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
          upcom1dbncfninc[radnum]++;
	  if (upcom1dbncfninc[radnum] == 3) {
	    XPLMCommandOnce(Com1StbyFineUp);
	    upcom1dbncfninc[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
	  upcom1dbncfndec[radnum]++;
	  if (upcom1dbncfndec[radnum] == 3) {
	    XPLMCommandOnce(Com1StbyFineDn);
	    upcom1dbncfndec[radnum] = 0;
	  }	
        }
        if(testbit(radiobuf[radnum],UPPER_COARSE_UP)) {
	  upcom1dbnccorinc[radnum]++;
	  if (upcom1dbnccorinc[radnum] == 3) {
	    XPLMCommandOnce(Com1StbyCorseUp);
	    upcom1dbnccorinc[radnum] = 0;
	  }
        }  
        if(testbit(radiobuf[radnum],UPPER_COARSE_DN)) {
	  upcom1dbnccordec[radnum]++;
	  if (upcom1dbnccordec[radnum] == 3) {
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

/***************** Upper COM2 Switch Position *******************/

    if(testbit(radiobuf[radnum],UPPER_COM2)) {
      upseldis[radnum] = 2;
      if (radiores == 1) {
        if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
          upcom2dbncfninc[radnum]++;
	  if (upcom2dbncfninc[radnum] == 3) {
	    XPLMCommandOnce(Com2StbyFineUp);
	    upcom2dbncfninc[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
	  upcom2dbncfndec[radnum]++;
	  if (upcom2dbncfndec[radnum] == 3) {
	    XPLMCommandOnce(Com2StbyFineDn);
	    upcom2dbncfndec[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],UPPER_COARSE_UP)) {
	  upcom2dbnccorinc[radnum]++;
	  if (upcom2dbnccorinc[radnum] == 3) {
	    XPLMCommandOnce(Com2StbyCorseUp);
	    upcom2dbnccorinc[radnum] = 0;
	  }   
        }  
        if(testbit(radiobuf[radnum],UPPER_COARSE_DN)) {
	  upcom2dbnccordec[radnum]++;
	  if (upcom2dbnccordec[radnum] == 3) {
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

/***************** Upper NAV1 Switch Position *******************/

    if(testbit(radiobuf[radnum],UPPER_NAV1)) {
      upseldis[radnum] = 3;
      if (radiores == 1) {  
        if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
          upnav1dbncfninc[radnum]++;
	  if (upnav1dbncfninc[radnum] == 3) {
	    XPLMCommandOnce(Nav1StbyFineUp);
	    upnav1dbncfninc[radnum] = 0;
	  } 
        }
        if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
	  upnav1dbncfndec[radnum]++;
	  if (upnav1dbncfndec[radnum] == 3) {
	    XPLMCommandOnce(Nav1StbyFineDn);
	    upnav1dbncfndec[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],UPPER_COARSE_UP)) {
	  upnav1dbnccorinc[radnum]++;
	  if (upnav1dbnccorinc[radnum] == 3) {
	    XPLMCommandOnce(Nav1StbyCorseUp);
	    upnav1dbnccorinc[radnum] = 0;
	  }
        }  
        if(testbit(radiobuf[radnum],UPPER_COARSE_DN)) {
	  upnav1dbnccordec[radnum]++;
	  if (upnav1dbnccordec[radnum] == 3) {
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

/***************** Upper NAV2 Switch Position *******************/

    if(testbit(radiobuf[radnum],UPPER_NAV2)) {
      upseldis[radnum] = 4;
      if (radiores == 1) {
        if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
          upnav2dbncfninc[radnum]++;
	  if (upnav2dbncfninc[radnum] == 3) {
	    XPLMCommandOnce(Nav2StbyFineUp);
	    upnav2dbncfninc[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
	  upnav2dbncfndec[radnum]++;
	  if (upnav2dbncfndec[radnum] == 3) {
	    XPLMCommandOnce(Nav2StbyFineDn);
	    upnav2dbncfndec[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],UPPER_COARSE_UP)) {
	  upnav2dbnccorinc[radnum]++;
	  if (upnav2dbnccorinc[radnum] == 3) {
	    XPLMCommandOnce(Nav2StbyCorseUp);
	    upnav2dbnccorinc[radnum] = 0;
	  }
        }  
        if(testbit(radiobuf[radnum],UPPER_COARSE_DN)) {
	  upnav2dbnccordec[radnum]++;
	  if (upnav2dbnccordec[radnum] == 3) {
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

/***************** Upper AFD Switch Position *******************/

    if(testbit(radiobuf[radnum],UPPER_ADF)) {
      upseldis[radnum] = 5;
      if (radiores == 1) {
        if (upadfsel[radnum] == 1) {
	  if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
	    upadfdbncfninc[radnum]++;
	    if (upadfdbncfninc[radnum] == 3) {
	      XPLMCommandOnce(Afd1OnesUp);
	      upadfdbncfninc[radnum] = 0;
	    }
          }
          if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
	    upadfdbncfndec[radnum]++;
	    if (upadfdbncfndec[radnum] == 3) {
	      XPLMCommandOnce(Afd1OnesDn);
	      upadfdbncfndec[radnum] = 0;
	    }
          }
          if(testbit(radiobuf[radnum],UPPER_COARSE_UP)) {
	    upadfdbnccorinc[radnum]++;
	    if (upadfdbnccorinc[radnum] == 3) {
	      XPLMCommandOnce(Afd1TensUp);
	      upadfdbnccorinc[radnum] = 0;
	    }
          }  
          if(testbit(radiobuf[radnum],UPPER_COARSE_DN)) {
	    upadfdbnccordec[radnum]++;
	    if (upadfdbnccordec[radnum] == 3) {
	      XPLMCommandOnce(Afd1TensDn);
	      upadfdbncfndec[radnum] = 0;
	    }
          }
	}
          if (upadfsel[radnum] == 2) {
            if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
	      upadfdbncfninc[radnum]++;
	      if (upadfdbncfninc[radnum] == 3) {
	        XPLMCommandOnce(Afd1HunUp);
	        upadfdbncfninc[radnum] = 0;
	      }		
            }
            if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
	      upadfdbncfndec[radnum]++;
	      if (upadfdbncfndec[radnum] == 3) {
		XPLMCommandOnce(Afd1HunDn);
	        upadfdbncfndec[radnum] = 0;
	      } 
            }
            if(testbit(radiobuf[radnum],UPPER_COARSE_UP)) {
	      upadfdbnccorinc[radnum]++;
	      if (upadfdbnccorinc[radnum] == 3) {
		XPLMCommandOnce(Afd1HunUp);
	        upadfdbnccorinc[radnum] = 0;
	      }
            }  
            if(testbit(radiobuf[radnum],UPPER_COARSE_DN)) {
	      upadfdbnccordec[radnum]++;
	      if (upadfdbnccordec[radnum] == 3) {
		XPLMCommandOnce(Afd1HunDn);
	        upadfdbncfndec[radnum] = 0;
	      }
            }	
	  }

	    if(testbit(radiobuf[radnum],UPPER_ACT_STBY)) {
	      upadfsel[radnum] ++;
	      if (upadfsel[radnum] == 3) {
	        upadfsel[radnum] = 1;
	      }
            }
	  }
    upadffreq[radnum] = XPLMGetDatai(AdfFreq);
    }

/***************** Upper DME Switch Position *******************/

    if(testbit(radiobuf[radnum],UPPER_DME)) {
      upseldis[radnum] = 6;
      updmesource[radnum] = XPLMGetDatai(DmeSlvSource);
      if (updmesource[radnum] == 0){
        updmedistf[radnum] = XPLMGetDataf(Nav1DmeNmDist);
	updmedist[radnum] = (int)(updmedistf[radnum] * 10.0f);
	updmespeedf[radnum] = XPLMGetDataf(Nav1DmeSpeed);
	updmespeed[radnum] = (int)(updmespeedf[radnum]);
      }
      else if (updmesource[radnum] == 1){
        updmedistf[radnum] = XPLMGetDataf(Nav2DmeNmDist);
	updmedist[radnum] = (int)(updmedistf[radnum] * 10.0f);
	updmespeedf[radnum] = XPLMGetDataf(Nav2DmeSpeed);
	updmespeed[radnum] = (int)(updmespeedf[radnum]);
       }
    }

/***************** Upper Transponder Switch Position *******************/

    if(testbit(radiobuf[radnum],UPPER_XPDR)) {
      upseldis[radnum] = 7;
      if (radiores == 1) {
        if (upxpdrsel[radnum] == 1) {	
          if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
            upxpdrdbncfninc[radnum]++;
	    if (upxpdrdbncfninc[radnum] == 3) {
	      XPLMCommandOnce(XpdrOnesUp);
	      upxpdrdbncfninc[radnum] = 0;
	    }
          }
          if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
	    upxpdrdbncfndec[radnum]++;
	    if (upxpdrdbncfndec[radnum] == 3) {
	      XPLMCommandOnce(XpdrOnesDn);
	      upxpdrdbncfndec[radnum] = 0;
	    }
          }
          if(testbit(radiobuf[radnum],UPPER_COARSE_UP)) {
	    upxpdrdbnccorinc[radnum]++;
	    if (upxpdrdbnccorinc[radnum] == 3) {
	      XPLMCommandOnce(XpdrTensUp);
	      upxpdrdbnccorinc[radnum] = 0;
	    }
          }  
          if(testbit(radiobuf[radnum],UPPER_COARSE_DN)) {
	    upxpdrdbnccordec[radnum]++;
	    if (upxpdrdbnccordec[radnum] == 3) {
	      XPLMCommandOnce(XpdrTensDn);
	      upxpdrdbnccordec[radnum] = 0;
	    }
          }
	}
        else if (upxpdrsel[radnum] == 2) { 
	  if(testbit(radiobuf[radnum],UPPER_FINE_UP)) {
	    upxpdrdbncfninc[radnum]++;
	    if (upxpdrdbncfninc[radnum] == 3) {
	      XPLMCommandOnce(XpdrHunUp);
	      upxpdrdbncfninc[radnum] = 0;
	    }
          }
          if(testbit(radiobuf[radnum],UPPER_FINE_DN)) {
	    upxpdrdbncfndec[radnum]++;
	    if (upxpdrdbncfndec[radnum] == 3) {
	      XPLMCommandOnce(XpdrHunDn);
	      upxpdrdbncfndec[radnum] = 0;
	    }
          }
          if(testbit(radiobuf[radnum],UPPER_COARSE_UP)) {
	    upxpdrdbnccorinc[radnum]++;
	    if (upxpdrdbnccorinc[radnum] == 3) {
	      XPLMCommandOnce(XpdrThUp);
	      upxpdrdbnccorinc[radnum] = 0;
	    }
          }  
          if(testbit(radiobuf[radnum],UPPER_COARSE_DN)) {
	    upxpdrdbnccordec[radnum]++;
	    if (upxpdrdbnccordec[radnum] == 3) {
	      XPLMCommandOnce(XpdrThDn);
	      upxpdrdbnccordec[radnum] = 0;
	    }
	  }
        }
          if(testbit(radiobuf[radnum],UPPER_ACT_STBY)) {
	    upxpdrsel[radnum] ++;
	    if (upxpdrsel[radnum] == 3) {
	      upxpdrsel[radnum] = 1;
	    }
	  }
        }
    upxpdrcode[radnum] = XPLMGetDatai(XpdrCode);
    }

/***************** Lower COM1 Switch Position *******************/

    if(testbit(radiobuf[radnum],LOWER_COM1)) {
      loseldis[radnum] = 1;
      if (radiores == 1) {
        if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
          locom1dbncfninc[radnum]++;
          if (locom1dbncfninc[radnum] == 3) {
            XPLMCommandOnce(Com1StbyFineUp);
            locom1dbncfninc[radnum] = 0;
          }
        }
        if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
	  locom1dbncfndec[radnum]++;
	  if (locom1dbncfndec[radnum] == 3) {
	    XPLMCommandOnce(Com1StbyFineDn);
	    locom1dbncfndec[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],LOWER_COARSE_UP)) {
	  locom1dbnccorinc[radnum]++;
	  if (locom1dbnccorinc[radnum] == 3) {
	    XPLMCommandOnce(Com1StbyCorseUp);
	    locom1dbnccorinc[radnum] = 0;
	  }
        }  
        if(testbit(radiobuf[radnum],LOWER_COARSE_DN)) {
	  locom1dbnccordec[radnum]++;
	  if (locom1dbnccordec[radnum] == 3) {
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

/***************** Lower COM2 Switch Position *******************/

   if(testbit(radiobuf[radnum],LOWER_COM2)) {
     loseldis[radnum] = 2;
     if (radiores == 1) {
       if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
         locom2dbncfninc[radnum]++;
         if (locom2dbncfninc[radnum] == 3) {
           XPLMCommandOnce(Com2StbyFineUp);
	   locom2dbncfninc[radnum] = 0;
	 }
       }
       if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
         locom2dbncfndec[radnum]++;
	 if (locom2dbncfndec[radnum] == 3) {
	   XPLMCommandOnce(Com2StbyFineDn);
	   locom2dbncfndec[radnum] = 0;
	 }
       }
       if(testbit(radiobuf[radnum],LOWER_COARSE_UP)) {
         locom2dbnccorinc[radnum]++;
	 if (locom2dbnccorinc[radnum] == 3) {
	   XPLMCommandOnce(Com2StbyCorseUp);
	   locom2dbnccorinc[radnum] = 0;
	 }
       }  
       if(testbit(radiobuf[radnum],LOWER_COARSE_DN)) {
         locom2dbnccordec[radnum]++;
         if (locom2dbnccordec[radnum] == 3) {
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

/***************** Lower NAV1 Switch Position *******************/

    if(testbit(radiobuf[radnum],LOWER_NAV1)) {
      loseldis[radnum] = 3;
      if (radiores == 1) {
        if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
          lonav1dbncfninc[radnum]++;
	  if (lonav1dbncfninc[radnum] == 3) {
	    XPLMCommandOnce(Nav1StbyFineUp);
	    lonav1dbncfninc[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
	  lonav1dbncfndec[radnum]++;
	  if (lonav1dbncfndec[radnum] == 3) {
	    XPLMCommandOnce(Nav1StbyFineDn);
	    lonav1dbncfndec[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],LOWER_COARSE_UP)) {
	  lonav1dbnccorinc[radnum]++;
	  if (lonav1dbnccorinc[radnum] == 3) {
	    XPLMCommandOnce(Nav1StbyCorseUp);
	    lonav1dbnccorinc[radnum] = 0;
	  }
        }  
        if(testbit(radiobuf[radnum],LOWER_COARSE_DN)) {
	  lonav1dbnccordec[radnum]++;
	  if (lonav1dbnccordec[radnum] == 3) {
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

/***************** Lower NAV2 Switch Position *******************/

    if(testbit(radiobuf[radnum],LOWER_NAV2)) {
      loseldis[radnum] = 4;
      if (radiores == 1) {
        if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
          lonav2dbncfninc[radnum]++;
	  if (lonav2dbncfninc[radnum] == 3) {
	    XPLMCommandOnce(Nav2StbyFineUp);
	    lonav2dbncfninc[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
	  lonav2dbncfndec[radnum]++;
	  if (lonav2dbncfndec[radnum] == 3) {
	    XPLMCommandOnce(Nav2StbyFineDn);
	    lonav2dbncfndec[radnum] = 0;
	  }
        }
        if(testbit(radiobuf[radnum],LOWER_COARSE_UP)) {
	  lonav2dbnccorinc[radnum]++;
	  if (lonav2dbnccorinc[radnum] == 3) {
	    XPLMCommandOnce(Nav2StbyCorseUp);
	    lonav2dbnccorinc[radnum] = 0;
	  }
        }  
        if(testbit(radiobuf[radnum],LOWER_COARSE_DN)) {
	  lonav2dbnccordec[radnum]++;
	  if (lonav2dbnccordec[radnum] == 3) {
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

/***************** Lower ADF Switch Position *******************/

    if(testbit(radiobuf[radnum],LOWER_ADF)) {
      loseldis[radnum] = 5;
      if (radiores == 1) {
        if (loadfsel[radnum] == 1) {
	  if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
	    loadfdbncfninc[radnum]++;
	    if (loadfdbncfninc[radnum] == 3) {
	      XPLMCommandOnce(Afd1OnesUp);
	      loadfdbncfninc[radnum] = 0;
	    }	
          }
          if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
	    loadfdbncfndec[radnum]++;
	    if (loadfdbncfndec[radnum] == 3) {
	      XPLMCommandOnce(Afd1OnesDn);
	      loadfdbncfndec[radnum] = 0;
	    } 
          }
          if(testbit(radiobuf[radnum],LOWER_COARSE_UP)) {
	    loadfdbnccorinc[radnum]++;
	    if (loadfdbnccorinc[radnum] == 3) {
	      XPLMCommandOnce(Afd1TensUp);
	      loadfdbnccorinc[radnum] = 0;
	    }
          }  
          if(testbit(radiobuf[radnum],LOWER_COARSE_DN)) {
	    loadfdbnccordec[radnum]++;
	    if (loadfdbnccordec[radnum] == 3) {
	      XPLMCommandOnce(Afd1TensDn);
	      loadfdbnccordec[radnum] = 0;
	    }
          }
	}
          if (loadfsel[radnum] == 2) {
	    if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
	      loadfdbncfninc[radnum]++;
	      if (loadfdbncfninc[radnum] == 3) {
		XPLMCommandOnce(Afd1HunUp);
	        loadfdbncfninc[radnum] = 0;
	      }
            }
          if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
	    loadfdbncfndec[radnum]++;
	    if (loadfdbncfndec[radnum] == 3) {
	      XPLMCommandOnce(Afd1HunDn);
	      loadfdbncfndec[radnum] = 0;
	    }
          }
          if(testbit(radiobuf[radnum],LOWER_COARSE_UP)) {
	    loadfdbnccorinc[radnum]++;
	    if (loadfdbnccorinc[radnum] == 3) {
	      XPLMCommandOnce(Afd1HunUp);
	      loadfdbnccorinc[radnum] = 0;
	    }
          }  
          if(testbit(radiobuf[radnum],LOWER_COARSE_DN)) {
	    loadfdbnccordec[radnum]++;
	    if (loadfdbnccordec[radnum] == 3) {
	      XPLMCommandOnce(Afd1HunDn);
	      loadfdbnccordec[radnum] = 0;
	    }
          }	
	}
	  if(testbit(radiobuf[radnum],LOWER_ACT_STBY)) {
	    loadfsel[radnum] ++;
	    if (loadfsel[radnum] == 3) {
              loadfsel[radnum] = 1;
	    }
	  }
	}
    loadffreq[radnum] = XPLMGetDatai(AdfFreq);
    }

/***************** Lower DME Switch Position *******************/

    if(testbit(radiobuf[radnum],LOWER_DME)) {
      loseldis[radnum] = 6;
      lodmesource[radnum] = XPLMGetDatai(DmeSlvSource);
      if (lodmesource[radnum] == 0){
        lodmedistf[radnum] = XPLMGetDataf(Nav1DmeNmDist);
	lodmedist[radnum] = (int)(lodmedistf[radnum] * 10.0f);
	lodmespeedf[radnum] = XPLMGetDataf(Nav1DmeSpeed);
	lodmespeed[radnum] = (int)(lodmespeedf[radnum]);
      }
      else if (lodmesource[radnum] == 1){
        lodmedistf[radnum] = XPLMGetDataf(Nav2DmeNmDist);
	lodmedist[radnum] = (int)(lodmedistf[radnum] * 10.0f);
	lodmespeedf[radnum] = XPLMGetDataf(Nav2DmeSpeed);
	lodmespeed[radnum] = (int)(lodmespeedf[radnum]);
      }
    }

/***************** Lower Transponder Switch Position *******************/
 
   if(testbit(radiobuf[radnum],LOWER_XPDR)) {
     loseldis[radnum] = 7;
     if (radiores == 1) {
       if (loxpdrsel[radnum] == 1) {	
         if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
	   loxpdrdbncfninc[radnum]++;
	   if (loxpdrdbncfninc[radnum] == 3) {
	     XPLMCommandOnce(XpdrOnesUp);
	     loxpdrdbncfninc[radnum] = 0;
	   }
         }
         if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
	   loxpdrdbncfndec[radnum]++;
	   if (loxpdrdbncfndec[radnum] == 3) {
	     XPLMCommandOnce(XpdrOnesDn);
	     loxpdrdbncfndec[radnum] = 0;
	   }
         }
         if(testbit(radiobuf[radnum],LOWER_COARSE_UP)) {
	   loxpdrdbnccorinc[radnum]++;
	     if (loxpdrdbnccorinc[radnum] == 3) {
	       XPLMCommandOnce(XpdrTensUp);
	       loxpdrdbnccorinc[radnum] = 0;
	     }
           }  
           if(testbit(radiobuf[radnum],LOWER_COARSE_DN)) {
	     loxpdrdbnccordec[radnum]++;
	     if (loxpdrdbnccordec[radnum] == 3) {
	       XPLMCommandOnce(XpdrTensDn);
	       loxpdrdbnccordec[radnum] = 0;
	     }
           }
	 }
         if (loxpdrsel[radnum] == 2) {	
           if(testbit(radiobuf[radnum],LOWER_FINE_UP)) {
	     loxpdrdbncfninc[radnum]++;
	     if (loxpdrdbncfninc[radnum] == 3) {
	       XPLMCommandOnce(XpdrHunUp);
	       loxpdrdbncfninc[radnum] = 0;
	     }
           }
           if(testbit(radiobuf[radnum],LOWER_FINE_DN)) {
	     loxpdrdbncfndec[radnum]++;
	     if (loxpdrdbncfndec[radnum] == 3) {
	       XPLMCommandOnce(XpdrHunDn);
	       loxpdrdbncfndec[radnum] = 0;
	     }
           }
           if(testbit(radiobuf[radnum],LOWER_COARSE_UP)) {
	     loxpdrdbnccorinc[radnum]++;
	     if (loxpdrdbnccorinc[radnum] == 3) {
	       XPLMCommandOnce(XpdrThUp);
	       loxpdrdbnccorinc[radnum] = 0;
	     }
           }  
           if(testbit(radiobuf[radnum],LOWER_COARSE_DN)) {
	     loxpdrdbnccordec[radnum]++;
	     if (loxpdrdbnccordec[radnum] == 3) {
	       XPLMCommandOnce(XpdrThDn);
	       loxpdrdbnccordec[radnum] = 0;
	     }
           }
	 }
	   if(testbit(radiobuf[radnum],LOWER_ACT_STBY)) {
	     loxpdrsel[radnum]++;
	     if (loxpdrsel[radnum] == 3) {
               loxpdrsel[radnum] = 1;
	     }
	   }
         }
    loxpdrcode[radnum] = XPLMGetDatai(XpdrCode);
    }

/***************** Blank Display *******************/

	if (XPLMGetDatai(AvPwrOn) == 0) {
          upseldis[radnum] = 8;
          loseldis[radnum] = 8;	  
	}

	if (XPLMGetDatai(BatPwrOn) == 0) {
          upseldis[radnum] = 8;
          loseldis[radnum] = 8;	  
	}

	if (XPLMGetDatai(Com1PwrOn) == 0) {
          if (upcom1[radnum] == 1) {
            upseldis[radnum] = 8;
          }
          if (locom1[radnum] == 1) {
            loseldis[radnum] = 8;
          }
	}

	if (XPLMGetDatai(Com2PwrOn) == 0) {
          if (upcom2[radnum] == 2) {
            upseldis[radnum] = 8;
          }
          if (locom2[radnum] == 2) {
            loseldis[radnum] = 8;
          }
	}

	if (XPLMGetDatai(Nav1PwrOn) == 0) {
          if (upnav1[radnum] == 3) {
            upseldis[radnum] = 8;
          }
          if (lonav1[radnum] == 3) {
            loseldis[radnum] = 8;
          }
	}

	if (XPLMGetDatai(Nav2PwrOn) == 0) {
          if (upnav2[radnum] == 4) {
            upseldis[radnum] = 8;
          }
          if (lonav2[radnum] == 4) {
            loseldis[radnum] = 8;
          }
	}

/*********** loop untill all radios serviced *************/
/**************   then start again    *******************/

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
