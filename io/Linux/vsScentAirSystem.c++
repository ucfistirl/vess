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
//    VESS Module:  vsScentAirSystem.c++
//
//    Description:  Support for the Scent Air system at IST
//
//    Author(s):    Ryan Wilson
//
//------------------------------------------------------------------------

#include "vsScentAirSystem.h++"

#define VS_DEFAULT_NUMBER_OF_SCENT_CHANNELS  3

// A Structure to store information about each scent air channel
struct ScentChannel
{
    // When pulsating, how long to keep it on or off (in seconds)
    double onTime, offTime;

    // The timer to track the pulses
    vsTimer timer;

    // is the channel physically running at this time?
    bool isOn;

    // if the zone is marked as on and should be pulsating
    bool isRunning;

    ScentChannel()
        : onTime( 1.0 ), offTime( 0.0 ), isRunning( false ), isOn( false )
        { };
};

// ------------------------------------------------------------------------
// Constructor
// Setup an interface to a Scent Air System through the parallel port
// ------------------------------------------------------------------------
vsScentAirSystem::vsScentAirSystem(int portNumber)
    : scentChannelArray( 0, 1 ), scentChannelCount( 0 ), port( NULL ),
    parallelPortDataByte( 0x0 )
{
    char portDevice[30];
    int i;

    // Open the requested parallel port
    sprintf(portDevice, "/dev/parport%d", portNumber - 1);
    port = new vsParallelPort(portDevice);

    // If the port is open, set up our scent channels
    if( port->isPortOpen() )
    {
        // Set up the scent channels
        setNumberOfScentChannels( VS_DEFAULT_NUMBER_OF_SCENT_CHANNELS );

	// Turn all scents off
	for (i = 0; i < 8; i++)
            setParallelPin(i, false);
	port->setDataLines(parallelPortDataByte);
    }
    else
    {
        printf("vsScentAirSystem::vsScentAirSystem:  Unable to open port "
            "%s\n", portDevice);
        setNumberOfScentChannels( 0 );
    }
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsScentAirSystem::~vsScentAirSystem()
{
    int i;

    // Clean up all the data we stored on the scent channels
    setNumberOfScentChannels( 0 );

    // Close the port, making sure all scents are off first
    if (port)
    {
	// Turn all scents off
	for (i = 0; i < 8; i++)
            setParallelPin(i, false);
	port->setDataLines(parallelPortDataByte);

	// Delete the port object
        delete port;
    }
}

// ------------------------------------------------------------------------
// Derived from vsObject
// ------------------------------------------------------------------------
const char * vsScentAirSystem::getClassName()
{
    return "vsScentAirSystem";
}

// ------------------------------------------------------------------------
// Derived from vsUpdatable. The update function keeps track of pulsating
// the various scent air channels
// ------------------------------------------------------------------------
void vsScentAirSystem::update()
{
    int i;
    // Did the parallel port data byte change? If so, we'll update the port
    bool changed = false;

    // Update each scent channel
    for (i=0; i<scentChannelCount; i++)
    {
        ScentChannel * channel = (ScentChannel *)scentChannelArray.getData(i);

        // is this channel running? (i.e. it should be emitting scents at the
        // given interval)
        if (channel->isRunning)
        {
            // If the channel is on and it's run for the full onTime
            // (and the off time is not 0.0), turn it off
            if (channel->isOn
                    && channel->timer.getElapsed()>=channel->onTime
                    && !channel->offTime<1.0e-6 )
            {
                // Mark the channel as off and restart the timer
                channel->isOn = false;
                channel->timer.mark();

                // Change the state of the parallel port and mark that
                // it needs to be updated
                setParallelPin( i, false );
                changed = true;
            }
            // If the channel is off and it's run for the full offTime
            else if (!channel->isOn
                    && channel->timer.getElapsed()>=channel->offTime )
            {
                // Mark the channel as off and restart the timer
                channel->isOn = true;
                channel->timer.mark();

                // Change the state of the parallel port and mark that
                // it needs to be updated
                setParallelPin( i, true );
                changed = true;
            }
        }
    }

    // if we need to update the parallel port, send the new byte
    if( changed )
        port->setDataLines( parallelPortDataByte );
}

// ------------------------------------------------------------------------
// Private
// Sets the number of scent channels we store data about
// ------------------------------------------------------------------------
void vsScentAirSystem::setNumberOfScentChannels( int numberOfChannels )
{
    int i;

    if (numberOfChannels >= 0)
    {
        // Are we shrinking the number of channels
        if (numberOfChannels<scentChannelCount)
        {
            for( i=numberOfChannels; i<scentChannelCount; i++ )
            {
                ScentChannel * channel =
                    (ScentChannel *)scentChannelArray.getData(i);

                // If this channel is on, turn it off
                if( channel->isOn )
                {
                    setParallelPin( i, false );

                    // Output the new data byte
                    port->setDataLines( parallelPortDataByte );
                }

                // Free the channel's data
                delete channel;
            }
        }

        // Set the new size of the array
        scentChannelArray.setSize(numberOfChannels);

        // If the array grew in size, allocate the new structures
        if (numberOfChannels>scentChannelCount)
        {
            for (i=scentChannelCount; i<numberOfChannels; i++)
            {
                // Allocate & Store the data for the new channel
                ScentChannel * channel = new ScentChannel;

                scentChannelArray.setData(i, (void *)channel);
            }
        }

        // Set the new size count
        scentChannelCount = numberOfChannels;
    }
}

// ------------------------------------------------------------------------
// Get the number of available scent air channels
// ------------------------------------------------------------------------
int vsScentAirSystem::getNumberOfScentChannels()
{
    return scentChannelCount;
}

// ------------------------------------------------------------------------
// Private
// Set a bit in a byte to the given value
// ------------------------------------------------------------------------
void vsScentAirSystem::setParallelPin( int pin, bool state )
{
    // If it's a valid pin on the port [0,8) 
    if (pin>=0 && pin<8)
        parallelPortDataByte =
            (parallelPortDataByte & ~(1<<pin))
            | (( state ? 1 : 0 )<<pin);
}

// ------------------------------------------------------------------------
// Set the pulse rate on the given channel. This allows you to set up how
// long a channel should be on or off without worrying about doing it
// yourself.
// ------------------------------------------------------------------------
void vsScentAirSystem::setPulseRate( int whichChannel,
        double onTime, double offTime )
{
    // Is this a valid channel?
    if (whichChannel>=0 && whichChannel<scentChannelCount)
    {
        ScentChannel * channel =
            (ScentChannel *)scentChannelArray.getData(whichChannel);

        // Set the new times
        channel->onTime = onTime;
        channel->offTime = offTime;
    }
}

// ------------------------------------------------------------------------
// Turn a scent channel on or off
// ------------------------------------------------------------------------
void vsScentAirSystem::setChannelState( int whichChannel, bool isOn )
{
    if (whichChannel>=0 && whichChannel<scentChannelCount)
    {
        ScentChannel * channel =
            (ScentChannel *)scentChannelArray.getData(whichChannel);

        // If this ScentAir channel is not already running...
        if( isOn && !channel->isRunning )
        {
            // Mark the channel as on
            channel->isRunning = true;
            channel->isOn = true;
            channel->timer.mark();

            // Turn this scent on now
            setParallelPin( whichChannel, true );

            // Set the parallel port to the new data byte
            port->setDataLines( parallelPortDataByte );
        }
        else if( !isOn && channel->isRunning )
        {
            // Mark the channel as off
            channel->isRunning = false;
            channel->isOn = false;
            channel->timer.mark();

            // Turn this scent off now
            setParallelPin( whichChannel, false );

            // Set the parallel port to the new data byte
            port->setDataLines( parallelPortDataByte );
        }
    }
}

// ------------------------------------------------------------------------
// Find out if a channel is on or off
// ------------------------------------------------------------------------
bool vsScentAirSystem::getChannelState( int whichChannel )
{
    // Is this a valid channel?
    if (whichChannel>=0 && whichChannel<scentChannelCount)
    {
        ScentChannel * channel =
            (ScentChannel *)scentChannelArray.getData(whichChannel);

        // Return if the channel is on
        return channel->isRunning;
    }
    else
        return false;
}

