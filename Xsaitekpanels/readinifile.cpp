// ****** readinifile.cpp **********
// ****  William R. Good  ********

#include "XPLMPlugin.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMCamera.h"
#include "XPLMPlanes.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"
#include "XPLMMenus.h"



#include "saitekpanels.h"
#include "inireader.h"

#include <iostream>
#include <fstream>
//#include <string>
#include <string.h>

// ***** Configuration File Process ******
void process_read_ini_file()

{

    char *defaultConfigFile;
    const char *defaultConfigFileName;

    defaultConfigFile = "./Resources/plugins/Xsaitekpanels/xsaitekpanels.ini";
    defaultConfigFileName = "xsaitekpanels.ini";

    /* set defaults */

    bataltinverse            = 0,
    deiceswitchenable        = 1,
    cowlflapsenable          = 1,
    panellightsenable        = 1,
    landinggearknobenable    = 1,
    radspeed                 = 3,
    numadf                   = 1,
    trimspeed                = 1,
    multispeed               = 3,
    autothrottleswitchenable = 1;

    char acfFilename[256], acfFullPath[512];

    char *configPath;
    cleanupIniReader();

    XPLMGetNthAircraftModel(0, acfFilename, acfFullPath);

    if(strlen(acfFullPath) == 0){
      return;
    }

    configPath = strstr(acfFullPath, acfFilename);
    strncpy(configPath, defaultConfigFileName, sizeof(acfFilename));
    puts(acfFullPath);

    std::ifstream ifile(acfFullPath);
    if (ifile) {
        parseIniFile(acfFullPath);
    } else {
        parseIniFile(defaultConfigFile);
    }

    bataltinverse = getOptionToInt("Bat Alt inverse");
    deiceswitchenable = getOptionToInt("Deice Switch enable");
    cowlflapsenable = getOptionToInt("Cowl Flaps enable");
    panellightsenable = getOptionToInt("Panel Lights Switch enable");
    landinggearknobenable = getOptionToInt("Landing Gear Knob enable");
    radspeed = getOptionToInt("Radio Freq Knob Pulse per Command");
    numadf = getOptionToInt("Radio Number of ADF's");
    trimspeed = getOptionToInt("Multi Trim Speed");
    multispeed = getOptionToInt("Multi Freq Knob Pulse per Command");
    autothrottleswitchenable = getOptionToInt("Auto Throttle Switch enable");

    if (deiceswitchenable == 2) {
        deice_switch_on = getOptionToString("deice_switch_on_cmd");
        deice_switch_off = getOptionToString("deice_switch_off_cmd");
        DeiceOnCmd   = XPLMFindCommand(deice_switch_on.c_str());
        DeiceOffCmd   = XPLMFindCommand(deice_switch_off.c_str());
    }

    if (cowlflapsenable == 2) {
        cowl_flaps_open = getOptionToString("cowl_flaps_open_cmd");
        cowl_flaps_close = getOptionToString("cowl_flaps_close_cmd");
        CowlFlapsOpenCmd   = XPLMFindCommand(cowl_flaps_open.c_str());
        CowlFlapsCloseCmd   = XPLMFindCommand(cowl_flaps_close.c_str());
    }

    if (panellightsenable == 2) {
        panel_lights_switch_on = getOptionToString("panel_lights_switch_on_cmd");
        panel_lights_switch_off = getOptionToString("panel_lights_switch_off_cmd");
        PanelLightsOnCmd   = XPLMFindCommand(panel_lights_switch_on.c_str());
        PanelLightsOffCmd   = XPLMFindCommand(panel_lights_switch_off.c_str());
    }

    if (landinggearknobenable == 2) {
        gear_switch_up = getOptionToString("gear_switch_up_cmd");
        gear_switch_down = getOptionToString("gear_switch_down_cmd");
        GearUpCmd   = XPLMFindCommand(gear_switch_up.c_str());
        GearDnCmd   = XPLMFindCommand(gear_switch_down.c_str());
    }

  return;
}
