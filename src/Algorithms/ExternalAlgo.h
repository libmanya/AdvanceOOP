#ifndef __EXTERNAL_ALGO_H 
#define __EXTERNAL_ALGO_H

#include <map> 
#include <string>

// typedef to make it easier to set up our factory 
typedef AbstractAlgorithm* maker_t();
// our global factory 
extern map<string, maker_t *> factory;
#endif // __EXTERNAL_ALGO_H