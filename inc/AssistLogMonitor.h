//
// ASSISTLOGMONITOR.H - header file for SMS Assistant application
// Copyright (c) 2001-2003 Tuomas Eerola
//   

#ifndef __ASSISTLOGMONITOR_H__
#define __ASSISTLOGMONITOR_H__


class CLogClient;
class CLogViewEvent;
class CLogFilter;


class CAssistLogMonitor : public CActive
    {
public:
    CAssistLogMonitor();
    ~CAssistLogMonitor();
    void ConstructL();
    void Start();
protected:
    void DoCancel();
    void RunL();
private:
    void IssueRequest();
private:
    enum TLogMonitorState
        {
        EInitializing=0,
        EWaitingForLogChange,
        EFindingNewestEvent,
        EHandlingNewEvent
        };
private:
    RFs iRFs;
    CLogClient* iLogClient;
    CLogFilter* iFilter;
    CLogViewEvent* iLogView;
    TLogString iLogDirectionString;
    TLogMonitorState iMonitorState;
    TInt iNewestLogEventId;
    };


#endif // __ASSISTLOGMONITOR_H__