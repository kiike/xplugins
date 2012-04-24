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

    char *xspdefaultConfigFile;
    const char *xspdefaultConfigFileName;

    xspdefaultConfigFile = "./Resources/plugins/Xsaitekpanels/xsaitekpanels.ini";
    xspdefaultConfigFileName = "xsaitekpanels.ini";

    /* set defaults */

    bataltinverse            = 0,
    fuelpumpswitchenable     = 1,
    deiceswitchenable        = 1,
    cowlflapsenable          = 1,
    panellightsenable        = 1,
    landinggearknobenable    = 1,
    radspeed                 = 3,
    numadf                   = 1,
    trimspeed                = 1,
    multispeed               = 3,
    autothrottleswitchenable = 1;
    metricpressenable        = 0;


    char xspacfFilename[256], xspacfFullPath[512];

    char *xspconfigPath;

    cleanupIniReader();

    // Get user aircraft filename and full path
    XPLMGetNthAircraftModel(0, xspacfFilename, xspacfFullPath);

    // Put the xspacfFilename and xspacfFullPath in Log.txt
    XPLMDebugString("xspacfFilename = ");
    XPLMDebugString(xspacfFilename);
    XPLMDebugString("\n");
    XPLMDebugString("xspacfFullPath = ");
    XPLMDebugString(xspacfFullPath);
    XPLMDebugString("\n");

    if(strlen(xspacfFullPath) == 0){
      XPLMDebugString("strlen(xspacfFullPath) == 0");
      return;
    }

    xspconfigPath = strstr(xspacfFullPath, xspacfFilename);
    XPLMDebugString("xspconfigPath = ");
    XPLMDebugString(xspconfigPath);
    XPLMDebugString("\n");

    strncpy(xspconfigPath, xspdefaultConfigFileName, sizeof(xspacfFilename));
    XPLMDebugString("xspconfigPath = ");
    XPLMDebugString(xspconfigPath);
    XPLMDebugString("\n");

    XPLMDebugString("xspdefaultConfigFileName = ");
    XPLMDebugString(xspdefaultConfigFileName);
    XPLMDebugString("\n");

    puts(xspacfFullPath);
    XPLMDebugString("xspacfFullPath = ");
    XPLMDebugString(xspacfFullPath);
    XPLMDebugString("\n");

    std::ifstream ifile(xspacfFullPath);
    if (ifile) {
        parseIniFile(xspacfFullPath);
        XPLMDebugString("Found xsaitekpanels.ini in the current aircraft path and it is\n");
        XPLMDebugString("xspacfFullPath = ");
        XPLMDebugString(xspacfFullPath);
        XPLMDebugString("\n");
    } else {
        std::ifstream ifile(xspdefaultConfigFile);
       if (ifile) {
           XPLMDebugString("Found xsaitekpanels.ini in the Xsaitekpanels plugin path and it is\n");
           XPLMDebugString("xspdefaultConfigFile = ");
           XPLMDebugString(xspdefaultConfigFile);
           XPLMDebugString("\n");
           parseIniFile(xspdefaultConfigFile);
       } else {
           XPLMDebugString("Did not find xsaitekpanels.ini anywhere\n");
           return;
       }
    }

    bataltinverse = getOptionToInt("Bat Alt inverse");
    fuelpumpswitchenable = getOptionToInt("Fuel Pump Switch enable");
    deiceswitchenable = getOptionToInt("Deice Switch enable");
    cowlflapsenable = getOptionToInt("Cowl Flaps enable");
    panellightsenable = getOptionToInt("Panel Lights Switch enable");
    landinggearknobenable = getOptionToInt("Landing Gear Knob enable");
    radspeed = getOptionToInt("Radio Freq Knob Pulse per Command");
    numadf = getOptionToInt("Radio Number of ADF's");
    trimspeed = getOptionToInt("Multi Trim Speed");
    multispeed = getOptionToInt("Multi Freq Knob Pulse per Command");
    autothrottleswitchenable = getOptionToInt("Auto Throttle Switch enable");
    metricpressenable = getOptionToInt("Metric Press enable");

    if (fuelpumpswitchenable == 2) {
        fuel_pump_switch_on = getOptionToString("fuel_pump_switch_on_cmd");
        fuel_pump_switch_off = getOptionToString("fuel_pump_switch_off_cmd");
        FuelPumpOnCmd   = XPLMFindCommand(fuel_pump_switch_on.c_str());
        FuelPumpOffCmd   = XPLMFindCommand(fuel_pump_switch_off.c_str());
    }

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
