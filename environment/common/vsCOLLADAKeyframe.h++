#ifndef VS_COLLADA_KEYFRAME_HPP
#define VS_COLLADA_KEYFRAME_HPP

#include "vsObject.h++"

class VESS_SYM vsCOLLADAKeyframe : public vsObject
{
protected:

    double    time;
    double    data[16];

public:

                          vsCOLLADAKeyframe(double t, int count, double *d);
                          vsCOLLADAKeyframe();
    virtual               ~vsCOLLADAKeyframe();

    virtual const char    *getClassName();

    double                getTime();
    void                  setTime(double newTime);

    double                getData(int index);
    void                  setData(int count, double *values);
    void                  setData(int index, double value);
};

#endif
