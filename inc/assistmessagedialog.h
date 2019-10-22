//
// ASSISTMESSAGEDIALOG.H - header file for SMS Assistant application
// Copyright (c) 2001-2005 Tuomas Eerola
//   

#ifndef __ASSISTMESSAGEDIALOG_H__
#define __ASSISTMESSAGEDIALOG_H__

#ifndef __EIKDIALG_H__
#include <eikdialg.h>
#endif

class TAssistData;


class CAssistMessageDialog : public CEikDialog
    {
public:
    CAssistMessageDialog(TAssistData& aData);
private:
    TBool OkToExitL(TInt aButtonId);
    void PreLayoutDynInitL();
private:
    TAssistData* iData;
    };


#endif // __ASSISTMESSAGEDIALOG_H__