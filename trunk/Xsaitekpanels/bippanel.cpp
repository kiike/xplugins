 // ****** bippanel.cpp **********
// ****  William R. Good  ********


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


#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"
#include "XPLMGraphics.h"
#include "XPLMMenus.h"
#include "XPWidgets.h"
#include "XPStandardWidgets.h"

#include "hidapi.h"
#include "saitekpanels.h"

#include <time.h>
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

//MAXINDICATORS is the Position of indicators in one row
#define MAXINDICATORS 8

// ********************** Bip Panel variables ***********************
float   LookAtThisValue;
int     LookAtThisIntValue;

static int bipnum = 0, bipnowrite[4] = {0, 0, 0, 0};
static unsigned char bipwbuf[4][10];
static unsigned char lastbipwbuf[4][10];

static int bipchange, biploop[4], res, i;
static ifstream ReadFile[4];

struct  BipTableStructure
{
    char            Row; // A B or C
    int             Position; // 0 tru 7
    char            Color;  // G green R red A amber
    XPLMDataRef     DataRefToSet;
    XPLMDataTypeID  DataRefType;
    int             DataRefIndex;
    char            WhatToDo;
    float           FloatValueToSet;
    float           FloatLimit;
    string          CSVDebugString;
};

static BipTableStructure    BipTable[MAXTABLEELEMENTS];
static int                  LastValues[MAXINDICATORS];
static int                  ActualValues[MAXINDICATORS];
static int                  LastTableElement = -1;
static int                  ErrorInLine = 0;
static bool                 InSilentMode = false;


static char     MenuEntries[50][250];
static int      LastMenuEntry = 0;

bool ReadConfigFile(string PlaneICAO);
void WriteCSVTableToDisk(void);


void LetWidgetSay(string TextToDisplay)
{
    XPSetWidgetDescriptor(BipWidgetID, TextToDisplay.c_str());
    int x = XPLMGetDatai(XPLMFindDataRef("sim/graphics/view/window_width")) - (int) XPLMMeasureString(xplmFont_Proportional, TextToDisplay.c_str(), TextToDisplay.length()) - 10;
    int y = XPLMGetDatai(XPLMFindDataRef("sim/graphics/view/window_height")) - 35;
    XPSetWidgetGeometry(BipWidgetID, x, y, x+5, y-5);
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

void WriteCSVTableToDisk(void)
{
    time_t      rawtime;
    struct      tm * timeinfo;
    char        CSVFileName[512];

    time( &rawtime );
    timeinfo = localtime( &rawtime );

    strftime(CSVFileName, 512, "Resources/plugins/Xsaitekpanels/D2B_debug_%Y-%m-%d_%H%M%S.csv", timeinfo);
    ofstream CSVFile(CSVFileName);
    if (CSVFile.is_open() != true)
    {
        logMsg("Xdatareaf2BIP: Fatal Error: Unable to write a CSV.");
        return;
    }
    CSVFile << "\"Row\",\"Position\",\"Color\",\"DataRefToSet\",\"DataRefType\",\"DataRefIndex\"\"";
    CSVFile << "WhatToDo\",\"FloatLimit\",\"CSVDebugString\"\n";

    for (int i = 0; i < MAXTABLEELEMENTS; i++)
    {
        CSVFile << "\"" << BipTable[i].Row << "\",";
        CSVFile << "\"" << BipTable[i].Position << "\",";
        CSVFile << "\"" << BipTable[i].Color << "\",";
        CSVFile << "\"" << BipTable[i].DataRefToSet << "\",";
        CSVFile << "\"" << BipTable[i].DataRefType << "\",";
        CSVFile << "\"" << BipTable[i].DataRefIndex << "\",";
        CSVFile << "\"" << BipTable[i].WhatToDo << "\",";
        CSVFile << "\"" << BipTable[i].FloatLimit << "\",";
        CSVFile << "\"" << BipTable[i].CSVDebugString << "\"\n";
    }
    CSVFile.close();
}


bool ReadConfigFile(string PlaneICAO)
{
    string          LineToEncrypt;
    bool            CorrectICAO = true;
    char            RowString[1];
    int             BipPosition;
    char            ColorString[1];
    char            DataRefString[512];
    XPLMDataRef     DataRefNumber;
    XPLMDataTypeID  DataRefType;
    float           Argument, Limit;
    int             Index;
    int             i;

    ReadFile[0].open("Resources/plugins/Xsaitekpanels/D2B_config.txt");
    ReadFile[1].open("Resources/plugins/Xsaitekpanels/D2B_config2.txt");
    ReadFile[2].open("Resources/plugins/Xsaitekpanels/D2B_config3.txt");
    ReadFile[3].open("Resources/plugins/Xsaitekpanels/D2B_config4.txt");


    PlaneICAO.erase(PlaneICAO.find(']')+1);
    LetWidgetSay(PlaneICAO);

    LastMenuEntry = -1;

    if(bipcnt > 0) {

      XPLMClearAllMenuItems(BipMenuId);
      XPLMAppendMenuItem(BipMenuId, "[DEFAULT]", (void *) "[DEFAULT]", 1);
      XPLMAppendMenuItem(BipMenuId, "Write a CSV Table for debugging", (void *) "<<CSV>>", 1);
      XPLMAppendMenuSeparator(BipMenuId);

    if (ReadFile[bipnum].is_open() != true)
    {
        logMsg("Error: Can't read Xdataref2BIP config file!");
        return false;
    }
    ErrorInLine = 0;

    LastTableElement = -1;
    for (i = 0; i < MAXTABLEELEMENTS; i++)
    {
        BipTable[i].Row = '0';
        BipTable[i].Position = 0;
        BipTable[i].Color = '0';
        BipTable[i].DataRefToSet = NULL;
        BipTable[i].DataRefType = 0;
        BipTable[i].DataRefIndex = 0;
        BipTable[i].WhatToDo = '0';
        BipTable[i].FloatValueToSet = 0;
        BipTable[i].FloatLimit = 0;
        BipTable[i].CSVDebugString = "";
    }

    while (getline(ReadFile[bipnum], LineToEncrypt))
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
            XPShowWidget(BipWidgetID);
            continue;
        }
        if (LineToEncrypt.find("#HIDE ICAO") == 0)
        {
            XPHideWidget(BipWidgetID);
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
                XPLMAppendMenuItem(BipMenuId, MenuEntries[LastMenuEntry], (void *) MenuEntries[LastMenuEntry], 1);
            }
        }
        if (!CorrectICAO) continue;

        if (LineToEncrypt.find("#RESET AUTHORITY") == 0)
        {
            if (++LastTableElement >= MAXTABLEELEMENTS)
            {
                logMsg("Xdataref2BIP: Fatal Error: Too much code to handle!");
                ReadFile[bipnum].close();
                LastTableElement = MAXTABLEELEMENTS - 1;
                return false;
            }
            BipTable[LastTableElement].WhatToDo = 'T';
            continue;
        }

        if (sscanf(LineToEncrypt.c_str(), "#SET BIP %c %i %c FROM ARRAY %s %i RANGE %f TO %f", RowString, &BipPosition, ColorString, DataRefString, &Index, &Argument, &Limit) == 7)
        {
            if (++LastTableElement >= MAXTABLEELEMENTS)
            {
                logMsg("Xdataref2BIP: Fatal Error: Too much code to handle!");
                ReadFile[bipnum].close();
                LastTableElement = MAXTABLEELEMENTS - 1;
                return false;
            }
            DataRefNumber = XPLMFindDataRef(DataRefString);
            if (DataRefNumber == NULL)
            {
                logMsg("Xdataref2BIP: A DataRef you want to use is not defined!");
                ReadFile[bipnum].close();
                return false;
            }
            DataRefType = XPLMGetDataRefTypes(DataRefNumber);
            if (!((DataRefType == xplmType_IntArray) || (DataRefType == xplmType_FloatArray)))
            {
                logMsg("Xdataref2BIP: A DataRef you want to use can not be read (wrong type)!");
                ReadFile[bipnum].close();
                return false;
            }

            if ((BipPosition >= MAXINDICATORS) || (BipPosition < 0))
            {
                logMsg("Xdataref2BIP: Indicator does not exist!");
                ReadFile[bipnum].close();
                LastTableElement = MAXTABLEELEMENTS - 1;
                return false;
            }
            BipTable[LastTableElement].Row = *RowString;
            BipTable[LastTableElement].Position = BipPosition;
            BipTable[LastTableElement].Color = *ColorString;
            BipTable[LastTableElement].DataRefToSet = DataRefNumber;
            BipTable[LastTableElement].DataRefType = DataRefType;
            BipTable[LastTableElement].DataRefIndex = Index;
            BipTable[LastTableElement].FloatValueToSet = Argument;
            BipTable[LastTableElement].FloatLimit = Limit;
            BipTable[LastTableElement].WhatToDo = 'v';
            BipTable[LastTableElement].CSVDebugString = DataRefString;
            continue;
        }
        if (sscanf(LineToEncrypt.c_str(), "#SET BIP %c %i %c FROM DATAREF %s RANGE %f TO %f", RowString, &BipPosition, ColorString, DataRefString, &Argument, &Limit) == 6)
        {
            if (++LastTableElement >= MAXTABLEELEMENTS)
            {
                logMsg("Xdataref2BIP: Fatal Error: Too much code to handle!");
                ReadFile[bipnum].close();
                LastTableElement = MAXTABLEELEMENTS - 1;
                return false;
            }
            DataRefNumber = XPLMFindDataRef(DataRefString);
            if (DataRefNumber == NULL)
            {
                logMsg("Xdataref2BIP: A DataRef you want to use is not defined!");
                ReadFile[bipnum].close();
                return false;
            }
            DataRefType = XPLMGetDataRefTypes(DataRefNumber);
            if (!((DataRefType == xplmType_Int) || (DataRefType == xplmType_Float)))
            {
                logMsg("Xdataref2BIP: A DataRef you want to use can not be read (wrong type)!");
                ReadFile[bipnum].close();
                return false;
            }
            if ((BipPosition >= MAXINDICATORS) || (BipPosition < 0))
            {
                logMsg("Xdataref2BIP: Indicator does not exist!");
                ReadFile[bipnum].close();
                LastTableElement = MAXTABLEELEMENTS - 1;
                return false;
            }
            BipTable[LastTableElement].Row = *RowString;
            BipTable[LastTableElement].Position = BipPosition;
            BipTable[LastTableElement].Color = *ColorString;
            BipTable[LastTableElement].DataRefToSet = DataRefNumber;
            BipTable[LastTableElement].DataRefType = DataRefType;
            BipTable[LastTableElement].FloatValueToSet = Argument;
            BipTable[LastTableElement].FloatLimit = Limit;
            BipTable[LastTableElement].WhatToDo = 'v';
            BipTable[LastTableElement].CSVDebugString = DataRefString;
            continue;
        }


        if (LineToEncrypt.find('#') == 0)
        {
            logMsg("Xdataref2BIP: Can't understand the line of code!");
            ReadFile[bipnum].close();
            LastTableElement = MAXTABLEELEMENTS - 1;
            return false;
        }
    }

    ReadFile[bipnum].close();
    return true;

   }


}


// ***** BIP Panel Process  *******
void process_bip_panel()

{

    if (LastTableElement == -1) {
        LetWidgetSay("Xdatasref2BIP STANDBY");
        return ;
    }


    for (i = 0; i <= LastTableElement; i++) {

        if (BipTable[i].WhatToDo == 'v') {
            // "#SET BIP %c %i %c FROM DATAREF %s RANGE %f TO %f" or "#SET BIP %s %i %s FROM ARRAY %s %i RANGE %f TO %f""
            if (BipTable[i].DataRefType == xplmType_Float) LookAtThisValue = XPLMGetDataf(BipTable[i].DataRefToSet);
            if (BipTable[i].DataRefType == xplmType_Int) LookAtThisValue = (float) XPLMGetDatai(BipTable[i].DataRefToSet);
            if (BipTable[i].DataRefType == xplmType_FloatArray) XPLMGetDatavf(BipTable[i].DataRefToSet, &LookAtThisValue, BipTable[i].DataRefIndex, 1);
            if (BipTable[i].DataRefType == xplmType_IntArray) {
                XPLMGetDatavi(BipTable[i].DataRefToSet, &LookAtThisIntValue, BipTable[i].DataRefIndex, 1);
                LookAtThisValue = (float) LookAtThisIntValue;
            }
            if ((LookAtThisValue >= BipTable[i].FloatValueToSet) && (LookAtThisValue <= BipTable[i].FloatLimit)) {

                //printf("********************************  ON   **************************************\n");
                if (BipTable[i].Row == 'A') {
                    if (BipTable[i].Color == 'G') {
                        // Make BipTable[i].Position position Top Row Green
                        bipwbuf[bipnum][1] |= (1<<(BipTable[i].Position)), bipwbuf[bipnum][4] &= ~(1<<(BipTable[i].Position));
                    }
                    if (BipTable[i].Color == 'R') {
                        // Make BipTable[i].Position position Top Row Red
                        bipwbuf[bipnum][1] &= ~(1<<(BipTable[i].Position)), bipwbuf[bipnum][4] |= (1<<(BipTable[i].Position));
                    }
                    if (BipTable[i].Color == 'A') {
                        // Make BipTable[i].Position position Top Row Amber
                        bipwbuf[bipnum][1] |= (1<<(BipTable[i].Position)), bipwbuf[bipnum][4] |= (1<<(BipTable[i].Position));
                    }
                }

                if (BipTable[i].Row == 'B') {
                    if (BipTable[i].Color == 'G') {
                        // Make BipTable[i].Position position Middle Row Green
                        bipwbuf[bipnum][2] |= (1<<(BipTable[i].Position)), bipwbuf[bipnum][5] &= ~(1<<(BipTable[i].Position));
                    }
                    if (BipTable[i].Color == 'R') {
                        // Make BipTable[i].Position position Middle Row Red
                        bipwbuf[bipnum][2] &= ~(1<<(BipTable[i].Position)), bipwbuf[bipnum][5] |= (1<<(BipTable[i].Position));
                    }
                    if (BipTable[i].Color == 'A') {
                        // Make BipTable[i].Position position Middle Row Amber
                        bipwbuf[bipnum][2] |= (1<<(BipTable[i].Position)), bipwbuf[bipnum][5] |= (1<<(BipTable[i].Position));
                    }
                }

                if (BipTable[i].Row == 'C') {
                    if (BipTable[i].Color == 'G') {
                        // Make BipTable[i].Position position Bottom Row Green
                        bipwbuf[bipnum][3] |= (1<<(BipTable[i].Position)), bipwbuf[bipnum][6] &= ~(1<<(BipTable[i].Position));
                    }
                    if (BipTable[i].Color == 'R') {
                        // Make BipTable[i].Position position Bottom Row Red
                        bipwbuf[bipnum][3] &= ~(1<<(BipTable[i].Position)), bipwbuf[bipnum][6] |= (1<<(BipTable[i].Position));
                    }
                    if (BipTable[i].Color == 'A') {
                        // Make BipTable[i].Position position Bottom Row Amber
                        bipwbuf[bipnum][3] |= (1<<(BipTable[i].Position)), bipwbuf[bipnum][6] |= (1<<(BipTable[i].Position));
                    }
                }
            }

            else {
                //printf("**********************   OFF   *********************************\n");
                if (BipTable[i].Row == 'A') {
                    // Make BipTable[i].Position position Top Row Off
                    bipwbuf[bipnum][1] &= ~(1<<(BipTable[i].Position)), bipwbuf[bipnum][4] &= ~(1<<(BipTable[i].Position));
                }
                if (BipTable[i].Row == 'B') {
                    // Make BipTable[i].Position position Middle Row Off
                    bipwbuf[bipnum][2] &= ~(1<<(BipTable[i].Position)), bipwbuf[bipnum][5] &= ~(1<<(BipTable[i].Position));
                }
                if (BipTable[i].Row == 'C')  {
                    // Make BipTable[i].Position position Bottom Row Off
                    bipwbuf[bipnum][3] &= ~(1<<(BipTable[i].Position)), bipwbuf[bipnum][6] &= ~(1<<(BipTable[i].Position));
                }


            }

        }

    }

    for (i=0; i<MAXINDICATORS; i++) {
        LastValues[i] = ActualValues[i];
    }

    if (biploop[bipnum] < 2) {
      // Clear Display on first loop
      bipwbuf[bipnum][0] = 0xb8;  //0xb8 Report ID to display
      bipwbuf[bipnum][1] = 0, bipwbuf[bipnum][2] = 0, bipwbuf[bipnum][3] = 0;
      bipwbuf[bipnum][4] = 0, bipwbuf[bipnum][5] = 0, bipwbuf[bipnum][6] = 0;
      res = hid_send_feature_report(biphandle[bipnum], bipwbuf[bipnum], 10);
      biploop[bipnum]++;
    }

    // Trying to only write on changes to improve FPS impact
    if(biploop[bipnum] == 2) {
        bipchange = memcmp(bipwbuf[bipnum], lastbipwbuf[bipnum], 10);
      if (bipchange == 0) {
      }
      if (bipchange != 0) {
        if (XPLMGetDatai(BatPwrOn) == 0) {
              bipwbuf[bipnum][0] = 0xb8;  //0xb8 Report ID to display
              bipwbuf[bipnum][1] = 0, bipwbuf[bipnum][2] = 0, bipwbuf[bipnum][3] = 0;
              bipwbuf[bipnum][4] = 0, bipwbuf[bipnum][5] = 0, bipwbuf[bipnum][6] = 0;
              res = hid_send_feature_report(biphandle[bipnum], bipwbuf[bipnum], 10);
              memcpy(lastbipwbuf[bipnum], bipwbuf[bipnum], 10);
        }
        if (XPLMGetDatai(BatPwrOn) == 1) {
            res = hid_send_feature_report(biphandle[bipnum], bipwbuf[bipnum], 10);
            memcpy(lastbipwbuf[bipnum], bipwbuf[bipnum], 10);
        }
    }

  }

  // *********** loop untill all bips serviced *************
  // **************   then start again    *******************

  bipnum++;
  if (bipnum == bipcnt) {
    bipnum = 0;
  }

  return;
}
