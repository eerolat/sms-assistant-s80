//
// ASSISTFORWARDINGDIALOG.H - header file for SMS Assistant application
// Copyright (c) 2001-2005 Tuomas Eerola
//   

#ifndef __ASSISTFORWARDINGDIALOG_H__
#define __ASSISTFORWARDINGDIALOG_H__

#ifndef __EIKDIALG_H__
#include <eikdialg.h>
#endif

class TAssistData;


class CAssistForwardingDialog : public CEikDialog
    {
public:
    CAssistForwardingDialog(TAssistData& aData);
private:
    TBool OkToExitL(TInt aButtonId);
    void PreLayoutDynInitL();
private:
    TAssistData* iData;
    };


#endif // __ASSISTFORWARDINGDIALOG_H__