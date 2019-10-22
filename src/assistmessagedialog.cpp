//
// ASSISTMESSAGEDIALOG.CPP - source file for SMS Assistant application
// Copyright (c) 2001-2005 Tuomas Eerola
//   


#include <eikenv.h>
#include <cknconf.h>
#include "assistdata.h"
#include "assistappui.h"
#include "assistmessagedialog.h"
#include <assist.rsg>


CAssistMessageDialog::CAssistMessageDialog(TAssistData& aData)
    : iData(&aData)
    {
    }


void CAssistMessageDialog::PreLayoutDynInitL()  
    {    
    TInt choiceListItemIndex=static_cast<TInt>(iData->iReplyTo);
    SetChoiceListCurrentItem(EAssistCommunicationsSelector,choiceListItemIndex);
#pragma message ("TE: Possible problem in header parsing.")
    //HBufC* header=NULL;
    //header=iEikonEnv->AllocReadResourceLC(R_ASSIST_MESSAGE_HEADER);

    //TInt search=iData->iMessage.Find(*header);
    //if(search==0)
    //    {
    //    TInt headerLength=header->Length();
    //    iData->iMessage=iData->iMessage.Mid(headerLength);
    //    }

    SetEdwinTextL(EAssistMessageEditor, &(iData->iMessage));

    //CleanupStack::PopAndDestroy(); // header
    }


TBool CAssistMessageDialog::OkToExitL(TInt aButtonId)
    {
    _LIT(KEmptyMessage,"");

    //HBufC* header=NULL;
    //header=iEikonEnv->AllocReadResourceLC(R_ASSIST_MESSAGE_HEADER);

    if(aButtonId==EAssistBidCancel)
        {
        if(iData->iMessage==KEmptyMessage)
            {
            TBool returnValue;
            HBufC* title=iEikonEnv->AllocReadResourceLC(R_ASSIST_NO_MESSAGE_NOTE_TITLE);
            HBufC* message=iEikonEnv->AllocReadResourceLC(R_ASSIST_NO_MESSAGE_NOTE_MESSAGE);
            returnValue=CCknConfirmationDialog::RunDlgLD(*title,*message);
            //CleanupStack::PopAndDestroy(3); // title, message, header
            CleanupStack::PopAndDestroy(2); // title, message
            return returnValue;
            }
        //CleanupStack::PopAndDestroy(); // header
		return ETrue;
        }
    if(aButtonId==EAssistBidOk)
        {
        iData->iReplyTo=static_cast<TCommunicationsChannel>(ChoiceListCurrentItem(EAssistCommunicationsSelector));

        HBufC* message=NULL;

        message=(static_cast<CEikEdwin*>(Control(EAssistMessageEditor)))->GetTextInHBufL();

        CleanupStack::PushL(message);

        if(!message)
            {
            TBool returnValue;
            HBufC* title=iEikonEnv->AllocReadResourceLC(R_ASSIST_NO_MESSAGE_NOTE_TITLE);
            HBufC* message=iEikonEnv->AllocReadResourceLC(R_ASSIST_NO_MESSAGE_NOTE_MESSAGE);
            returnValue=CCknConfirmationDialog::RunDlgLD(*title,*message);
            CleanupStack::PopAndDestroy(2); // title, message
            if(returnValue)
                {
                iData->iMessage=KEmptyMessage;
                (static_cast<CAssistAppUi*>(iEikonEnv->AppUi()))->SaveL();
                }
            //CleanupStack::PopAndDestroy(2); // message, header
            CleanupStack::PopAndDestroy(); // message
            return returnValue;
            }


        HBufC* header=NULL;
        header=iEikonEnv->AllocReadResourceLC(R_ASSIST_MESSAGE_HEADER);
        TInt headerLength=header->Length();
        TInt messageLength=message->Length();
    
        if(messageLength+headerLength>KAssistMaxMessageLength)
            {
            HBufC* info=iEikonEnv->AllocReadResourceLC(R_ASSIST_MESSAGE_TOO_LONG);
            iEikonEnv->InfoMsg(*info);
            CleanupStack::PopAndDestroy(3); // info, header, message
            return EFalse;
            }

        //iData->iMessage=header->Des();
        //iData->iMessage.Append(message->Des());
        iData->iMessage=message->Des();

        CleanupStack::PopAndDestroy(2); // header, message

        (static_cast<CAssistAppUi*>(iEikonEnv->AppUi()))->SaveL();

        return ETrue;
        }
    else
        return EFalse;
    }
