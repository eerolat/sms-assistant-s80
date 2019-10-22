//
// ASSISTAPPVIEW.CPP - source file for SMS Assistant application
// Copyright (c) 2001-2005 Tuomas Eerola
//  


#include <gulicon.h>
#include <eikenv.h>
#include <eikapp.h>
#include <eikappui.h>
#include <ckntitle.h>
#include "assistappview.h"
#include "assistdata.h"
#include "assist.hrh"
#include "assist.mbg"
#include <assist.rsg>


KIconFile


CAssistAppView* CAssistAppView::NewL(const TRect& aRect, TAssistData& aData)
    {
    CAssistAppView* self=NewLC(aRect, aData);
    CleanupStack::Pop(); // self
    return self;
    }


CAssistAppView* CAssistAppView::NewLC(const TRect& aRect, TAssistData& aData)
    {
    CAssistAppView* self = new(ELeave) CAssistAppView(aData);
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    return self;
    }


CAssistAppView::CAssistAppView(TAssistData& aData)
    : iData(&aData)
    {
    }


CAssistAppView::~CAssistAppView()
    {
    delete iTitle;
    delete iWallpaper;
    delete iMessage;
    delete iReplyIcon;
    delete iForwardIcon;
    delete iReplyInactiveIcon;
    delete iForwardInactiveIcon;
    }


void CAssistAppView::ConstructL(const TRect& aRect)
    {
    CreateWindowL();

    TFileName iconFileName;
    TFileName appFileName=CEikonEnv::Static()->EikAppUi()->Application()->AppFullName();
    TParse parse;
    parse.Set(appFileName,NULL,NULL);
    iconFileName=parse.Drive();
    iconFileName+=parse.Path();
    iconFileName+=KIconFileName;

    iReplyIcon=iEikonEnv->CreateIconL(iconFileName,EMbmAssistReply,EMbmAssistReplym);
    iForwardIcon=iEikonEnv->CreateIconL(iconFileName,EMbmAssistForward,EMbmAssistForwardm);
    iReplyInactiveIcon=iEikonEnv->CreateIconL(iconFileName,EMbmAssistReplyin,EMbmAssistReplyinm);
    iForwardInactiveIcon=iEikonEnv->CreateIconL(iconFileName,EMbmAssistForwardin,EMbmAssistForwardinm);

    HBufC* buf=iEikonEnv->AllocReadResourceLC(R_ASSIST_TITLE);
    const TSize maxIconSize=TSize(20,20);
    iTitle=CCknAppTitle::NewL(KNullDesC,KNullDesC,KNullDesC,KNullDesC,CCknAppTitle::EWindow,maxIconSize);
    iTitle->SetTextL(*buf,CCknAppTitle::EMainTitle);
    CleanupStack::PopAndDestroy(); // buf
    iTitle->InsertStatusIconL(iReplyIcon);

    iWallpaper=iEikonEnv->CreateBitmapL(iconFileName,EMbmAssistWallpaper);

    iMessage=new(ELeave)CEikLabel();

    UpdateApplicationViewL();

    SetRect(aRect);

    iTitle->SetFocus(ETrue);
	ActivateL();
    }


void CAssistAppView::SizeChanged()
    {
    TRect rect(Rect());
    TSize messageSize=iMessage->MinimumSize();
    TSize titleSize=iTitle->MinimumSize();

    iTitle->SetRect(TRect(rect.iTl,TPoint(rect.iBr.iX,rect.iTl.iY+titleSize.iHeight)));
    
    rect.iTl.iY+=titleSize.iHeight;
    rect.iTl.iY+=(rect.Height()-messageSize.iHeight)/2;
    rect.iTl.iX+=(rect.Width()-messageSize.iWidth)/2;
    rect.iBr=rect.iTl+messageSize;
    
    iMessage->SetRect(rect);
    }


TInt CAssistAppView::CountComponentControls() const
    {
    return 2; // iTitle, iMessage
    }


CCoeControl* CAssistAppView::ComponentControl(TInt aIndex) const
    {
    if(aIndex==0)
        return iTitle;
    if(aIndex==1)
        return iMessage;
    return NULL;
    }

void CAssistAppView::Draw(const TRect& /*aRect*/) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle(CGraphicsContext::ENullPen);

    gc.UseBrushPattern(iWallpaper);
    gc.SetBrushStyle(CGraphicsContext::EPatternedBrush);
    gc.SetBrushOrigin(Rect().iTl);
    gc.DrawRect(Rect());
    gc.DiscardBrushPattern();
    }

void CAssistAppView::UpdateApplicationViewL()
    {
    HBufC* instructionsBuf=NULL;

    if(iData->iApplicationMode==TAssistData::EReply)
        {
        iTitle->DeleteStatusIcon(0);

        if(iData->iActivated)
            iTitle->InsertStatusIconL(iReplyIcon);
        else
            iTitle->InsertStatusIconL(iReplyInactiveIcon);

        instructionsBuf=iEikonEnv->AllocReadResourceLC(R_ASSIST_MESSAGE_REPLY);
        }
    else
        {
        iTitle->DeleteStatusIcon(0);

        if(iData->iActivated)
            iTitle->InsertStatusIconL(iForwardIcon);
        else
            iTitle->InsertStatusIconL(iForwardInactiveIcon);

        instructionsBuf=iEikonEnv->AllocReadResourceLC(R_ASSIST_MESSAGE_FORWARD);
        }

    iMessage->SetTextL(*instructionsBuf);
    CleanupStack::PopAndDestroy(); // instructionsBuf
    }