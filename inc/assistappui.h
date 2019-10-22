//
// ASSISTAPPUI.H - header file for SMS Assistant application
// Copyright (c) 2001-2005 Tuomas Eerola
//  


#ifndef __ASSISTAPPUI_H__
#define __ASSISTAPPUI_H__

#if !defined(__EIKAPPUI_H__)
#include <eikappui.h>
#endif

#if !defined(__MSVAPI_H__)
#include <msvapi.h>
#endif

class CClientMtmRegistry;
class CMsvSession;
class CBaseMtm;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CAssistAppView;
class CAssistLogMonitor;
class CAssistDeactivationTimer;
class TAssistData;


class CAssistAppUi : public CEikAppUi, public MMsvSessionObserver
    {
public:
    CAssistAppUi();
    void ConstructL();
    ~CAssistAppUi();
    void ExternalizeL(RWriteStream& aOut) const;
    void InternalizeL(RReadStream& aIn);
    void SaveL();
    TStreamId StoreL( CStreamStore& aStore) const;
    void RestoreL(const CStreamStore& aStore, TStreamId aId);
    void HandleIncomingVoiceCallL(TDesC& aCallingNumber);
    void SetDeactivationTimerL(TTime aLaunchTime);
    void ConstructAndStartLogMonitorL();
    void StopAndDeleteLogMonitor();
    void ResetLogL();
    TBool ReplyToVoice() const;
    TBool ReplyToSMS() const;
    TBool ReplyToMMS() const;
    void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
protected:
private:
    void CompleteConstructL();
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
    TMsvId CreateNewMessageL();
    void SetEntryL(TMsvId aEntryId);
    TBool SendSMSReplyMessageL();
    TMsvId MoveMessageEntryL(TMsvId aTarget);
    void SetScheduledSendingStateL(CMsvEntrySelection* aSelection);
    TBool DeleteSentEntry(TMsvId aEntryId);
    void HandleCommandL(TInt aCommand);
    TBool LaunchExitNoteL();
    void ConfigureApplicationL();
    void AboutApplicationL();
    void ConfigureDeactivationL();
    void SendApplicationToBackgroundL();
    void SaveIniFileL();
    void LoadIniFileL();
    TBool IsNumberInLogL(TDesC* aNumber) const;
    void AddNumberToLogL(TDesC* aNumber);
    void MessageReceivedL(TMsvId aEntryId);
    void ConfigureMessagingL();
    void ConfigureForwardingL();
    TBool LaunchStartupNoteL();
    void LaunchModeSelectDialogL();
    TBool DoSMSForwardL(TMsvId aEntryId);
    void ExitL();
private:
    TMsvId iMsvId;
    CMsvSession* iSession;
    CBaseMtm* iMtm;
    CClientMtmRegistry* iMtmReg;
    CAssistAppView* iAppView;
    CAssistLogMonitor* iLogMonitor;
    CAssistDeactivationTimer* iDeactivationTimer;
    TAssistData* iData;
    TTime iDeactivationTime;
    };

#endif // __ASSISTAPPUI_H__
