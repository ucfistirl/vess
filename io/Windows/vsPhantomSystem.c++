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
//    VESS Module:  vsPhantomSystem.c++
//
//    Description:  A class for accessing and controlling the Phantom
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include <string.h>
#include <stdlib.h>
#include "vsPhantomSystem.h++"
#include "vsPhantomProtocol.h++"

vsPhantomSystem::vsPhantomSystem(char *serverName, u_short port,
                                 char *phantomName)
{
    bool connected;

    phantom = NULL;
    initialized = false;
    forces = false;
    connected = false;

    // Create a network interface and connect to the phantom server.
    netInterface = new vsTCPNetworkInterface(serverName, port);
    if (netInterface->makeConnection() == -1)
    {
        printf("vsPhantomSystem::vsPhantomSystem: Cannot Connect!\n");
        delete netInterface;
        netInterface = NULL;
        connected = false;
    }
    else
    {
        printf("Connected!\n");
        netInterface->enableBlocking();
        connected = true;
    }

    // Initialize the position scale to 1, which changes nothing.
    positionScale = 1.0;

    // Allocate the space for the receive and send buffers.
    receiveBufferLength = PS_MAX_COMMAND_LENGTH;
    receiveBuffer = (u_char *) malloc(receiveBufferLength);
    sendBufferLength = PS_MAX_COMMAND_LENGTH;
    sendBuffer = (u_char *) malloc(sendBufferLength);

    // Matrix to convert from coordinate systems, GHOST to VESS.
    gstToVsRotation = atMatrix();
    gstToVsRotation.setEulerRotation(AT_EULER_ANGLES_XYZ_R, 90.0, 0.0, 0.0);

    // Matrix to convert from coordinate systems, VESS to GHOST.
    vsToGstRotation = atMatrix();
    vsToGstRotation.setEulerRotation(AT_EULER_ANGLES_XYZ_R, -90.0, 0.0, 0.0);

    // Create the phantom object which will store all the phantom information.
    phantom = new vsPhantom();

    // Initialize its position and orientation to nothing (zero).
    phantom->setPosition(atVector(0.0, 0.0, 0.0));
    phantom->setOrientation(atVector(0.0, 0.0, 0.0), AT_EULER_ANGLES_XYZ_R);

    // If a connection was established, perform initialization for the phantom.
    if (connected)
    {
        printf("Attempting to initialize Phantom: %s\n", phantomName);

        // Initialize the phantom named by the variable phantomName.
        writeCommand(PS_PROTOCOL_VERSION, PS_COMMAND_INITIALIZE,
          (u_char) strlen(phantomName)+1, (u_char *) phantomName);

        // If we got an acknowledge packet, no errors, then it worked.
        if (readAcknowledge())
        {
            initialized = true;

            printf("Phantom: %s, initialized!\n", phantomName);

            // If the phantom needs a reset, print the message.
            if (isResetNeeded())
            {
                printf("vsPhantomSystem::vsPhantomSystem: "
                  "Phantom reset needed\n");
            }
        }
        // Else there must have been an error, so print an error message.
        else
        {
            printf("vsPhantomSystem::vsPhantomSystem: "
              "Error initializing Phantom\n");
        }
    }
}

vsPhantomSystem::~vsPhantomSystem(void)
{
    // If the phantom object was crested, delete it.
    if (phantom)
    {
        delete phantom;
    }

    // Free the other resources.
    delete netInterface;
    free(receiveBuffer);
    free(sendBuffer);
}

const char *vsPhantomSystem::getClassName(void)
{
    return("vsPhantomSystem");
}

void vsPhantomSystem::readCommand(u_char *version, u_char *command,
                                  u_short *dataLength, u_char **data)
{
    PhantomCommandHeader    *packetHeader;
    int                     packetLength;

    // Attempt to read a complete header.
    if ((packetLength = netInterface->read(receiveBuffer, PS_HEADER_LENGTH))
        == PS_HEADER_LENGTH)
    {
        // Cast the data into a pointer, so we can access it easier.        
        packetHeader = (PhantomCommandHeader *) receiveBuffer;

        // Byte swap the length to match this machines endianness.
        packetHeader->length = ntohs(packetHeader->length);

        // If the length of the incoming command is greater than the
        // size of the buffer, reallocate the buffer to insure we
        // have enough space.
        if (packetHeader->length > receiveBufferLength)
        {
            // If the reallocation was successful, update the buffer
            // length.  Add an additional character to the size so
            // we can place a null character at the end without
            // overwriting data.
            if ((receiveBuffer = (u_char *) realloc(receiveBuffer,
                packetHeader->length)) != NULL)
            {
                receiveBufferLength = packetHeader->length;
            }
            // Else print an error and do nothing.
            else
            {
                printf("vsPhantomSystem::readCommand: "
                  "Unable to allocate enough space for command!\n");
            }
        }

        // If the length specified is the length of what we have read,
        // then we are done reading a command.
        if (packetHeader->length == PS_HEADER_LENGTH)
        {
            // Store the information in the output pointers.
            *version = packetHeader->version;
            *command = packetHeader->command;
            *dataLength = 0;
            *data = NULL;
        }
        // Else if we have completed reading the header and we are not done
        // with the command, read the data.
        else if (packetHeader->length > PS_HEADER_LENGTH)
        {
            // Attempt to read the data portion of the command.
            if ((packetLength =
                netInterface->read(&receiveBuffer[PS_HEADER_LENGTH],
                (packetHeader->length - PS_HEADER_LENGTH))) > 0)
            {
                // Store the information in the output pointers.
                *version = packetHeader->version;
                *command = packetHeader->command;
                if (packetHeader->length > PS_MAX_COMMAND_LENGTH)
                {
                    *dataLength = PS_MAX_COMMAND_LENGTH - PS_HEADER_LENGTH;
                }
                else
                {
                    *dataLength = packetHeader->length - PS_HEADER_LENGTH;
                }

                *data = &receiveBuffer[PS_HEADER_LENGTH];
            }
            // Else if we read zero bytes, the connection was closed.
            else if (packetLength == 0)
            {
                printf("Connection Closed\n");

                initialized = false;
            }
        }
    }
    // Else if we read zero bytes, the connection was closed.
    else if (packetLength == 0)
    {
        printf("Connection Closed\n");

        initialized = false;
    }
    else
    {
        printf("Not enough bytes to form a valid header: %d!\n", packetLength);
    }
}

void vsPhantomSystem::writeCommand(u_char version, u_char command,
                                   u_short dataLength, u_char *data)
{
    PhantomCommandHeader    *packetHeader;
    u_short                 messageLength;
    int                     dataSent;

    // Calculate the length of this message.
    // If there is no data, then insure the message length is just the header.
    if (data == NULL)
    {
        messageLength = PS_HEADER_LENGTH;
    }
    else
    {
        messageLength = PS_HEADER_LENGTH + dataLength;
    }

    // If the length of the message is greater than our max.
    if (messageLength > PS_MAX_COMMAND_LENGTH)
    {
        // Print an error and set the message size to ignore the header.
        printf("vsPhantomSystem::writeCommand: Command exceeds the "
          "maximum size of %d\n", PS_MAX_COMMAND_LENGTH);
        messageLength = PS_HEADER_LENGTH;
    }

    // Cast the send buffer into the packet header to easily write
    // the header into the buffer.
    packetHeader = (PhantomCommandHeader *) sendBuffer;

    // Set the version of the command message.
    packetHeader->version = version;

    // Set the command of the command message.
    packetHeader->command = command;

    // Set the length of the command message.
    packetHeader->length = htons(messageLength);

    // If the length of the message is greater than the header, copy the
    // data to the send buffer.
    if (messageLength > PS_HEADER_LENGTH)
    {
        memcpy(&sendBuffer[PS_HEADER_LENGTH], data, dataLength);
    }

    // If the send did not send the entire message, continue to try sending
    // the rest.
    if ((dataSent = netInterface->write(sendBuffer, messageLength))
        != messageLength)
    {
        printf("vsPhantomSystem::writeCommand: "
          "Error sending command: %d Size: %d\n", dataSent, messageLength);
    }
}

bool vsPhantomSystem::readAcknowledge(void)
{
    u_char  version;
    u_char  command;
    u_short dataLength;
    u_char  *data;
    bool    status;

    status = false;

    // Read in a command.
    readCommand(&version, &command, &dataLength, &data);

    // If it is of a valid version, process it.
    if (version == PS_PROTOCOL_VERSION)
    {
        // If the command is an acknowledge command, set return state to true.
        if (command == PS_COMMAND_ACKNOWLEDGE)
        {
            status = true;
        }
        // If it is an error reply, print that.
        else if (command == PS_COMMAND_ERROR)
        {
            printf("vsPhantomSystem::readAcknowledge: Error Reply!\n");
        }
        // If it is an unextected reply, print that.
        else
        {
            printf("vsPhantomSystem::readAcknowledge: Unexpected Reply!\n");
        }
    }
    else
    {
        printf("vsPhantomSystem::readAcknowledge: "
          "Unrecognized Version: %d\n", version);
    }

    return(status);
}

vsPhantom *vsPhantomSystem::getPhantom(void)
{
    return(phantom);
}

void vsPhantomSystem::setScale(double newScale)
{
    positionScale = newScale;
}

double vsPhantomSystem::getScale(void)
{
    return(positionScale);
}

bool vsPhantomSystem::setForce(atVector force)
{
    double vectorValues[3];
    bool   status;

    status = true;

    // If the phantom has been initialized.
    if (initialized)
    {
        // If the force vector has a valid size of 3, use it.
        if (force.getSize() == 3)
        {
            // Rotate the vector to corespond to the GHOST coordinate system.
            force = vsToGstRotation.getVectorXform(force);

            // Convert to Big Endian, and place in array that will be sent.
            vectorValues[0] = htond(force[0]);
            vectorValues[1] = htond(force[1]);
            vectorValues[2] = htond(force[2]);      
        }
        else
        {
            printf("vsPhantomSystem::setForce: Invalid force vector\n");

            // Set the force to nothing.
            vectorValues[0] = 0.0;
            vectorValues[1] = 0.0;
            vectorValues[2] = 0.0;      

            status = false;
        }

        // Write out the request to apply the force.
        writeCommand(PS_PROTOCOL_VERSION, PS_COMMAND_APPLYFORCE,
          (sizeof(double) * 3), (u_char *) vectorValues);

        // Attempt to apply the force, if there is an error, handle it.
        if (!readAcknowledge())
        {
            disableForces();
            printf("vsPhantomSystem::setForce: Disable forces due to error\n");
            status = false;
        }
    }
    else
    {
        status = false;
    }

    return(status);
}

bool vsPhantomSystem::enableForces(void)
{
    // If the phantom has been initialized and forces are not enabled.
    if (initialized && !forces)
    {
        // Send request to enable the force.
        writeCommand(PS_PROTOCOL_VERSION, PS_COMMAND_ENABLEFORCE, 0, NULL);

        // Attempt to enable forces, if there is an error, handle it.
        if (!readAcknowledge())
        {
            printf("vsPhantomSystem::enableForces: "
              "Phantom enable force error\n");

            initialized = false;
            forces = false;
        }
        else
        {
            forces = true;
        }
    }

    return(initialized && forces);
}

bool vsPhantomSystem::disableForces(void)
{
    // If the phantom has been initialized and forces and enabled.
    if (initialized && forces)
    {
        // Attempt to disable forces, if there is an error, handle it.
        writeCommand(PS_PROTOCOL_VERSION, PS_COMMAND_DISABLEFORCE, 0, NULL);

        // If we did not receive an acknowledge, print error and disable
        // forces and the phantom.
        if (!readAcknowledge())
        {
            printf("vsPhantomSystem::disableForces: "
              "Phantom disable force error\n");

            initialized = false;
            forces = false;
        }

        forces = false;
    }

    return(initialized && !forces);
}

bool vsPhantomSystem::isForceEnabled(void)
{
    return (forces);
}

float vsPhantomSystem::getUpdateRate(void)
{
    u_char  version;
    u_char  command;
    u_short dataLength;
    u_char  *data;
    float   updateRate;

    // Set the default update rate to 0.
    updateRate = 0.0;

    // If the phantom has been initialized.
    if (initialized)
    {
        // Attempt to disable forces, if there is an error, handle it.
        writeCommand(PS_PROTOCOL_VERSION, PS_COMMAND_GETUPDATERATE, 0, NULL);
        readCommand(&version, &command, &dataLength, &data);
        if ((command == PS_COMMAND_GETUPDATERATE) &&
            (sizeof(float) == dataLength))
        {
            updateRate = ntohf(*((float *) data));
        }
        else
        {
            printf("vsPhantomSystem::getUpdateRate: Unexpected Reply!\n");
        }
    }

    // Return the update rate we now have.
    return(updateRate);
}

float vsPhantomSystem::getMaxStiffness(void)
{
    u_char  version;
    u_char  command;
    u_short dataLength;
    u_char  *data;
    float   stiffness;

    // Set the default stiffness to 0.
    stiffness = 0.0;

    // If the phantom has been initialized.
    if (initialized)
    {
        // Attempt to get the max stiffness, if there is an error, return 0.0.
        writeCommand(PS_PROTOCOL_VERSION, PS_COMMAND_GETMAXSTIFFNESS, 0, NULL);
        readCommand(&version, &command, &dataLength, &data);
        if ((command == PS_COMMAND_GETMAXSTIFFNESS) &&
            (sizeof(float) == dataLength))
        {
            stiffness = ntohf(*((float *) data));
        }
        else
        {
            printf("vsPhantomSystem::getMaxStiffness: Unexpected Reply!\n");
        }
    }

    // Return the stiffness we now have.
    return(stiffness);
}

bool vsPhantomSystem::isResetNeeded(void)
{
    u_char  version;
    u_char  command;
    u_short dataLength;
    u_char  *data;
    bool    status;

    // Set the default status to false.
    status = false;

    // If the phantom has been initialized.
    if (initialized)
    {
        // Ask to server if the Phantom needs to be reset.
        writeCommand(PS_PROTOCOL_VERSION, PS_COMMAND_ISRESETNEEDED, 0, NULL);
        readCommand(&version, &command, &dataLength, &data);
        if (command == PS_COMMAND_ISRESETNEEDED)
        {
            if (*((long *) data))
            {
                status = true;
            }
        }
        else if (command == PS_COMMAND_ERROR)
        {
            printf("vsPhantomSystem::isResetNeeded: Error Reply!\n");
        }
        else
        {
            printf("vsPhantomSystem::isResetNeeded: Unexpected Reply!\n");
        }
    }

    // Return the status (if reset is needed or not).
    return(status);
}

bool vsPhantomSystem::resetPhantom(void)
{
    // If the phantom has been initialized.
    if (initialized)
    {
        printf("Resetting Phantom to its current position.\n");

        // Set the phantom device information to reset values.
        phantom->setPosition(atVector(0.0, 0.0, 0.0));
        phantom->setOrientation(atVector(0.0, 0.0, 0.0), AT_EULER_ANGLES_XYZ_R);

        // Attempt to reset, if there is an error, handle it.
        writeCommand(PS_PROTOCOL_VERSION, PS_COMMAND_RESET, 0, NULL);
        if (!readAcknowledge())
        {
            initialized = false;
            forces = false;

            printf("vsPhantomSystem::resetPhantom: Error resetting phantom\n");
        }
    }

    return(initialized);
}

void vsPhantomSystem::update(void)
{
    u_char       version;
    u_char       command;
    u_short      dataLength;
    u_char       *data;
    PhantomState *phantomState;
    atVector     position;
    atVector     velocity;
    atMatrix     vsStylusMatrix;
    int          i;
    int          j;

    // If the phantom has been initialized.
    if (initialized)
    {
        // Request an image of the Phantom's current state.
        writeCommand(PS_PROTOCOL_VERSION, PS_COMMAND_GETSTATE, 0, NULL);
        readCommand(&version, &command, &dataLength, &data);
        if (command == PS_COMMAND_GETSTATE)
        {
            phantomState = (PhantomState *) data;

            // Set the sate of the stylus button.
            if (phantomState->switchState)
            {
                phantom->getButton(0)->setPressed();
            }
            else
            {
                phantom->getButton(0)->setReleased();
            }

            // Create the velocity vector.
            velocity = atVector(ntohd(phantomState->velocityData[0]),
              ntohd(phantomState->velocityData[1]),
              ntohd(phantomState->velocityData[2]));

            // Convert the velocity vector to VESS space.
            velocity = gstToVsRotation.getVectorXform(velocity);

            // Get the position and orientation of the phantom stylus.
            // Create a VESS matrix from the GHOST data.  As we transfer
            // the values also transpose it, since GHOST matricies are in the
            // transpose order of VESS matricies.
            for (i = 0; i < 4; i++)
            {
                for (j = 0; j < 4; j++)
                {
                    vsStylusMatrix.setValue(j, i,
                      ntohd(phantomState->matrixData[4*i+j]));
                }
            }

            // Create a matrix which will methematically convert the VESS
            // point into GHOST space, transform it using the GHOST matrix,
            // and then convert it back to VESS space.
            vsStylusMatrix = gstToVsRotation * vsStylusMatrix * vsToGstRotation;

            // Get the translation values from the matrix and use them as
            // position.  This can be done since they are a translation from
            // the origin.
            position.setSize(3);
            position[AT_X] = vsStylusMatrix.getValue(0,3);
            position[AT_Y] = vsStylusMatrix.getValue(1,3);
            position[AT_Z] = vsStylusMatrix.getValue(2,3);

            // Set the phantom position values to the VESS device.
            // Scale it according to the set position scale.
            phantom->setPosition(position * positionScale);

            // Set the phantom velocity values to the VESS device.
            phantom->setVelocity(velocity * positionScale);

            // Set the phantom orientation to the VESS device.
            phantom->setOrientation(vsStylusMatrix);
        }
        else
        {
            printf("vsPhantomSystem::update: Error updating phantom\n");
        }
    }
}

