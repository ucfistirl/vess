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
//    VESS Module:  vsVestSystem.c++
//
//    Description:  Interface to IST's vibrating vest
//
//    Author(s):    Ryan Wilson
//
//------------------------------------------------------------------------

#include "vsVestSystem.h++"
#include <string.h>

#define VS_VEST_NUMBER_OF_ZONES 8

// How long to wait for the vest to respond after sending
// the reset/begin command
#define VS_VEST_RESET_TIMEOUT   .25
#define VS_VEST_BEGIN_TIMEOUT   1.0

// -----------------------------------------------------------------------
// Initialize the vest for use
// -----------------------------------------------------------------------
vsVestSystem::vsVestSystem( int portNumber )
    : vestState( VS_VEST_STATE_UNKNOWN ), zonesChanged(false),
    currentState( 0 ), bufferLength( 0 ), bytesToIgnore( 0 )
{
    char   portDevice[30];
    int    i;

    // Determine the platform-dependent serial device
    // name

#ifdef IRIX
    sprintf(portDevice, "/dev/ttyd%d", portNumber);
#endif

#ifdef IRIX64
    sprintf(portDevice, "/dev/ttyd%d", portNumber);
#endif

#ifdef __linux__
    sprintf(portDevice, "/dev/ttyS%d", portNumber - 1);
#endif

#ifdef WIN32
    sprintf(portNumber, "COM%d", portNumber );
#endif

    port = new vsSerialPort(portDevice);

    // Check: Does starting the vest turn all zones to off?

    // Create the vest object and configure it
    vest = new vsVest( VS_VEST_NUMBER_OF_ZONES );
    for(i=0; i<VS_VEST_NUMBER_OF_ZONES; i++)
    {
        // check to see if this vest zone is on
        vsInputButton * button = vest->getButton(i);
        if ( currentState & (1<<i) )
            button->setPressed();
        else
            button->setReleased();
    }

    initializeVest();
}

// -----------------------------------------------------------------------
// Clean up our resources
// -----------------------------------------------------------------------
vsVestSystem::~vsVestSystem()
{
    char   buffer[10];
    int    length;

    // reset the vest
    length = sprintf( buffer, "R*" );
    addToBuffer( buffer, length );
    
    // wait for buffer to be sent (if it can be sent)
    while ( vestState>VS_VEST_STATE_STOPPED && !isSendBufferEmpty() )
        sendVestData();
    // see if there is any more data to be read from the vest
    while ( port->isDataWaiting( 0.5 ) )
        readVestData();

    // Clean up the serial port
    delete port;
}

// -----------------------------------------------------------------------
// What object are we
// -----------------------------------------------------------------------
const char * vsVestSystem::getClassName()
{
    return "vsVestSystem";
}

// -----------------------------------------------------------------------
// Protected
// Read status information from the vest. This function will block until
// data (one character) is read!
// -----------------------------------------------------------------------
void vsVestSystem::readVestData()
{
    int readCharacter = port->readCharacter();

    if ( bytesToIgnore > 0 )
    {
        bytesToIgnore--;
        return;
    }

    switch( readCharacter )
    {
        case 'R':
            vestState = VS_VEST_STATE_RUNNING;
            break;
        case 'S':
            vestState = VS_VEST_STATE_STOPPED;
            break;
        case 'L':
            vestState = VS_VEST_STATE_LISTENING;
            break;
        case 'N':
            vestState = VS_VEST_STATE_RUNNING;
            break;
        case 'E':
            fprintf(stderr,"Vest Command Error!\n");
            break;
        default:
            break;
    }
}

// -----------------------------------------------------------------------
// Protected
// If the vest is listening, send it data
// -----------------------------------------------------------------------
void vsVestSystem::sendVestData()
{
    // if the vest isn't listening, we need to wait for it to be ready
    while ( port->isDataWaiting() )
        readVestData();

    // if the vest is ready and now listening, send it a command
    while ( bufferLength>0 && vestState == VS_VEST_STATE_LISTENING )
    {
        // actually commands are limited to 15 bytes...
        int commandLength = 0;
        while ( commandLength<bufferLength )
        {
            // Commands end with '*'. The 'B' command is all by itself
            if ( buffer[ commandLength ]=='*' || buffer[ commandLength ]=='B' )
            {
                commandLength++;
                break;
            }

            commandLength++;
        }

        // if we have a command to send, send it
        if ( commandLength > 0 )
        {
            if ( port->writePacket( buffer, commandLength ) != -1 )
            {
                // Because of the echo, we need to ignore the
                // command we just sent
                bytesToIgnore += commandLength;

                // Remove the send command from the buffer
                consumeBuffer( commandLength );
            }

            // The vest will always stop listening after a '*' is sent
            // We can't wait for the 'N' output from the vest because sometimes
            // it's too slow! This code would already try sending the next
            // command before the vest had informed us it had stopped
            // listening
            vestState = VS_VEST_STATE_RUNNING;

            // see if the vest is ready to talk to us again
            while ( port->isDataWaiting( 0.001 ) )
                readVestData();
        }
    }
}

// -----------------------------------------------------------------------
// Use this if you need to initialize the vest manually (i.e. if the vest
// wasn't attached when the constructor was called)
// -----------------------------------------------------------------------
void vsVestSystem::initializeVest()
{
    char tempBuffer[10];
    int length;

    // First, try and reset the vest back to a known state
    // if hasn't been started before, it won't respond to this reset call
    if (vestState == VS_VEST_STATE_UNKNOWN)
    {
        // send reset first
        // Start the Vest
        length = sprintf( tempBuffer, "R*" );
        port->writePacket( (unsigned char *)tempBuffer, length );

        // deal with the echo
        bytesToIgnore += length;
    
        // see if the vest is ready to talk to us
        while (port->isDataWaiting(VS_VEST_RESET_TIMEOUT))
            readVestData();
    }

    // Now send the begin command
    if (vestState == VS_VEST_STATE_UNKNOWN
            || vestState == VS_VEST_STATE_STOPPED)
    {
        // Start the Vest
        char tempBuffer[10];
        int length = sprintf( tempBuffer, "B" );
        port->writePacket( (unsigned char *)tempBuffer, length );

        // deal with the echo
        bytesToIgnore += length;
    
        // see if the vest is ready to talk to us
        while (vestState < VS_VEST_STATE_RUNNING
                && port->isDataWaiting(VS_VEST_BEGIN_TIMEOUT))
            readVestData();

        // if we haven't heard from the vest within 1 second,
        // it's probably not available
        if (vestState != VS_VEST_STATE_RUNNING
                && vestState != VS_VEST_STATE_LISTENING)
        {
            vestState = VS_VEST_STATE_NOT_AVAILABLE;
            fprintf(stderr,"vsVestSystem - can't initialize vest!\n");
        }
    }
}

// -----------------------------------------------------------------------
// update() needs to be called every frame. Will send data to the vest if
// the vest is listening (sendVestData)
// -----------------------------------------------------------------------
void vsVestSystem::update()
{
    if ( zonesChanged )
    {
        char tempBuffer[10];
        int length;

        // tell the vest about the change (for some reason, in the vest code,
        // bit off = zone on and bit on = zone off so we binary NOT the value)
        length = snprintf( tempBuffer, 10, "C%c*", ~(currentState) );
        addToBuffer( tempBuffer, length );

        zonesChanged = false;
    }

    // Run the sendVestData that will send any data in the buffer to the vest
    sendVestData();
}

// -----------------------------------------------------------------------
// How many zones are available for use?
// -----------------------------------------------------------------------
int vsVestSystem::getNumberOfZones()
{
    return VS_VEST_NUMBER_OF_ZONES;
}

// -----------------------------------------------------------------------
// Determine if the given zone is on or off
// -----------------------------------------------------------------------
bool vsVestSystem::getZoneState( int whichZone )
{
    if ( whichZone >=0 && whichZone < VS_VEST_NUMBER_OF_ZONES
            && (currentState & (1<<whichZone) ) )
        return true;
    else
        return false;
}

// -----------------------------------------------------------------------
// Turn zone *whichZone* on/off
// -----------------------------------------------------------------------
void vsVestSystem::setZoneState( int whichZone, bool newState )
{
    // make sure we have a valid zone
    if ( whichZone >= 0 && whichZone < VS_VEST_NUMBER_OF_ZONES )
    {
        // remove the bit (if it's set)
        currentState = currentState & (~((unsigned char)(1<<whichZone)));

        // add the bit (if we need too)
        if ( newState )
        {
            currentState |= (1<<whichZone);
            vest->getButton(whichZone)->setPressed();
        }
        else
            vest->getButton(whichZone)->setReleased();

        zonesChanged = true;
    }
}

// -----------------------------------------------------------------------
// Get the vest object - can be used to determine which zones are on
// by querying the buttons
// -----------------------------------------------------------------------
vsVest * vsVestSystem::getVest()
{
    return vest;
}

// -----------------------------------------------------------------------
// Is there any data in the send buffer?
// -----------------------------------------------------------------------
bool vsVestSystem::isSendBufferEmpty()
{
    return (bufferLength == 0);
}

// -----------------------------------------------------------------------
// Protected
// Adds length bytes from commands into the send buffer. It won't actually
// be sent until update() or sendVestData() is called
// -----------------------------------------------------------------------
bool vsVestSystem::addToBuffer( char * commands, int length )
{
    // add the given command to the buffer if it's running
    if ( (bufferLength + length) <= VS_VEST_BUFFER_SIZE )
    {
        memcpy( buffer + bufferLength, commands, length );
        bufferLength += length;

        return true;
    }
    else
        return false;
}

// -----------------------------------------------------------------------
// Protected
// Removes length bytes from our send buffer
// -----------------------------------------------------------------------
void vsVestSystem::consumeBuffer( int length )
{
    if ( length <= bufferLength && length >= 0 )
    {
        // adjust the length
        bufferLength -= length;

        // shift the buffer to consume what we don't need anymore
        // memmove (unlike memcpy) allows overlapping memory segments
        memmove( buffer, buffer+length, bufferLength );
    }
}

// -----------------------------------------------------------------------
// Returns the current state of the vest(see vsVestSystem.h++ for details)
// -----------------------------------------------------------------------
int vsVestSystem::getVestState( )
{
    return vestState;
}
