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
//    VESS Module:  vsMenuLabel.h++
//
//    Description:  This is a menu object that is represented by a text
//                  component. It takes a pointer to a text builder
//                  object so it can create and modify its text.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_MENU_LABEL_HPP
#define VS_MENU_LABEL_HPP

#include "vsComponent.h++"
#include "vsGlobals.h++"
#include "vsKinematics.h++"
#include "vsMenuObject.h++"
#include "vsTextBuilder.h++"
#include "atVector.h++"

class VS_MENU_DLL vsMenuLabel : public vsMenuObject
{
private:

    char             *labelText;
    vsTextBuilder    *textBuilder;

    vsComponent      *textComponent;

VS_INTERNAL:

public:

                          vsMenuLabel(vsTextBuilder *newTextBuilder, char *text);
    virtual               ~vsMenuLabel();

    virtual const char    *getClassName();

    virtual void          update(vsMenuSignal signal, vsMenuFrame *frame);

    void                  setTextBuilder(vsTextBuilder *newTextBuilder);
    vsTextBuilder         *getTextBuilder();

    void                  setText(char *text);
    char                  *getText();
};

#endif
