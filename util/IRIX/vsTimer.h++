//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2001, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsTimer.h++
//
//    Description:  Object for measuring elapsed (real) time
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_TIMER_HPP
#define VS_TIMER_HPP

#include "vsGlobals.h++"

class VS_UTIL_DLL vsTimer
{
private:

    static vsTimer    *systemTimer;

    double	      markTime;
    double	      markInterval;

VS_INTERNAL:

    static void       deleteSystemTimer();

public:

                      vsTimer();
		      ~vsTimer();

    static vsTimer    *getSystemTimer();

    void	      mark();
    double	      getInterval();
    double	      getElapsed();
};

#endif
