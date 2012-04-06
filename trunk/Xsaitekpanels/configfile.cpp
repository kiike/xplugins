// ****** configfile.cpp **********
// ****  William R. Good  ********

#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"

#include "saitekpanels.h"

#include "XPPConfig.h"

#include <iostream>
#include <fstream>
#include <string>


// ***** Configuration File Process ******
void process_config_file()

{
    int num_configs = 8;
    XPPCItem *configs = XPPCInit( num_configs );

    /* set defaults */

    bataltinverse            = 0,
    cowlflapsenable          = 1,
    landinggearknobenable    = 1,
    radspeed                 = 3,
    numadf                   = 1,
    trimspeed                = 1,
    multispeed               = 3;
    autothrottleswitchenable = 1;

    configs[0].type = XPPC_TYPE_INT;
    configs[0].key  = "Bat Alt inverse";
    configs[0].ref  = &bataltinverse;

    configs[1].type = XPPC_TYPE_INT;
    configs[1].key  = "Cowl Flaps enable";
    configs[1].ref  = &cowlflapsenable;

    configs[2].type = XPPC_TYPE_INT;
    configs[2].key  = "Landing Gear Knob enable";
    configs[2].ref  = &landinggearknobenable;

    configs[3].type = XPPC_TYPE_INT;
    configs[3].key  = "Radio Freq Knob Pulse per Command";
    configs[3].ref  = &radspeed;

    configs[4].type = XPPC_TYPE_INT;
    configs[4].key  = "Radio Number of ADF's";
    configs[4].ref  = &numadf;

    configs[5].type = XPPC_TYPE_INT;
    configs[5].key  = "Multi Trim Speed";
    configs[5].ref  = &trimspeed;

    configs[6].type = XPPC_TYPE_INT;
    configs[6].key  = "Multi Freq Knob Pulse per Command";
    configs[6].ref  = &multispeed;

    configs[7].type = XPPC_TYPE_INT;
    configs[7].key  = "Auto Throttle Switch enable";
    configs[7].ref  = &autothrottleswitchenable;

    if( !XPPCParseConfigFile( "Resources/plugins/Xsaitekpanels/xsaitekpanels.conf", configs, num_configs ) ) {
        printf( "%s\n", XPPCLastError() );
        return;
    }

/*
    bataltinverse = one;
    radspeed = two;
    numadf = three;
    trimspeed = four;
    multispeed = five;

*/

  return;
}
