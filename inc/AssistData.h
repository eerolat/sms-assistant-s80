//
// ASSISTDATA.H - header file for SMS Assistant application
// Copyright (c) 2001-2005 Tuomas Eerola
//   

#ifndef __ASSISTDATA_H__
#define __ASSISTDATA_H__

#ifndef __ASSIST_HRH__
#include "assist.hrh"
#endif

#ifndef __E32STD_H__
#include <e32std.h>
#endif


enum TCommunicationsChannel
    {
    EVoice,
    ESMS,
    EMMS,
    EAll
    };


class TAssistData
    {
public:
    enum TApplicationMode
        {
        EReply=0,
        EForward
        };
public:
    static TAssistData* NewL();
    TBuf<KMaxTelephoneNumberLength> iRecipient;
    TBuf<KAssistMaxMessageLength> iMessage;
    TBool iActivated;
    TBool iUseDeactivation;
    TBool iUseHeader;
    TInt iReplyTo;
    TBool iAskForSendingPermission;
    TApplicationMode iApplicationMode;
    TBuf<KMaxTelephoneNumberLength> iForwardRecipient;
protected:
    TAssistData();
    };


#endif // __ASSISTDATA_H__