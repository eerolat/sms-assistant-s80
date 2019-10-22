//
// ASSISTDEACTIVATIONDIALOG.CPP - source file for SMS Assistant application
// Copyright (c) 2001-2005 Tuomas Eerola
//   


#include <eikenv.h>
#include "assistdata.h"
#include "assistappui.h"
#include "assistdeactivationdialog.h"


CAssistDeactivationDialog::CAssistDeactivationDialog(TAssistData& aData,TTime& aDeactivationTime)
    : iData(&aData), iTime(&aDeactivationTime)
    {
    }


void CAssistDeactivationDialog::PreLayoutDynInitL()  
    {    
    TInt choiceListItemIndex=static_cast<TInt>(iData->iUseDeactivation);
    SetChoiceListCurrentItem(EAssistDeactivationSelector,choiceListItemIndex);
    SetTTimeEditorValue(EAssistDeactivationTime,*iTime);
    }


void CAssistDeactivationDialog::PostLayoutDynInitL()  
    {
    if(ChoiceListCurrentItem(EAssistDeactivationSelector))
        SetLineDimmedNow(EAssistDeactivationTime,EFalse);
    else
        SetLineDimmedNow(EAssistDeactivationTime,ETrue);
    }


void CAssistDeactivationDialog::HandleControlStateChangeL(TInt aControlId)
	{
    if(aControlId!=EAssistDeactivationSelector)
	    return;

    if(ChoiceListCurrentItem(EAssistDeactivationSelector))
        SetLineDimmedNow(EAssistDeactivationTime,EFalse);
    else
        SetLineDimmedNow(EAssistDeactivationTime,ETrue);
    }


TBool CAssistDeactivationDialog::OkToExitL(TInt aButtonId)
    {
    if(aButtonId==EAssistBidOk)
        {
        iData->iUseDeactivation=static_cast<TBool>(ChoiceListCurrentItem(EAssistDeactivationSelector));
        if(iData->iUseDeactivation)
            {
            TTime editorTime=TTimeEditorValue(EAssistDeactivationTime);

            TDateTime oldDateTime=iTime->DateTime();
            TDateTime newDateTime=editorTime.DateTime();

            newDateTime.SetYear(oldDateTime.Year());
            newDateTime.SetMonth(oldDateTime.Month());
            if(oldDateTime.Hour()==0)
                newDateTime.SetDay(oldDateTime.Day()-1);
            else
                newDateTime.SetDay(oldDateTime.Day());
            newDateTime.SetSecond(0);
            newDateTime.SetMicroSecond(0);

            TTime currentTime;
            currentTime.HomeTime();

            if(newDateTime.Hour()<currentTime.DateTime().Hour())
                {
                newDateTime.SetDay(oldDateTime.Day()+1);
                }
            else if(newDateTime.Hour()==currentTime.DateTime().Hour() && newDateTime.Minute()<currentTime.DateTime().Minute())
                {
                newDateTime.SetDay(oldDateTime.Day()+1);
                }
            *iTime=TTime(newDateTime);
                        }

        (static_cast<CAssistAppUi*>(iEikonEnv->AppUi()))->SaveL();

        return ETrue;
        }

    return EFalse;
    }