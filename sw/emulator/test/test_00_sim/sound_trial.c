#include <SDL2/SDL.h>
#include <math.h>
double Frequency;
SDL_AudioSpec Desired;
SDL_AudioSpec Obtained;

double wave(double t)
{
  // return t - abs(t) < 0.5 ? 1 : -1;
  return cos(t * 2 * 3.14);
}

void callback(void *userdata, Uint8 *stream, int len)
{
  int i;
  static unsigned int step= 0;
  Uint16 *frames = (Uint16 *) stream;
  int framesize = len / 2;
  for (i = 0; i < framesize; i++, step++) {
    frames[i]= wave(step * Frequency / Obtained.freq) * 3000;
  }
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(stderr, "usage: %s frequency\n", argv[0]);
    return 0;
  } else {
    Frequency = atof(argv[1]);
  }

  Desired.freq= 22050; /* Sampling rate: 22050Hz */
  Desired.format= AUDIO_S16LSB; /* 16-bit signed audio */
  Desired.channels= 0; /* Mono */
  Desired.samples= 8192; /* Buffer size: 8K = 0.37 sec. */
  Desired.callback= callback;
  Desired.userdata= NULL;

  SDL_OpenAudio(&Desired, &Obtained);
  SDL_PauseAudio(0);
  SDL_Delay(200);
  SDL_Quit();
  return 0;
}
