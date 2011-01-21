#ifndef VS_COLLADA_LOADER_HPP
#define VS_COLLADA_LOADER_HPP

#include "atXMLDocument.h++"
#include "atList.h++"
#include "atMap.h++"
#include "atString.h++"
#include "atStringTokenizer.h++"
#include "vsComponent.h++"
#include "vsSkeleton.h++"
#include "vsSkeletonKinematics.h++"
#include "vsPathMotionManager.h++"
#include "vsList.h++"
#include "vsCharacter.h++"
#include "vsCOLLADADocument.h++"

class VESS_SYM vsCOLLADALoader : public vsObject
{
protected:

    atList              *pathList;

    atString            documentPath;

    vsCOLLADADocument   *mainDocument;

    atString               findFile(const char *filename);


public:

                          vsCOLLADALoader();
    virtual               ~vsCOLLADALoader();

    virtual const char    *getClassName();

    void                  addPath(const char *path);
    void                  clearPath();

    void                  parseFile(const char *filename);

    vsComponent           *getScene();

    vsCharacter           *getCharacter();
};


#endif
