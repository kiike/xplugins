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

    std::size_t len = in_path.length();
    std::size_t pos = in_path.find(":");
    in_path.erase (in_path.begin()+0, in_path.end()-(len - pos));

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

    // switch panel defaults

    // switches
    magoffswitchenable           = 1;
    magrightswitchenable         = 1;
    magleftswitchenable          = 1;
    magbothswitchenable          = 1;
    magstartswitchenable         = 1;
    batmasterswitchenable        = 1;
    altmasterswitchenable        = 1;
    avionicsmasterswitchenable   = 1;
    fuelpumpswitchenable         = 1;
    deiceswitchenable            = 1;
    pitotheatswitchenable        = 1;
    landinggearknobupenable      = 1;
    landinggearknobdnenable      = 1;
    cowlflapsenable              = 1;
    panellightswitchenable       = 1;
    beaconlightswitchenable      = 1;
    navlightswitchenable         = 1;
    strobelightswitchenable      = 1;
    taxilightswitchenable        = 1;
    landinglightswitchenable     = 1;

    bataltinverse = 0;

    // radio panel
    radspeed                 = 3;
    numadf                   = 1;
    metricpressenable        = 0;

    // multi panel
    trimspeed                = 1;
    multispeed               = 3;
    autothrottleswitchenable = 1;


    char *configPath;

    cleanupIniReader();

    char xpsacfname[512];
    char xpsacfpath[512];
    XPLMGetNthAircraftModel(0, xpsacfname, xpsacfpath);

    XPLMDebugString("\nRaw Current aircraft path is \n");
    XPLMDebugString(xpsacfpath);
    XPLMDebugString("\n");

    if(strlen(xpsacfpath) == 0){
      return;
    }

    std::string xpsini_path_name = xpsacfpath;
    std::size_t pos = xpsini_path_name.find(xpsacfname);
    xpsini_path_name = xpsini_path_name.substr(0, pos);

    #if APL && __MACH__
        std::string mac_converted_path = convert_Mac_Path(xpsini_path_name);
        XPLMDebugString("\nmac_converted_path is \n");
        XPLMDebugString(mac_converted_path.c_str());
        XPLMDebugString("\n");
        xpsini_path_name = mac_converted_path;
    #endif

    xpsini_path_name.append("xsaitekpanels.ini");

    XPLMDebugString("\nThe full path to xsaitekpanels.ini in the current aircraft folder is \n");
    XPLMDebugString(xpsini_path_name.c_str());
    XPLMDebugString("\n");

    std::vector<char> parse_ini_path_name(xpsini_path_name.size() + 1);
    std::copy(xpsini_path_name.begin(), xpsini_path_name.end(), parse_ini_path_name.begin());

    std::ifstream ifile(&parse_ini_path_name[0]);
    if (ifile) {
        XPLMDebugString("\nFound xsaitekpanels.ini in the current aircraft path and it is\n");
        XPLMDebugString(&parse_ini_path_name[0]);
        XPLMDebugString("\n");

        parseIniFile(&parse_ini_path_name[0]);

    } else {
        std::ifstream ifile(iniDefaultPluginPath);
       if (ifile) {
           XPLMDebugString("\nFound xsaitekpanels.ini in the Xsaitekpanels plugin path and it is\n");
           XPLMDebugString(iniDefaultPluginPath);
           XPLMDebugString("\n");

           parseIniFile(iniDefaultPluginPath);

       } else {
           return;
       }
    }


    // mag off switch disable - enable - remap
    magoffswitchenable = getOptionToInt("Mag Off Switch enable");
    if (magoffswitchenable == 0) {
        XPSetWidgetProperty(SwitchDisableCheckWidget[0], xpProperty_ButtonState, 1);
        XPSetWidgetProperty(SwitchEnableCheckWidget[0], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchRemapCheckWidget[0], xpProperty_ButtonState, 0);
    }

    if (magoffswitchenable == 1) {
        XPSetWidgetProperty(SwitchDisableCheckWidget[0], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchEnableCheckWidget[0], xpProperty_ButtonState, 1);
        XPSetWidgetProperty(SwitchRemapCheckWidget[0], xpProperty_ButtonState, 0);
    }

    if (magoffswitchenable == 2) {
        XPSetWidgetProperty(SwitchDisableCheckWidget[0], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchEnableCheckWidget[0], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchRemapCheckWidget[0], xpProperty_ButtonState, 1);
        mag_off_switch_on = getOptionToString("mag_off_switch_on_cmd");
        mag_off_switch_off = getOptionToString("mag_off_switch_off_cmd");
        MagOffSwitchOnCmd   = XPLMFindCommand(mag_off_switch_on.c_str());
        MagOffSwitchOffCmd   = XPLMFindCommand(mag_off_switch_off.c_str());
     }

    // mag right switch disable - enable - remap
    magrightswitchenable = getOptionToInt("Mag Right Switch enable");
    if (magrightswitchenable == 0) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[1], xpProperty_ButtonState, 1);
            XPSetWidgetProperty(SwitchEnableCheckWidget[1], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchRemapCheckWidget[1], xpProperty_ButtonState, 0);
    }

    if (magrightswitchenable == 1) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[1], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchEnableCheckWidget[1], xpProperty_ButtonState, 1);
            XPSetWidgetProperty(SwitchRemapCheckWidget[1], xpProperty_ButtonState, 0);
    }

    if (magrightswitchenable == 2) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[1], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchEnableCheckWidget[1], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchRemapCheckWidget[1], xpProperty_ButtonState, 1);
            mag_right_switch_on = getOptionToString("mag_right_switch_on_cmd");
            mag_right_switch_off = getOptionToString("mag_right_switch_off_cmd");
            MagRightSwitchOnCmd   = XPLMFindCommand(mag_right_switch_on.c_str());
            MagRightSwitchOffCmd   = XPLMFindCommand(mag_right_switch_off.c_str());
    }

    // mag left switch disable - enable - remap
    magleftswitchenable = getOptionToInt("Mag Left Switch enable");
    if (magleftswitchenable == 0) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[2], xpProperty_ButtonState, 1);
            XPSetWidgetProperty(SwitchEnableCheckWidget[2], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchRemapCheckWidget[2], xpProperty_ButtonState, 0);
    }

    if (magleftswitchenable == 1) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[2], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchEnableCheckWidget[2], xpProperty_ButtonState, 1);
            XPSetWidgetProperty(SwitchRemapCheckWidget[2], xpProperty_ButtonState, 0);
    }

    if (magleftswitchenable == 2) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[2], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchEnableCheckWidget[2], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchRemapCheckWidget[2], xpProperty_ButtonState, 1);
            mag_left_switch_on = getOptionToString("mag_left_switch_on_cmd");
            mag_left_switch_off = getOptionToString("mag_left_switch_off_cmd");
            MagLeftSwitchOnCmd   = XPLMFindCommand(mag_left_switch_on.c_str());
            MagLeftSwitchOffCmd   = XPLMFindCommand(mag_left_switch_off.c_str());
    }

    // mag both switch disable - enable - remap
    magbothswitchenable = getOptionToInt("Mag Both Switch enable");
    if (magbothswitchenable == 0) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[3], xpProperty_ButtonState, 1);
            XPSetWidgetProperty(SwitchEnableCheckWidget[3], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchRemapCheckWidget[3], xpProperty_ButtonState, 0);
    }

    if (magbothswitchenable == 1) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[3], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchEnableCheckWidget[3], xpProperty_ButtonState, 1);
            XPSetWidgetProperty(SwitchRemapCheckWidget[3], xpProperty_ButtonState, 0);
    }

    if (magbothswitchenable == 2) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[3], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchEnableCheckWidget[3], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchRemapCheckWidget[3], xpProperty_ButtonState, 1);
            mag_both_switch_on = getOptionToString("mag_both_switch_on_cmd");
            mag_both_switch_off = getOptionToString("mag_both_switch_off_cmd");
            MagBothSwitchOnCmd   = XPLMFindCommand(mag_both_switch_on.c_str());
            MagBothSwitchOffCmd   = XPLMFindCommand(mag_both_switch_off.c_str());
    }

    // mag start switch disable - enable - remap
    magstartswitchenable = getOptionToInt("Mag Start Switch enable");
    if (magstartswitchenable == 0) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[4], xpProperty_ButtonState, 1);
            XPSetWidgetProperty(SwitchEnableCheckWidget[4], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchRemapCheckWidget[4], xpProperty_ButtonState, 0);
    }

    if (magstartswitchenable == 1) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[4], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchEnableCheckWidget[4], xpProperty_ButtonState, 1);
            XPSetWidgetProperty(SwitchRemapCheckWidget[4], xpProperty_ButtonState, 0);
    }

    if (magstartswitchenable == 2) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[4], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchEnableCheckWidget[4], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchRemapCheckWidget[4], xpProperty_ButtonState, 1);
            mag_start_switch_on = getOptionToString("mag_start_switch_on_cmd");
            mag_start_switch_off = getOptionToString("mag_start_switch_off_cmd");
            MagStartSwitchOnCmd   = XPLMFindCommand(mag_start_switch_on.c_str());
            MagStartSwitchOffCmd   = XPLMFindCommand(mag_start_switch_off.c_str());
    }

    // bat master switch disable - enable - remap
    batmasterswitchenable = getOptionToInt("Bat Master Switch enable");
    if (batmasterswitchenable == 0) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[5], xpProperty_ButtonState, 1);
            XPSetWidgetProperty(SwitchEnableCheckWidget[5], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchRemapCheckWidget[5], xpProperty_ButtonState, 0);
    }

    if (batmasterswitchenable == 1) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[5], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchEnableCheckWidget[5], xpProperty_ButtonState, 1);
            XPSetWidgetProperty(SwitchRemapCheckWidget[5], xpProperty_ButtonState, 0);
    }

    if (batmasterswitchenable == 2) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[5], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchEnableCheckWidget[5], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchRemapCheckWidget[5], xpProperty_ButtonState, 1);
            bat_master_switch_on = getOptionToString("bat_master_switch_on_cmd");
            bat_master_switch_off = getOptionToString("bat_master_switch_cmd");
            BatMasterSwitchOnCmd   = XPLMFindCommand(bat_master_switch_on.c_str());
            BatMasterSwitchOffCmd   = XPLMFindCommand(bat_master_switch_off.c_str());
    }


    // alt master switch disable - enable - remap
    altmasterswitchenable = getOptionToInt("Alt Master Switch enable");
    if (altmasterswitchenable == 0) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[6], xpProperty_ButtonState, 1);
            XPSetWidgetProperty(SwitchEnableCheckWidget[6], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchRemapCheckWidget[6], xpProperty_ButtonState, 0);
    }

    if (altmasterswitchenable == 1) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[6], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchEnableCheckWidget[6], xpProperty_ButtonState, 1);
            XPSetWidgetProperty(SwitchRemapCheckWidget[6], xpProperty_ButtonState, 0);
    }

    if (altmasterswitchenable == 2) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[6], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchEnableCheckWidget[6], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchRemapCheckWidget[6], xpProperty_ButtonState, 1);
            alt_master_switch_on = getOptionToString("alt_master_switch_on_cmd");
            alt_master_switch_off = getOptionToString("alt_master_switch_off_cmd");
            AltMasterSwitchOnCmd   = XPLMFindCommand(alt_master_switch_on.c_str());
            AltMasterSwitchOffCmd   = XPLMFindCommand(alt_master_switch_off.c_str());
    }

    // avionics master switch disable - enable - remap
    avionicsmasterswitchenable = getOptionToInt("Avionics master Switch enable");
    if (avionicsmasterswitchenable == 0) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[7], xpProperty_ButtonState, 1);
            XPSetWidgetProperty(SwitchEnableCheckWidget[7], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchRemapCheckWidget[7], xpProperty_ButtonState, 0);
    }

    if (avionicsmasterswitchenable == 1) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[7], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchEnableCheckWidget[7], xpProperty_ButtonState, 1);
            XPSetWidgetProperty(SwitchRemapCheckWidget[7], xpProperty_ButtonState, 0);
    }

    if (avionicsmasterswitchenable == 2) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[7], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchEnableCheckWidget[7], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchRemapCheckWidget[7], xpProperty_ButtonState, 1);
            av_master_switch_on = getOptionToString("av_master_switch_on_cmd");
            av_master_switch_off = getOptionToString("av_master_switch_off_cmd");
            AvMasterSwitchOnCmd   = XPLMFindCommand(av_master_switch_on.c_str());
            AvMasterSwitchOffCmd   = XPLMFindCommand(av_master_switch_off.c_str());
    }

    // fuel pump switch disable - enable - remap
    fuelpumpswitchenable = getOptionToInt("Fuel Pump Switch enable");
    if (fuelpumpswitchenable == 0) {
        XPSetWidgetProperty(SwitchDisableCheckWidget[8], xpProperty_ButtonState, 1);
        XPSetWidgetProperty(SwitchEnableCheckWidget[8], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchRemapCheckWidget[8], xpProperty_ButtonState, 0);
    }

    if (fuelpumpswitchenable == 1) {
        XPSetWidgetProperty(SwitchDisableCheckWidget[8], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchEnableCheckWidget[8], xpProperty_ButtonState, 1);
        XPSetWidgetProperty(SwitchRemapCheckWidget[8], xpProperty_ButtonState, 0);
    }

    if (fuelpumpswitchenable == 2) {
        XPSetWidgetProperty(SwitchDisableCheckWidget[8], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchEnableCheckWidget[8], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchRemapCheckWidget[8], xpProperty_ButtonState, 1);
        fuel_pump_switch_on = getOptionToString("fuel_pump_switch_on_cmd");
        fuel_pump_switch_off = getOptionToString("fuel_pump_switch_off_cmd");
        FuelPumpOnCmd   = XPLMFindCommand(fuel_pump_switch_on.c_str());
        FuelPumpOffCmd   = XPLMFindCommand(fuel_pump_switch_off.c_str());
    }



    // deice switch disable - enable - remap
    deiceswitchenable = getOptionToInt("Deice Switch enable");
    if (deiceswitchenable == 0) {
        XPSetWidgetProperty(SwitchDisableCheckWidget[9], xpProperty_ButtonState, 1);
        XPSetWidgetProperty(SwitchEnableCheckWidget[9], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchRemapCheckWidget[9], xpProperty_ButtonState, 0);
    }

    if (deiceswitchenable == 1) {
        XPSetWidgetProperty(SwitchDisableCheckWidget[9], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchEnableCheckWidget[9], xpProperty_ButtonState, 1);
        XPSetWidgetProperty(SwitchRemapCheckWidget[9], xpProperty_ButtonState, 0);
    }

    if (deiceswitchenable == 2) {
        XPSetWidgetProperty(SwitchDisableCheckWidget[9], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchEnableCheckWidget[9], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchRemapCheckWidget[9], xpProperty_ButtonState, 1);
        deice_switch_on = getOptionToString("deice_switch_on_cmd");
        deice_switch_off = getOptionToString("deice_switch_off_cmd");
        DeiceOnCmd   = XPLMFindCommand(deice_switch_on.c_str());
        DeiceOffCmd   = XPLMFindCommand(deice_switch_off.c_str());
    }


    // pitot heat switch disable - enable - remap
    pitotheatswitchenable = getOptionToInt("Pitot Heat Switch enable");
    if (pitotheatswitchenable == 0) {
        XPSetWidgetProperty(SwitchDisableCheckWidget[10], xpProperty_ButtonState, 1);
        XPSetWidgetProperty(SwitchEnableCheckWidget[10], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchRemapCheckWidget[10], xpProperty_ButtonState, 0);
    }

    if (pitotheatswitchenable == 1) {
        XPSetWidgetProperty(SwitchDisableCheckWidget[10], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchEnableCheckWidget[10], xpProperty_ButtonState, 1);
        XPSetWidgetProperty(SwitchRemapCheckWidget[10], xpProperty_ButtonState, 0);
    }

    if (pitotheatswitchenable == 2) {
        XPSetWidgetProperty(SwitchDisableCheckWidget[10], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchEnableCheckWidget[10], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchRemapCheckWidget[10], xpProperty_ButtonState, 1);
        pitot_heat_switch_on = getOptionToString("pitot_heat_switch_on_cmd");
        pitot_heat_switch_off = getOptionToString("pitot_heat_switch_off_cmd");
        PitotHeatOnCmd   = XPLMFindCommand(pitot_heat_switch_on.c_str());
        PitotHeatOffCmd   = XPLMFindCommand(pitot_heat_switch_off.c_str());
    }


    // landing gear up knob disable - enable - remap
    landinggearknobupenable = getOptionToInt("Landing Gear Knob Up enable");
    if (landinggearknobupenable == 0) {
         XPSetWidgetProperty(SwitchDisableCheckWidget[11], xpProperty_ButtonState, 1);
         XPSetWidgetProperty(SwitchEnableCheckWidget[11], xpProperty_ButtonState, 0);
         XPSetWidgetProperty(SwitchRemapCheckWidget[11], xpProperty_ButtonState, 0);
     }

     if (landinggearknobupenable == 1) {
         XPSetWidgetProperty(SwitchDisableCheckWidget[11], xpProperty_ButtonState, 0);
         XPSetWidgetProperty(SwitchEnableCheckWidget[11], xpProperty_ButtonState, 1);
         XPSetWidgetProperty(SwitchRemapCheckWidget[11], xpProperty_ButtonState, 0);
     }

     if (landinggearknobupenable == 2) {
         XPSetWidgetProperty(SwitchDisableCheckWidget[11], xpProperty_ButtonState, 0);
         XPSetWidgetProperty(SwitchEnableCheckWidget[11], xpProperty_ButtonState, 0);
         XPSetWidgetProperty(SwitchRemapCheckWidget[11], xpProperty_ButtonState, 1);
         gear_switch_up = getOptionToString("gear_switch_up_cmd");
         gear_switch_down = getOptionToString("gear_switch_down_cmd");
         GearUpCmd   = XPLMFindCommand(gear_switch_up.c_str());
         GearDnCmd   = XPLMFindCommand(gear_switch_down.c_str());
     }

   // cowl flaps switch disable - enable - remap
    cowlflapsenable = getOptionToInt("Cowl Flaps enable");
    if (cowlflapsenable == 0) {
        XPSetWidgetProperty(SwitchDisableCheckWidget[13], xpProperty_ButtonState, 1);
        XPSetWidgetProperty(SwitchEnableCheckWidget[13], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchRemapCheckWidget[13], xpProperty_ButtonState, 0);
    }

    if (cowlflapsenable == 1) {
        XPSetWidgetProperty(SwitchDisableCheckWidget[13], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchEnableCheckWidget[13], xpProperty_ButtonState, 1);
        XPSetWidgetProperty(SwitchRemapCheckWidget[13], xpProperty_ButtonState, 0);
    }

    if (cowlflapsenable == 2) {
        XPSetWidgetProperty(SwitchDisableCheckWidget[13], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchEnableCheckWidget[13], xpProperty_ButtonState, 0);
        XPSetWidgetProperty(SwitchRemapCheckWidget[13], xpProperty_ButtonState, 1);
        cowl_flaps_open = getOptionToString("cowl_flaps_open_cmd");
        cowl_flaps_close = getOptionToString("cowl_flaps_close_cmd");
        CowlFlapsOpenCmd   = XPLMFindCommand(cowl_flaps_open.c_str());
        CowlFlapsCloseCmd   = XPLMFindCommand(cowl_flaps_close.c_str());
    }

    // panel lights switch disable - enable - remap
    panellightswitchenable = getOptionToInt("Panel Lights Switch enable");
    if (panellightswitchenable == 0) {
         XPSetWidgetProperty(SwitchDisableCheckWidget[14], xpProperty_ButtonState, 1);
         XPSetWidgetProperty(SwitchEnableCheckWidget[14], xpProperty_ButtonState, 0);
         XPSetWidgetProperty(SwitchRemapCheckWidget[14], xpProperty_ButtonState, 0);
     }

     if (panellightswitchenable == 1) {
         XPSetWidgetProperty(SwitchDisableCheckWidget[14], xpProperty_ButtonState, 0);
         XPSetWidgetProperty(SwitchEnableCheckWidget[14], xpProperty_ButtonState, 1);
         XPSetWidgetProperty(SwitchRemapCheckWidget[14], xpProperty_ButtonState, 0);
     }

     if (panellightswitchenable == 2) {
         XPSetWidgetProperty(SwitchDisableCheckWidget[14], xpProperty_ButtonState, 0);
         XPSetWidgetProperty(SwitchEnableCheckWidget[14], xpProperty_ButtonState, 0);
         XPSetWidgetProperty(SwitchRemapCheckWidget[14], xpProperty_ButtonState, 1);
         panel_lights_switch_on = getOptionToString("panel_lights_switch_on_cmd");
         panel_lights_switch_off = getOptionToString("panel_lights_switch_off_cmd");
         PanelLightsOnCmd   = XPLMFindCommand(panel_lights_switch_on.c_str());
         PanelLightsOffCmd   = XPLMFindCommand(panel_lights_switch_off.c_str());
     }

     // beacon lights switch disable - enable - remap
     beaconlightswitchenable = getOptionToInt("Beacon Lights Switch enable");
     if (beaconlightswitchenable == 0) {
          XPSetWidgetProperty(SwitchDisableCheckWidget[15], xpProperty_ButtonState, 1);
          XPSetWidgetProperty(SwitchEnableCheckWidget[15], xpProperty_ButtonState, 0);
          XPSetWidgetProperty(SwitchRemapCheckWidget[15], xpProperty_ButtonState, 0);
      }

      if (beaconlightswitchenable == 1) {
          XPSetWidgetProperty(SwitchDisableCheckWidget[15], xpProperty_ButtonState, 0);
          XPSetWidgetProperty(SwitchEnableCheckWidget[15], xpProperty_ButtonState, 1);
          XPSetWidgetProperty(SwitchRemapCheckWidget[15], xpProperty_ButtonState, 0);
      }

      if (beaconlightswitchenable == 2) {
          XPSetWidgetProperty(SwitchDisableCheckWidget[15], xpProperty_ButtonState, 0);
          XPSetWidgetProperty(SwitchEnableCheckWidget[15], xpProperty_ButtonState, 0);
          XPSetWidgetProperty(SwitchRemapCheckWidget[15], xpProperty_ButtonState, 1);
          beacon_lights_switch_on = getOptionToString("beacon_lights_switch_on_cmd");
          beacon_lights_switch_off = getOptionToString("beacon_lights_switch_off_cmd");
          BeaconLightsOnCmd   = XPLMFindCommand(beacon_lights_switch_on.c_str());
          BeaconLightsOffCmd   = XPLMFindCommand(beacon_lights_switch_off.c_str());
      }

      // nav lights switch disable - enable - remap
      navlightswitchenable = getOptionToInt("Nav Lights Switch enable");
      if (navlightswitchenable == 0) {
           XPSetWidgetProperty(SwitchDisableCheckWidget[16], xpProperty_ButtonState, 1);
           XPSetWidgetProperty(SwitchEnableCheckWidget[16], xpProperty_ButtonState, 0);
           XPSetWidgetProperty(SwitchRemapCheckWidget[16], xpProperty_ButtonState, 0);
       }

       if (navlightswitchenable == 1) {
           XPSetWidgetProperty(SwitchDisableCheckWidget[16], xpProperty_ButtonState, 0);
           XPSetWidgetProperty(SwitchEnableCheckWidget[16], xpProperty_ButtonState, 1);
           XPSetWidgetProperty(SwitchRemapCheckWidget[16], xpProperty_ButtonState, 0);
       }

       if (navlightswitchenable == 2) {
           XPSetWidgetProperty(SwitchDisableCheckWidget[16], xpProperty_ButtonState, 0);
           XPSetWidgetProperty(SwitchEnableCheckWidget[16], xpProperty_ButtonState, 0);
           XPSetWidgetProperty(SwitchRemapCheckWidget[16], xpProperty_ButtonState, 1);
           nav_lights_switch_on = getOptionToString("nav_lights_switch_on_cmd");
           nav_lights_switch_off = getOptionToString("nav_lights_switch_off_cmd");
           NavLightsOnCmd   = XPLMFindCommand(nav_lights_switch_on.c_str());
           NavLightsOffCmd   = XPLMFindCommand(nav_lights_switch_off.c_str());
       }

       // strobe lights switch disable - enable - remap
       strobelightswitchenable = getOptionToInt("Strobe Lights Switch enable");
       if (strobelightswitchenable == 0) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[17], xpProperty_ButtonState, 1);
            XPSetWidgetProperty(SwitchEnableCheckWidget[17], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchRemapCheckWidget[17], xpProperty_ButtonState, 0);
        }

        if (strobelightswitchenable == 1) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[17], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchEnableCheckWidget[17], xpProperty_ButtonState, 1);
            XPSetWidgetProperty(SwitchRemapCheckWidget[17], xpProperty_ButtonState, 0);
        }

        if (strobelightswitchenable == 2) {
            XPSetWidgetProperty(SwitchDisableCheckWidget[17], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchEnableCheckWidget[17], xpProperty_ButtonState, 0);
            XPSetWidgetProperty(SwitchRemapCheckWidget[17], xpProperty_ButtonState, 1);
            strobe_lights_switch_on = getOptionToString("strobe_lights_switch_on_cmd");
            strobe_lights_switch_off = getOptionToString("strobe_lights_switch_off_cmd");
            StrobeLightsOnCmd   = XPLMFindCommand(strobe_lights_switch_on.c_str());
            StrobeLightsOffCmd   = XPLMFindCommand(strobe_lights_switch_off.c_str());
        }

        // taxi lights switch disable - enable - remap
        taxilightswitchenable = getOptionToInt("Strobe Lights Switch enable");
        if (taxilightswitchenable == 0) {
             XPSetWidgetProperty(SwitchDisableCheckWidget[18], xpProperty_ButtonState, 1);
             XPSetWidgetProperty(SwitchEnableCheckWidget[18], xpProperty_ButtonState, 0);
             XPSetWidgetProperty(SwitchRemapCheckWidget[18], xpProperty_ButtonState, 0);
         }

         if (taxilightswitchenable == 1) {
             XPSetWidgetProperty(SwitchDisableCheckWidget[18], xpProperty_ButtonState, 0);
             XPSetWidgetProperty(SwitchEnableCheckWidget[18], xpProperty_ButtonState, 1);
             XPSetWidgetProperty(SwitchRemapCheckWidget[18], xpProperty_ButtonState, 0);
         }

         if (taxilightswitchenable == 2) {
             XPSetWidgetProperty(SwitchDisableCheckWidget[18], xpProperty_ButtonState, 0);
             XPSetWidgetProperty(SwitchEnableCheckWidget[18], xpProperty_ButtonState, 0);
             XPSetWidgetProperty(SwitchRemapCheckWidget[18], xpProperty_ButtonState, 1);
             taxi_lights_switch_on = getOptionToString("taxi_lights_switch_on_cmd");
             taxi_lights_switch_off = getOptionToString("taxi_lights_switch_off_cmd");
             TaxiLightsOnCmd   = XPLMFindCommand(taxi_lights_switch_on.c_str());
             TaxiLightsOffCmd   = XPLMFindCommand(taxi_lights_switch_off.c_str());
         }

         // landing lights switch disable - enable - remap
         landinglightswitchenable = getOptionToInt("Strobe Lights Switch enable");
         if (landinglightswitchenable == 0) {
              XPSetWidgetProperty(SwitchDisableCheckWidget[19], xpProperty_ButtonState, 1);
              XPSetWidgetProperty(SwitchEnableCheckWidget[19], xpProperty_ButtonState, 0);
              XPSetWidgetProperty(SwitchRemapCheckWidget[19], xpProperty_ButtonState, 0);
          }

          if (landinglightswitchenable == 1) {
              XPSetWidgetProperty(SwitchDisableCheckWidget[19], xpProperty_ButtonState, 0);
              XPSetWidgetProperty(SwitchEnableCheckWidget[19], xpProperty_ButtonState, 1);
              XPSetWidgetProperty(SwitchRemapCheckWidget[19], xpProperty_ButtonState, 0);
          }

          if (landinglightswitchenable == 2) {
              XPSetWidgetProperty(SwitchDisableCheckWidget[19], xpProperty_ButtonState, 0);
              XPSetWidgetProperty(SwitchEnableCheckWidget[19], xpProperty_ButtonState, 0);
              XPSetWidgetProperty(SwitchRemapCheckWidget[19], xpProperty_ButtonState, 1);
              landing_lights_switch_on = getOptionToString("landing_lights_switch_on_cmd");
              landing_lights_switch_off = getOptionToString("landing_lights_switch_off_cmd");
              LandingLightsOnCmd   = XPLMFindCommand(landing_lights_switch_on.c_str());
              LandingLightsOffCmd   = XPLMFindCommand(landing_lights_switch_off.c_str());
          }

    bataltinverse = getOptionToInt("Bat Alt inverse");




    radspeed = getOptionToInt("Radio Freq Knob Pulse per Command");
    numadf = getOptionToInt("Radio Number of ADF's");

    trimspeed = getOptionToInt("Multi Trim Speed");
    multispeed = getOptionToInt("Multi Freq Knob Pulse per Command");
    autothrottleswitchenable = getOptionToInt("Auto Throttle Switch enable");
    metricpressenable= getOptionToInt("Metric Press enable");



  return;
}
