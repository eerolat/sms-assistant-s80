//
// ASSISTDOCUMENT.CPP - source file for SMS Assistant application
// Copyright (c) 2001-2005 Tuomas Eerola
//


#include "assistdocument.h"
#include "assistappui.h"


CAssistDocument::CAssistDocument(CEikApplication& aApp)
	: CEikDocument(aApp)    
    {
    }


CAssistDocument* CAssistDocument::NewL(CEikApplication& aApp)
    {
    CAssistDocument* self=new (ELeave) CAssistDocument(aApp);
    return self;
    }

    
CEikAppUi* CAssistDocument::CreateAppUiL()
    {
    return new(ELeave)CAssistAppUi;
    }

