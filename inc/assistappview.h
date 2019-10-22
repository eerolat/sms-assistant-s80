//
// ASSISTAPPVIEW.H - header file for SMS Assistant application
// Copyright (c) 2001-2005 Tuomas Eerola
//   

#ifndef __ASSISTAPPVIEW_H__
#define __ASSISTAPPVIEW_H__

#if !defined(__COECNTRL_H__)
#include <coecntrl.h>
#endif


class CCknAppTitle;
class CEikLabel;
class TAssistData;
class CGulIcon;


class CAssistAppView : public CCoeControl
    {
public:
    static CAssistAppView* NewL(const TRect& aRect, TAssistData& aData);
    static CAssistAppView* NewLC(const TRect& aRect, TAssistData& aData);
    ~CAssistAppView();
    void UpdateApplicationViewL();
protected:
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
private:
    CAssistAppView(TAssistData& aData);
    void ConstructL(const TRect& aRect);
    void Draw(const TRect& aRect) const;
private:
    CCknAppTitle* iTitle;
    CFbsBitmap* iWallpaper;
    CEikLabel* iMessage;
    TAssistData* iData;
    CGulIcon* iReplyIcon;
    CGulIcon* iForwardIcon;
    CGulIcon* iReplyInactiveIcon;
    CGulIcon* iForwardInactiveIcon;
    };


#endif // __ASSISTAPPVIEW_H__