// Xdataref2BIP Plugin for X-Plane 9
// Allows use of Saitek BIP on all platforms
// Linux Windows Mac
// Uses Hidapi for Hid interface
// https://github.com/signal11/hidapi
// Version 1.0d
// William Good
// 07-24-11


#if IBM
#include <windows.h>
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return true;
}
#endif


#include "XPLMPlugin.h"
#include "XPLMProcessing.h"
#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMMenus.h"
#include "XPWidgets.h"
#include "XPStandardWidgets.h"

#include "hidapi.h"

#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


using namespace std;

#define MAXTABLEELEMENTS 1000

//MAXINDICATORS is the number of indicators in one row
#define MAXINDICATORS 8

/******* Bip Panel Defines *******/
#define BIP "/dev/saitekbippanel"

struct  D2BTableStructure
{
    char            Row;
    int             Number;
    char            Color;
    XPLMDataRef     DataRefToSet;
    XPLMDataTypeID  DataRefType;
    int             DataRefIndex;
    char            WhatToDo;
    float           FloatValueToSet;
    float           FloatLimit;
    string          CSVDebugString;
};

static D2BTableStructure    D2BTable[MAXTABLEELEMENTS];
static int                  LastValues[MAXINDICATORS];
static int                  ActualValues[MAXINDICATORS];
static int                  LastTableElement = -1;
static int                  ErrorInLine = 0;
static bool                 InSilentMode = false;

int loop;
int res;
hid_device *biphandle;
int i;


void            D2BMenuHandler(void *, void *);
XPLMMenuID      D2BMenuId;
int             D2BMenuItem;
static char     MenuEntries[50][250];
static int      LastMenuEntry = 0;

XPWidgetID      D2BWidgetID = NULL;

bool ReadConfigFile(string PlaneICAO);
void WriteCSVTableToDisk(void);

void LetWidgetSay(string TextToDisplay)
{
    XPSetWidgetDescriptor(D2BWidgetID, TextToDisplay.c_str());
    int x = XPLMGetDatai(XPLMFindDataRef("sim/graphics/view/window_width")) - (int) XPLMMeasureString(xplmFont_Proportional, TextToDisplay.c_str(), TextToDisplay.length()) - 10;
    int y = XPLMGetDatai(XPLMFindDataRef("sim/graphics/view/window_height")) - 35;
    XPSetWidgetGeometry(D2BWidgetID, x, y, x+5, y-5);
}

void logMsg ( std::string message )
{
  std::string out = message + "\n";
  XPLMDebugString(const_cast<char *> (out.c_str()));
  if (!InSilentMode) XPLMSpeakString(message.c_str());
  stringstream  ErrorHelper;
  out.erase();
  ErrorHelper << ErrorInLine;
  ErrorHelper >> out;
  LetWidgetSay(message.append(" (in line no. ").append(out).append(")"));
}

void D2BMenuHandler(void * mRef, void * iRef)
{
    (void) mRef; // To get rid of warnings on unused variables
    if (strcmp((char *) iRef, "<<CSV>>") == 0)
    {
        WriteCSVTableToDisk();
    }
    else
    {
        ReadConfigFile((char *) iRef);
    }
    return;
}


float	D2BLoopCallback(
    float                inElapsedSinceLastCall,
    float                inElapsedTimeSinceLastFlightLoop,
    int                  inCounter,
    void *               inRefcon);


XPLMDataRef gTimeSimIsRunningXDataRef = NULL;

void WriteCSVTableToDisk(void)
{
    time_t      rawtime;
    struct      tm * timeinfo;
    char        CSVFileName[512];

    time( &rawtime );
    timeinfo = localtime( &rawtime );

    strftime(CSVFileName, 512, "Resources/plugins/Xdataref2BIP/D2B_debug_%Y-%m-%d_%H%M%S.csv", timeinfo);
    ofstream CSVFile(CSVFileName);
    if (CSVFile.is_open() != true)
    {
        logMsg("Xdatareaf2BIP: Fatal Error: Unable to write a CSV.");
        return;
    }
    CSVFile << "\"Row\",\"Number\",\"Number2\",\"Color\",\"ValueFrom\",\"ValueFrom2\",\"ValueTo\",\"ValueTo2\",\"";
    CSVFile << "DataRefToSet\",\"DataRefType\",\"CommandToExecute\",\"DataRefIndex\",\"WhatToDo\",\"IntValueToSet\",\"";
    CSVFile << "FloatValueToSet\",\"IntLimit\",\"FloatLimit\",\"IntToggle\",\"FloatToggle\",\"CSVDebugString\"\n";

    for (int i = 0; i < MAXTABLEELEMENTS; i++)
    {
        CSVFile << "\"" << D2BTable[i].Row << "\",";
        CSVFile << "\"" << D2BTable[i].Number << "\",";
        CSVFile << "\"" << D2BTable[i].Color << "\",";
        CSVFile << "\"" << D2BTable[i].DataRefToSet << "\",";
        CSVFile << "\"" << D2BTable[i].DataRefType << "\",";
        CSVFile << "\"" << D2BTable[i].DataRefIndex << "\",";
        CSVFile << "\"" << D2BTable[i].WhatToDo << "\",";
        CSVFile << "\"" << D2BTable[i].FloatLimit << "\",";
        CSVFile << "\"" << D2BTable[i].CSVDebugString << "\"\n";
    }
    CSVFile.close();
}


bool ReadConfigFile(string PlaneICAO)
{
    string          LineToEncrypt;
    bool            CorrectICAO = true;
    char            RowString[1];
    int             BipNumber;
    char            ColorString[1];
    char            DataRefString[512];
    XPLMDataRef     DataRefNumber;
    XPLMDataTypeID  DataRefType;
    float           Argument, Limit;
    int             Index;
    int             i;

    PlaneICAO.erase(PlaneICAO.find(']')+1);
    LetWidgetSay(PlaneICAO);

    LastMenuEntry = -1;

    XPLMClearAllMenuItems(D2BMenuId);
    XPLMAppendMenuItem(D2BMenuId, "[DEFAULT]", (void *) "[DEFAULT]", 1);
    XPLMAppendMenuItem(D2BMenuId, "Write a CSV Table for debugging", (void *) "<<CSV>>", 1);
    XPLMAppendMenuSeparator(D2BMenuId);

    ifstream ReadFile("Resources/plugins/Xdataref2BIP/D2B_config.txt");

    if (ReadFile.is_open() != true)
    {
        logMsg("Error: Can't read Xdataref2BIP config file!");
        return false;
    }
    ErrorInLine = 0;

    LastTableElement = -1;
    for (i = 0; i < MAXTABLEELEMENTS; i++)
    {
        D2BTable[i].Row = '0';
        D2BTable[i].Number = 0;
        D2BTable[i].DataRefToSet = NULL;
        D2BTable[i].DataRefType = 0;
        D2BTable[i].DataRefIndex = 0;
        D2BTable[i].WhatToDo = '0';
        D2BTable[i].FloatValueToSet = 0;
        D2BTable[i].FloatLimit = 0;
        D2BTable[i].CSVDebugString = "";
    }

    while (getline(ReadFile, LineToEncrypt))
    {
        ErrorInLine++;
        if (LineToEncrypt.find("#BE SILENT") == 0)
        {
            InSilentMode = true;
            continue;
        }
        if (LineToEncrypt.find("#BE CHATTY") == 0)
        {
            InSilentMode = false;
            continue;
        }
        if (LineToEncrypt.find("#SHOW ICAO") == 0)
        {
            XPShowWidget(D2BWidgetID);
            continue;
        }
        if (LineToEncrypt.find("#HIDE ICAO") == 0)
        {
            XPHideWidget(D2BWidgetID);
            continue;
        }
        if (LineToEncrypt.find('[') == 0)
        {
            if ((LineToEncrypt.find("[DEFAULT]") == 0) || (LineToEncrypt.find(PlaneICAO) == 0))
            {
                CorrectICAO = true;
            }
            else
            {
                CorrectICAO = false;
            }
            if ((LineToEncrypt.find("[DEFAULT]") != 0) && (++LastMenuEntry < 50))
            {
                strcpy(MenuEntries[LastMenuEntry], LineToEncrypt.c_str());
                XPLMAppendMenuItem(D2BMenuId, MenuEntries[LastMenuEntry], (void *) MenuEntries[LastMenuEntry], 1);
            }
        }
        if (!CorrectICAO) continue;

        if (LineToEncrypt.find("#RESET AUTHORITY") == 0)
        {
            if (++LastTableElement >= MAXTABLEELEMENTS)
            {
                logMsg("Xdataref2BIP: Fatal Error: Too much code to handle!");
                ReadFile.close();
                LastTableElement = MAXTABLEELEMENTS - 1;
                return false;
            }
            D2BTable[LastTableElement].WhatToDo = 'T';
            continue;
        }

        if (sscanf(LineToEncrypt.c_str(), "#SET BIP %c %i %c FROM ARRAY %s %i RANGE %f TO %f", RowString, &BipNumber, ColorString, DataRefString, &Index, &Argument, &Limit) == 7)
        {
            if (++LastTableElement >= MAXTABLEELEMENTS)
            {
                logMsg("Xdataref2BIP: Fatal Error: Too much code to handle!");
                ReadFile.close();
                LastTableElement = MAXTABLEELEMENTS - 1;
                return false;
            }
            DataRefNumber = XPLMFindDataRef(DataRefString);
            if (DataRefNumber == NULL)
            {
                logMsg("Xdataref2BIP: A DataRef you want to use is not defined!");
                ReadFile.close();
                return false;
            }
            DataRefType = XPLMGetDataRefTypes(DataRefNumber);
            if (!((DataRefType == xplmType_IntArray) || (DataRefType == xplmType_FloatArray)))
            {
                logMsg("Xdataref2BIP: A DataRef you want to use can not be read (wrong type)!");
                ReadFile.close();
                return false;
            }

            if ((BipNumber >= MAXINDICATORS) || (BipNumber < 0))
            {
                logMsg("Xdataref2BIP: Indicator does not exist!");
                ReadFile.close();
                LastTableElement = MAXTABLEELEMENTS - 1;
                return false;
            }
            D2BTable[LastTableElement].Row = *RowString;
            D2BTable[LastTableElement].Number = BipNumber;
            D2BTable[LastTableElement].Color = *ColorString;
            D2BTable[LastTableElement].DataRefToSet = DataRefNumber;
            D2BTable[LastTableElement].DataRefType = DataRefType;
            D2BTable[LastTableElement].DataRefIndex = Index;
            D2BTable[LastTableElement].FloatValueToSet = Argument;
            D2BTable[LastTableElement].FloatLimit = Limit;
            D2BTable[LastTableElement].WhatToDo = 'v';
            D2BTable[LastTableElement].CSVDebugString = DataRefString;
            continue;
        }
        if (sscanf(LineToEncrypt.c_str(), "#SET BIP %c %i %c FROM DATAREF %s RANGE %f TO %f", RowString, &BipNumber, ColorString, DataRefString, &Argument, &Limit) == 6)
        {
            if (++LastTableElement >= MAXTABLEELEMENTS)
            {
                logMsg("Xdataref2BIP: Fatal Error: Too much code to handle!");
                ReadFile.close();
                LastTableElement = MAXTABLEELEMENTS - 1;
                return false;
            }
            DataRefNumber = XPLMFindDataRef(DataRefString);
            if (DataRefNumber == NULL)
            {
                logMsg("Xdataref2BIP: A DataRef you want to use is not defined!");
                ReadFile.close();
                return false;
            }
            DataRefType = XPLMGetDataRefTypes(DataRefNumber);
            if (!((DataRefType == xplmType_Int) || (DataRefType == xplmType_Float)))
            {
                logMsg("Xdataref2BIP: A DataRef you want to use can not be read (wrong type)!");
                ReadFile.close();
                return false;
            }
            if ((BipNumber >= MAXINDICATORS) || (BipNumber < 0))
            {
                logMsg("Xdataref2BIP: Indicator does not exist!");
                ReadFile.close();
                LastTableElement = MAXTABLEELEMENTS - 1;
                return false;
            }
            D2BTable[LastTableElement].Row = *RowString;
            D2BTable[LastTableElement].Number = BipNumber;
            D2BTable[LastTableElement].Color = *ColorString;
            D2BTable[LastTableElement].DataRefToSet = DataRefNumber;
            D2BTable[LastTableElement].DataRefType = DataRefType;
            D2BTable[LastTableElement].FloatValueToSet = Argument;
            D2BTable[LastTableElement].FloatLimit = Limit;
            D2BTable[LastTableElement].WhatToDo = 'v';
            D2BTable[LastTableElement].CSVDebugString = DataRefString;
            continue;
        }


        if (LineToEncrypt.find('#') == 0)
        {
            logMsg("Xdataref2BIP: Can't understand the line of code!");
            ReadFile.close();
            LastTableElement = MAXTABLEELEMENTS - 1;
            return false;
        }
    }

    ReadFile.close();
    return true;
}


/********************** Bip Panel variables ***********************/
int bipfd = -1;
int bipfdw = -1;
int change;

unsigned char bipwbuf[10];
unsigned char lastbipwbuf[10];

PLUGIN_API int XPluginStart(
    char *        outName,
    char *        outSig,
    char *        outDesc)
{
    // Plugin Info
    strcpy(outName, "Xdataref2BIP V1.0d");
    strcpy(outSig, "BIP_Plugin.D2B");
    strcpy(outDesc, "Plugin for Saitek BIP.");

    // Create our menu

    D2BMenuItem = XPLMAppendMenuItem(
                XPLMFindPluginsMenu(),
                "Xdataref2BIP",
                NULL,
                1);

    D2BMenuId = XPLMCreateMenu(
                "Xdataref2BIP",
                XPLMFindPluginsMenu(),
                D2BMenuItem,
                D2BMenuHandler,
                NULL);


    gTimeSimIsRunningXDataRef = XPLMFindDataRef("sim/time/total_running_time_sec");

    for (int i = 0; i < MAXINDICATORS; i++)
    {
        ActualValues[i] = 0;
        LastValues[i] = 0;
    }

    //   Find Connected Bip Panel
    biphandle = hid_open(0x6a3, 0xb4e, NULL);
   //  If Found Set brightness to full (0 - 100)
   // 0xb2 Report ID for brightness
   // Next byte 0 - 100 brightness value
   if (biphandle > 0) {
      bipwbuf[0] = 0xb2; // 0xb2 Report ID for brightness
      bipwbuf[1] = 100;  // Set brightness to 100%
      res = hid_send_feature_report(biphandle, bipwbuf, 10);
      loop = 1;

    }

    // Check in the main function
    XPLMRegisterFlightLoopCallback(
        D2BLoopCallback,	    /* Callback */
        -1.0,               	/* Interval */
        NULL);  	     	    /* refcon not used. */

    D2BWidgetID = XPCreateWidget(XPLMGetDatai(XPLMFindDataRef("sim/graphics/view/window_width"))-150,
                                 XPLMGetDatai(XPLMFindDataRef("sim/graphics/view/window_height")),
                                 XPLMGetDatai(XPLMFindDataRef("sim/graphics/view/window_width"))-10,
                                 XPLMGetDatai(XPLMFindDataRef("sim/graphics/view/window_height"))-10,         // screen coordinates
                                 1,                            // visible
                                 "Xdataref2BIP is working!", // description
                                 1, NULL,                      // we want it root
                                 xpWidgetClass_Caption);
    XPSetWidgetProperty(D2BWidgetID, xpProperty_CaptionLit, 0);



    return 1;
}

PLUGIN_API void    XPluginStop(void)
{
    XPLMUnregisterFlightLoopCallback(D2BLoopCallback, NULL);
    XPDestroyWidget(D2BWidgetID, 1);
    XPLMDestroyMenu(D2BMenuId);

    /*** if open close that bip panel ****/
      if (biphandle > 0) {
        bipwbuf[0] = 0xb8;  //0xb8 Report ID to display
        bipwbuf[1] = 0, bipwbuf[2] = 0, bipwbuf[3] = 0;
        bipwbuf[4] = 0, bipwbuf[5] = 0, bipwbuf[6] = 0;
        res = hid_send_feature_report(biphandle, bipwbuf, 10);

        hid_close(biphandle);
      }
}



PLUGIN_API int XPluginEnable(void)
{
    return 1;
}

PLUGIN_API void XPluginDisable(void)
{
}

PLUGIN_API void XPluginReceiveMessage(
    XPLMPluginID    inFromWho,
    long            inMessage,
    void *          inParam)
{
    (void) inFromWho; // To get rid of warnings on unused variables

    string          PlaneICAO = "[]";
    char            ICAOString[40];

    XPLMGetDatab(XPLMFindDataRef("sim/aircraft/view/acf_ICAO"), ICAOString, 0, 40);
    PlaneICAO.insert(1,ICAOString);

    if ((inMessage == XPLM_MSG_PLANE_LOADED) & ((long) inParam == 0))
    {
        ReadConfigFile(PlaneICAO);
    }
    if (inMessage == XPLM_MSG_AIRPORT_LOADED)
    {
        ReadConfigFile(PlaneICAO);
    }


}

float	D2BLoopCallback(
    float                inElapsedSinceLastCall,
    float                inElapsedTimeSinceLastFlightLoop,
    int                  inCounter,
    void *               inRefcon)
{
    (void) inElapsedSinceLastCall;            // To get rid of warnings on unused variables
    (void) inElapsedTimeSinceLastFlightLoop;  // To get rid of warnings on unused variables
    (void) inCounter;                         // To get rid of warnings on unused variables
    (void) inRefcon;                          // To get rid of warnings on unused variables
    int     xi;
    float   xf;
    float   Now;
    int     i;
    bool    WorkPermitted = NULL;
    float   LookAtThisValue;
    int     LookAtThisIntValue;

    if (LastTableElement == -1)
    {
        LetWidgetSay("Xdatasref2BIP STANDBY");
        return 5.0;
    }

    Now = XPLMGetDataf(gTimeSimIsRunningXDataRef);

    for (i = 0; i <= LastTableElement; i++) {

        if (D2BTable[i].WhatToDo == 'v') {
            // "#SET BIP %c %i %c FROM DATAREF %s RANGE %f TO %f" or "#SET BIP %s %i %s FROM ARRAY %s %i RANGE %f TO %f""
            if (D2BTable[i].DataRefType == xplmType_Float) LookAtThisValue = XPLMGetDataf(D2BTable[i].DataRefToSet);
            if (D2BTable[i].DataRefType == xplmType_Int) LookAtThisValue = (float) XPLMGetDatai(D2BTable[i].DataRefToSet);
            if (D2BTable[i].DataRefType == xplmType_FloatArray) XPLMGetDatavf(D2BTable[i].DataRefToSet, &LookAtThisValue, D2BTable[i].DataRefIndex, 1);
            if (D2BTable[i].DataRefType == xplmType_IntArray) {
                XPLMGetDatavi(D2BTable[i].DataRefToSet, &LookAtThisIntValue, D2BTable[i].DataRefIndex, 1);
                LookAtThisValue = (float) LookAtThisIntValue;
            }
            if ((LookAtThisValue >= D2BTable[i].FloatValueToSet) && (LookAtThisValue <= D2BTable[i].FloatLimit)) {

                //printf("********************************  ON   **************************************\n");
                if (D2BTable[i].Row == 'A') {
                    if (D2BTable[i].Color == 'G') {
                        // Make D2BTable[i].Number position Top Row Green
                        bipwbuf[1] |= (1<<(D2BTable[i].Number)), bipwbuf[4] &= ~(1<<(D2BTable[i].Number));
                    }
                    if (D2BTable[i].Color == 'R') {
                        // Make D2BTable[i].Number position Top Row Red
                        bipwbuf[1] &= ~(1<<(D2BTable[i].Number)), bipwbuf[4] |= (1<<(D2BTable[i].Number));
                    }
                    if (D2BTable[i].Color == 'A') {
                        // Make D2BTable[i].Number position Top Row Amber
                        bipwbuf[1] |= (1<<(D2BTable[i].Number)), bipwbuf[4] |= (1<<(D2BTable[i].Number));
                    }
                }

                if (D2BTable[i].Row == 'B') {
                    if (D2BTable[i].Color == 'G') {
                        // Make D2BTable[i].Number position Middle Row Green
                        bipwbuf[2] |= (1<<(D2BTable[i].Number)), bipwbuf[5] &= ~(1<<(D2BTable[i].Number));
                    }
                    if (D2BTable[i].Color == 'R') {
                        // Make D2BTable[i].Number position Middle Row Red
                        bipwbuf[2] &= ~(1<<(D2BTable[i].Number)), bipwbuf[5] |= (1<<(D2BTable[i].Number));
                    }
                    if (D2BTable[i].Color == 'A') {
                        // Make D2BTable[i].Number position Middle Row Amber
                        bipwbuf[2] |= (1<<(D2BTable[i].Number)), bipwbuf[5] |= (1<<(D2BTable[i].Number));
                    }
                }

                if (D2BTable[i].Row == 'C') {
                    if (D2BTable[i].Color == 'G') {
                        // Make D2BTable[i].Number position Bottom Row Green
                        bipwbuf[3] |= (1<<(D2BTable[i].Number)), bipwbuf[6] &= ~(1<<(D2BTable[i].Number));
                    }
                    if (D2BTable[i].Color == 'R') {
                        // Make D2BTable[i].Number position Bottom Row Red
                        bipwbuf[3] &= ~(1<<(D2BTable[i].Number)), bipwbuf[6] |= (1<<(D2BTable[i].Number));
                    }
                    if (D2BTable[i].Color == 'A') {
                        // Make D2BTable[i].Number position Bottom Row Amber
                        bipwbuf[3] |= (1<<(D2BTable[i].Number)), bipwbuf[6] |= (1<<(D2BTable[i].Number));
                    }
                }
            }

            else {
                //printf("**********************   OFF   *********************************\n");
                if (D2BTable[i].Row == 'A') {
                    // Make D2BTable[i].Number position Top Row Off
                    bipwbuf[1] &= ~(1<<(D2BTable[i].Number)), bipwbuf[4] &= ~(1<<(D2BTable[i].Number));
                }
                if (D2BTable[i].Row == 'B') {
                    // Make D2BTable[i].Number position Middle Row Off
                    bipwbuf[2] &= ~(1<<(D2BTable[i].Number)), bipwbuf[5] &= ~(1<<(D2BTable[i].Number));
                }
                if (D2BTable[i].Row == 'C')  {
                    // Make D2BTable[i].Number position Bottom Row Off
                    bipwbuf[3] &= ~(1<<(D2BTable[i].Number)), bipwbuf[6] &= ~(1<<(D2BTable[i].Number));
                }


            }

        }

    }

    for (i=0; i<MAXINDICATORS; i++)
    {
        LastValues[i] = ActualValues[i];
    }

    if (biphandle > 0) {
        if (loop < 2){
          // Clear Display on first loop
          bipwbuf[0] = 0xb8;  //0xb8 Report ID to display
          bipwbuf[1] = 0, bipwbuf[2] = 0, bipwbuf[3] = 0;
          bipwbuf[4] = 0, bipwbuf[5] = 0, bipwbuf[6] = 0;
          res = hid_send_feature_report(biphandle, bipwbuf, 10);
          loop++;
        }

        // Trying to only write on changes to improve FPS impact
        if(loop == 2){
          change = memcmp(bipwbuf, lastbipwbuf, 10);
          if (change == 0){
          }
          if (change != 0){
            res = hid_send_feature_report(biphandle, bipwbuf, 10);
            memcpy(lastbipwbuf, bipwbuf, 10);
          }
       }
    }

    return -1.0;
}
