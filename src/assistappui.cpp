//
// ASSISTAPPUI.CPP - source file for SMS Assistant application
// Copyright (c) 2001-2005 Tuomas Eerola
//

#include <barsread.h>
#include <txtrich.h>
#include <smscmds.h>
#include <mtclbase.h>
#include <msvids.h>
#include <smut.h>
#include <msvuids.h>
#include <smsclnt.h>
#include <mmsclient.h>
#include <mtclreg.h>
#include <smutset.h>
#include <smuthdr.h>
#include <eikenv.h>
#include <eikbtgpc.h>
#include <eikmenup.h>
#include <ckninfo.h>
#include <cknconf.h>

#include "assistapp.h"
#include "assistappui.h"
#include "assistappview.h"
#include "assistmessagedialog.h"
#include "assistforwardingdialog.h"
#include "assistdeactivationdialog.h"
#include "assistmodeselectdialog.h"
#include "assistdata.h"
#include "assistlogmonitor.h"
#include "assistdeactivationtimer.h"
#include "assist.hrh"
#include "assist.mbg"
#include <assist.rsg>

#include <eikon.hrh>


_LIT(KIniFileName,"assist.ini");
_LIT(KLogFileName,"assist.log");
KIconFile


CAssistAppUi::CAssistAppUi()
    : iDeactivationTime(0)
    {
    }


void CAssistAppUi::ConstructL()
    {
    BaseConstructL();

    iData=TAssistData::NewL();
    LoadIniFileL();

    CEikButtonGroupContainer* buttonGroup=CEikonEnv::Static()->AppUiFactory()->ToolBar();
    buttonGroup->SetCommandL(0,R_ASSIST_BUTTON_ACTIVATE);

    iAppView=CAssistAppView::NewL(ClientRect(), *iData); 

    iMsvId=NULL;
    iLogMonitor=NULL;

    iSession=CMsvSession::OpenAsyncL(*this);

    if(iData->iAskForSendingPermission)
        {
        if(LaunchStartupNoteL())
            {
            iData->iAskForSendingPermission=EFalse;
            SaveL();
            }
        else
            Exit();
        }
    }


void CAssistAppUi::ConstructAndStartLogMonitorL()
    {
    iLogMonitor=new(ELeave)CAssistLogMonitor();
    iLogMonitor->ConstructL();
    iLogMonitor->Start();
    }


void CAssistAppUi::StopAndDeleteLogMonitor()
    {
    if(iLogMonitor)
        {
        iLogMonitor->Cancel();
        delete iLogMonitor;
        iLogMonitor=NULL;
        }   
    }


void CAssistAppUi::ExternalizeL(RWriteStream& aOut) const
    {
    const TChar KContentDelimiter(0x3);

    aOut.WriteInt32L(KIniFileVersion);

    HBufC* content=HBufC::NewLC(iData->iMessage.Length()+1);
    content->Des()=iData->iMessage;
    (content->Des()).Append(KContentDelimiter);
    aOut.WriteL(*content);
    CleanupStack::PopAndDestroy(); // content

    aOut.WriteInt32L(static_cast<TInt>(iData->iUseDeactivation));
    aOut.WriteInt32L(static_cast<TInt>(iData->iUseHeader));
    aOut.WriteInt32L(static_cast<TInt>(iData->iReplyTo));
    aOut.WriteInt32L(static_cast<TInt>(iData->iAskForSendingPermission));
    aOut.WriteInt32L(static_cast<TInt>(iData->iApplicationMode));

    content=HBufC::NewLC(iData->iForwardRecipient.Length()+1);
    content->Des()=iData->iForwardRecipient;
    (content->Des()).Append(KContentDelimiter);
    aOut.WriteL(*content);
    CleanupStack::PopAndDestroy(); // content
    }


void CAssistAppUi::InternalizeL(RReadStream& aIn)
    {
    const TChar KContentDelimiter(0x3);

    TInt iniFileVersion=aIn.ReadInt32L();
    if(iniFileVersion!=KIniFileVersion)
        return;

    TBuf<KAssistMaxMessageLength> msgBuf;
    aIn.ReadL(msgBuf,KContentDelimiter);
    TInt length=msgBuf.Length();
    iData->iMessage=msgBuf.Left(length-1);

    iData->iUseDeactivation=static_cast<TBool>(aIn.ReadInt32L());
    iData->iUseHeader=static_cast<TBool>(aIn.ReadInt32L());
    iData->iReplyTo=static_cast<TInt>(aIn.ReadInt32L());
    iData->iAskForSendingPermission=static_cast<TBool>(aIn.ReadInt32L());
    iData->iApplicationMode=static_cast<TAssistData::TApplicationMode>(aIn.ReadInt32L());

    TBuf<KMaxTelephoneNumberLength> numBuf;
    aIn.ReadL(numBuf,KContentDelimiter);
    length=numBuf.Length();
    iData->iForwardRecipient=numBuf.Left(length-1);
    }


void CAssistAppUi::RestoreL(const CStreamStore& aStore, TStreamId aId)
    {
    RStoreReadStream inStream;
    inStream.OpenLC(aStore,aId);
    InternalizeL(inStream);
    CleanupStack::PopAndDestroy(); // inStream
    }


TStreamId CAssistAppUi::StoreL(CStreamStore& aStore) const
    {
    RStoreWriteStream outStream;
    TStreamId id=outStream.CreateLC(aStore);
    ExternalizeL(outStream);
    outStream.CommitL();
    CleanupStack::PopAndDestroy();  // outStream
    return id;
    }


void CAssistAppUi::SaveL()
    {
    SaveIniFileL();
    }


void CAssistAppUi::LoadIniFileL()
    {
    TFileName iniFileName;
    TFileName applicationName=Application()->AppFullName();
    TParse parse;
    parse.Set(applicationName,NULL,NULL);

    iniFileName=parse.Drive();
    iniFileName+=parse.Path();
#if defined(__WINS__)
    iniFileName=_L("c:\\");
#endif
    iniFileName+=KIniFileName;

    RFs fs;
    fs.Connect();
    CFileStore* store=NULL;
    TRAPD(err,store=CDirectFileStore::OpenL(fs,iniFileName,EFileShareReadersOnly));
    if (err==KErrNone)
        {
        CleanupStack::PushL(store);
        RStoreReadStream inStream;
        inStream.OpenLC(*store,store->Root());
        InternalizeL(inStream);
        CleanupStack::PopAndDestroy(2); // inStream + store
        }
    fs.Close();
    }


void CAssistAppUi::SaveIniFileL()
    {
    RFs fs;
    fs.Connect();

    TFileName iniFileName;
    TFileName applicationName=Application()->AppFullName();
    TParse parse;
    parse.Set(applicationName,NULL,NULL);

    iniFileName=parse.Drive();
    iniFileName+=parse.Path();
#if defined(__WINS__)
    iniFileName=_L("c:\\");
#endif
    iniFileName+=KIniFileName;

    CFileStore* store=NULL;;

    TRAPD(replaceErrorCode,store=CDirectFileStore::ReplaceL(fs,iniFileName,EFileShareAny|EFileWrite));

	if (replaceErrorCode!=KErrNone)
        {
        store=CDirectFileStore::CreateL(fs,iniFileName,EFileShareAny|EFileWrite);
        }

    CleanupStack::PushL(store);
    store->SetTypeL(KDirectFileStoreLayoutUid);
    RStoreWriteStream outStream;
    TStreamId id=outStream.CreateLC(*store);
    ExternalizeL(outStream);
    outStream.CommitL();
    store->SetRootL(id);
    store->CommitL();
    CleanupStack::PopAndDestroy(2); // outStream + store

    fs.Close();
    }


CAssistAppUi::~CAssistAppUi()
    {
    if(iDeactivationTimer)
        iDeactivationTimer->Cancel();
    delete iDeactivationTimer;
    iDeactivationTimer=NULL;

    StopAndDeleteLogMonitor();

    delete iMtm;

    delete iMtmReg;
    iMsvId = NULL;
    delete iSession;

    delete iData;

    delete iAppView;
    }


void CAssistAppUi::SetDeactivationTimerL(TTime aLaunchTime)
    {
    iDeactivationTime=aLaunchTime;

    if(iDeactivationTimer)
        iDeactivationTimer->Cancel();
    delete iDeactivationTimer;
    iDeactivationTimer=NULL;

    iDeactivationTimer=CAssistDeactivationTimer::NewL(*this);
    iDeactivationTimer->At(aLaunchTime);
    }


void CAssistAppUi::CompleteConstructL()
    {
    iMtmReg=CClientMtmRegistry::NewL(*iSession);
    }


void CAssistAppUi::HandleIncomingVoiceCallL(TDesC& aCallingNumber)
    {
    if(!iData->iActivated)
        return;
    if(aCallingNumber.Length()==0)
        return;

    iData->iRecipient=aCallingNumber;

    SendSMSReplyMessageL();
    }


void CAssistAppUi::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* /*aArg3*/)
    {
    if(aEvent==EMsvServerReady)
        CompleteConstructL();

    if((iData->iApplicationMode==TAssistData::EForward || (iData->iApplicationMode==TAssistData::EReply && ReplyToMMS()))
       && aEvent==EMsvEntriesCreated)
        {
        TMsvId* entryId;
        entryId = static_cast<TMsvId*>(aArg2);

        if (*entryId==KMsvGlobalInBoxIndexEntryId)
            {
            CMsvEntrySelection* entries = static_cast<CMsvEntrySelection*>(aArg1);

            for(TInt i = 0; i < entries->Count(); i++)
                {
                MessageReceivedL(entries->At(i));
                }
            }
        }
    }


TMsvId CAssistAppUi::CreateNewMessageL()
	{
    TMsvEntry newEntry;
    newEntry.iMtm = KUidMsgTypeSMS;
    newEntry.iType = KUidMsvMessageEntry;
    newEntry.iServiceId = KMsvLocalServiceIndexEntryId;
    newEntry.iDate.HomeTime();
    newEntry.SetInPreparation(ETrue);

    CMsvEntry* entry = CMsvEntry::NewL(*iSession, KMsvDraftEntryIdValue ,TMsvSelectionOrdering());
    CleanupStack::PushL(entry);

    CMsvOperationWait* wait = CMsvOperationWait::NewLC();
    wait->Start();

    CMsvOperation* oper = entry->CreateL(newEntry,wait->iStatus);
    CleanupStack::PushL(oper);
    CActiveScheduler::Start();

    TMsvLocalOperationProgress progress = McliUtils::GetLocalProgressL(*oper);
    User::LeaveIfError(progress.iError);

    entry->SetEntryL(progress.iId);

    CleanupStack::PopAndDestroy(3); // entry, oper, wait

    return progress.iId;
	}


void CAssistAppUi::SetEntryL(TMsvId aEntryId)
    {
    CMsvEntry* entry = iSession->GetEntryL(aEntryId);
    CleanupStack::PushL(entry);
    
    if (iMtm == NULL || entry->Entry().iMtm != (iMtm->Entry()).Entry().iMtm)
        {
        delete iMtm;
        iMtm = NULL;
        
        iMtm = iMtmReg->NewMtmL(entry->Entry().iMtm);
        iMtm->SetCurrentEntryL(entry);
        CleanupStack::Pop(); //entry
        }
    else
        {
        iMtm->SetCurrentEntryL(entry);
        CleanupStack::Pop(); //entry
        }
    }



TBool CAssistAppUi::DoSMSForwardL(TMsvId aEntryId)
    {
    TMsvEntry originalMsvEntry = (iSession->GetEntryL(aEntryId))->Entry();
    
    CSmsClientMtm* originalSmsMtm = static_cast<CSmsClientMtm*>(iMtmReg->NewMtmL(originalMsvEntry.iMtm));
    CleanupStack::PushL(originalSmsMtm);
    originalSmsMtm->SwitchCurrentEntryL(aEntryId);
    originalSmsMtm->LoadMessageL();

    iMsvId = CreateNewMessageL();

    SetEntryL(iMsvId);

    TMsvEntry msvEntry = (iMtm->Entry()).Entry();

    CRichText& mtmBody = iMtm->Body();
    mtmBody.Reset();

    TInt fromAddressLength=iData->iRecipient.Length();
    #pragma message("TE: Contact matching to be implemented here.")    
    mtmBody.InsertL(0, iData->iRecipient);
    mtmBody.InsertL(fromAddressLength, _L(": ")); 
    mtmBody.InsertL(fromAddressLength+2, originalSmsMtm->Body().Read(0));

    msvEntry.iDetails.Set(iData->iForwardRecipient);
    msvEntry.SetInPreparation(EFalse);

    msvEntry.SetSendingState(KMsvSendStateWaiting);
	msvEntry.iDate.HomeTime();
 
    CSmsClientMtm* smsMtm = static_cast<CSmsClientMtm*>(iMtm);
    smsMtm->RestoreServiceAndSettingsL();
    CSmsHeader& header = smsMtm->SmsHeader();

    CSmsSettings* sendOptions = CSmsSettings::NewL();
    CleanupStack::PushL(sendOptions);
    sendOptions->CopyL(smsMtm->ServiceSettings());

    sendOptions->SetDelivery(ESmsDeliveryImmediately);
    header.SetSmsSettingsL(*sendOptions);

	if (header.Message().ServiceCenterAddress().Length() == 0)
		{
		CSmsSettings* serviceSettings = &(smsMtm->ServiceSettings());
        
        if (!serviceSettings->NumSCAddresses())
            {
            HBufC* title=iEikonEnv->AllocReadResourceLC(R_ASSIST_ERROR_NOTE_NO_SMSC_DEFINED_TITLE);
            HBufC* message=iEikonEnv->AllocReadResourceLC(R_ASSIST_ERROR_NOTE_NO_SMSC_DEFINED_MESSAGE);
            CCknInfoDialog::RunDlgLD(*title,*message);
            CleanupStack::PopAndDestroy(2); // message + title
			}
		else
			{
            CSmsNumber* sc = 0;
			sc = &(serviceSettings->SCAddress(serviceSettings->DefaultSC()));
			header.Message().SetServiceCenterAddressL(sc->Address());
			}
		}
	CleanupStack::PopAndDestroy(); // sendOptions 
    
	smsMtm->AddAddresseeL(iData->iForwardRecipient,msvEntry.iDetails);
    
    msvEntry.iMtmData3 = KUidAssist.iUid;

    CMsvEntry& entry = iMtm->Entry();
    entry.ChangeL(msvEntry);
	smsMtm->SaveMessageL();

    TMsvId movedId = MoveMessageEntryL(KMsvGlobalOutBoxIndexEntryId);

    CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
    CleanupStack::PushL(selection);

    selection->AppendL(movedId);
    SetScheduledSendingStateL(selection);

    CleanupStack::PopAndDestroy(2); // selection, originalSmsMTM

    return ETrue;
    }


TBool CAssistAppUi::SendSMSReplyMessageL()
    {
    if(IsNumberInLogL(&(iData->iRecipient)))
        return ETrue;
 
    iMsvId=CreateNewMessageL();

    SetEntryL(iMsvId);

    TMsvEntry msvEntry = (iMtm->Entry()).Entry();

    CRichText& mtmBody = iMtm->Body();
    mtmBody.Reset();
    
    TBuf<KAssistMaxMessageLength> message=iData->iMessage;
    
    if(iData->iUseHeader)
        {
        HBufC* headerText=NULL;
        headerText=iEikonEnv->AllocReadResourceLC(R_ASSIST_MESSAGE_HEADER);
        message.Insert(0,headerText->Des());
        CleanupStack::PopAndDestroy(); // headerText
        }

    mtmBody.InsertL(0, message);

    msvEntry.iDetails.Set(iData->iRecipient);
    msvEntry.SetInPreparation(EFalse);

    msvEntry.SetSendingState(KMsvSendStateWaiting);
	msvEntry.iDate.HomeTime();
 
    CSmsClientMtm* smsMtm = static_cast<CSmsClientMtm*>(iMtm);
    smsMtm->RestoreServiceAndSettingsL();

    CSmsHeader& header = smsMtm->SmsHeader();
    CSmsSettings* sendOptions = CSmsSettings::NewL();
    CleanupStack::PushL(sendOptions);
    sendOptions->CopyL(smsMtm->ServiceSettings());

    sendOptions->SetDelivery(ESmsDeliveryImmediately);
    header.SetSmsSettingsL(*sendOptions);

	if (header.Message().ServiceCenterAddress().Length()==0)
		{
		CSmsSettings* serviceSettings = &(smsMtm->ServiceSettings());
        
        if (!serviceSettings->NumSCAddresses())
            {
            HBufC* title=iEikonEnv->AllocReadResourceLC(R_ASSIST_ERROR_NOTE_NO_SMSC_DEFINED_TITLE);
            HBufC* message=iEikonEnv->AllocReadResourceLC(R_ASSIST_ERROR_NOTE_NO_SMSC_DEFINED_MESSAGE);
            CCknInfoDialog::RunDlgLD(*title,*message);
            CleanupStack::PopAndDestroy(2); // message + title
            }
		else
			{
            CSmsNumber* sc=0;
			sc=&(serviceSettings->SCAddress(serviceSettings->DefaultSC()));
			header.Message().SetServiceCenterAddressL(sc->Address());
			}
		}
	CleanupStack::PopAndDestroy(); // sendOptions 
    
	smsMtm->AddAddresseeL(iData->iRecipient,msvEntry.iDetails);
    
    msvEntry.iMtmData3 = KUidAssist.iUid;

    CMsvEntry& entry = iMtm->Entry();
    entry.ChangeL(msvEntry);
	smsMtm->SaveMessageL();
    
    TMsvId movedId = MoveMessageEntryL( KMsvGlobalOutBoxIndexEntryId );

    
    CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
    CleanupStack::PushL(selection);

    selection->AppendL(movedId);
    SetScheduledSendingStateL(selection);

    CleanupStack::PopAndDestroy(); // selection

    AddNumberToLogL(&(iData->iRecipient));

    return ETrue;
    }


TMsvId CAssistAppUi::MoveMessageEntryL( TMsvId aTarget )
    {
	TMsvEntry msvEntry( (iMtm->Entry()).Entry() );
    TMsvId id = msvEntry.Id();

	if (msvEntry.Parent() != aTarget)
		{
		TMsvSelectionOrdering sort;
		sort.SetShowInvisibleEntries(ETrue);

		CMsvEntry* parentEntry = CMsvEntry::NewL(iMtm->Session(), msvEntry.Parent(), sort);
		CleanupStack::PushL(parentEntry);
		
		CMsvOperationWait* wait = CMsvOperationWait::NewLC();
		wait->Start();
		
		CMsvOperation* op = parentEntry->MoveL(msvEntry.Id(), aTarget, wait->iStatus);
		
		CleanupStack::PushL(op);
		CActiveScheduler::Start();		 
		TMsvLocalOperationProgress prog=McliUtils::GetLocalProgressL(*op);
		User::LeaveIfError(prog.iError);
		
		id = prog.iId;
		
		CleanupStack::PopAndDestroy(3);// op, wait, parentEntry
		}
	return id;
    }


void CAssistAppUi::SetScheduledSendingStateL(CMsvEntrySelection* aSelection)
    {
    CBaseMtm* smsMtm = iMtm;

    TBuf8<1> dummyParams;

    CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
    waiter->Start();

    CMsvOperation* op= smsMtm->InvokeAsyncFunctionL(ESmsMtmCommandScheduleCopy,*aSelection,dummyParams,waiter->iStatus);
    CleanupStack::PushL(op);
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy(2); // waiter, op
    }


TBool CAssistAppUi::DeleteSentEntry(TMsvId aEntryId)
    {
    SetEntryL( aEntryId ); 

    TMsvEntry msvEntry( (iMtm->Entry()).Entry() );

    if (msvEntry.Parent() == KMsvSentEntryId)
		{
        if (msvEntry.iMtmData3 == KUidAssist.iUid)
            {
		    TMsvSelectionOrdering sort;
		    sort.SetShowInvisibleEntries(ETrue);

		    CMsvEntry* parentEntry = CMsvEntry::NewL(iMtm->Session(), msvEntry.Parent(), sort);
		    CleanupStack::PushL(parentEntry);
		
		    CMsvOperationWait* wait = CMsvOperationWait::NewLC();
		    wait->Start();
		    
		    CMsvOperation* op = parentEntry->DeleteL(msvEntry.Id(), wait->iStatus);
		
		    CleanupStack::PushL(op);
		    CActiveScheduler::Start();		 
		    TMsvLocalOperationProgress prog=McliUtils::GetLocalProgressL(*op);
		    User::LeaveIfError(prog.iError);
		
            CleanupStack::PopAndDestroy(3);// op, wait, parentEntry
            
            return ETrue;
            }
        }

    return EFalse;
    }


void CAssistAppUi::HandleCommandL(TInt aCommand)
    {
    CEikButtonGroupContainer* buttonGroup=CEikonEnv::Static()->AppUiFactory()->ToolBar();

    switch (aCommand)
        {
    case EAssistCmdActivate:
        {
        if(iData->iApplicationMode==TAssistData::EReply && iData->iMessage.Length()==0)
            ConfigureMessagingL();
        else if(iData->iApplicationMode==TAssistData::EForward && iData->iForwardRecipient.Length()==0)
            ConfigureForwardingL();

        ConfigureDeactivationL();
        ResetLogL();

        if(iData->iApplicationMode==TAssistData::EReply)
            ConstructAndStartLogMonitorL();

        iData->iActivated=ETrue;
        iAppView->UpdateApplicationViewL();
        iAppView->DrawNow();
        buttonGroup->SetCommandL(0,R_ASSIST_BUTTON_DEACTIVATE);
        buttonGroup->DrawNow();
        }
        break;
    case EAssistCmdAutomaticDeactivation:
        {
        TTime homeTime;
        homeTime.HomeTime();
        if(homeTime<iDeactivationTime)
            {
            SetDeactivationTimerL(iDeactivationTime);
            break;
            }
        }
        //"break" left out in purpose.
    case EAssistCmdDeactivate:
        StopAndDeleteLogMonitor();
        iData->iActivated=EFalse;
        iAppView->UpdateApplicationViewL();
        iAppView->DrawNow();
        buttonGroup->SetCommandL(0,R_ASSIST_BUTTON_ACTIVATE);
        buttonGroup->DrawNow();

        if(iDeactivationTimer)
            iDeactivationTimer->Cancel();
        delete iDeactivationTimer;
        iDeactivationTimer=NULL;

        break;
    case EAssistCmdForward:
        iData->iActivated=EFalse;
        iData->iApplicationMode=TAssistData::EForward;
        iAppView->UpdateApplicationViewL();
        iAppView->DrawNow();
        buttonGroup->SetCommandL(0,R_ASSIST_BUTTON_ACTIVATE);
        buttonGroup->DrawNow();
        break;
    case EAssistCmdReply:
        iData->iActivated=EFalse;
        iData->iApplicationMode=TAssistData::EReply;
        iAppView->UpdateApplicationViewL();
        iAppView->DrawNow();
        buttonGroup->SetCommandL(0,R_ASSIST_BUTTON_ACTIVATE);
        buttonGroup->DrawNow();
        break;
    case EAssistCmdMessage:
        if(iData->iApplicationMode==TAssistData::EReply)
            ConfigureMessagingL();
        else
            ConfigureForwardingL();
        break;
    case EAssistCmdMode:
        {
        TAssistData::TApplicationMode mode=iData->iApplicationMode;
        LaunchModeSelectDialogL();
        if(iData->iApplicationMode!=mode)
            {
            iData->iActivated=EFalse;
            iAppView->UpdateApplicationViewL();
            iAppView->DrawNow();
            buttonGroup->SetCommandL(0,R_ASSIST_BUTTON_ACTIVATE);
            buttonGroup->DrawNow();
            }
        break;
        }
    case EAssistCmdAbout:
        AboutApplicationL();
        break;
    case EEikCmdExit: 
        ExitL();
        break;
    default:
        break;
        }
    }


void CAssistAppUi::DynInitMenuPaneL(TInt aMenuId,CEikMenuPane* aMenuPane)
    {
    if(aMenuId==R_ASSIST_MODE_MENU)
        {
        if(iData->iApplicationMode==TAssistData::EReply)
            aMenuPane->SetItemButtonState(EAssistCmdReply,EEikMenuItemSymbolOn);
        else
            aMenuPane->SetItemButtonState(EAssistCmdForward,EEikMenuItemSymbolOn);
        }
    }


void CAssistAppUi::ConfigureMessagingL()
    {
    CAssistMessageDialog* msgDlg=new(ELeave)CAssistMessageDialog(*iData);
    msgDlg->ExecuteLD(R_ASSIST_MESSAGING_DIALOG);
    }


void CAssistAppUi::ConfigureForwardingL()
    {
    CAssistForwardingDialog* forwDlg=new(ELeave)CAssistForwardingDialog(*iData);
    forwDlg->ExecuteLD(R_ASSIST_FORWARDING_DIALOG);
    }


void CAssistAppUi::ConfigureDeactivationL()
    {
    TTime timerLaunchTime;
    TTimeIntervalHours timeIncrement(1);
    timerLaunchTime.HomeTime();
    timerLaunchTime+=timeIncrement;

    CAssistDeactivationDialog* dlg=new(ELeave)CAssistDeactivationDialog(*iData,timerLaunchTime);  
    dlg->ExecuteLD(R_ASSIST_DEACTIVATION_DIALOG);

    if(iDeactivationTimer)
        iDeactivationTimer->Cancel();
    delete iDeactivationTimer;
    iDeactivationTimer=NULL;

    if(iData->iUseDeactivation)
        SetDeactivationTimerL(timerLaunchTime);
    }


void CAssistAppUi::LaunchModeSelectDialogL()
    {
    CAssistModeSelectDialog* dlg=new(ELeave)CAssistModeSelectDialog(*iData);
    dlg->ExecuteLD(R_ASSIST_MODE_SELECT_DIALOG);
    }


TBool CAssistAppUi::LaunchExitNoteL()
    {
    TBool returnValue;
    HBufC* message=iEikonEnv->AllocReadResourceLC(R_ASSIST_EXIT_NOTE_MESSAGE);
    returnValue=CCknConfirmationDialog::RunDlgLD(KNullDesC,*message);
    CleanupStack::PopAndDestroy(); // message
    return returnValue;
    }


void CAssistAppUi::AboutApplicationL()
    {
    TFileName iconFileName;
    TFileName appFileName=iEikonEnv->EikAppUi()->Application()->AppFullName();
    TParse parse;
    parse.Set(appFileName,NULL,NULL);
    iconFileName=parse.Drive();
    iconFileName+=parse.Path();
    iconFileName+=KIconFileName;

    HBufC* title=iEikonEnv->AllocReadResourceLC(R_ASSIST_ABOUT_NOTE_TITLE);
    HBufC* message=iEikonEnv->AllocReadResourceLC(R_ASSIST_ABOUT_NOTE_MESSAGE);
    CGulIcon* icon=iEikonEnv->CreateIconL(iconFileName,EMbmAssistAbout,EMbmAssistAboutm);
    CleanupStack::PushL(icon);
    CCknInfoDialog::RunDlgLD(*title,*message,icon);
    CleanupStack::PopAndDestroy(3); // message, title, icon
    }


TBool CAssistAppUi::LaunchStartupNoteL()
    {
    TBool returnValue;
    HBufC* message=iEikonEnv->AllocReadResourceLC(R_ASSIST_STARTUP_NOTE_MESSAGE);
    returnValue=CCknConfirmationDialog::RunDlgLD(KNullDesC,*message);
    CleanupStack::PopAndDestroy(); // message
    if(!returnValue)
        {
        HBufC* message=iEikonEnv->AllocReadResourceLC(R_ASSIST_STARTUP_NOTE_CLOSING_MESSAGE);
        CCknInfoDialog::RunDlgLD(KNullDesC,*message);
        CleanupStack::PopAndDestroy(); // message
        }
    return returnValue;
    }


void CAssistAppUi::ResetLogL()
    {
    TFileName logFileName;
    TFileName applicationName=Application()->AppFullName();
    TParse parse;
    parse.Set(applicationName,NULL,NULL);

    logFileName=parse.Drive();
    logFileName+=parse.Path();
#if defined(__WINS__)
    logFileName=_L("c:\\");
#endif
    logFileName+=KLogFileName;

    RFs fs;
    fs.Connect();

    CFileStore* store=NULL;;

    TRAPD(replaceErrorCode,store=CDirectFileStore::ReplaceL(fs,logFileName,EFileShareAny|EFileWrite));

    CleanupStack::PushL(store);

	if (replaceErrorCode!=KErrNone)
        {
        store=CDirectFileStore::CreateL(fs,logFileName,EFileShareAny|EFileWrite);
        }

    store->SetTypeL(KDirectFileStoreLayoutUid);
    RStoreWriteStream outStream;
    TStreamId id=outStream.CreateLC(*store);

    outStream.WriteInt16L(0); // Set initial number of log items to 0

    outStream.CommitL();
    store->SetRootL(id);
    store->CommitL();
    CleanupStack::PopAndDestroy(2); // outStream + store

    fs.Close();
    }


TBool CAssistAppUi::IsNumberInLogL(TDesC* aNumber) const
    {
    TBool isInLog=EFalse;

    TFileName logFileName;
    TFileName applicationName=Application()->AppFullName();
    TParse parse;
    parse.Set(applicationName,NULL,NULL);

    logFileName=parse.Drive();
    logFileName+=parse.Path();
    logFileName+=KLogFileName;

    RFs fs;
    fs.Connect();
    CFileStore* store=NULL;

    TRAPD(err,store=CDirectFileStore::OpenL(fs,logFileName,EFileShareReadersOnly));

    CleanupStack::PushL(store);

	if (err==KErrNone)
        {
        RStoreReadStream inStream;
        inStream.OpenLC(*store,store->Root());

        TInt numberOfLogItems=inStream.ReadInt16L();

        for(TInt i=0;i<numberOfLogItems;i++)
            {
            const TChar KItemDelimiter(0x3);
            TBuf<KAssistMaxMessageLength> buf;

            inStream.ReadL(buf,KItemDelimiter);

            TInt length=buf.Length();
            length--;

            if(length>0)
                {
                buf=buf.Left(length);

                if(*aNumber==buf)
                    isInLog=ETrue;
                else if(length>7)
                    {
                    if((*aNumber).Right(7)==buf.Right(7))
                        isInLog=ETrue;
                    }
                }
            }

        CleanupStack::PopAndDestroy(); // inStream
        }

    CleanupStack::PopAndDestroy(); // store

    fs.Close();
    return isInLog;
    }


void CAssistAppUi::AddNumberToLogL(TDesC* aNumber)
    {
    CArrayFixFlat<TDesC*>* logItems=new(ELeave)CArrayFixFlat<TDesC*>(4);
    CleanupStack::PushL(logItems);

    TFileName logFileName;
    TFileName applicationName=Application()->AppFullName();
    TParse parse;
    parse.Set(applicationName,NULL,NULL);

    logFileName=parse.Drive();
    logFileName+=parse.Path();
    logFileName+=KLogFileName;

    const TChar KItemDelimiter(0x3);

    RFs fs;
    fs.Connect();
    CFileStore* store=NULL;
    TInt numberOfLogItems=0;

    TRAPD(err,store=CDirectFileStore::OpenL(fs,logFileName,EFileShareReadersOnly));
	
    if (err==KErrNone)
        {
        CleanupStack::PushL(store);
        RStoreReadStream inStream;
        inStream.OpenLC(*store,store->Root());

        numberOfLogItems=inStream.ReadInt16L();

        for(TInt i=0;i<numberOfLogItems;i++)
            {
            TBuf<KAssistMaxMessageLength> buf;

            inStream.ReadL(buf,KItemDelimiter);
            TInt length=buf.Length();

            if(length>2)
                {
                HBufC* numberItem=HBufC::NewLC(KMaxTelephoneNumberLength+1);
                numberItem->Des()=buf;
                logItems->AppendL(numberItem);
                CleanupStack::Pop(); // numberItem
                }
            }

        CleanupStack::PopAndDestroy(2); // inStream + store
        }

    parse.Set(logFileName,NULL,NULL);

    TRAPD(replaceErrorCode,store=CDirectFileStore::ReplaceL(fs,logFileName,EFileShareAny|EFileWrite));

	if (replaceErrorCode!=KErrNone)
        {
        store=CDirectFileStore::CreateL(fs,logFileName,EFileShareAny|EFileWrite);
        }

    CleanupStack::PushL(store);
    store->SetTypeL(KDirectFileStoreLayoutUid);
    RStoreWriteStream outStream;
    TStreamId id=outStream.CreateLC(*store);

    outStream.WriteInt16L(numberOfLogItems+1);

    for(TInt i=0;i<numberOfLogItems;i++)
        {
        TDesC* item=logItems->At(i);
        outStream.WriteL(*item);
        delete item;
        }

    HBufC* newItem=HBufC::NewLC(aNumber->Length()+1);
    newItem->Des()=*aNumber;
    (newItem->Des()).Append(KItemDelimiter);
    outStream.WriteL(*newItem);
    outStream.CommitL();
    store->SetRootL(id);
    store->CommitL();

    CleanupStack::PopAndDestroy(4); // newItem + outStream + store + logItems
    fs.Close();
    }


TBool CAssistAppUi::ReplyToVoice() const
    {
    TBool returnValue=EFalse;
     
    if(iData->iReplyTo==EAll || iData->iReplyTo==EVoice)
        returnValue=ETrue;

    return returnValue;
    }


TBool CAssistAppUi::ReplyToSMS() const
    {
    TBool returnValue=EFalse;
     
    if(iData->iReplyTo==EAll || iData->iReplyTo==ESMS)
        returnValue=ETrue;

    return returnValue;
    }


TBool CAssistAppUi::ReplyToMMS() const
    {
    TBool returnValue=EFalse;
     
    if(iData->iReplyTo==EAll || iData->iReplyTo==EMMS)
        returnValue=ETrue;

    return returnValue;
    }


void CAssistAppUi::MessageReceivedL(TMsvId aEntryId)
    {
    if(!(iData->iActivated))
        return;

    TMsvEntry msvEntry = (iSession->GetEntryL(aEntryId))->Entry();

    CBaseMtm* mtm=iMtmReg->NewMtmL(msvEntry.iMtm);
    CleanupStack::PushL(mtm);

    if(iData->iApplicationMode==TAssistData::EForward)
        {
        if(mtm->Type()==KUidMsgTypeSMS)
            {
            CSmsClientMtm* smsMtm=static_cast<CSmsClientMtm*>(mtm);
    
            smsMtm->SwitchCurrentEntryL(aEntryId);
    
            smsMtm->LoadMessageL();

            CSmsHeader& header=smsMtm->SmsHeader();

            if(header.FromAddress().Length()>0)
                {
                iData->iRecipient=header.FromAddress();
                DoSMSForwardL(aEntryId);
                }
            }
        }
    else if(mtm->Type()==KUidMsgTypeMultimedia && ReplyToMMS())
        {
        CMmsClientMtm* mmsMtm=static_cast<CMmsClientMtm*>(mtm);
        
        mmsMtm->SwitchCurrentEntryL(aEntryId);
        mmsMtm->LoadMessageL();
        
        CEikonEnv::Static()->InfoWinL(_L("mmsMtm->Sender"),mmsMtm->Sender());

        if(mmsMtm->Sender().Length()>0)
            {
            iData->iRecipient=mmsMtm->Sender();
            SendSMSReplyMessageL();
            }
        }

    CleanupStack::PopAndDestroy(); // mtm
    }


void CAssistAppUi::ExitL()
    {
    SaveL();
    if(!(iData->iActivated))
        Exit();
    else if(LaunchExitNoteL())
        Exit();
    }
