//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MSGBOX.h"
#include "Uint1.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMsg *frmMsg;
//---------------------------------------------------------------------------
__fastcall TfrmMsg::TfrmMsg(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TfrmMsg::btnID_okClick(TObject *Sender)
{
	if(edtSetWorkOrderNumber->Text=="")
		MessageBoxA(NULL,"請輸入完整資訊","WARNING", MB_ICONEXCLAMATION);
	else if(edtSetModel->Text=="")
		MessageBoxA(NULL,"請輸入完整資訊","WARNING", MB_ICONEXCLAMATION);
	else if(edtSetEmployeeID->Text=="")
		MessageBoxA(NULL,"請輸入完整資訊","WARNING", MB_ICONEXCLAMATION);
	else
		ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMsg::plSwitchClick(TObject *Sender)
{
	if(plSwitch->Caption=="V")
	{
		plSwitch->Caption = "^";
		frmMsg->Height = 452;
	}
	else
	{
		plSwitch->Caption = "V";
		frmMsg->Height = 269;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::edtSetEmployeeIDKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(Key==13)
		btnID_ok->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::edtSetWorkOrderNumberKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift)
{
	if(Key==13)
		edtWorkStation->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::edtSetModelKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(Key==13)
		edtSetEmployeeID->SetFocus();
}
//---------------------------------------------------------------------------




void __fastcall TfrmMsg::FormShow(TObject *Sender)
{
	if(frmMsg->Tag)
	{
		frmMsg->plWOInfo->Enabled = false;
		frmMsg->plWOInfo->Height  = 0;
		frmMsg->plSwitch->Enabled = false;
		frmMsg->plSwitch->Height  = 0;
		frmMsg->TouchKeyboard->Top = 182;
		frmMsg->Height = 160;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::edtWorkStationKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(Key==13)
		edtSetModel->SetFocus();
}
//---------------------------------------------------------------------------

