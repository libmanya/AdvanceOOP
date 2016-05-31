#ifndef _MONTAGE__H_
#define _MONTAGE__H_

#include <vector>
#include <string>
#include "Logger.h"
using namespace std;

class Encoder
{
public:
  static void encode(const string& imagesString, const string& videoOutput);
};

#endif //_MONTAGE__H_
