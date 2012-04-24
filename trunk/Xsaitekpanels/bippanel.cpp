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
#include "XPLMPlanes.h"

#include "hidapi.h"
#include "saitekpanels.h"

#include <time.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

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

static int bipchange, biploop[4], res, i[4];
static int bip0loop, bip1loop;

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

static BipTableStructure    BipTable[4][MAXTABLEELEMENTS];
static int                  LastValues[4][MAXINDICATORS];
static int                  ActualValues[4][MAXINDICATORS];
static int                  LastTableElement[4] = {-1, -1, -1, -1};
static int                  ErrorInLine = 0;
static bool                 InSilentMode = false;

static char     MenuEntries[4][50][250];

static int      LastMenuEntry[4] = {0, 0, 0, 0};

bool ReadConfigFile(string PlaneICAO);
void WriteCSVTableToDisk(void);


void LetWidgetSay(string BipTextToDisplay)
{

    XPSetWidgetDescriptor(BipWidgetID, BipTextToDisplay.c_str());
    int x = XPLMGetDatai(XPLMFindDataRef("sim/graphics/view/window_width")) - (int) XPLMMeasureString(xplmFont_Proportional, BipTextToDisplay.c_str(), BipTextToDisplay.length()) - 10;
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
        CSVFile << "\"" << BipTable[0][i].Row << "\",";
        CSVFile << "\"" << BipTable[0][i].Position << "\",";
        CSVFile << "\"" << BipTable[0][i].Color << "\",";
        CSVFile << "\"" << BipTable[0][i].DataRefToSet << "\",";
        CSVFile << "\"" << BipTable[0][i].DataRefType << "\",";
        CSVFile << "\"" << BipTable[0][i].DataRefIndex << "\",";
        CSVFile << "\"" << BipTable[0][i].WhatToDo << "\",";
        CSVFile << "\"" << BipTable[0][i].FloatLimit << "\",";
        CSVFile << "\"" << BipTable[0][i].CSVDebugString << "\"\n";
    }
    CSVFile.close();
}


char* bipgetConfigurationPath() {
    char bipacfFilename[256], bipacfFullPath[512];
    char *bipconfigPath;
    char *bipdefaultConfigFile;
    const char *bipdefaultConfigFileName;
    bipdefaultConfigFile = "./Resources/plugins/Xsaitekpanels/D2B_config.txt";
    bipdefaultConfigFileName = "D2B_config.txt";

    // Get user aircraft filename and full path
    XPLMGetNthAircraftModel(0, bipacfFilename, bipacfFullPath);

    // Put the xspacfFilename and xspacfFullPath in Log.txt
    XPLMDebugString("bipacfFilename = ");
    XPLMDebugString(bipacfFilename);
    XPLMDebugString("\n");
    XPLMDebugString("bipacfFullPath = ");
    XPLMDebugString(bipacfFullPath);
    XPLMDebugString("\n");

    bipconfigPath = strstr(bipacfFullPath, bipacfFilename);
    XPLMDebugString("bipconfigPath = ");
    XPLMDebugString(bipconfigPath);
    XPLMDebugString("\n");

    strncpy(bipconfigPath, bipdefaultConfigFileName, sizeof(bipacfFilename));
    XPLMDebugString("bipdefaultConfigFileName = ");
    XPLMDebugString(bipdefaultConfigFileName);
    XPLMDebugString("\n");

    puts(bipacfFullPath);
    XPLMDebugString("bipacfFullPath = ");
    XPLMDebugString(bipacfFullPath);
    XPLMDebugString("\n");

    // Check if ACF-specific configuration exists
    std::ifstream bipcustomStream(bipacfFullPath);
    if (bipcustomStream.good()) {
        XPLMDebugString("Found D2B_config.txt in the current aircraft path and it is\n");
        XPLMDebugString("bipacfFullPath = ");
        XPLMDebugString(bipacfFullPath);
        XPLMDebugString("\n");
        return bipacfFullPath;
    } else {
        std::ifstream bipdefaultStream(bipdefaultConfigFile);
        if (bipdefaultStream.good()) {
            XPLMDebugString("Found D2B_config.txt in the Xsaitekpanels plugin path and it is\n");
            XPLMDebugString("bipdefaultConfigFile = ");
            XPLMDebugString(bipdefaultConfigFile);
            XPLMDebugString("\n");
            return bipdefaultConfigFile;
        }

    }
}


char* bip2getConfigurationPath() {
    char bip2acfFilename[256], bip2acfFullPath[512];
    char *bip2configPath;
    char *bip2defaultConfigFile;
    const char *bip2defaultConfigFileName;
    bip2defaultConfigFile = "./Resources/plugins/Xsaitekpanels/D2B_config2.txt";
    bip2defaultConfigFileName = "D2B_config2.txt";
    XPLMGetNthAircraftModel(0, bip2acfFilename, bip2acfFullPath);

    bip2configPath = strstr(bip2acfFullPath, bip2acfFilename);
    strncpy(bip2configPath, bip2defaultConfigFileName, sizeof(bip2acfFilename));
    puts(bip2acfFullPath);

    // Check if ACF-specific configuration exists
    std::ifstream bip2customStream(bip2acfFullPath);
    if (bip2customStream.good()) {
        return bip2acfFullPath;
    } else {
        std::ifstream bip2defaultStream(bip2defaultConfigFile);
        if (bip2defaultStream.good()) {
            return bip2defaultConfigFile;
        }

    }
}

bool ReadConfigFile(string PlaneICAO)
{

  string          LineToEncrypt[4];
  bool            CorrectICAO = true;
  char            RowString[1];
  int             BipPosition;
  char            ColorString[1];
  char            DataRefString[512];
  XPLMDataRef     DataRefNumber;
  XPLMDataTypeID  DataRefType;
  float           Argument, Limit;
  int             Index;
  int             i, i1;

  char           *bip1ConfigurationPath;
  char           *bip2ConfigurationPath;
  char *test, *test2;
  char *test_path;
  char *x;


  PlaneICAO.erase(PlaneICAO.find(']')+1);
  LetWidgetSay(PlaneICAO);

  LastMenuEntry[0] = -1;
  LastMenuEntry[1] = -1;

  if(bipcnt > 0) {

    bip1ConfigurationPath = bipgetConfigurationPath();

    // For Windows uncomment the next line untill I find a better soultion
    //bip1ConfigurationPath = "./Resources/plugins/Xsaitekpanels/D2B_config.txt";

    ifstream ReadBipFile;
    ReadBipFile.open(bip1ConfigurationPath);

    if (ReadBipFile.is_open() != true)
    {
      logMsg("Error: Can't read D2B_config config file!");
      return false;
    }
    ErrorInLine = 0;

    LastTableElement[0] = -1;
    for (i = 0; i < MAXTABLEELEMENTS; i++)
    {
      BipTable[0][i].Row = '0';
      BipTable[0][i].Position = 0;
      BipTable[0][i].Color = '0';
      BipTable[0][i].DataRefToSet = NULL;
      BipTable[0][i].DataRefType = 0;
      BipTable[0][i].DataRefIndex = 0;
      BipTable[0][i].WhatToDo = '0';
      BipTable[0][i].FloatValueToSet = 0;
      BipTable[0][i].FloatLimit = 0;
      BipTable[0][i].CSVDebugString = "";
    }

    while (getline(ReadBipFile, LineToEncrypt[0]))
    {

        ErrorInLine++;
        if (LineToEncrypt[0].find("#BE SILENT") == 0)
        {
            InSilentMode = true;
            continue;
        }
        if (LineToEncrypt[0].find("#BE CHATTY") == 0)
        {
            InSilentMode = false;
            continue;
        }
        if (LineToEncrypt[0].find("#SHOW ICAO") == 0)
        {
            XPShowWidget(BipWidgetID);
            continue;
        }
        if (LineToEncrypt[0].find("#HIDE ICAO") == 0)
        {
            XPHideWidget(BipWidgetID);
            continue;
        }
        if (LineToEncrypt[0].find("[") == 0)
        {
            if ((LineToEncrypt[0].find("[DEFAULT]") == 0) || (LineToEncrypt[0].find(PlaneICAO) == 0))
            {
                CorrectICAO = true;
            }
            else
            {
                CorrectICAO = false;
            }
            if ((LineToEncrypt[0].find("[DEFAULT]") != 0) && (++LastMenuEntry[0] < 50))
            {
                strcpy(MenuEntries[0][LastMenuEntry[0]], LineToEncrypt[0].c_str());

            }
        }
        if (!CorrectICAO) continue;

        if (LineToEncrypt[0].find("#RESET AUTHORITY") == 0)
        {
            if (++LastTableElement[0] >= MAXTABLEELEMENTS)
            {
                logMsg("Xdataref2BIP: Fatal Error: Too much code to handle!");
                ReadBipFile.close();
                LastTableElement[0] = MAXTABLEELEMENTS - 1;
                return false;
            }
            BipTable[0][LastTableElement[0]].WhatToDo = 'T';
            continue;
        }

        if (sscanf(LineToEncrypt[0].c_str(), "#SET BIP %c %i %c FROM ARRAY %s %i RANGE %f TO %f", RowString, &BipPosition, ColorString, DataRefString, &Index, &Argument, &Limit) == 7)
        {
            if (++LastTableElement[0] >= MAXTABLEELEMENTS)
            {
                logMsg("Xdataref2BIP: Fatal Error: Too much code to handle!");
                ReadBipFile.close();
                LastTableElement[0] = MAXTABLEELEMENTS - 1;
                return false;
            }
            DataRefNumber = XPLMFindDataRef(DataRefString);
            if (DataRefNumber == NULL)
            {
                logMsg("Xdataref2BIP: A DataRef you want to use is not defined!");
                ReadBipFile.close();
                return false;
            }
            DataRefType = XPLMGetDataRefTypes(DataRefNumber);
            if (!((DataRefType == xplmType_IntArray) || (DataRefType == xplmType_FloatArray)))
            {
                logMsg("Xdataref2BIP: A DataRef you want to use can not be read (wrong type)!");
                ReadBipFile.close();
                return false;
            }

            if ((BipPosition >= MAXINDICATORS) || (BipPosition < 0))
            {
                logMsg("Xdataref2BIP: Indicator does not exist!");
                ReadBipFile.close();
                LastTableElement[0] = MAXTABLEELEMENTS - 1;
                return false;
            }
            BipTable[0][LastTableElement[0]].Row = *RowString;
            BipTable[0][LastTableElement[0]].Position = BipPosition;
            BipTable[0][LastTableElement[0]].Color = *ColorString;
            BipTable[0][LastTableElement[0]].DataRefToSet = DataRefNumber;
            BipTable[0][LastTableElement[0]].DataRefType = DataRefType;
            BipTable[0][LastTableElement[0]].DataRefIndex = Index;
            BipTable[0][LastTableElement[0]].FloatValueToSet = Argument;
            BipTable[0][LastTableElement[0]].FloatLimit = Limit;
            BipTable[0][LastTableElement[0]].WhatToDo = 'v';
            BipTable[0][LastTableElement[0]].CSVDebugString = DataRefString;
            continue;
        }
        if (sscanf(LineToEncrypt[0].c_str(), "#SET BIP %c %i %c FROM DATAREF %s RANGE %f TO %f", RowString, &BipPosition, ColorString, DataRefString, &Argument, &Limit) == 6)
        {
            if (++LastTableElement[0] >= MAXTABLEELEMENTS)
            {
                logMsg("Xdataref2BIP: Fatal Error: Too much code to handle!");
                ReadBipFile.close();
                LastTableElement[0] = MAXTABLEELEMENTS - 1;
                return false;
            }
            DataRefNumber = XPLMFindDataRef(DataRefString);
            if (DataRefNumber == NULL)
            {
                logMsg("Xdataref2BIP: A DataRef you want to use is not defined!");
                ReadBipFile.close();
                return false;
            }
            DataRefType = XPLMGetDataRefTypes(DataRefNumber);
            if (!((DataRefType == xplmType_Int) || (DataRefType == xplmType_Float)))
            {
                logMsg("Xdataref2BIP: A DataRef you want to use can not be read (wrong type)!");
                ReadBipFile.close();
                return false;
            }
            if ((BipPosition >= MAXINDICATORS) || (BipPosition < 0))
            {
                logMsg("Xdataref2BIP: Indicator does not exist!");
                ReadBipFile.close();
                LastTableElement[0] = MAXTABLEELEMENTS - 1;
                return false;
            }
            BipTable[0][LastTableElement[0]].Row = *RowString;
            BipTable[0][LastTableElement[0]].Position = BipPosition;
            BipTable[0][LastTableElement[0]].Color = *ColorString;
            BipTable[0][LastTableElement[0]].DataRefToSet = DataRefNumber;
            BipTable[0][LastTableElement[0]].DataRefType = DataRefType;
            BipTable[0][LastTableElement[0]].FloatValueToSet = Argument;
            BipTable[0][LastTableElement[0]].FloatLimit = Limit;
            BipTable[0][LastTableElement[0]].WhatToDo = 'v';
            BipTable[0][LastTableElement[0]].CSVDebugString = DataRefString;
            continue;
        }


        if (LineToEncrypt[0].find('#') == 0)
        {
            logMsg("Xdataref2BIP: Can't understand the line of code!");
            ReadBipFile.close();
            LastTableElement[0] = MAXTABLEELEMENTS - 1;
            return false;
        }
    }

    ReadBipFile.close();
    //return true;

   }

  if(bipcnt > 1) {

     bip2ConfigurationPath = bip2getConfigurationPath();

    // uncoment the following line on Windows untill I have a beter solution.
    //bip2ConfigurationPath = "./Resources/plugins/Xsaitekpanels/D2B_config2.txt";

    ifstream ReadBip2File;
    ReadBip2File.open(bip2ConfigurationPath);

    if (ReadBip2File.is_open() != true)
    {
          logMsg("Error: Can't read D2B_config2 config file!");
          return false;
    }
    ErrorInLine = 0;

    LastTableElement[1] = -1;
    for (i1 = 0; i1 < MAXTABLEELEMENTS; i1++)
    {
        BipTable[1][i1].Row = '0';
        BipTable[1][i1].Position = 0;
        BipTable[1][i1].Color = '0';
        BipTable[1][i1].DataRefToSet = NULL;
        BipTable[1][i1].DataRefType = 0;
        BipTable[1][i1].DataRefIndex = 0;
        BipTable[1][i1].WhatToDo = '0';
        BipTable[1][i1].FloatValueToSet = 0;
        BipTable[1][i1].FloatLimit = 0;
        BipTable[1][i1].CSVDebugString = "";
    }

    while (getline(ReadBip2File, LineToEncrypt[1]))
    {

        ErrorInLine++;
        if (LineToEncrypt[1].find("#BE SILENT") == 0)
        {
            InSilentMode = true;
            continue;
        }
        if (LineToEncrypt[1].find("#BE CHATTY") == 0)
        {
            InSilentMode = false;
            continue;
        }
        if (LineToEncrypt[1].find("#SHOW ICAO") == 0)
        {
            XPShowWidget(Bip2WidgetID);
            continue;
        }
        if (LineToEncrypt[1].find("#HIDE ICAO") == 0)
        {
            XPHideWidget(Bip2WidgetID);
            continue;
        }
        if (LineToEncrypt[1].find("[") == 0)
        {
            if ((LineToEncrypt[1].find("[DEFAULT2]") == 0) || (LineToEncrypt[1].find(PlaneICAO) == 0))
            {
                CorrectICAO = true;
            }
            else
            {
                CorrectICAO = false;
            }
            if ((LineToEncrypt[1].find("[DEFAULT2]") != 0) && (++LastMenuEntry[1] < 50))
            {
                strcpy(MenuEntries[1][LastMenuEntry[1]], LineToEncrypt[1].c_str());

            }
        }
        if (!CorrectICAO) continue;

        if (LineToEncrypt[1].find("#RESET AUTHORITY") == 0)
        {
            if (++LastTableElement[1] >= MAXTABLEELEMENTS)
            {
                logMsg("Xdataref2BIP: Fatal Error: Too much code to handle!");
                ReadBip2File.close();
                LastTableElement[1] = MAXTABLEELEMENTS - 1;
                return false;
            }
            BipTable[1][LastTableElement[1]].WhatToDo = 'T';
            continue;
        }

        if (sscanf(LineToEncrypt[1].c_str(), "#SET BIP %c %i %c FROM ARRAY %s %i RANGE %f TO %f", RowString, &BipPosition, ColorString, DataRefString, &Index, &Argument, &Limit) == 7)
        {
            if (++LastTableElement[1] >= MAXTABLEELEMENTS)
            {
                logMsg("Xdataref2BIP: Fatal Error: Too much code to handle!");
                ReadBip2File.close();
                LastTableElement[1] = MAXTABLEELEMENTS - 1;
                return false;
            }
            DataRefNumber = XPLMFindDataRef(DataRefString);
            if (DataRefNumber == NULL)
            {
                logMsg("Xdataref2BIP: A DataRef you want to use is not defined!");
                ReadBip2File.close();
                return false;
            }
            DataRefType = XPLMGetDataRefTypes(DataRefNumber);
            if (!((DataRefType == xplmType_IntArray) || (DataRefType == xplmType_FloatArray)))
            {
                logMsg("Xdataref2BIP: A DataRef you want to use can not be read (wrong type)!");
                ReadBip2File.close();
                return false;
            }

            if ((BipPosition >= MAXINDICATORS) || (BipPosition < 0))
            {
                logMsg("Xdataref2BIP: Indicator does not exist!");
                ReadBip2File.close();
                LastTableElement[1] = MAXTABLEELEMENTS - 1;
                return false;
            }
            BipTable[1][LastTableElement[1]].Row = *RowString;
            BipTable[1][LastTableElement[1]].Position = BipPosition;
            BipTable[1][LastTableElement[1]].Color = *ColorString;
            BipTable[1][LastTableElement[1]].DataRefToSet = DataRefNumber;
            BipTable[1][LastTableElement[1]].DataRefType = DataRefType;
            BipTable[1][LastTableElement[1]].DataRefIndex = Index;
            BipTable[1][LastTableElement[1]].FloatValueToSet = Argument;
            BipTable[1][LastTableElement[1]].FloatLimit = Limit;
            BipTable[1][LastTableElement[1]].WhatToDo = 'v';
            BipTable[1][LastTableElement[1]].CSVDebugString = DataRefString;
            continue;
        }
        if (sscanf(LineToEncrypt[1].c_str(), "#SET BIP %c %i %c FROM DATAREF %s RANGE %f TO %f", RowString, &BipPosition, ColorString, DataRefString, &Argument, &Limit) == 6)
        {
            if (++LastTableElement[1] >= MAXTABLEELEMENTS)
            {
                logMsg("Xdataref2BIP: Fatal Error: Too much code to handle!");
                ReadBip2File.close();
                LastTableElement[1] = MAXTABLEELEMENTS - 1;
                return false;
            }
            DataRefNumber = XPLMFindDataRef(DataRefString);
            if (DataRefNumber == NULL)
            {
                logMsg("Xdataref2BIP: A DataRef you want to use is not defined!");
                ReadBip2File.close();
                return false;
            }
            DataRefType = XPLMGetDataRefTypes(DataRefNumber);
            if (!((DataRefType == xplmType_Int) || (DataRefType == xplmType_Float)))
            {
                logMsg("Xdataref2BIP: A DataRef you want to use can not be read (wrong type)!");
                ReadBip2File.close();
                return false;
            }
            if ((BipPosition >= MAXINDICATORS) || (BipPosition < 0))
            {
                logMsg("Xdataref2BIP: Indicator does not exist!");
                ReadBip2File.close();
                LastTableElement[1] = MAXTABLEELEMENTS - 1;
                return false;
            }
            BipTable[1][LastTableElement[1]].Row = *RowString;
            BipTable[1][LastTableElement[1]].Position = BipPosition;
            BipTable[1][LastTableElement[1]].Color = *ColorString;
            BipTable[1][LastTableElement[1]].DataRefToSet = DataRefNumber;
            BipTable[1][LastTableElement[1]].DataRefType = DataRefType;
            BipTable[1][LastTableElement[1]].FloatValueToSet = Argument;
            BipTable[1][LastTableElement[1]].FloatLimit = Limit;
            BipTable[1][LastTableElement[1]].WhatToDo = 'v';
            BipTable[1][LastTableElement[1]].CSVDebugString = DataRefString;
            continue;
        }


        if (LineToEncrypt[1].find('#') == 0)
        {
            logMsg("Xdataref2BIP: Can't understand the line of code!");
            ReadBip2File.close();
            LastTableElement[1] = MAXTABLEELEMENTS - 1;
            return false;
        }

    }

    ReadBip2File.close();
    //return true;

   }



return true;
}

void process_bip_menu()

{

    if(bipcnt > 0) {

      XPLMClearAllMenuItems(BipMenuId);
      XPLMAppendMenuItem(BipMenuId, "[DEFAULT]", (void *) "[DEFAULT]", 1);
      XPLMAppendMenuItem(BipMenuId, "Write a CSV Table for debugging", (void *) "<<CSV>>", 1);
      XPLMAppendMenuSeparator(BipMenuId);
      XPLMAppendMenuItem(BipMenuId, MenuEntries[0][LastMenuEntry[0]], (void *) MenuEntries[0][LastMenuEntry[0]], 1);

    }

    if(bipcnt > 1) {

        XPLMClearAllMenuItems(Bip2MenuId);
        XPLMAppendMenuItem(Bip2MenuId, "[DEFAULT2]", (void *) "[DEFAULT2]", 1);
        XPLMAppendMenuItem(Bip2MenuId, "Write a CSV Table for debugging", (void *) "<<CSV>>", 1);
        XPLMAppendMenuSeparator(Bip2MenuId);
        XPLMAppendMenuItem(Bip2MenuId, MenuEntries[1][LastMenuEntry[0]], (void *) MenuEntries[1][LastMenuEntry[0]], 1);
        XPLMAppendMenuItem(Bip2MenuId, MenuEntries[1][LastMenuEntry[1]], (void *) MenuEntries[1][LastMenuEntry[1]], 1);

    }

    return;

}

void process_bip_what_to_do_v()

{

    for (i[bipnum] = 0; i[bipnum] <= LastTableElement[bipnum]; i[bipnum]++) {

        if (BipTable[bipnum][i[bipnum]].WhatToDo == 'v') {
            // "#SET BIP %c %i %c FROM DATAREF %s RANGE %f TO %f" or "#SET BIP %s %i %s FROM ARRAY %s %i RANGE %f TO %f""
            if (BipTable[bipnum][i[bipnum]].DataRefType == xplmType_Float) LookAtThisValue = XPLMGetDataf(BipTable[bipnum][i[bipnum]].DataRefToSet);
            if (BipTable[bipnum][i[bipnum]].DataRefType == xplmType_Int) LookAtThisValue = (float) XPLMGetDatai(BipTable[bipnum][i[bipnum]].DataRefToSet);
            if (BipTable[bipnum][i[bipnum]].DataRefType == xplmType_FloatArray) XPLMGetDatavf(BipTable[bipnum][i[bipnum]].DataRefToSet, &LookAtThisValue, BipTable[bipnum][i[bipnum]].DataRefIndex, 1);
            if (BipTable[bipnum][i[bipnum]].DataRefType == xplmType_IntArray) {
                XPLMGetDatavi(BipTable[bipnum][i[bipnum]].DataRefToSet, &LookAtThisIntValue, BipTable[bipnum][i[bipnum]].DataRefIndex, 1);
                LookAtThisValue = (float) LookAtThisIntValue;
            }
            if ((LookAtThisValue >= BipTable[bipnum][i[bipnum]].FloatValueToSet) && (LookAtThisValue <= BipTable[bipnum][i[bipnum]].FloatLimit)) {

                //printf("********************************  ON   **************************************\n");
                if (BipTable[bipnum][i[bipnum]].Row == 'A') {
                    if (BipTable[bipnum][i[bipnum]].Color == 'G') {
                        // Make BipTable[i].Position position Top Row Green
                        bipwbuf[bipnum][1] |= (1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][4] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position));
                    }
                    if (BipTable[bipnum][i[bipnum]].Color == 'R') {
                        // Make BipTable[i].Position position Top Row Red
                        bipwbuf[bipnum][1] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][4] |= (1<<(BipTable[bipnum][i[bipnum]].Position));
                    }
                    if (BipTable[bipnum][i[bipnum]].Color == 'A') {
                        // Make BipTable[i].Position position Top Row Amber
                        bipwbuf[bipnum][1] |= (1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][4] |= (1<<(BipTable[bipnum][i[bipnum]].Position));
                    }    for (i[bipnum] = 0; i[bipnum] <= LastTableElement[bipnum]; i[bipnum]++) {

                        if (BipTable[bipnum][i[bipnum]].WhatToDo == 'v') {
                            // "#SET BIP %c %i %c FROM DATAREF %s RANGE %f TO %f" or "#SET BIP %s %i %s FROM ARRAY %s %i RANGE %f TO %f""
                            if (BipTable[bipnum][i[bipnum]].DataRefType == xplmType_Float) LookAtThisValue = XPLMGetDataf(BipTable[bipnum][i[bipnum]].DataRefToSet);
                            if (BipTable[bipnum][i[bipnum]].DataRefType == xplmType_Int) LookAtThisValue = (float) XPLMGetDatai(BipTable[bipnum][i[bipnum]].DataRefToSet);
                            if (BipTable[bipnum][i[bipnum]].DataRefType == xplmType_FloatArray) XPLMGetDatavf(BipTable[bipnum][i[bipnum]].DataRefToSet, &LookAtThisValue, BipTable[bipnum][i[bipnum]].DataRefIndex, 1);
                            if (BipTable[bipnum][i[bipnum]].DataRefType == xplmType_IntArray) {
                                XPLMGetDatavi(BipTable[bipnum][i[bipnum]].DataRefToSet, &LookAtThisIntValue, BipTable[bipnum][i[bipnum]].DataRefIndex, 1);
                                LookAtThisValue = (float) LookAtThisIntValue;
                            }
                            if ((LookAtThisValue >= BipTable[bipnum][i[bipnum]].FloatValueToSet) && (LookAtThisValue <= BipTable[bipnum][i[bipnum]].FloatLimit)) {

                                //printf("********************************  ON   **************************************\n");
                                if (BipTable[bipnum][i[bipnum]].Row == 'A') {
                                    if (BipTable[bipnum][i[bipnum]].Color == 'G') {
                                        // Make BipTable[i].Position position Top Row Green
                                        bipwbuf[bipnum][1] |= (1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][4] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position));
                                    }
                                    if (BipTable[bipnum][i[bipnum]].Color == 'R') {
                                        // Make BipTable[i].Position position Top Row Red
                                        bipwbuf[bipnum][1] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][4] |= (1<<(BipTable[bipnum][i[bipnum]].Position));
                                    }
                                    if (BipTable[bipnum][i[bipnum]].Color == 'A') {
                                        // Make BipTable[i].Position position Top Row Amber
                                        bipwbuf[bipnum][1] |= (1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][4] |= (1<<(BipTable[bipnum][i[bipnum]].Position));
                                    }
                                }

                                if (BipTable[bipnum][i[bipnum]].Row == 'B') {
                                    if (BipTable[bipnum][i[bipnum]].Color == 'G') {
                                        // Make BipTable[i].Position position Middle Row Green
                                        bipwbuf[bipnum][2] |= (1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][5] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position));
                                    }
                                    if (BipTable[bipnum][i[bipnum]].Color == 'R') {
                                        // Make BipTable[i].Position position Middle Row Red
                                        bipwbuf[bipnum][2] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][5] |= (1<<(BipTable[bipnum][i[bipnum]].Position));
                                    }
                                    if (BipTable[bipnum][i[bipnum]].Color == 'A') {
                                        // Make BipTable[i].Position position Middle Row Amber
                                        bipwbuf[bipnum][2] |= (1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][5] |= (1<<(BipTable[bipnum][i[bipnum]].Position));
                                    }
                                }

                                if (BipTable[bipnum][i[bipnum]].Row == 'C') {
                                    if (BipTable[bipnum][i[bipnum]].Color == 'G') {
                                        // Make BipTable[i].Position position Bottom Row Green
                                        bipwbuf[bipnum][3] |= (1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][6] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position));
                                    }
                                    if (BipTable[bipnum][i[bipnum]].Color == 'R') {
                                        // Make BipTable[i].Position position Bottom Row Red
                                        bipwbuf[bipnum][3] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][6] |= (1<<(BipTable[bipnum][i[bipnum]].Position));
                                    }
                                    if (BipTable[bipnum][i[bipnum]].Color == 'A') {
                                        // Make BipTable[i].Position position Bottom Row Amber
                                        bipwbuf[bipnum][3] |= (1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][6] |= (1<<(BipTable[bipnum][i[bipnum]].Position));
                                    }
                                }
                            }

                            else {
                                //printf("**********************   OFF   *********************************\n");
                                if (BipTable[bipnum][i[bipnum]].Row == 'A') {
                                    // Make BipTable[i].Position position Top Row Off
                                    bipwbuf[bipnum][1] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][4] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position));
                                }
                                if (BipTable[bipnum][i[bipnum]].Row == 'B') {
                                    // Make BipTable[i].Position position Middle Row Off
                                    bipwbuf[bipnum][2] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][5] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position));
                                }
                                if (BipTable[bipnum][i[bipnum]].Row == 'C')  {
                                    // Make BipTable[i].Position position Bottom Row Off
                                    bipwbuf[bipnum][3] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][6] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position));
                                }


                            }

                        }

                    }
                }

                if (BipTable[bipnum][i[bipnum]].Row == 'B') {
                    if (BipTable[bipnum][i[bipnum]].Color == 'G') {
                        // Make BipTable[i].Position position Middle Row Green
                        bipwbuf[bipnum][2] |= (1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][5] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position));
                    }
                    if (BipTable[bipnum][i[bipnum]].Color == 'R') {
                        // Make BipTable[i].Position position Middle Row Red
                        bipwbuf[bipnum][2] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][5] |= (1<<(BipTable[bipnum][i[bipnum]].Position));
                    }
                    if (BipTable[bipnum][i[bipnum]].Color == 'A') {
                        // Make BipTable[i].Position position Middle Row Amber
                        bipwbuf[bipnum][2] |= (1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][5] |= (1<<(BipTable[bipnum][i[bipnum]].Position));
                    }
                }

                if (BipTable[bipnum][i[bipnum]].Row == 'C') {
                    if (BipTable[bipnum][i[bipnum]].Color == 'G') {
                        // Make BipTable[i].Position position Bottom Row Green
                        bipwbuf[bipnum][3] |= (1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][6] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position));
                    }
                    if (BipTable[bipnum][i[bipnum]].Color == 'R') {
                        // Make BipTable[i].Position position Bottom Row Red
                        bipwbuf[bipnum][3] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][6] |= (1<<(BipTable[bipnum][i[bipnum]].Position));
                    }
                    if (BipTable[bipnum][i[bipnum]].Color == 'A') {
                        // Make BipTable[i].Position position Bottom Row Amber
                        bipwbuf[bipnum][3] |= (1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][6] |= (1<<(BipTable[bipnum][i[bipnum]].Position));
                    }
                }
            }

            else {
                //printf("**********************   OFF   *********************************\n");
                if (BipTable[bipnum][i[bipnum]].Row == 'A') {
                    // Make BipTable[i].Position position Top Row Off
                    bipwbuf[bipnum][1] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][4] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position));
                }
                if (BipTable[bipnum][i[bipnum]].Row == 'B') {
                    // Make BipTable[i].Position position Middle Row Off
                    bipwbuf[bipnum][2] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][5] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position));
                }
                if (BipTable[bipnum][i[bipnum]].Row == 'C')  {
                    // Make BipTable[i].Position position Bottom Row Off
                    bipwbuf[bipnum][3] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position)), bipwbuf[bipnum][6] &= ~(1<<(BipTable[bipnum][i[bipnum]].Position));
                }


            }

        }

    }


 return;

}


// ***** BIP Panel Process  *******
void process_bip_panel()

{


    if (biploop[bipnum] < 2) {
      // Clear Display on first loop
      bipwbuf[bipnum][0] = 0xb8;  //0xb8 Report ID to display
      bipwbuf[bipnum][1] = 0, bipwbuf[bipnum][2] = 0, bipwbuf[bipnum][3] = 0;
      bipwbuf[bipnum][4] = 0, bipwbuf[bipnum][5] = 0, bipwbuf[bipnum][6] = 0;
      res = hid_send_feature_report(biphandle[bipnum], bipwbuf[bipnum], 10);
      biploop[bipnum]++;
    }


    process_bip_menu();
    process_bip_what_to_do_v();


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
