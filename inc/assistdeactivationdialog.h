//
// ASSISTDEACTIVATIONDIALOG.H - header file for SMS Assistant application
// Copyright (c) 2001-2005 Tuomas Eerola
//   

#ifndef __ASSISTDEACTIVATIONDIALOG_H__
#define __ASSISTDEACTIVATIONDIALOG_H__

#ifndef __EIKDIALG_H__
#include <eikdialg.h>
#endif

class TAssistData;
class TTime;

class CAssistDeactivationDialog : public CEikDialog
    {
public:
    CAssistDeactivationDialog(TAssistData& aData,TTime& aDeactivationTime);
protected:
    void HandleControlStateChangeL(TInt aControlId);
private:
    TBool OkToExitL(TInt aButtonId);
    void PreLayoutDynInitL();
    void PostLayoutDynInitL();
private:
    TAssistData* iData;
    TTime* iTime;
    };


#endif // __ASSISTDEACTIVATIONDIALOG_H__