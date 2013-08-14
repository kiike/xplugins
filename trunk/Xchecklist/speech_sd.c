#include "XPLMUtilities.h"
#if LIN

#include <stdio.h>
#include <libspeechd.h>
#include <dlfcn.h>
#include <stdbool.h>


typedef SPDConnection* (*spd_open_t)(const char* client_name, const char* connection_name, 
                                     const char* user_name, SPDConnectionMode mode);
typedef void (*spd_close_t)(SPDConnection* connection);
typedef int (*spd_say_t)(SPDConnection* connection, SPDPriority priority, const char* text);
typedef int (*spd_set_notification_t)(SPDConnection* connection, SPDNotification notification);
typedef int (*spd_set_notification_oxx_t)(SPDConnection* connection, SPDNotification notification);

static spd_open_t spd_open_fun = NULL;
static spd_close_t spd_close_fun = NULL;
static spd_say_t spd_say_fun = NULL;
static spd_set_notification_oxx_t spd_set_notification_on_fun = NULL;
static spd_set_notification_oxx_t spd_set_notification_off_fun = NULL;

static void *lib_handle = NULL;

struct func_defs_t{
  char *name;
  void *ref;
};

struct func_defs_t functions[] = 
{
  {(char*)"spd_open", (void*)&spd_open_fun},
  {(char*)"spd_close", (void*)&spd_close_fun},
  {(char*)"spd_say", (void*)&spd_say_fun},
  {(char*)"spd_set_notification_on", (void*)&spd_set_notification_on_fun},
  {(char*)"spd_set_notification_off", (void*)&spd_set_notification_off_fun},
  {NULL, NULL}
};

static SPDConnection* connection = NULL;
static bool speaking = 0;


static int load_functions()
{
  dlerror();
  lib_handle = dlopen("libspeechd.so.2", RTLD_NOW | RTLD_LOCAL);
  if(lib_handle == NULL){
    fprintf(stderr, "Couldn't load library!\n");
    return -1;
  }
  int i = 0;
  void *symbol;
  while((functions[i]).name != NULL){
    dlerror();
    if((symbol = dlsym(lib_handle, (functions[i]).name)) == NULL){
      fprintf(stderr, "Couldn't load symbol '%s': %s\n", (functions[i]).name, dlerror());
      return -1;
    }
    *((void **)(functions[i]).ref) = symbol;
    ++i;
  }
  return 0;
}



static SPDConnection* wspd_open(const char* client_name, const char* connection_name, 
                                     const char* user_name, SPDConnectionMode mode)
{
  if(spd_open_fun == NULL){
    return NULL;
  }
  return spd_open_fun(client_name, connection_name, user_name, mode);
}

static void wspd_close(SPDConnection* connection)
{
  if(spd_close_fun == NULL){
    return;
  }
  spd_close_fun(connection);
}

static int wspd_say(SPDConnection* connection, SPDPriority priority, const char* text)
{
  if(spd_say_fun == NULL){
    return -1;
  }
  return spd_say_fun(connection, priority, text);
}

static int wspd_set_notification_on(SPDConnection* connection, SPDNotification notification)
{
  if(spd_set_notification_on_fun == NULL){
    return 0;
  }
  return spd_set_notification_on_fun(connection, notification);
}

static int wspd_set_notification_off(SPDConnection* connection, SPDNotification notification)
{
  if(spd_set_notification_off_fun == NULL){
    return 0;
  }
  return spd_set_notification_off_fun(connection, notification);
}

static void finish_callback(size_t msg_id, size_t client_id, SPDNotificationType state)
{
  (void)msg_id;
  (void)client_id;
  switch(state){
    case SPD_EVENT_END:
    case SPD_EVENT_CANCEL:
      --speaking;
      break;
    default:
      break;
  }
}
#endif

bool init_speech()
{
#if LIN
  if(load_functions()){
    fprintf(stderr, "Failed to load speech-dispatch library!\n");
    return false;
  }
  
  connection = wspd_open("xchecklist", "copilot", NULL, SPD_MODE_THREADED);
  if(connection == NULL){
    fprintf(stderr, "Failed to open speech-dispatch connection!\n");
    return false;
  }
  
  connection->callback_end = connection->callback_cancel = finish_callback;
  wspd_set_notification_on(connection, SPD_END);
  wspd_set_notification_on(connection, SPD_CANCEL);
#endif  
  return true;
}

void say(const char *text)
{
  XPLMSpeakString(text);
#if LIN
  int res = wspd_say(connection, SPD_MESSAGE, text);
  if(res != 0){
    ++speaking;
  }
#endif
}

void close_speech()
{
#if LIN
  wspd_set_notification_off(connection, SPD_END);
  wspd_set_notification_off(connection, SPD_CANCEL);
  wspd_close(connection);
#endif
}

void cleanup_speech()
{
}

bool speech_active()
{
#if LIN
  if(connection != NULL){
    return true;
  }else{
    return false;
  }
#else
  return true;
#endif
}

bool spoken()
{
#if LIN
  if(speaking == 0){
    return true;
  }else{
    return false;
  }
#else
  return true;
#endif
}


