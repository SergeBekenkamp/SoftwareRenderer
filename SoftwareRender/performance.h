#pragma once
#include <time.h>

#ifndef SYSOUT_F
#define SYSOUT_F(f, ...)      _RPT1( 0, f, __VA_ARGS__ ) // For Visual studio
#endif

#ifndef speedtest__             
#define speedtest__(data)   for (long blockTime = NULL; (blockTime == NULL ? (blockTime = clock()) != NULL : false); SYSOUT_F(data "%.9fs \n", (double) (clock() - blockTime) / CLOCKS_PER_SEC))
#endif