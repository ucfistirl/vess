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
//    VESS Module:  vsScentManager.h++
//
//    Description:  Singleton class to watch over all olfactory 
//                  operations.  Adjusts the strength of all available
//                  scents to match the current source/detector positions.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SCENT_MANAGER_HPP
#define VS_SCENT_MANAGER_HPP

#include "vsObject.h++"
#include "vsScent.h++"
#include "vsScentSourceAttribute.h++"
#include "vsScentDetectorAttribute.h++"

#define VS_SM_MAX_SCENTS 20

class vsScentManager : public vsObject
{
protected:

    static vsScentManager       *instance;

    vsScent                     *scents[VS_SM_MAX_SCENTS];
    double                      scentStrengths[VS_SM_MAX_SCENTS];
    int                         numScents;

    vsScentSourceAttribute      *scentSources[VS_SM_MAX_SCENTS];
    int                         numScentSources;

    vsScentDetectorAttribute    *scentDetector;

                                vsScentManager(); 

    int                         getScentIndex(vsScent *scent);

VS_INTERNAL:

    virtual     ~vsScentManager();

    void        addScent(vsScent *scent);
    void        removeScent(vsScent *scent);
    void        addScentSource(vsScentSourceAttribute *attr);
    void        removeScentSource(vsScentSourceAttribute *attr);
    void        setScentDetector(vsScentDetectorAttribute *attr);
    void        removeScentDetector(vsScentDetectorAttribute *attr);

public:

    virtual const char       *getClassName();

    static vsScentManager    *getInstance();

    void                     update();
}; 
#endif
