//---------------------------------------------------------------------------

#include <vcl.h>
#include <tchar.h>
#include "ConstantString.h"
#pragma hdrstop
USEFORM("MSGBOX.cpp", frmMsg);
USEFORM("Uint1.cpp", USBHIDForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
HANDLE ASMC_USBPHONE_AP_HANDLE;
try
	{
    Application->Initialize();
    Application->CreateForm(__classid(TUSBHIDForm), &USBHIDForm);
		Application->CreateForm(__classid(TfrmMsg), &frmMsg);
		Application->Run();
    }
catch (Exception &exception)
    {
    Application->ShowException(&exception);
    }
//ReleaseMutex(ASMC_USBPHONE_AP_HANDLE);
return 0;
}
//---------------------------------------------------------------------------

