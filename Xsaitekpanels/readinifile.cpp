// ****** readinifile.cpp **********
// ****  William R. Good  ********

#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"

#include "saitekpanels.h"
#include "inireader.h"

#include <iostream>
#include <fstream>
#include <string>


// ***** Configuration File Process ******
void process_read_ini_file()

{

    char *configPath;
    char *defaultConfigFile;

    cleanupIniReader();

    defaultConfigFile = "./Resources/plugins/Xsaitekpanels/xsaitekpanels.ini";

    parseIniFile(defaultConfigFile);

    /* set defaults */

    bataltinverse            = 0,
    deiceswitchenable        = 1,
    cowlflapsenable          = 1,
    panellightsenable        = 1,
    landinggearknobenable    = 1,
    radspeed                 = 3,
    numadf                   = 1,
    trimspeed                = 1,
    multispeed               = 3;
    autothrottleswitchenable = 1;

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

  return;
}
