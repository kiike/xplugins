// *********************************************************
//
//     Xchecklist Plugin
//
//     Michal
//     William Good
//
//
//     A plugin to display a clist.txt in widget window
//
// *********************************************************

#include "XPLMPlugin.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMProcessing.h"
#include "XPLMMenus.h"
#include "XPLMUtilities.h"
#include "XPLMPlanes.h"
#include "XPWidgets.h"
#include "XPStandardWidgets.h"
#include "XPLMDataAccess.h"

#include "parser.h"
#include "interface.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#if IBM
#include <windows.h>
#endif
#include <vector>

checklist_binder *binder = NULL;

int gMenuItem;

int x = 10;
int y = 550;
int w = 300;
int h = 400;
int x2, y2;

int outLeft, outTop, outRight, outBottom;

int max_items = -1;
int checkable = -1;
int loopnum = 0;
int item;

int Item;
bool state[7];

char FileName[256], AircraftPath[256];
char prefsPath[256];

char state1[10];
char state2[10];
char state3[10];
char state4[10];
char state5[10];
char state6[10];
char state7[10];

int numb;

int MyNextChecklistCommandCallback(
                                   XPLMCommandRef       inCommand,
                                   XPLMCommandPhase     inPhase,
                                   void *               inRefcon);


int MyCheckItemCommandCallback(
                                   XPLMCommandRef       inCommand,
                                   XPLMCommandPhase     inPhase,
                                   void *               inRefcon);



FILE *my_stream;

XPWidgetID	linuxCheckListWidget = NULL;

XPWidgetID	setupWidget = NULL;

XPWidgetID linuxCheckListCopilotInfoWidget;

XPWidgetID      linuxChecklistPreviousButton = NULL;
XPWidgetID      linuxChecklistNextButton = NULL;
XPWidgetID	linuxCheckListCopilotWidget[50] = {NULL};
XPWidgetID	linuxCheckListCheckWidget[50] = {NULL};
XPWidgetID	linuxCheckListTextWidget[50] = {NULL};
XPWidgetID	linuxCheckListTextAWidget[50] = {NULL};

XPWidgetID	setupCheckWidget[10] = {NULL};
XPWidgetID	setupTextWidget[10] = {NULL};

XPWidgetID      setupCheckItemButton = NULL;
XPWidgetID      setupNextChecklistButton = NULL;
XPWidgetID      setupSaveSettingsButton = NULL;

XPLMMenuID      PluginMenu     = NULL;
XPLMMenuID      checklistsMenu = NULL;

XPLMCommandRef nextchecklist = NULL;

XPLMCommandRef cmdcheckitem;
XPLMCommandRef cmdnextchecklist;


int checklists_count = -1;

void linuxCheckListMenuHandler(void *, void *);

static void CreateSetupWidget(int xx1, int yy1, int ww, int hh);
static int linuxCheckListHandler(XPWidgetMessage  inMessage, XPWidgetID  inWidget, long  inParam1, long  inParam2);
static int linuxSetupHandler(XPWidgetMessage  inMessage, XPWidgetID  inWidget, long  inParam1, long  inParam2);
static float dataProcessingCallback(float inElapsed1, float inElapsed2, int cntr, void *ref);
static bool init_checklists();
static bool init_setup();
static bool do_cleanup();


const char setupText[10][200] = {"Translucent Window", "Show Checklist if Checklist exist", \
                                 "Turn Copilot On", "Voice Prompt", "Auto Hide"};

char *test;

PLUGIN_API int XPluginStart(
						char *		outName,
						char *		outSig,
						char *		outDesc)
{
        int		PluginSubMenuItem;
	int             ChecklistsSubMenuItem;
        strcpy(outName, "Xchecklist");
        strcpy(outSig, "Michal_Bill.Example.Xchecklist");
        strcpy(outDesc, "A plugin to display checklists in a widget window.");


// Create our menu
        PluginSubMenuItem = XPLMAppendMenuItem(
                    XPLMFindPluginsMenu(),
                    "Xchecklist",
                    NULL,
                    1);

        PluginMenu = XPLMCreateMenu(
                    "Xchecklist",
                    XPLMFindPluginsMenu(),
                    PluginSubMenuItem,
                    linuxCheckListMenuHandler,
                    (void *)0);

        XPLMAppendMenuItem(PluginMenu, "Open CheckList", (void *) "checklist", 1);
        XPLMAppendMenuItem(PluginMenu, "Open Setup", (void *) "setup", 1);
	
        ChecklistsSubMenuItem = XPLMAppendMenuItem(
                    PluginMenu,
                    "CheckLists",
                    NULL,
                    1);

        checklistsMenu = XPLMCreateMenu(
                    "CheckLists",
                    PluginMenu,
                    ChecklistsSubMenuItem,
                    linuxCheckListMenuHandler,
                    (void *)1);

        XPLMAppendMenuItem(checklistsMenu, "CheckList1", (void *) 0, 1);
        XPLMAppendMenuItem(checklistsMenu, "CheckList2", (void *) 1, 1);
        
        XPLMRegisterFlightLoopCallback(dataProcessingCallback, 0.1f, NULL);

        init_setup();
	do_cleanup();
	init_checklists();

        cmdcheckitem = XPLMCreateCommand("sim/operation/check_item","Check Item");
        cmdnextchecklist = XPLMCreateCommand("sim/operation/next_checklist","Next Checklist");

        XPLMRegisterCommandHandler(
                    cmdcheckitem,
                    MyCheckItemCommandCallback,
                    true,
                    NULL);

        XPLMRegisterCommandHandler(
                    cmdnextchecklist,
                    MyNextChecklistCommandCallback,
                    true,
                    NULL);


        return 1;
}

PLUGIN_API void	XPluginStop(void)
{
	if (gMenuItem == 1)
	{
                XPDestroyWidget(linuxCheckListWidget, 1);
		gMenuItem = 0;
	}

        if (gMenuItem == 2)
        {
                XPDestroyWidget(setupWidget, 1);
                gMenuItem = 0;
        }
        XPLMUnregisterFlightLoopCallback(dataProcessingCallback, NULL);
	XPLMDestroyMenu(checklistsMenu);
	XPLMDestroyMenu(PluginMenu);
	stop_checklists();
}

PLUGIN_API int XPluginEnable(void)
{
	return 1;
}

PLUGIN_API void XPluginDisable(void)
{
}

bool do_cleanup()
{
  XPLMClearAllMenuItems(checklistsMenu);
  checklists_count = -1;
  if(linuxCheckListWidget != NULL){
    XPHideWidget(linuxCheckListWidget);
    XPDestroyWidget(linuxCheckListWidget, 1);
    linuxCheckListWidget = NULL;
  }
  return true;
}

bool create_checklists_menu(void)
{
  int size;
  constname_t *names;
  XPLMClearAllMenuItems(checklistsMenu);
  if(get_checklist_names(&size, &names)){
    checklists_count = size;
    for(int i = 0; i < size; ++i){
      XPLMAppendMenuItem(checklistsMenu, names[i], (void *) i, 1);
    }
    free_checklist_names(size, &names);
    return true;
  }
  return false;
}

bool init_checklists()
{
        XPLMGetNthAircraftModel(0, FileName, AircraftPath);
	//To make sure I don't corrupt XPlane stuff
	if(strlen(AircraftPath) == 0){
	  return false; 
	}
	char *myACFPath = strdup(AircraftPath);
	char *last_slash = strrchr(myACFPath, '/');
	*(++last_slash) = '\0';
        //Add clist.txt to aircraft path
        size_t size = strlen(myACFPath) + strlen("clist.txt") + 1; //strlen doesn't count terminating null byte!
        char *cat = (char *)malloc(size); //allocate memory
        snprintf(cat, size, "%s%s", myACFPath, "clist.txt");
        //printf("\nPrefs Path  %s \n\n", cat);

        printf("\nFilename  %s  \n\nAircraft Path  %s \n\n", FileName, cat);
        checklists_count = -1; // to make it rebuild menus...
        //Get corect checklist for current aircraft
        bool res = start_checklists((char *)cat);
        free(cat);
        free(myACFPath);
	return res;
}

bool get_truth_val(const char* str)
{
  return strcasecmp(str, "true") == 0;
}

bool init_setup()
{
    XPLMGetPrefsPath(prefsPath);
    //To make sure I don't corrupt XPlane stuff
    char *myPrefsPath = strdup(prefsPath);
    char * last_slash = strrchr(myPrefsPath, '/');
    *(++last_slash) = '\0';

    //Add linuxchecklist.prf to preferences path
    size_t size = strlen(myPrefsPath) + strlen("Xchecklist.prf") + 1; //strlen doesn't count terminating null byte!
    char *cat = (char *)malloc(size); //allocate memory
    snprintf(cat, size, "%s%s", myPrefsPath, "Xchecklist.prf");
    printf("\nPrefs Path to initilize setup  %s \n\n", cat);
    my_stream = fopen (cat, "r+");
    if (my_stream!=NULL)
      {
        //ToDo read the preference file and set check boxes to match
        printf ("preference file is open for updating read and write \n\n");
        rewind (my_stream);
        numb = fscanf (my_stream, "%s %s %s %s %s %s %s",
                state1, state2, state3, state4, state5,
                state6, state7);
        
        state[0] = get_truth_val(state1);
        state[1] = get_truth_val(state2);
        state[2] = get_truth_val(state3);
        state[3] = get_truth_val(state4);
        state[4] = get_truth_val(state5);
        state[5] = get_truth_val(state6);
        state[6] = get_truth_val(state7);

        fclose (my_stream);
      }

    free(cat); //to avoid the memory leak
    cat = NULL; //if you try to use it now, you know it immediately

    return 1;

}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, long inMsg, void * inParam)
{
  (void) inFrom; // To get rid of warnings on unused variables
  if((inMsg == XPLM_MSG_PLANE_LOADED) && (inParam == 0)){
    //user plane loaded / reloaded
    do_cleanup();
    init_checklists();
  }
}

float dataProcessingCallback(float inElapsed1, float inElapsed2, int cntr, void *ref)
{
  (void) inElapsed1;
  (void) inElapsed2;
  (void) cntr;
  (void) ref;

  if(state[2])
    do_processing(XPIsWidgetVisible(linuxCheckListWidget));
  
  return 0.1f;
}

void linuxCheckListMenuHandler(void * inMenuRef, void * inItemRef)
{
  (void) inMenuRef;
unsigned int pageSize = 0;
const char *pageTitle = "Default Title";
checklist_item_desc_t pageItems[50];

  if((int)inMenuRef == 0){
    if (!strcmp((char *) inItemRef, "checklist")){
      if (linuxCheckListWidget == NULL){
        create_checklist(pageSize, pageTitle, pageItems, 120, 0, 0);
      }else{
        if(!XPIsWidgetVisible(linuxCheckListWidget))
          XPShowWidget(linuxCheckListWidget);
      }
    }
    if (!strcmp((char *) inItemRef, "setup")){
      if (setupWidget == NULL){
        CreateSetupWidget(400, 550, 215, 225);	//left, top, right, bottom.
      }else{
        if(!XPIsWidgetVisible(setupWidget))
          XPShowWidget(setupWidget);
      }
    }
  }else if((int)inMenuRef == 1){
    open_checklist((int)inItemRef);
  }
}

// This will create our setup widget dialog.
void CreateSetupWidget(int xx, int yy, int ww, int hh)
{

        int xx2 = xx + ww;
        int yy2 = yy - hh;

        int yOffset;
        int lines = 5;


// Create the Main Widget window.
        setupWidget = XPCreateWidget(xx, yy, xx2, yy2,
                                        1,		  // Visible
                                        "Checklist Setup",  // desc
                                        1,			  // root
                                        NULL,			  // no container
                                        xpWidgetClass_MainWindow);


// Add Close Box to the Main Widget.  Other options are available.  See the SDK Documentation.
        XPSetWidgetProperty(setupWidget, xpProperty_MainWindowHasCloseBoxes, 1);

// Display each line of setup.

        for(int l = 0; l < lines; ++l){

        yOffset = (5+18+(l*25));

        setupCheckWidget[l] = XPCreateWidget(xx-5, yy-yOffset, xx+15+20, yy-yOffset-20,
                       1,	// Visible
                       "",      // desc
                       0,	// root
        setupWidget,
        xpWidgetClass_Button);

        XPSetWidgetProperty(setupCheckWidget[l], xpProperty_ButtonType, xpRadioButton);
        XPSetWidgetProperty(setupCheckWidget[l], xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
        XPSetWidgetProperty(setupCheckWidget[l], xpProperty_ButtonState, state[l]);

        // Create a checklist item description text widget
        setupTextWidget[l] = XPCreateWidget(xx+30, yy-yOffset, xx+60+200, yy-yOffset-20,
                       1,	        // Visible
                       setupText[l],    // desc
                       0,		// root
                       setupWidget,
                       xpWidgetClass_Caption);
        }

        yOffset = (5+18+(7*25));

        setupSaveSettingsButton = XPCreateWidget(xx+10, yy-yOffset, xx+5+200, yy-yOffset-20,
                                        1, "Save Settings", 0, setupWidget,
                                        xpWidgetClass_Button);

        XPSetWidgetProperty(setupSaveSettingsButton, xpProperty_ButtonType, xpPushButton);


        // Register our widget handler
        XPAddWidgetCallback(setupWidget, linuxSetupHandler);


}


// This is our widget handler.  In this example we are only interested when the close box is pressed.
int	linuxSetupHandler(XPWidgetMessage  inMessage, XPWidgetID  inWidget, long  inParam1, long  inParam2)
{
  (void) inParam1;
  (void) inParam2;


	if (inMessage == xpMessage_CloseButtonPushed)
	{
                if (inWidget == setupWidget)
                {
                      XPHideWidget(setupWidget);
                      return 1;

                }

        }

        if (inMessage == xpMsg_ButtonStateChanged)
        {
                printf("Got button state chenge message!\n");
                for (Item=0; Item<4; Item++)
                {
		  long tmp;
                        // If the setupCheckWidget check box is checked then set state[Item] true
                        if ((tmp = XPGetWidgetProperty(setupCheckWidget[Item], xpProperty_ButtonState, 0))){
                          state[Item] = true;
                        }
                        else {
                          state[Item] = false;
                        }

                }

                if(state[0]){

                  XPSetWidgetProperty(linuxCheckListWidget, xpProperty_MainWindowType, xpMainWindowStyle_Translucent);
                  
		  for(int iii = 0; iii < 16; ++iii){
                    XPSetWidgetProperty(linuxCheckListCopilotWidget[iii], xpProperty_CaptionLit, 1);
                    XPSetWidgetProperty(linuxCheckListTextWidget[iii], xpProperty_CaptionLit, 1);
                    XPSetWidgetProperty(linuxCheckListTextAWidget[iii], xpProperty_CaptionLit, 1);
		  }

                  XPSetWidgetProperty(linuxCheckListCopilotInfoWidget, xpProperty_CaptionLit, 1);


	        }else{
                  XPSetWidgetProperty(linuxCheckListWidget, xpProperty_MainWindowType, xpMainWindowStyle_MainWindow);

		  for(int iii = 0; iii < 16; ++iii){
                    XPSetWidgetProperty(linuxCheckListCopilotWidget[iii], xpProperty_CaptionLit, 0);
                    XPSetWidgetProperty(linuxCheckListTextWidget[iii], xpProperty_CaptionLit, 0);
                    XPSetWidgetProperty(linuxCheckListTextAWidget[iii], xpProperty_CaptionLit, 0);
		  }

                  XPSetWidgetProperty(linuxCheckListCopilotInfoWidget, xpProperty_CaptionLit, 0);

		}
        }

        if (inMessage == xpMsg_PushButtonPressed)
        {
                if (inParam1 == (long)setupSaveSettingsButton)
                {
                        // ToDo Need to add saving settings to a file
                        XPHideWidget(setupWidget);
                        printf("Save settings pressed \n");

                        //Prefs Path  /home/bill/X-Plane_9.61/Output/preferences/Set X-Plane.prf
                        XPLMGetPrefsPath(prefsPath);
                        //To make sure I don't corrupt XPlane stuff
                        char *myPrefsPath = strdup(prefsPath);
                        char * last_slash = strrchr(myPrefsPath, '/');
                        *(++last_slash) = '\0';
                        //Add linuxchecklist.prf to preferences path
                        size_t size = strlen(myPrefsPath) + strlen("Xchecklist.prf") + 1; //strlen doesn't count terminating null byte!
                        char *cat = (char *)malloc(size); //allocate memory
                        snprintf(cat, size, "%s%s", myPrefsPath, "Xchecklist.prf");
                        printf("\nPrefs Path  %s \n\n", cat);
                        my_stream = fopen (cat, "w");

                        fprintf (my_stream, "%s %s %s %s %s %s %s",
                                ((state[0])?"true":"false"),
                                ((state[1])?"true":"false"),
                                ((state[2])?"true":"false"),
                                ((state[3])?"true":"false"),
                                ((state[4])?"true":"false"),
                                ((state[5])?"true":"false"),
                                ((state[6])?"true":"false"));

                        fclose (my_stream);

                        free(cat); //to avoid the memory leak
                        cat = NULL; //if you try to use it now, you know it immediately
                        return 1;
                }


        }


	return 0;
}

// This is our widget handler.  In this example we are only interested when the close box is pressed.
int	linuxCheckListHandler(XPWidgetMessage  inMessage, XPWidgetID  inWidget, long  inParam1, long  inParam2)
{
  (void) inParam2;
        if (inMessage == xpMessage_CloseButtonPushed)
        {

              if (inWidget == linuxCheckListWidget)
              {
                        XPHideWidget(linuxCheckListWidget);
                        return 1;
               }
        }


        if (inMessage == xpMsg_PushButtonPressed)
        {
	  printf("Button pushed...\n");
                if (inParam1 == (long)linuxChecklistPreviousButton)
                {
                        prev_checklist();
                        return 1;
                }

                if (inParam1 == (long)linuxChecklistNextButton)
                {
                        next_checklist();
                        return 1;
                }
        }
        if(inMessage == xpMsg_ButtonStateChanged)
	{
                for(int i = 0; i < max_items; ++i){
		  if(inParam1 == (long)linuxCheckListCheckWidget[i]){
		    if(i == checkable){
		      item_checked(i);
		      return 1;
		    }else{
                      XPSetWidgetProperty(linuxCheckListCheckWidget[i], 
					  xpProperty_ButtonState, (i < checkable) ? 1 : 0);
		    }
		  }
		}
	}


        return 0;
}

bool create_checklist(unsigned int size, const char *title,
                      checklist_item_desc_t items[], int width,
                      int index, int force_show)
{
  (void) width;
  unsigned int i;
  if(checklists_count == -1){
    create_checklists_menu();
  }
  checkable = 0;
                    if (linuxCheckListWidget != NULL) {

                        //get current window dimensions
                        XPGetWidgetGeometry(linuxCheckListWidget, &outLeft, &outTop, &outRight, &outBottom);
                        x = outLeft;
                        y = outTop;
                        x2 = outRight;
                        y2 = outBottom;

                        XPDestroyWidget(linuxCheckListWidget, 1);

                    }
  float maxw_1 = 0;
  float maxw_2 = 0;
  float tmp;
  for(i = 0; i < size; ++i){
    tmp = XPLMMeasureString(xplmFont_Basic, items[i].text, strlen(items[i].text));
    if(tmp > maxw_1){
      maxw_1 = tmp;
    }
    tmp = XPLMMeasureString(xplmFont_Basic, items[i].suffix, strlen(items[i].suffix));
    if(tmp > maxw_2){
      maxw_2 = tmp;
    }
  }
  w = maxw_1 + maxw_2 + 75;
    int x2 = x + w;
    int y2 = y - h;
    //int Index;  //unused
    int WindowCentre = x+w/2;
    int yOffset;
    bool flip, cop;

    max_items = size;
    
    
    // Create the Main Widget window.

    linuxCheckListWidget = XPCreateWidget(x, y, x2, y2,
                                    state[1],	// Visible
                                    title,	// desc
                                    1,		// root
                                    NULL,	// no container
                                    xpWidgetClass_MainWindow);


// Add Close Box to the Main Widget.  Other options are available.  See the SDK Documentation.
    XPSetWidgetProperty(linuxCheckListWidget, xpProperty_MainWindowHasCloseBoxes, 1);

    printf("Button # %d has value %s \n", Item, (state[Item])?"true":"false");
    if (state[0] == true) {
        XPSetWidgetProperty(linuxCheckListWidget, xpProperty_MainWindowType, xpMainWindowStyle_Translucent);

    }
    else {


    }

// Print each line of the checklist in widget window

     for(i = 0; i < size; ++i){

     yOffset = (5+18+(i*20));
     flip = items[i].info_only;

            // Create a copilot text widget
            cop = !(flip);
            linuxCheckListCopilotWidget[i] = XPCreateWidget(x+5, y-yOffset, x+60+200, y-yOffset-20,
            items[i].copilot_controlled,	// Visible
            "+",// desc
            0,		// root
            linuxCheckListWidget,
            xpWidgetClass_Caption);
            if (state[0] == true) {

             XPSetWidgetProperty(linuxCheckListCopilotWidget[i], xpProperty_CaptionLit, 1);

             }


             // Create a check box for a checklist item widget

             linuxCheckListCheckWidget[i] = XPCreateWidget(x+25, y-yOffset, x+15+20, y-yOffset-20,
             !(items[i].item_void),	// Visible
             "",// desc
             0,		// root
             linuxCheckListWidget,
             xpWidgetClass_Button);

             XPSetWidgetProperty(linuxCheckListCheckWidget[i], xpProperty_ButtonType, xpRadioButton);
             XPSetWidgetProperty(linuxCheckListCheckWidget[i], xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
             XPSetWidgetProperty(linuxCheckListCheckWidget[i], xpProperty_ButtonState, 0);

            // Create the description section for checklist item widget
            linuxCheckListTextWidget[i] = XPCreateWidget(x+40, y-yOffset, x+maxw_1+20, y-yOffset-20,
            1,	// Visible
            items[i].text,// desc
            0,		// root
            linuxCheckListWidget,
            xpWidgetClass_Caption);

            if (state[0] == true) {

               XPSetWidgetProperty(linuxCheckListTextWidget[i], xpProperty_CaptionLit, 1);

             }

             // Create the action for a checklist item widget
             linuxCheckListTextAWidget[i] = XPCreateWidget(x+maxw_1+40, y-yOffset, x+maxw_1+maxw_2+40, y-yOffset-20,
             1,	// Visible
             items[i].suffix,// desc
             0,		// root
             linuxCheckListWidget,
             xpWidgetClass_Caption);

             if (state[0] == true) {

                XPSetWidgetProperty(linuxCheckListTextAWidget[i], xpProperty_CaptionLit, 1);

              }
     }


     // Create a checklist item sction description text widget
     yOffset = (5+18+(15*20));

     linuxCheckListCopilotInfoWidget = XPCreateWidget(x+5, y-yOffset, x+60+200, y-yOffset-20,
     1,	// Visible
     "+ = Automaticly Checked Items (Copilot)",// desc
     0,		// root
     linuxCheckListWidget,
     xpWidgetClass_Caption);

     if (state[0] == true) {

      XPSetWidgetProperty(linuxCheckListCopilotInfoWidget, xpProperty_CaptionLit, 1);

      }

     int bw = w / 2 - 10;
     linuxChecklistPreviousButton = XPCreateWidget(WindowCentre-bw, y2+40, WindowCentre-5, y2+10,
                                     1, "Previous", 0, linuxCheckListWidget,
                                     xpWidgetClass_Button);

     XPSetWidgetProperty(linuxChecklistPreviousButton, xpProperty_ButtonType, xpPushButton);
     XPSetWidgetProperty(linuxChecklistPreviousButton, xpProperty_Enabled, (index == 0) ? 0 : 1);

    linuxChecklistNextButton = XPCreateWidget(WindowCentre+5, y2+40, WindowCentre+bw, y2+10,
                                     1, "Next", 0, linuxCheckListWidget,
                                     xpWidgetClass_Button);

     XPSetWidgetProperty(linuxChecklistNextButton, xpProperty_ButtonType, xpPushButton);
     XPSetWidgetProperty(linuxChecklistNextButton, xpProperty_Enabled, (index < (checklists_count-1)) ? 1 : 0);
printf("Checklist index %d (of %d)\n", index, checklists_count);
     // Register our widget handler
     XPAddWidgetCallback(linuxCheckListWidget, linuxCheckListHandler);

     if(!XPIsWidgetVisible(linuxCheckListWidget)){
       XPShowWidget(linuxCheckListWidget);
     }

     if((!force_show) && (state[1] == false)){
       XPHideWidget(linuxCheckListWidget);
     }
  return true;
}

bool check_item(int item)
{
  printf("Checking item %d\n", item);
  if(item >= 0){
    XPSetWidgetProperty(linuxCheckListCheckWidget[item], xpProperty_ButtonState, 1);
    item_checked(item);
    return true;
  }
  return false;
}

bool activate_item(int item)
{
  checkable = item;
  printf("Activating item %d\n", item);
  return true;
}

struct dataref_t{
  XPLMDataRef dref;
  XPLMDataTypeID dref_type;
  int index;
};

bool find_dataref(const char *name, dataref_p *dref)
{
  XPLMDataRef tmp = XPLMFindDataRef(name);
  if(tmp == NULL){
    *dref= NULL;
    return false;
  }
  *dref = new struct dataref_t;
  (*dref)->dref = tmp;
  (*dref)->dref_type = XPLMGetDataRefTypes(tmp);
  (*dref)->index = -1;
  return true;
}


bool find_array_dataref(const char *name, int index, dataref_p *dref)
{
  XPLMDataRef tmp = XPLMFindDataRef(name);
  if(tmp == NULL){
    *dref= NULL;
    return false;
  }
  *dref = new struct dataref_t;
  (*dref)->dref = tmp;
  (*dref)->dref_type = XPLMGetDataRefTypes(tmp);
  (*dref)->index = index;
  return true;
}

//Get value of float dataref
//  name is the dataref name
//
//  Returns the float value of selected dataref
float get_float_dataref(dataref_p dref)
{
  XPLMDataTypeID type = dref->dref_type;
  if(type == xplmType_Unknown){
    return 0.0f;
  }
  if((type & xplmType_FloatArray) != 0){
    int index = (dref->index) < 0 ? 0 : dref->index; 
    float val = 0;
    if(XPLMGetDatavf(dref->dref, &val, index, 1) != 1){
      return 0.0f;
    }else{
      return val;
    }
  }
  if((type & xplmType_IntArray) != 0){
    int index = (dref->index) < 0 ? 0 : dref->index; 
    int val = 0;
    if(XPLMGetDatavi(dref->dref, &val, index, 1) != 1){
      return 0.0f;
    }else{
      return (float) val;
    }
  }
  if((type & xplmType_Int) != 0){
    return (float)XPLMGetDatai(dref->dref);
  }
  if((type & xplmType_Float) != 0){
    return XPLMGetDataf(dref->dref);
  }
  if((type & xplmType_Double) != 0){
    return XPLMGetDatad(dref->dref);
  }
  return 0.0f;
}


bool dispose_dataref(dataref_p *dref)
{
  (void)dref;
  return true;
}

//We don't need this one...
void dataref_val(dataref_p dref, float f)
{
  (void) dref;
  (void) f;
}

void get_sim_path(char *path)
{
  XPLMGetSystemPath(path);
}


int MyCheckItemCommandCallback(
                                   XPLMCommandRef       inCommand,
                                   XPLMCommandPhase     inPhase,
                                   void *               inRefcon)
{
(void) inCommand;
//(void) inPhase;
(void) inRefcon;

if (inPhase == xplm_CommandBegin) {
          printf ("trying to make check_item to work \n");
          check_item(checkable);
}

return 1;
}

int MyNextChecklistCommandCallback(
                                   XPLMCommandRef       inCommand,
                                   XPLMCommandPhase     inPhase,
                                   void *               inRefcon)
{
  (void) inCommand;
  //(void) inPhase;
  (void) inRefcon;


  if (inPhase == xplm_CommandBegin) {
            next_checklist();
  }

  return 1;
}
