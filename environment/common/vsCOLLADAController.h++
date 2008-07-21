#ifndef VS_COLLADA_CONTROLLER_HPP
#define VS_COLLADA_CONTROLLER_HPP

#include "atMap.h++"
#include "vsObject.h++"
#include "vsCOLLADAGeometry.h++"

class VESS_SYM vsCOLLADAController : public vsObject
{
protected:

    vsCOLLADAGeometry    *sourceGeometry;
    atMap                *dataSources;

public:

                           vsCOLLADAController(vsCOLLADAGeometry *source);
                           ~vsCOLLADAController();

    virtual vsComponent    *instance() = 0;
};

#endif
