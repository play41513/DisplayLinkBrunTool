﻿//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MSGBOX.h"


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
	{
		MessageBoxA(NULL,"請輸入完整資訊","WARNING", MB_ICONEXCLAMATION);
		edtSetWorkOrderNumber->SetFocus();
		edtSetWorkOrderNumber->SelectAll();
	}
	else if(edtSetEmployeeID->Text=="")
	{
		MessageBoxA(NULL,"請輸入完整資訊","WARNING", MB_ICONEXCLAMATION);
		edtSetEmployeeID->SetFocus();
		edtSetEmployeeID->SelectAll();
	}
	else if(plckbCustomSN->Caption.Pos("√") && cboxSN->Text == "")
	{
		MessageBoxA(NULL,"請輸入序號資訊","WARNING", MB_ICONEXCLAMATION);
		cboxSN->SetFocus();
		cboxSN->SelectAll();
    }
	else
	{
		if(plckbBarcode->Caption.Pos("√"))
		{
			if(CheckMACValue())
			{
				ModalResult = mrOk;
			}
			else
			{
				MessageBoxA(NULL,"請檢查輸入的MAC","WARNING", MB_ICONEXCLAMATION);
				mskedBurntMacStart->SetFocus();
				mskedBurntMacStart->SelectAll();
			}
		}
		else
		{
			ModalResult = mrOk;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmMsg::plSwitchClick(TObject *Sender)
{
	/*if(plSwitch->Caption=="V")
	{
		plSwitch->Caption = "^";
		frmMsg->Height = 452;
	}
	else
	{
		plSwitch->Caption = "V";
		frmMsg->Height = 269;
	}*/
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
	{
		if(plckbBarcode->Caption.Pos("√"))
			mskedBurntMacStart->SetFocus();
		else
			edtSetEmployeeID->SetFocus();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------




void __fastcall TfrmMsg::FormShow(TObject *Sender)
{
	if(plckbBarcode->Caption.Pos("√"))
	{
		frmMsg->Height = 411;
		plBarcode->Height = 277;
		plNormalMode->Enabled	= true;
		plReworkMode->Enabled	= true;
		mskedBurntMacStart->Enabled = true;
		mskedBurntMacLast->Enabled	= true;
	}
	else
	{
		frmMsg->Height = 278;
		plBarcode->Height = 144;
		plNormalMode->Enabled	= false;
		plReworkMode->Enabled	= false;
		mskedBurntMacStart->Enabled = false;
		mskedBurntMacLast->Enabled	= false;
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmMsg::plckbBarcodeClick(TObject *Sender)
{
	TPanel* pl = (TPanel*)Sender;
	if(pl->Name.Pos("Barcode"))
	{
		if(plckbBarcode->Caption.Pos("√"))
		{
			frmMsg->Height = 278;
			plBarcode->Height = 144;
			plckbBarcode->Caption = "";
			plNormalMode->Enabled	= false;
			plReworkMode->Enabled	= false;
			mskedBurntMacStart->Enabled = false;
			mskedBurntMacLast->Enabled	= false;
		}
		else
		{
			frmMsg->Height = 411;
			plBarcode->Height = 277;
			plckbBarcode->Caption = "√";
			plNormalMode->Enabled	= true;
			plReworkMode->Enabled	= true;
			mskedBurntMacStart->Enabled = true;
			mskedBurntMacLast->Enabled	= true;
		}
	}
	else if(pl->Name.Pos("CustomSN"))
	{
		if(plckbCustomSN->Caption.Pos("√"))
		{
			plckbCustomSN->Caption = "";
			cboxSN->Visible = false;

		}
		else
		{
			plckbCustomSN->Caption = "√";
			cboxSN->Visible = true;
			cboxSN->ItemIndex = 0;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::plNormalModeClick(TObject *Sender)
{
	TPanel* pl = (TPanel*)Sender;
	if(pl->Name.Pos("Normal"))
	{
		plNormalMode->Color = clLime;
		plReworkMode->Color = clCream;
	}
	else
	{
		plNormalMode->Color = clCream;
		plReworkMode->Color = clLime;
		mskedBurntMacStart->Text = "000000000000";
		mskedBurntMacLast->Text  = "FFFFFFFFFFFF";
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::mskedBurntMacStartKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(Key==13)
	{
		if(mskedBurntMacStart->Text.Length()!= 12)
			mskedBurntMacStart->Text = "";
		else
			mskedBurntMacLast->SetFocus();
	}
	else if(Key==8)
		mskedBurntMacStart->Text = "";
	else
	{
		if(mskedBurntMacStart->Text.Length()== 12)
			mskedBurntMacStart->Text = "";
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::mskedBurntMacLastKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(Key==13)
	{
		if(mskedBurntMacLast->Text.Length()!= 12)
			mskedBurntMacLast->Text = "";
		else
		{
			if(plckbCustomSN->Caption.Pos("√"))
			{
				plckbCustomSN->SetFocus();
			}
			else
				edtSetEmployeeID->SetFocus();
		}
	}
	else if(Key==8)
		mskedBurntMacLast->Text = "";
	else
	{
		if(mskedBurntMacLast->Text.Length()== 12)
			mskedBurntMacLast->Text = "";
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmMsg::edtSetWorkOrderNumberExit(TObject *Sender)
{
	edtSetWorkOrderNumber->Text = edtSetWorkOrderNumber->Text.UpperCase();
	AnsiString strTemp = "";
	if(edtSetWorkOrderNumber->Text.Length()== 15)
	{
		strTemp = edtSetWorkOrderNumber->Text.SubString(4,1);
		if(!strTemp.Pos("-"))
		{
			edtSetWorkOrderNumber->Text = "";
			edtSetWorkOrderNumber->SetFocus();
		}
		strTemp = edtSetWorkOrderNumber->Text.SubString(5,11);
		if(strTemp.Pos("-"))
		{
			edtSetWorkOrderNumber->Text = "";
			edtSetWorkOrderNumber->SetFocus();
		}
	}
	else
	{
		edtSetWorkOrderNumber->Text = "";
		edtSetWorkOrderNumber->SetFocus();
	}
}
//---------------------------------------------------------------------------
int TfrmMsg::HexToInt(AnsiString HexStr)
{
	int iIndex, iHex, totalChar;
	TCHAR HexChar[8];

	HexStr.UpperCase(); // 把字串轉成全大寫

	_tcscpy_s(HexChar, 8, HexStr.c_str());
	iHex = 0;
	totalChar = HexStr.Length(); // 取得字串的長度
	for (iIndex = 0; iIndex < totalChar; iIndex++) {
		iHex <<= 4;
		if ((HexChar[iIndex] >= 0x30) && (HexChar[iIndex] <= 0x39)) {
			iHex += (HexChar[iIndex] - 0x30); // 把 0 - 9 字元轉成數字
		}
		else if ((HexChar[iIndex] >= 0x41) && (HexChar[iIndex] <= 0x46)) {
			iHex += (HexChar[iIndex] - 0x37); // ­把 A - F 字元轉成數字
		}
		else {
			iHex = 0;
			break;
		}
	}
	return iHex;
}
bool TfrmMsg::CheckMACValue()
{
	if(mskedBurntMacStart->Text == "" || mskedBurntMacLast->Text == "")
		return false;
	int tmp_1 = HexToInt(mskedBurntMacStart->Text.SubString(1,2)+mskedBurntMacStart->Text.SubString(3,2)
			+mskedBurntMacStart->Text.SubString(5,2));
	int tmp_2 = HexToInt(mskedBurntMacLast->Text.SubString(1,2)+mskedBurntMacLast->Text.SubString(3,2)
			+mskedBurntMacLast->Text.SubString(5,2));
	if(tmp_2 > tmp_1)
	{
		tmp_1 = HexToInt(mskedBurntMacStart->Text.SubString(7,2)+mskedBurntMacStart->Text.SubString(9,2)
				+mskedBurntMacStart->Text.SubString(11,2));
		tmp_2 = HexToInt(mskedBurntMacLast->Text.SubString(7,2)+mskedBurntMacLast->Text.SubString(9,2)
				+mskedBurntMacLast->Text.SubString(11,2));
		if(tmp_2 >= tmp_1)
			return true;
		else return false;
	}
	else if(tmp_2 == tmp_1)
	{
		tmp_1 = HexToInt(mskedBurntMacStart->Text.SubString(7,2)+mskedBurntMacStart->Text.SubString(9,2)
				+mskedBurntMacStart->Text.SubString(11,2));
		tmp_2 = HexToInt(mskedBurntMacLast->Text.SubString(7,2)+mskedBurntMacLast->Text.SubString(9,2)
				+mskedBurntMacLast->Text.SubString(11,2));
		if(tmp_2 > tmp_1)
			return true;
		else return false;
	}
	else return false;
}


void __fastcall TfrmMsg::mskedBurntMacStartChange(TObject *Sender)
{
	TMaskEdit* edt=(TMaskEdit*)Sender;
	TCHAR HexChar[15];

	edt->Text = edt->Text.UpperCase();// 把字串轉成全大寫
	_tcscpy_s(HexChar, 15, AnsiString(edt->Text).c_str());
	for(int i=0;i <edt->Text.Length();i++)
	{
		if(HexChar[i] < 0x30 || HexChar[i] > 0x39)//非數字
		{
			if(HexChar[i] < 0x41 || HexChar[i] > 0x46)//非大寫字母a~f
			{
				edt->Text =
					edt->Text.Delete(edt->Text.Length(),1);
            }
		}
	}
	if(edt->Text.Length()==3 || edt->Text.Length()==4)
		edt->SelStart=edt->Text.Length()+1;
	else if(edt->Text.Length()==5 || edt->Text.Length()==6)
		edt->SelStart=edt->Text.Length()+2;
	else if(edt->Text.Length()==7 || edt->Text.Length()==8)
		edt->SelStart=edt->Text.Length()+3;
	else if(edt->Text.Length()==9 || edt->Text.Length()==10)
		edt->SelStart=edt->Text.Length()+4;
	else if(edt->Text.Length()==11 || edt->Text.Length()==12)
		edt->SelStart=edt->Text.Length()+5;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMsg::FormClose(TObject *Sender, TCloseAction &Action)
{
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------


void __fastcall TfrmMsg::Timer1Timer(TObject *Sender)
{
	Timer1->Enabled = false;
	DWORD DiskInfo = GetLogicalDrives();
	AnsiString DiskName,DiskVerifty = "DEVICE_LOG",SS;
	char Disk;
	ImgDisk->Visible = false;
	for(int i =3;i<=25;i++)
	{
		if((DiskInfo&(0x01<<i))!=0)
		{
			char Volumelabel[20];
			DWORD SerialNumber;
			DWORD MaxCLength;
			DWORD FileSysFlag;
			char FileSysName[10];
			Disk = 0x41+i;
			DiskName = AnsiString(Disk)+":\\";
			GetVolumeInformation(DiskName.c_str(),Volumelabel,255,&SerialNumber,&MaxCLength,&FileSysFlag,FileSysName,255);
			if(!strcmp(Volumelabel,DiskVerifty.c_str()))
			{
				ImgDisk->Visible = true;
				break;
			}
		}
	}
	Timer1->Enabled = true;
}
//---------------------------------------------------------------------------


void __fastcall TfrmMsg::cboxSNKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(Key==13)
		edtSetEmployeeID->SetFocus();
}
//---------------------------------------------------------------------------

