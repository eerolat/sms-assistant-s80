//
// ASSISTFORWARDINGDIALOG.CPP - source file for SMS Assistant application
// Copyright (c) 2001-2005 Tuomas Eerola
//   


#include <eikenv.h>
#include <cknconf.h>
#include "assistdata.h"
#include "assistappui.h"
#include "assistforwardingdialog.h"
#include "assist.hrh"
#include <assist.rsg>


CAssistForwardingDialog::CAssistForwardingDialog(TAssistData& aData)
    : iData(&aData)
    {
    }


void CAssistForwardingDialog::PreLayoutDynInitL()
    {
    CEikTelephoneNumberEditor* editor=static_cast<CEikTelephoneNumberEditor*>(Control(EAssistForwardRecipient));
    if(iData->iForwardRecipient.Length())
        editor->SetNumberL(iData->iForwardRecipient);
    }


TBool CAssistForwardingDialog::OkToExitL(TInt aButtonId)
    {
    TBool returnValue=EFalse;

    if(aButtonId==EEikBidCancel)
        return ETrue;

    if(aButtonId==EEikBidOk)
        {
        TBuf<100> hackBuf;
        (static_cast<CEikTelephoneNumberEditor*>(Control(EAssistForwardRecipient)))->GetNumber(hackBuf);
        iData->iForwardRecipient=hackBuf;

        if (iData->iForwardRecipient.Length()==0)
            {
            HBufC* title=iEikonEnv->AllocReadResourceLC(R_ASSIST_NO_RECIPIENT_NOTE_TITLE);
            HBufC* message=iEikonEnv->AllocReadResourceLC(R_ASSIST_NO_RECIPIENT_NOTE_MESSAGE);
            returnValue=CCknConfirmationDialog::RunDlgLD(*title,*message);
            if(returnValue)
                iData->iActivated=EFalse;
            CleanupStack::PopAndDestroy(2); // title, message
            return returnValue;
            }

        iData->iApplicationMode=TAssistData::EForward;
        iData->iActivated=ETrue;

        (static_cast<CAssistAppUi*>(iEikonEnv->AppUi()))->SaveL();

        return ETrue;
        }

    return EFalse;
    }