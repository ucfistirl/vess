//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2003, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsScentAirSystem.h++
//
//    Description:  Support for the Scent Air system at IST
//
//    Author(s):    Ryan Wilson
//
//------------------------------------------------------------------------

#ifndef VS_SCENT_AIR_SYSTEM_HPP
#define VS_SCENT_AIR_SYSTEM_HPP

#include "vsIOSystem.h++"
#include "vsParallelPort.h++"
#include "vsTimer.h++"

class vsScentAirSystem : public vsIOSystem
{
    private:
        vsGrowableArray     scentChannelArray;
        int                 scentChannelCount;

        vsParallelPort *    port;

        // Allocates internal data structures for each scent channel
        void                setNumberOfScentChannels(int numberOfChannels);

        // The data that is sent to the parallel port
        unsigned char       parallelPortDataByte;
        void                setParallelPin(int pin, bool state);

    public:
                        vsScentAirSystem(int portNumber);
                        ~vsScentAirSystem();

        const char *    getClassName();

        void            update();

        // Set the pulse rate of the given channel
        void            setPulseRate(int whichChannel, double onTime, 
                                     double offTime);

        // Determine the number of channels
        int             getNumberOfScentChannels( );

        // Turn a channel on/off or find out if it's on/off
        void            setChannelState(int whichChannel, bool isOn);
        bool            getChannelState(int whichChannel);
};

#endif

