
#ifndef VS_COLLADA_ANIMATION_HPP
#define VS_COLLADA_ANIMATION_HPP

#include "vsObject.h++"
#include "vsCOLLADADataSource.h++"
#include "vsCOLLADAChannel.h++"
#include "vsPathMotion.h++"

#include "atString.h++"
#include "atMap.h++"
#include "atXMLDocument.h++"

class VESS_SYM vsCOLLADAAnimation : public vsObject
{
protected:

    atString                         animationID;
    atMap                            *sources;
    atMap                            *samplers;
    atList                           *channels;
    atList                           *children;


                           vsCOLLADAAnimation(atString id, atXMLDocument *doc,
                                              atXMLDocumentNodePtr current,
                                              atMap *parentSources);

    vsCOLLADADataSource    *getDataSource(atString id);

    void                   processSource(atXMLDocument *doc,
                                         atXMLDocumentNodePtr current);

public:

                          vsCOLLADAAnimation(atString id, atXMLDocument *doc,
                                             atXMLDocumentNodePtr current);
                          ~vsCOLLADAAnimation();

    virtual const char    *getClassName();

    atString              getID();

    int                   getNumChannels();
    vsCOLLADAChannel      *getChannel(int index);

    int                   getNumChildren();
    vsCOLLADAAnimation    *getChild(int index);
};

#endif 
