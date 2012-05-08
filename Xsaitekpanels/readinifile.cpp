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
#include "iniReader.h"
#include <vector>

//#include "settings.h" // from PPL
//#include "pluginpath.h" // from PPL
//#include "namespaces.h" // from PPL
//#include "SimpleIni.h" // from PPL

//using namespace PPLXSP;


/*
string convert_Mac_Path() {

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

}

*/



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

    cleanupIniReader();


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
    std::string xpsini_path_name = xpspath;
    XPLMDebugString("xpsini_path_name = ");
    XPLMDebugString(xpspath);
    XPLMDebugString("\n");

    std::size_t pos = xpsini_path_name.find(xpsname);
    xpsini_path_name = xpsini_path_name.substr(0, pos);

    xpsini_path_name.append("xsaitekpanels.ini");

    XPLMDebugString("xpsini_path_name = ");
    XPLMDebugString(xpsini_path_name.c_str());
    XPLMDebugString("\n");

    inipluginpath = "./Resources/plugins/Xsaitekpanels/xsaitekpanels.ini";

    // the name of the file, regardless of the directory
    //std::string config_file_name = "xsaitekpanels.ini";
/*
    #if APL && __MACH__
        // the name of the file, regardless of the directory
        config_file_name = "/xsaitekpanels.ini";
    #endif
*/
    // now put the path to the aircraft directory in front of it
    //std::string xpsini_file_absolute_path = PPLXSP::PluginPath::prependPlanePath(config_file_name);

    XPLMDebugString("Converted Current aircraft path = ");
    //XPLMDebugString(xpsini_file_absolute_path.c_str());
    XPLMDebugString("\n");

    // Check if ACF-specific configuration exists
    //char * parse_ini_path_name = xpsini_path_name.c_str();
    //char * parse_ini_path_name = new char[xpsini_path_name.size() + 1];

    std::vector<char> parse_ini_path_name(xpsini_path_name.size() + 1);
    std::copy(xpsini_path_name.begin(), xpsini_path_name.end(), parse_ini_path_name.begin());

    std::ifstream xpscustomStream(xpsini_path_name.c_str());
    if (xpscustomStream.good()) {
        XPLMDebugString("Found xsaitekpanels.ini in the current aircraft path and it is\n");
        XPLMDebugString(xpsini_path_name.c_str());
        XPLMDebugString("\n");

        parseIniFile(&parse_ini_path_name[0]);
        //foundinipath = xpsini_path_name.c_str();
    } else {
        std::ifstream xpsdefaultStream(inipluginpath);
        if (xpsdefaultStream.good()) {
            XPLMDebugString("Found xsaitekpanels.ini in the Xsaitekpanels plugin path and it is\n");
            XPLMDebugString(inipluginpath);
            XPLMDebugString("\n");
            parseIniFile(inipluginpath);
            //foundinipath = inipluginpath;
        }

    }


    //PPLXSP::Settings settings(foundinipath, true, false);

    //testfile = settings.loadFromFile(); // always is returning true it is always a 1

    // get some values from xsaitekpanels.ini file
    bataltinverse = getOptionToInt("Bat Alt inverse");
    fuelpumpswitchenable = getOptionToInt("Fuel Pump Switch enable");
    deiceswitchenable = getOptionToInt("Deice Switch enable");
    cowlflapsenable = getOptionToInt("Cowl Flaps enable");
    panellightsenable = getOptionToInt("Panel Lights Switch enable");
    landinggearknobenable = getOptionToInt("Landing Gear Knob enable");

    radspeed = getOptionToInt("Radio Freq Knob Pulse per Command");
    numadf = getOptionToInt("Radio Number of ADF's");
    metricpressenable = getOptionToInt("Metric Press enable");

    trimspeed = getOptionToInt("Multi Trim Speed");
    multispeed = getOptionToInt("Multi Freq Knob Pulse per Command");
    autothrottleswitchenable = getOptionToInt("Auto Throttle Switch enable");


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
