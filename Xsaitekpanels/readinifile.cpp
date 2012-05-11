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
#include <vector>


#include <iostream>
#include <fstream>
//#include <string>
#include <string.h>



string convert_Mac_Path(string in_path) {

    string out_path;

    XPLMDebugString("in_path = ");
    XPLMDebugString(in_path.c_str());
    XPLMDebugString("\n");

    std::size_t len = in_path.length();
    std::size_t pos = in_path.find(":");
    in_path.erase (in_path.begin()+0, in_path.end()-(len - pos));

    XPLMDebugString("in_path = ");
    XPLMDebugString(in_path.c_str());
    XPLMDebugString("\n");

    // Todo Find how many path seperators in the path

    size_t found;
    int n = 8;

    while (n>0) {
       found = in_path.find(":");
       in_path.replace(found, 1,"/");
       --n;
    }

    out_path = in_path;
    return out_path;

}


// ***** Configuration File Process ******
void process_read_ini_file()

{

    char *iniDefaultPluginPath;
    const char *defaultConfigFileName;

    iniDefaultPluginPath = "./Resources/plugins/Xsaitekpanels/xsaitekpanels.ini";
    defaultConfigFileName = "xsaitekpanels.ini";

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

    char *configPath;

    //int mac;
    //string mac_converted_path;

    cleanupIniReader();

    char xpsacfname[512];
    char xpsacfpath[512];
    XPLMGetNthAircraftModel(0, xpsacfname, xpsacfpath);

    XPLMDebugString("Raw Current aircraft path = ");
    XPLMDebugString(xpsacfpath);
    XPLMDebugString("\n");

    if(strlen(xpsacfpath) == 0){
      return;
    }

    std::string xpsini_path_name = xpsacfpath;
    XPLMDebugString("xpsini_path_name = ");
    XPLMDebugString(xpsini_path_name.c_str());
    XPLMDebugString("\n");

    std::size_t pos = xpsini_path_name.find(xpsacfname);
    xpsini_path_name = xpsini_path_name.substr(0, pos);

    XPLMDebugString("xpsini_path_name = ");
    XPLMDebugString(xpsini_path_name.c_str());
    XPLMDebugString("\n");

    #if APL && __MACH__
        std::string mac_converted_path = convert_Mac_Path(xpsini_path_name);
        XPLMDebugString("mac_converted_path = ");
        XPLMDebugString(mac_converted_path.c_str());
        XPLMDebugString("\n");
        xpsini_path_name = mac_converted_path;
    #endif

    XPLMDebugString("xpsini_path_name = ");
    XPLMDebugString(xpsini_path_name.c_str());
    XPLMDebugString("\n");

    xpsini_path_name.append("xsaitekpanels.ini");

    XPLMDebugString("xpsini_path_name = ");
    XPLMDebugString(xpsini_path_name.c_str());
    XPLMDebugString("\n");

    std::vector<char> parse_ini_path_name(xpsini_path_name.size() + 1);
    std::copy(xpsini_path_name.begin(), xpsini_path_name.end(), parse_ini_path_name.begin());

    std::ifstream ifile(&parse_ini_path_name[0]);
    if (ifile) {
        XPLMDebugString("Found xsaitekpanels.ini in the current aircraft path and it is\n");
        XPLMDebugString(xpsini_path_name.c_str());
        XPLMDebugString("\n");

        parseIniFile(&parse_ini_path_name[0]);

    } else {
        std::ifstream ifile(iniDefaultPluginPath);
       if (ifile) {
           XPLMDebugString("Found xsaitekpanels.ini in the Xsaitekpanels plugin path and it is\n");
           XPLMDebugString(iniDefaultPluginPath);
           XPLMDebugString("\n");

           parseIniFile(iniDefaultPluginPath);

       } else {
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
    metricpressenable= getOptionToInt("Metric Press enable");

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
