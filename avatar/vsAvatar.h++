// File vsAvatar.h++

#ifndef VS_AVATAR_HPP
#define VS_AVATAR_HPP

#include <stdio.h>
#include "vsSystem.h++"
#include "vsGrowableArray.h++"
#include "vsNode.h++"
#include "vsOptimizer.h++"

#define VS_AVATAR_LOCAL_ISECT_MASK 0x01000000

class vsAvatar
{
protected:

    FILE            *cfgFile;

    int             isInitted;
    
    vsComponent     *geometryRoot;

    virtual int     readCfgLine(char *buffer);
    
    virtual void    *createObject(char *idString);

    virtual void    setup(vsGrowableArray *objArray,
                          vsGrowableArray *strArray, int objCount) = 0;

    void            *makeGeometry();
    void            *makeVsISTJoystickBox();
    void            *makeVsUnwinder();
    void            *makeVsFlockOfBirds();
    void            *makeVsSerialMotionStar();
    void            *makeVsFastrak();
    void            *makeVsIS600();
    void            *makeVsEthernetMotionStar();

public:

                    vsAvatar();
    virtual         ~vsAvatar();

    virtual void    init(char *configFile);
    
    virtual void    update() = 0;
    
    vsNode          *getGeometry();
};

#endif
