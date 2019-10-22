//
// ASSISTMODESELECTDIALOG.CPP - source file for SMS Assistant application
// Copyright (c) 2001-2005 Tuomas Eerola
//   


#include <eikenv.h>
#include "assistdata.h"
#include "assistappui.h"
#include "assistmodeselectdialog.h"


CAssistModeSelectDialog::CAssistModeSelectDialog(TAssistData& aData)
    : iData(&aData)
    {
    }


void CAssistModeSelectDialog::PreLayoutDynInitL()  
    {    
    TInt choiceListItemIndex=static_cast<TInt>(iData->iApplicationMode);
    SetChoiceListCurrentItem(EAssistCommunicationsSelector,choiceListItemIndex);
    }


TBool CAssistModeSelectDialog::OkToExitL(TInt aButtonId)
    {
    if(aButtonId==EAssistBidCancel)
		return ETrue;

    if(aButtonId==EAssistBidOk)
        {
        iData->iApplicationMode=static_cast<TAssistData::TApplicationMode>(ChoiceListCurrentItem(EAssistModeSelector));

        (static_cast<CAssistAppUi*>(iEikonEnv->AppUi()))->SaveL();

        return ETrue;
        }

    return EFalse;
    }