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
    int num_configs = 5;
    XPPCItem *configs = XPPCInit( num_configs );

    /* set defaults */
    int one     = 0,
        two     = 3,
        three   = 1,
        four    = 1,
        five    = 3;


    configs[0].type = XPPC_TYPE_INT;
    configs[0].key  = "Bat Alt inverse";
    configs[0].ref  = &one;

    configs[1].type = XPPC_TYPE_INT;
    configs[1].key  = "Radio Freq Knob Pulse per Command";
    configs[1].ref  = &two;

    configs[2].type = XPPC_TYPE_INT;
    configs[2].key  = "Radio Number of ADF's";
    configs[2].ref  = &three;

    configs[3].type = XPPC_TYPE_INT;
    configs[3].key  = "Multi Trim Speed";
    configs[3].ref  = &four;

    configs[4].type = XPPC_TYPE_INT;
    configs[4].key  = "Multi Freq Knob Pulse per Command";
    configs[4].ref  = &five;

    if( !XPPCParseConfigFile( "Resources/plugins/Xsaitekpanels/xsaitekpanels.conf", configs, num_configs ) ) {
        printf( "%s\n", XPPCLastError() );
        return;
    }

    bataltinverse = one;
    radspeed = two;
    numadf = three;
    trimspeed = four;
    multispeed = five;


  return;
}
