#include "Encoder.h"

#include <stdlib.h>
#include <string>

int Encoder::encode(const string& imagesString, const string& videoOutput)
{
  string ffmpegCmd = "ffmpeg -y -loglevel quiet -i " + imagesString + " " + videoOutput;
  int ret = system(ffmpegCmd.c_str());
  return ret;
}

