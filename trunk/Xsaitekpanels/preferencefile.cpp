// ****** preference.cpp **********
// ****  William R. Good  ********

#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"

#include "saitekpanels.h"

#include <iostream>
#include <fstream>
#include <string>


// ***** Preference File Process ******
void process_pref_file()

{
    string prefline;
    string prefitems[16];
    ifstream myfile;
    myfile.open("Resources/plugins/Xsaitekpanels/xsaitekpanels.prf");
    int line = 1;
    if(!myfile) //Always test the file open.
    {
                cout<<"Error opening output file"<<endl;
                bataltinverse = 0;
                radspeed = 3, numadf = 1;
                trimspeed = 1, multispeed = 3;
                return;
    }
    while(!myfile.eof())
    {
        getline(myfile, prefline);
        prefitems[line] = prefline;
        line++;

    }
    myfile.close();

// Switch Panel preferences

    if (prefitems[11] == "0") {
        bataltinverse = 0;
    }
    if (prefitems[11] == "1") {
        bataltinverse = 1;
    }


// Radio Panel Preferences

    if (prefitems[12] == "1") {
        radspeed = 1;
    }
    if (prefitems[12] == "2") {
        radspeed = 2;
    }
    if (prefitems[12] == "3") {
        radspeed = 3;
    }
    if (prefitems[12] == "4") {
        radspeed = 4;
    }
    if (prefitems[12] == "5") {
        radspeed = 5;
    }


    if (prefitems[13] == "1") {
        numadf = 1;
    }
    if (prefitems[13] == "2") {
        numadf = 2;
    }


    // Multi Panel Preferences

    if (prefitems[14] == "1") {
        trimspeed = 1;
    }
    if (prefitems[14] == "2") {
        trimspeed = 2;
    }
    if (prefitems[14] == "3") {
        trimspeed = 3;
    }

    if (prefitems[15] == "1") {
        multispeed = 1;
    }
    if (prefitems[15] == "2") {
        multispeed = 2;
    }
    if (prefitems[15] == "3") {
        multispeed = 3;
    }
    if (prefitems[15] == "4") {
        multispeed = 4;
    }
    if (prefitems[15] == "5") {
        multispeed = 5;
    }


  return;
}
