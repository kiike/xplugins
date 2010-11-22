#include <espeak/speak_lib.h>

#include <AL/al.h>
#include <AL/alc.h>

#include <unistd.h>
#include <string>
#include <iostream>

#include "interface.h"

#define NUM_BUFFERS 3

class speech_alespeak
{
 public:
  speech_alespeak();
  ~speech_alespeak();
  static void say(std::string text);
 private:
  int init_speech();
  int close_speech();
  int init_al();
  int close_al();
  
  static ALenum check_error(bool verbose, const char *id);
  
  static void unqueue_buffers();
  static void play_if_needed();
  static ALuint find_buffer();
  static bool queue_buffer(short *wav, int numsamples);
  static int esp_callback(short *wav, int numsamples, espeak_EVENT *events);

  static ALCdevice *dev;
  static ALCcontext *ctx;
  static ALCcontext *old_ctx;
  static ALuint source;
  static ALuint buffers[NUM_BUFFERS];

  static int used_buffers;

  static ALenum format;
  static ALenum code;
  static int sample_rate;
  static unsigned int id;

  static bool verbose_flag;
  static bool report_success;
  static bool speaking;
};

ALCdevice *speech_alespeak::dev;
ALCcontext *speech_alespeak::ctx;
ALCcontext *speech_alespeak::old_ctx;
ALuint speech_alespeak::source;
ALuint speech_alespeak::buffers[NUM_BUFFERS];

int speech_alespeak::used_buffers;

ALenum speech_alespeak::format;
ALenum speech_alespeak::code;
int speech_alespeak::sample_rate;
unsigned int speech_alespeak::id;
bool speech_alespeak::verbose_flag;
bool speech_alespeak::report_success;
bool speech_alespeak::speaking;


int speech_alespeak::init_speech()
{
  int sample_rate;
  char path[1024];
  get_sim_path(path);
  std::string data_path = std::string(path) + "Resources/plugins/Xchecklist/"; 
  if((sample_rate = espeak_Initialize(AUDIO_OUTPUT_RETRIEVAL, 1000, data_path.c_str(), 0)) < 0){
    printf("Problem initializing espeak!");
    return -1;
  }
  espeak_SetSynthCallback(speech_alespeak::esp_callback);
  espeak_SetParameter(espeakRATE, 150, 0);
  speaking = false;
  return sample_rate;
}

int speech_alespeak::init_al()
{
  old_ctx = alcGetCurrentContext();
  dev = alcOpenDevice(NULL);
  if(!dev){
    printf("Problem opening device!\n");
    return -1;
  }
  ctx = alcCreateContext(dev, NULL);
  if(!ctx){
    printf("Problem creating context!\n");
    return -1;
  }
  alcMakeContextCurrent(ctx);
  alGetError();
  alGenSources(1, &source);
  if(speech_alespeak::check_error(verbose_flag, "alGenSources") != AL_NO_ERROR){
    printf("Problem generating source!\n");
    if(old_ctx){
      alcMakeContextCurrent(old_ctx);
    }
    return -1;
  }
  alGetError();
  alGenBuffers(NUM_BUFFERS, buffers);
  if(speech_alespeak::check_error(verbose_flag, "alGenBuffers") != AL_NO_ERROR){
    printf("Problem generating buffers!\n");
    return -1;
  }
  if(verbose_flag){
    printf("%d buffers generated\n", NUM_BUFFERS);
  }
  if(old_ctx){
    alcMakeContextCurrent(old_ctx);
  }
  return 0;
}

ALenum speech_alespeak::check_error(bool verbose, const char *id)
{
  ALenum res = alGetError();
  if(verbose){
    const char *err = "";
    switch(res){
      case AL_NO_ERROR:
	if(speech_alespeak::report_success){
	  err = "No error";
	}
	break;
      case AL_INVALID_NAME:
	err = "Invaid name";
	break;
      case AL_INVALID_ENUM:
	err = "Invalid enum";
	break;
      case AL_INVALID_VALUE:
	err = "Invalid value";
	break;
      case AL_INVALID_OPERATION:
	err = "Invalid operation";
	break;
      case AL_OUT_OF_MEMORY:
	err = "Out of memory";
	break;
      default:
	err = "UNKNOWN ERROR CODE!";
	break;
    }
    std::cerr<<id<<" : "<<err<<std::endl;
  }
  return res;
}

speech_alespeak::speech_alespeak()
{
  used_buffers = 0;
  format = AL_FORMAT_MONO16;
  verbose_flag = false;
  report_success = true;

  sample_rate = init_speech();
  if(sample_rate < 0){
    throw;
  }
  if(verbose_flag){
    std::cerr<<"Espeak initialized @ "<<sample_rate<<"Hz"<<std::endl;
  }
  if(init_al() == 0){
    if(verbose_flag){
      std::cerr<<"OpenAL initialized!"<<std::endl;
    }
  }
}

speech_alespeak::~speech_alespeak()
{
  std::cout<<"Closing speaker!"<<std::endl;
  close_speech();
  std::cout<<"Closing al!"<<std::endl;
  close_al();
}

void speech_alespeak::unqueue_buffers()
{
  ALuint tmp_buffers[NUM_BUFFERS];
  int res;
  alGetSourcei(source, AL_BUFFERS_PROCESSED, &res);
  if(check_error(verbose_flag, "alGetSourcei") != AL_NO_ERROR){
    printf("Problem querying processed buffers!\n");
    return;
  }
  alSourceUnqueueBuffers(source, res, tmp_buffers);
  if(check_error(verbose_flag, "alSourceUnqueueBuffers") != AL_NO_ERROR){
    printf("Problem unqueuing buffers!\n");
    return;
  }
}

void speech_alespeak::play_if_needed()
{
  ALint res = 0;
  alGetError();
  alGetSourcei(source, AL_SOURCE_STATE, &res);
  if(res != AL_PLAYING){
    alGetError();
    
    speech_alespeak::unqueue_buffers();
    
    alSourcePlay(source);
    if(check_error(verbose_flag, "alSourcePlay") != AL_NO_ERROR){
      printf("Problem playing!\n");
      return;
    }
  }
}

//Either use one of free buffers (only after startup) or unqueue one.
ALuint speech_alespeak::find_buffer()
{
  ALuint res = 0;
  if(used_buffers < NUM_BUFFERS){
    res = buffers[used_buffers];
    ++used_buffers;
  }else{
    bool jump_out = false;
    while(1){
      alGetError();
      alSourceUnqueueBuffers(source, 1, &res);
      code = alGetError();
      switch(code){
	case AL_NO_ERROR: //unqueued ok...
	  jump_out = true;
	  break;
	case AL_INVALID_VALUE:
	  usleep(50000); //wait for 50ms
	  break;
	default:
	  fprintf(stderr, "Problem unqueuing buffer...\n");
	  jump_out = true;
	  res = 0;
	  break;
      }
      if(jump_out) break;
    }
  }
  return res;
}


bool speech_alespeak::queue_buffer(short *wav, int numsamples)
{
    old_ctx = alcGetCurrentContext();
    alcMakeContextCurrent(ctx);

    ALuint buf = find_buffer();
    if(buf == 0){
      return false;
    }
    alGetError();
    alBufferData(buf, format, wav, numsamples * 2, sample_rate);
    if(check_error(verbose_flag, "alBufferData") != AL_NO_ERROR){
      printf("Problem buffering data!\n");
      return false;
    }
    alSourceQueueBuffers(source, 1, &buf);
    if(check_error(verbose_flag, "alSourceQueueBuffers") != AL_NO_ERROR){
      printf("Problem queuing buffers!\n");
      return false;
    }
    
    play_if_needed();
    if(old_ctx){
      alcMakeContextCurrent(old_ctx);
    }
  return true;
}


int speech_alespeak::esp_callback(short *wav, int numsamples, espeak_EVENT *events)
{
  (void) wav;
  if(verbose_flag){
    printf("Got %d samples...\n", numsamples);
  }
  if(numsamples > 0){
    queue_buffer(wav, numsamples);
  }
  
  bool end_flag = false;
  const char *msg = "";
  while(1){
    switch(events->type){
      case espeakEVENT_LIST_TERMINATED:
        msg = "List terminated!";
        end_flag = true;
	break;
      case espeakEVENT_WORD:
        msg = "Word!";
	break;
      case espeakEVENT_SENTENCE:
        msg = "Sentence!";
	break;
      case espeakEVENT_MARK:
        msg = "Mark!";
	break;
      case espeakEVENT_PLAY:
        msg = "Play!";
	break;
      case espeakEVENT_END:
        msg = "End!";
	break;
      case espeakEVENT_MSG_TERMINATED:
        msg = "Terminated!";
	speaking = false;
	break;
      case espeakEVENT_PHONEME:
        msg = "Phoneme!";
	break;
      default:
        msg = "I DON'T KNOW...";
	break;
    }
    if(verbose_flag){
      printf("%s\n", msg);
    }
    if(end_flag) break;
    ++events;
  }
  return 0;
}


int speech_alespeak::close_speech()
{
  espeak_Terminate();
  return 0;
}

int speech_alespeak::close_al()
{
  int cntr = 100;
  ALint res;

  old_ctx = alcGetCurrentContext();
  alcMakeContextCurrent(ctx);
  alGetError();
  alSourceStop(source);
  while(cntr != 0){
    alGetSourcei(source, AL_SOURCE_STATE, &res);
    if(res == AL_PLAYING){
      usleep(10000);
    }else{
      break;
    }
  }
  if(verbose_flag){
    printf("Cleaning up AL!\n");
  }
  
  speech_alespeak::unqueue_buffers();
  alDeleteBuffers(NUM_BUFFERS, buffers);
  if(check_error(verbose_flag, "alDeleteBuffers") != AL_NO_ERROR){
    printf("Problem deleting buffers!\n");
    return -1;
  }
  if(verbose_flag){
    printf("Closing AL!\n");
  }
  alDeleteSources(1, &source);
  if(old_ctx){
    alcMakeContextCurrent(old_ctx);
  }else{
    alcMakeContextCurrent(NULL);
  }
  alcDestroyContext(ctx);
  alcCloseDevice(dev);
  return 0;
}

void speech_alespeak::say(std::string text)
{
  static speech_alespeak speech_inst;
  text +=  "<break>";
  std::cout<<"Saying \""<<text.c_str()<<"\""<<std::endl;
  // Size must be + 1, since they count characters, not the trailing NULL.
  espeak_Synth(text.c_str(), text.size() + 1, 0, POS_CHARACTER, 0, 
    espeakCHARS_8BIT | espeakSSML, &id, NULL);
}

speech_alespeak *speech = NULL;

void say(const char *text)
{
  if(speech != NULL){
    speech->say(text);
  }
}

bool init_speech()
{
  if(speech == NULL){
    speech = new speech_alespeak();
    return true;
  }
  return false;
}

void close_speech()
{
  delete speech;
  speech = NULL;
}