//
// ASSISTDOCUMENT.H - header file for SMS Assistant application
// Copyright (c) 2001-2005 Tuomas Eerola
//  

#ifndef __ASSISTDOCUMENT_H__
#define __ASSISTDOCUMENT_H__

#ifndef __EIKDOC_H__
#include <eikdoc.h>
#endif

class CEikAppUi;

class CAssistDocument : public CEikDocument
    {
    public: // Constructors and destructor.
        static CAssistDocument* NewL(CEikApplication& aApp);
    private:
        CAssistDocument(CEikApplication& aApp);
    private: // from CEikDocument
        CEikAppUi* CreateAppUiL();
    };

#endif // __ASSISTDOCUMENT_H__

