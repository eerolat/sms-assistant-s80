//
// ASSISTMODESELECTDIALOG.H - header file for SMS Assistant application
// Copyright (c) 2001-2005 Tuomas Eerola
//   

#ifndef __ASSISTMODESELECTDIALOG_H__
#define __ASSISTMODESELECTDIALOG_H__

#ifndef __EIKDIALG_H__
#include <eikdialg.h>
#endif

class TAssistData;


class CAssistModeSelectDialog : public CEikDialog
    {
public:
    CAssistModeSelectDialog(TAssistData& aData);
private:
    TBool OkToExitL(TInt aButtonId);
    void PreLayoutDynInitL();
private:
    TAssistData* iData;
    };


#endif // __ASSISTMODESELECTDIALOG_H__