//
// ASSISTAPP.H - header file for SMS Assistant application
// Copyright (c) 2001-2003 Tuomas Eerola
//  


#ifndef __ASSISTAPP_H__
#define __ASSISTAPP_H__

#ifndef __EIKAPP_H__
#include <eikapp.h>
#endif


// UID of the application
const TUid KUidAssist={ 0x101F3CB6 };


class CAssistApp : public CEikApplication
    {
public:
protected:
private:
    CApaDocument* CreateDocumentL();
    TUid AppDllUid() const;
    };


#endif // __ASSISTAPP_H__


