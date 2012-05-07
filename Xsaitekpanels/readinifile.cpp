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

#include <iostream>
#include <fstream>
//#include <string>
#include <string.h>

#include "settings.h" // from PPL
#include "pluginpath.h" // from PPL
//#include "namespaces.h" // from PPL
//#include "SimpleIni.h" // from PPL

using namespace PPLXSP;

// ***** Configuration File Process ******
void process_read_ini_file()

{

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

    // Start of PPL use

    bool testfile = 0;

    //char buf[32];

    char *inipluginpath;
    const char *foundinipath;

    char xpsname[512];
    char xpspath[512];
    XPLMGetNthAircraftModel(0, xpsname, xpspath);

    XPLMDebugString("Raw Current aircraft path = ");
    XPLMDebugString(xpspath);
    XPLMDebugString("\n");

    //std::string test_path_name = "EaNa MBP SL:Users:carlos:Desktop:X-Plane 10:Aircraft:General Aviation:Skyhawk N172AC:Cessna_172SP.acf";
    std::string test_path_name = "Macintosh HD:Users:davideschachter:Desktop:X-Plane 10:Aircraft:Heavy Metal:Boeing737-800_x737_v4_5_0:737.acf";
    XPLMDebugString("test_path_name = ");
    XPLMDebugString(test_path_name.c_str());
    XPLMDebugString("\n");

    std::size_t pos = test_path_name.find("737.acf");
    test_path_name = test_path_name.substr(0, pos);

    XPLMDebugString("test_path_name = ");
    XPLMDebugString(test_path_name.c_str());
    XPLMDebugString("\n");

    std::size_t len = test_path_name.length();
    pos = test_path_name.find(":");
    test_path_name.erase (test_path_name.begin()+0, test_path_name.end()-(len - pos));

    XPLMDebugString("test_path_name = ");
    XPLMDebugString(test_path_name.c_str());
    XPLMDebugString("\n");

    size_t found;
    int n = 8;

    while (n>0) {
       found = test_path_name.find(":");
       test_path_name.replace(found, 1,"/");
       --n;
    }


    XPLMDebugString("test_path_name = ");
    XPLMDebugString(test_path_name.c_str());
    XPLMDebugString("\n");

    inipluginpath = "./Resources/plugins/Xsaitekpanels/xsaitekpanels.ini";

    // the name of the file, regardless of the directory
    std::string config_file_name = "xsaitekpanels.ini";

    #if APL && __MACH__
        // the name of the file, regardless of the directory
        config_file_name = "/xsaitekpanels.ini";
    #endif

    // now put the path to the aircraft directory in front of it
    std::string xpsini_file_absolute_path = PPLXSP::PluginPath::prependPlanePath(config_file_name);

    XPLMDebugString("Converted Current aircraft path = ");
    XPLMDebugString(xpsini_file_absolute_path.c_str());
    XPLMDebugString("\n");

    // Check if ACF-specific configuration exists
    std::ifstream xpscustomStream(xpsini_file_absolute_path.c_str());
    if (xpscustomStream.good()) {
        XPLMDebugString("Found xsaitekpanels.ini in the current aircraft path and it is\n");
        XPLMDebugString(xpsini_file_absolute_path.c_str());
        XPLMDebugString("\n");
        foundinipath = xpsini_file_absolute_path.c_str();
    } else {
        std::ifstream xpsdefaultStream(inipluginpath);
        if (xpsdefaultStream.good()) {
            XPLMDebugString("Found xsaitekpanels.ini in the Xsaitekpanels plugin path and it is\n");
            XPLMDebugString(inipluginpath);
            XPLMDebugString("\n");
            foundinipath = inipluginpath;
        }

    }


    PPLXSP::Settings settings(foundinipath, true, false);

    testfile = settings.loadFromFile(); // always is returning true it is always a 1

    // get some values from xsaitekpanels.ini file
    bataltinverse = settings.getLong("switch", "Bat Alt inverse");
    fuelpumpswitchenable = settings.getLong("switch", "Fuel Pump Switch enable");
    deiceswitchenable = settings.getLong("switch", "Deice Switch enable");
    cowlflapsenable = settings.getLong("switch", "Cowl Flaps enable");
    panellightsenable = settings.getLong("switch", "Panel Lights Switch enable");
    landinggearknobenable = settings.getLong("switch", "Landing Gear Knob enable");

    radspeed = settings.getLong("radio", "Radio Freq Knob Pulse per Command");
    numadf = settings.getLong("radio", "Radio Number of ADF's");
    metricpressenable = settings.getLong("radio", "Metric Press enable");

    trimspeed = settings.getLong("multi", "Multi Trim Speed");
    multispeed = settings.getLong("multi", "Multi Freq Knob Pulse per Command");
    autothrottleswitchenable = settings.getLong("multi", "Auto Throttle Switch enable");


    if (fuelpumpswitchenable == 2) {
        fuel_pump_switch_on = settings.get("commands", "fuel_pump_switch_on_cmd");
        fuel_pump_switch_off = settings.get("commands", "fuel_pump_switch_off_cmd");
        FuelPumpOnCmd   = XPLMFindCommand(fuel_pump_switch_on.c_str());
        FuelPumpOffCmd   = XPLMFindCommand(fuel_pump_switch_off.c_str());
    }

    if (deiceswitchenable == 2) {
        deice_switch_on = settings.get("commands", "deice_switch_on_cmd");
        deice_switch_off = settings.get("commands", "deice_switch_off_cmd");
        DeiceOnCmd   = XPLMFindCommand(deice_switch_on.c_str());
        DeiceOffCmd   = XPLMFindCommand(deice_switch_off.c_str());
    }

    if (cowlflapsenable == 2) {
        cowl_flaps_open = settings.get("commands", "cowl_flaps_open_cmd");
        cowl_flaps_close = settings.get("commands", "cowl_flaps_close_cmd");
        CowlFlapsOpenCmd   = XPLMFindCommand(cowl_flaps_open.c_str());
        CowlFlapsCloseCmd   = XPLMFindCommand(cowl_flaps_close.c_str());
    }

    if (panellightsenable == 2) {
        panel_lights_switch_on = settings.get("commands", "panel_lights_switch_on_cmd");
        panel_lights_switch_off = settings.get("commands", "panel_lights_switch_off_cmd");
        PanelLightsOnCmd   = XPLMFindCommand(panel_lights_switch_on.c_str());
        PanelLightsOffCmd   = XPLMFindCommand(panel_lights_switch_off.c_str());
    }

    if (landinggearknobenable == 2) {
        gear_switch_up = settings.get("commands", "gear_switch_up_cmd");
        gear_switch_down = settings.get("commands", "gear_switch_down_cmd");
        GearUpCmd   = XPLMFindCommand(gear_switch_up.c_str());
        GearDnCmd   = XPLMFindCommand(gear_switch_down.c_str());
    }


  return;
}
