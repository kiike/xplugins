#ifndef SPEECH__H
#define SPEECH__H


#ifndef __cplusplus
extern "C" {
#endif

bool init_speech();
void say(const char *text);
void close_speech();

#ifndef __cplusplus
}
#endif
#endif