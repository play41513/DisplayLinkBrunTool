﻿// Module:
// Notices: Copyright(c) 2019 Leno
// Purpose:
// Compiler: Embarcadero RAD Studio 2010
// OS: Windows 8.1 x64
// Version: 1.1.0__declspec(uuid("{2DD69FE9-6425-4DB2-A21F-7987EDF08B77}"))
// --------------------------------------------------------------------------
#include <vcl.h>
#include <vfw.h>
#include <direct.h>
#include <stdio.h>
#pragma hdrstop


extern "C" {
#include "hidsdi.h"
#include "setupapi.h"
}
#include "DUT.h"
#include "Uint1.h"
#include "MSGBOX.h"
#include "USBDevConnectionInfo.h"



#include <windows.h>
#include "wininet.h"
#pragma link "wininet.lib"
#include "WORKThread.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Gauges"
#pragma resource "*.dfm"
TUSBHIDForm *USBHIDForm;
// ---------------------------------------------------------------------------
bool bWin10 = false;
bool bWin8DeBug = false;

//
DUT m_dut;
USBDevConnectionInfo *USBCONInfo = new USBDevConnectionInfo();


__fastcall TUSBHIDForm::TUSBHIDForm(TComponent* Owner) : TForm(Owner)
{

}

void __fastcall TUSBHIDForm::FormShow(TObject *Sender)
{
	TestTime = 0;
	bPlugIn = false;
	g_OTP_SN="";
	g_OTP_SN_2="";
	// 隱藏Barcode畫面
	edtBarcodeMain->Height = 0;
	Caption = APP_TITLE;
	//	if (FileExists(ChangeFileExt(Application->ExeName, ".ini")))	{		TIniFile *ini;		ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));		//KIND		cmbNumOfChip->ItemIndex = ini->ReadInteger("KIND", "KIND", 0)-1;		//DDR		g_bDDR_Checck = ini->ReadBool("DDR", "DDR_CHECK", true);		//BURN_SETTING		ckbBurnSN->Checked = ini->ReadBool("BURN_SETTING", "BURN_SN", false);		ckbBurnMAC->Checked = ini->ReadBool("BURN_SETTING", "BURN_MAC", false);		ckbAutoIncrease->Checked = ini->ReadBool("BURN_SETTING", "AUTO", false);		ckbBarcode->Checked = ini->ReadBool("BURN_SETTING", "BARCODE", false);		ckbBarcodeClick(NULL);		ckbFWVerify->Checked = ini->ReadBool("BURN_SETTING", "FW_VERIFY", false);		ckbBarcodeVerify->Checked = ini->ReadBool("BURN_SETTING", "BARCODE_VERIFY", false);		ckbBarcodeVerifyClick(NULL);		//BURN_CONTENT		mskedtBurnSn->Text = ini->ReadString(_T("BURN_CONTENT"), _T("BURN_SN"), _T("0"));		mskedBurntMacPre->Text = ini->ReadString(_T("BURN_CONTENT"), _T("MAC_PREFIX"), _T("0"));		mskedtBurnMac->Text = ini->ReadString(_T("BURN_CONTENT"), _T("MAC_END"), _T("0"));		//FW Ver		pl_Sample_MinFW->Caption = ini->ReadString(_T("FW_VERSION"), _T("MIN_FW_1"), _T(""));		pl_Sample_MinFW2->Caption = ini->ReadString(_T("FW_VERSION"), _T("MIN_FW_2"), _T(""));		ckbOTP->Checked = ini->ReadBool("FW_VERSION", "OTP", false);		//WO		edtWorkOrderNumber->Text = ini->ReadString(_T("WorkOrder"), _T("WorkOrderNumber"), _T("000-00000000000"));		delete ini;	}	else	{		MessageBox(NULL, "找不到INI設定檔，請重新安裝程式", "警告訊息", MB_ICONEXCLAMATION);
		PostQuitMessage(WM_QUIT);
	}	//檢測windows版本
	OSVERSIONINFO osvi;
	memset(&osvi,0,sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	int windowsVer = osvi.dwMajorVersion *10 + osvi.dwMinorVersion;
	if(!bWin8DeBug)
	{
		if(windowsVer == 62){
			// win10 檢查驅動 移除DisplayLink一般驅動
			bWin10 = true;
			DWORD dwFlags = 0;
			if(InternetCheckConnection("http://google.com",FLAG_ICC_FORCE_CONNECTION,dwFlags))
			{
				MessageBox(NULL, "此程式需將網際網路關閉!!\r\n請關閉網路或拔除網路線再開啟程式", "警告訊息", MB_ICONEXCLAMATION);
				PostQuitMessage(WM_QUIT);
			}
			//else RemoveDisplayLinkDriver();
		}
	}
	ShowSettingForm();
	dThreadStatus = THREAD_WIAT;
	new TWORKThread(false);
}
// ---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::FormClose(TObject *Sender, TCloseAction &Action) {
	WriteINIFile();
}
// ---------------------------------------------------------------------------
char __fastcall TUSBHIDForm::DUTV(void) {
	int ERRC;
	UnicodeString mac1, sn1, mac2, sn2, msg ,min1 ,min2;
	UnicodeString refMac = mskedBurntMacPre->Text + mskedtBurnMac->Text;
	labMSG("驗證序號..",clYellow);
	//重新讀取晶片info flash
	if(m_dut.GetInfo()){
		//讀取成功
		ERRC = 0x00;
		sn1 = m_dut._If1.SN;
		mac1 = m_dut._If1.MAC;
		min1 = m_dut._If1.MinFwVersion;

		plChipName->Caption = "DisplayLink Chip 1:"+m_dut._Dev1;
		plMinFWVer->Caption = m_dut._If1.MinFwVersion;
		plMAC->Caption = m_dut._If1.MAC;
		plUSBSN->Caption = m_dut._If1.SN;
		if(ckbBarcodeVerify->Checked)
		{
			edtBarcodeMAC->Text = "";
			edtBarcodeMain->Height = 160;
			edtBarcodeMain->Top = USBHIDForm->Height/2 - 80;
			lbBarcode->Caption = "請掃描MAC燒錄條碼";
			edtBarcodeMAC->SetFocus();
			SetForegroundWindowInternal();
			while(true)
			{
				if(numBarcodeResult == BARCODE_FINISH)
				{
					edtBarcodeMain->Height = 0;
					ERRC =  0x00;
					break;
				}
				if(numBarcodeResult == BARCODE_CHANEL)
				{
					edtBarcodeMain->Height = 0;
					ERRC =  0x40;
					if(ERRC)
					{
						labMSG("(!)Barcode輸入異常",clRed);
						ErrorMsg = "(!)Barcode輸入異常";
					}
					break;
				}
				Delay(100);
			}
			refMac = mskedBurntMacPre->Text + mskedtBurnMac->Text;
		}

		if(m_dut._Kind == 2){
			sn2 = m_dut._If2.SN;
			mac2 = m_dut._If2.MAC;
			min2 = m_dut._If2.MinFwVersion;

			plChipName2->Caption = "DisplayLink Chip 2:"+m_dut._Dev2;
			plMinFWVer2->Caption = m_dut._If2.MinFwVersion;
			plMAC2->Caption = m_dut._If2.MAC;
			plUSBSN2->Caption = m_dut._If2.SN;
		}else{
			sn2 = L"";
			mac2 = L"";
			min2 = L"";
		}
	}else{
		//讀取失敗
		ERRC = 0xFF;
	}
	//---------------------------------------------------------Verify
	if (!ERRC) {
		if(ckbBurnSN->Checked || ckbBarcodeVerify->Checked)
		{   //比對SN
			memoMsg->Lines->Add(VERIFY_DUT_SERIAL);
			if(m_dut._Kind == 1){
				if(sn1 != mskedtBurnSn->Text){
					plUSBSN->Font->Color = clRed;;
					labMSG("(!)序號不符",clRed);
					ErrorMsg = "(!)序號不符:"+mskedtBurnSn->Text;
					ERRC = 0x40;
				}
			}else{
				if(sn1.Length() == 8 || sn1.Length() == 12){
					if(sn1 != mskedtBurnSn->Text){
						labMSG("(!)Flash1序號不符",clRed);
						ErrorMsg = "(!)序號不符:"+mskedtBurnSn->Text;
						plUSBSN->Font->Color = clRed;;
						ERRC = 0x40;
					}

					if(sn2 != mskedtBurnSn->Text + L"B"){
						labMSG("(!)Flash2序號不符",clRed);
						ErrorMsg = "(!)序號不符:"+mskedtBurnSn->Text+"B";
						plUSBSN2->Font->Color = clRed;
						ERRC = 0x40;
					}
				}else{
					if(sn1 != mskedtBurnSn->Text + L"B"){
						labMSG("(!)Flash1序號不符",clRed);
						ErrorMsg = "(!)序號不符:"+mskedtBurnSn->Text+"B";
						plUSBSN->Font->Color = clRed;
						ERRC = 0x40;
					}

					if(sn2 != mskedtBurnSn->Text){
						labMSG("(!)Flash2序號不符",clRed);
						ErrorMsg = "(!)序號不符:"+mskedtBurnSn->Text;
						plUSBSN2->Font->Color = clRed;;
						ERRC = 0x40;
					}
				}
			}
		}
		if(ckbBurnMAC->Checked || ckbBarcodeVerify->Checked)
		{
			memoMsg->Lines->Add(VERIFY_DUT_MAC);
			if(m_dut._Kind == 1){
				if(mac1 != refMac){
					plMAC->Font->Color = clRed;
					labMSG("(!)MAC不符",clRed);
					ErrorMsg += "(!)MAC不符:"+refMac;
					ERRC = 0x40;
				}
			}else{
				if(mac1 != mac2){
					if(mac1 == _T("000000000000") || mac1 == ""){
						if(mac2!= refMac){
							plMAC2->Font->Color = clRed;
							labMSG("(!)MAC不符 on flash2",clRed);
							ErrorMsg += "(!)MAC2不符:"+refMac;
							ERRC = 0x40;
						}
					}else if(mac2 == _T("000000000000") || mac2 == ""){
						if(mac1 != refMac){
							plMAC->Font->Color = clRed;
							labMSG("(!)MAC不符 on flash1",clRed);
							ErrorMsg += "(!)MAC1不符:"+refMac;
							ERRC = 0x40;
						}
					}else{
						plMAC->Font->Color = clRed;
						plMAC2->Font->Color = clRed;
						labMSG("(!)MAC異常",clRed);
						ErrorMsg += "(!)MAC異常:"+refMac;
						ERRC = 0x40;
					}
				}else if(mac1 != _T("000000000000") || mac1 == ""){
					plMAC->Font->Color = clRed;
					plMAC2->Font->Color = clRed;
					labMSG("(!)MAC 異常",clRed);
					ErrorMsg += "(!)MAC異常:"+refMac;
					ERRC = 0x40;
				}
			}
		}
		if(ckbFWVerify->Checked)
		{
			labMSG("驗證韌體版本..",clYellow);
			if(min1 != pl_Sample_MinFW->Caption){
				plMinFWVer->Font->Color = clRed;
				labMSG("(!)Min FW Version 不符",clRed);
				ErrorMsg += "(!)Min FW Version 不符";
				ERRC = 0x40;
			}
			if(m_dut._Kind == 2)
			{
				if(min2 != pl_Sample_MinFW2->Caption){
					plMinFWVer2->Font->Color = clRed;
					labMSG("(!)Min FW Version 不符 on Chip 2",clRed);
					ErrorMsg += "(!)Min FW Version 不符on Chip 2";
					ERRC = 0x40;
				}
			}
			//確認MAC 、 SN是否為空值
			if(!ERRC)
			{
				if(m_dut._Kind == 1)
				{
					if(plUSBSN->Caption=="" || plMAC->Caption=="")
					{
						labMSG("(!)MAC、SN NULL",clRed);
						USBHIDForm->ErrorMsg = "(!)MAC、SN 出現空值";
						ERRC = 0x40;
					}
				}
				else if(m_dut._Kind == 2)
				{
					if(plUSBSN->Caption=="" || plUSBSN2->Caption=="")
					{
						labMSG("(!)MAC、SN NULL",clRed);
						USBHIDForm->ErrorMsg = "(!)MAC、SN 出現空值";
						ERRC = 0x40;
					}
					if(plMAC->Caption==""&&plMAC2->Caption=="")
					{
						labMSG("(!)MAC、SN NULL",clRed);
						USBHIDForm->ErrorMsg = "(!)MAC、SN 出現空值";
						ERRC = 0x40;
					}
				}
			}
		}
		if(!ERRC)
		{
			labMSG("驗證正確",clYellow);
		}
	}
	return(ERRC);
}
// ---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::btnAutoTestClick(TObject *Sender) {
	btnAutoTest->Enabled = false;
	if(btnAutoTest->Caption=="Start")
	{
		WriteINIFile();
		bool bError = false;
		if(ckbFWVerify->Checked)
		{
			if(pl_Sample_MinFW->Caption == "")
				bError = true;
			if(cmbNumOfChip->Text.ToInt()==2)
				if(pl_Sample_MinFW2->Caption == "")
					bError = true;
		}
		if(bError)
			MessageBoxA(NULL,"未載入完整Firmware樣本資訊","WARNING", MB_ICONEXCLAMATION);
		else
		{
			// 按下Start同時將設定值寫進LOG檔
			writeLOG("",true);
			//
			SetUIEnable(false);

			labMSG("等候中",clSkyBlue);
			btnAutoTest->Caption="Stop";
			//
			dThreadStatus = THREAD_START;
		}
	}
	else
	{
		//WORKThread->Terminate(); 異常(待研究)
		USBHIDForm->dThreadStatus = THREAD_STOP;
		while(true)
		{
			if(USBHIDForm->dThreadStatus == THREAD_WIAT)
				break;
			Delay(500);
		}

		SetUIEnable(true);
		ckbBarcodeClick(NULL);
		ckbBarcodeVerifyClick(NULL);

		btnAutoTest->Caption="Start";
		bPlugIn = false;
		btnAutoTest->Enabled = true;
	}
}

// ---------------------------------------------------------------------------
//UI MAC or SN跳號
void TUSBHIDForm::AutoIncrease(){
	AnsiString SS, SD, SN, MAC;
	int tmp;

	if(ckbAutoIncrease->Checked && m_bAutoIncrease) {
		m_bAutoIncrease = false;

		tmp = mskedtBurnSn->Text.ToInt();
		if(ckbBurnSN->Checked)
		{
			tmp++;
			SS = IntToStr(tmp);
			SD = "";

			for (int j = 0; j < (8 - SS.Length()); j++) {
				SD += "0";
			}

			SD += SS.c_str();
			mskedtBurnSn->Text = SD;
		}
		tmp = HexToInt(mskedtBurnMac->Text);
		if(ckbBurnMAC->Checked)
		{
			tmp++;
			SD = IntToHex(tmp, 6);
			mskedtBurnMac->Text = SD;
		}
	}
}
// ---------------------------------------------------------------------------
//將DUT值填入UI
bool TUSBHIDForm::GetDutInfomation(bool bLOAD_DEV){
	//設定抓資料的秒數
	m_dut.SetDutMaxTimeGetId(10);
	//Chip個數
	m_dut.SetKind(cmbNumOfChip->ItemIndex+1);

	if(!m_dut.GetId()){
		lbNumberOfChip->Font->Color = clRed;
		return false;
    }

	if(!m_dut.GetInfo()){
		memoMsg->Lines->Add(DUT_STRING_NOT_FIND);
		return false;
	}
	memoMsg->Lines->Add(DUT_STRING_INFO_FLASH);
	memoMsg->Lines->Add(_T("------------------------------------------------------------"));
	memoMsg->Lines->Add(DUT_STRING_NUM_OF_CHIP + IntToStr(m_dut._Kind));
	memoMsg->Lines->Add(_T("------------------------------------------------------------"));
	memoMsg->Lines->Add(m_dut._Dev1);
	memoMsg->Lines->Add(_T("------------------------------------------------------------"));
	memoMsg->Lines->Add(DUT_FT + m_dut._If1.FirmwareType);
	memoMsg->Lines->Add(DUT_RL + m_dut._If1.ReleaseLevel);
	memoMsg->Lines->Add(DUT_VN + m_dut._If1.Version);
	memoMsg->Lines->Add(DUT_SV + m_dut._If1.SvnVersion);
	memoMsg->Lines->Add(DUT_BD + m_dut._If1.BuildDate);
	memoMsg->Lines->Add(DUT_TE + m_dut._If1.Time);
	memoMsg->Lines->Add(DUT_BF + m_dut._If1.BackFirmware);
	memoMsg->Lines->Add(DUT_EN + m_dut._If1.Name);
	memoMsg->Lines->Add(DUT_ED + m_dut._If1.Date);
	memoMsg->Lines->Add(DUT_EM + m_dut._If1.MinFwVersion);
	memoMsg->Lines->Add(DUT_MAC + m_dut._If1.MAC);
	memoMsg->Lines->Add(DUT_SN + m_dut._If1.SN);
	//otp
	AnsiString SS = DosCommand("dl_mfg_test info otp --dev "+m_dut._Dev1);
	SS = SS.SubString(19,SS.Pos("\r")-19).Trim();
	memoMsg->Lines->Add("OTP SN :"+SS);
	if(SS.Length()>5)
		g_OTP_SN = SS;
	//
	plChipName->Caption = "DisplayLink Chip 1:"+m_dut._Dev1;
	plMinFWVer->Caption = m_dut._If1.MinFwVersion;
	plMAC->Caption = m_dut._If1.MAC;
	plUSBSN->Caption = m_dut._If1.SN;
	if(bLOAD_DEV)
	{
		pl_Sample_MinFW->Caption = m_dut._If1.MinFwVersion;
	}

	if(m_dut._If1.HDCP != ""){
		memoMsg->Lines->Add(DUT_HDCP + m_dut._If1.HDCP);
	}

	memoMsg->Lines->Add(m_dut._If1.EDID);

	if(m_dut._Kind == 2){
		memoMsg->Lines->Add(_T("------------------------------------------------------------"));
		memoMsg->Lines->Add(m_dut._Dev2);
		memoMsg->Lines->Add(_T("------------------------------------------------------------"));
		memoMsg->Lines->Add(DUT_FT + m_dut._If2.FirmwareType);
		memoMsg->Lines->Add(DUT_RL + m_dut._If2.ReleaseLevel);
		memoMsg->Lines->Add(DUT_VN + m_dut._If2.Version);
		memoMsg->Lines->Add(DUT_SV + m_dut._If2.SvnVersion);
		memoMsg->Lines->Add(DUT_BD + m_dut._If2.BuildDate);
		memoMsg->Lines->Add(DUT_TE + m_dut._If2.Time);
		memoMsg->Lines->Add(DUT_BF + m_dut._If2.BackFirmware);
		memoMsg->Lines->Add(DUT_EN + m_dut._If2.Name);
		memoMsg->Lines->Add(DUT_ED + m_dut._If2.Date);
		memoMsg->Lines->Add(DUT_EM + m_dut._If2.MinFwVersion);
		memoMsg->Lines->Add(DUT_MAC + m_dut._If2.MAC);
		memoMsg->Lines->Add(DUT_SN + m_dut._If2.SN);
		//otp
		SS = DosCommand("dl_mfg_test info otp --dev "+m_dut._Dev2);
		SS = SS.SubString(19,SS.Pos("\r")-19).Trim();
		memoMsg->Lines->Add("OTP SN :"+SS);
		if(SS.Length()>5)
			g_OTP_SN_2 = SS;
		//
		plChipName2->Caption = "DisplayLink Chip 2:"+m_dut._Dev2;
		plMinFWVer2->Caption = m_dut._If2.MinFwVersion;
		plMAC2->Caption = m_dut._If2.MAC;
		plUSBSN2->Caption = m_dut._If2.SN;

		if(bLOAD_DEV)
		{
			pl_Sample_MinFW2->Caption = m_dut._If2.MinFwVersion;
		}
		if(m_dut._If2.HDCP != ""){
			memoMsg->Lines->Add(DUT_HDCP + m_dut._If2.HDCP);
		}

		memoMsg->Lines->Add(m_dut._If2.EDID);
	}
	USBHIDForm->Refresh();
	return true;
}
//---------------------------------------------------------------------------
bool TUSBHIDForm::GetDutInfo_DriverCheck(bool bDEV_LOAD)
{
	btnLoadDutVersion->Enabled = false;
	btnAutoTest->Enabled = false;
	btnGetInfo->Enabled = false;
	lbNumberOfChip->Font->Color = clBlack;
	ClearDevInfo();
	labMSG("取得裝置資料中...",clYellow);

	bool bResult = true;
	if(bWin10&&bDEV_LOAD)
		if(!CheckWin10Driver()) bResult = false;
	if(bResult)
	{
		memoMsg->Clear();
		memoMsg->Refresh();

		btnGetInfo->Enabled = false;
		if(!GetDutInfomation(bDEV_LOAD)){
			labMSG("(!)Load Data Fail",clRed);
			ErrorMsg = "(!)Load Data Fail";
			bResult = false;
		}
	}
	if(!bResult)
	{
		labMSG("查看裝置資訊失敗",clRed);
		ErrorMsg = "(!)查看裝置資訊失敗";
	}
	return bResult;
}
//---------------------------------------------------------------------------
void __fastcall TUSBHIDForm::btnLoadDutVersionClick(TObject *Sender)
{
	bool bHaveDev = false;
	AnsiString Resultdata = DosCommand("devcon find \"\*usb\\vid_17e9\*\"");
	char *pch = strtok(Resultdata.c_str(), "\r\n");
	while (pch != NULL)
	{
		if(strstr(pch,"MI_00")||strstr(pch,"MI_01"))
		{
			SetRegDriver(AnsiString(pch).SubString(6,17));
			bHaveDev =true;
		}
		else if(strstr(AnsiString(pch).c_str(),"No matching devices found"))
		{
			bHaveDev = false;
		}
		pch = strtok(NULL, "\r\n");
	}

	if(bHaveDev)
	{
		cmbNumOfChip->Enabled = false;
		btnLoadDutVersion->Enabled = false;
		btnAutoTest->Enabled = false;
		btnGetInfo->Enabled = false;

		bool bResult = true;
		TButton *btn = (TButton*)Sender;
		if(bWin10)
			if(!CheckWin10Driver()) bResult = false;
		if(bResult)
		{
			if(btn->Name.Pos("btnLoadDutVersion"))			{				pl_Sample_MinFW->Caption="";				pl_Sample_MinFW2->Caption="";				GetDutInfo_DriverCheck(true);			}			else				GetDutInfo_DriverCheck(false);			labMSG("等候中",clSkyBlue);		}
		else
		{
			labMSG("驅動安裝失敗",clRed);
			ErrorMsg = "(!)驅動安裝失敗";
		}

		cmbNumOfChip->Enabled = true;
		btnLoadDutVersion->Enabled = true;
		btnAutoTest->Enabled = true;
		btnGetInfo->Enabled = true;
	}
	else
	{
		MessageBoxA(NULL, "找不到裝置", "無法顯示裝置資訊", MB_ICONWARNING);
	}
}
//---------------------------------------------------------------------------



bool TUSBHIDForm::CheckWin10Driver()
{
	//列舉Display產品
	memoMsg->Clear();
	AnsiString Resultdata = DosCommand("devcon find \"\*usb\\vid_17e9\*\"");
	char *pch = strtok(Resultdata.c_str(), "\r\n");
	TStringList* NList = new TStringList();
	memoMsg->Lines->Add("Enum Display Chip...");
	while (pch != NULL)
	{
		memoMsg->Lines->Add(pch);
		if(strstr(pch,"MI_00")||strstr(pch,"MI_01"))
			NList->Add(pch);
		pch = strtok(NULL, "\r\n");
		if(strstr(AnsiString(memoMsg->Text).c_str(),"No matching devices found"))
		{
			delete NList;
			return false;
		}
	}
	USBHIDForm->Refresh();
	if(cmbNumOfChip->Text.ToInt()==1&&NList->Count>2)
	{
		memoMsg->Lines->Add("Chip num Error");
		delete NList;
		return false;
	}
	else if(cmbNumOfChip->Text.ToInt()==2&&NList->Count>4)
	{
		memoMsg->Lines->Add("Chip num Error");
		delete NList;
		return false;
	}
	//確認是否在USBIO模式
	bool bResult = true;
	for(int x=0;x<NList->Count;x++)
	{
		if(!strstr(AnsiString(NList->Strings[x]).c_str(),"USBIO"))
			bResult = false;
		memoMsg->Lines->Add(NList->Strings[x]);
		int usb_keyword = NList->Strings[x].Pos("USB");
		int usb_keyword_end = (NList->Strings[x].Pos("MI")+4)-usb_keyword+1;
		NList->Strings[x] = NList->Strings[x].SubString(usb_keyword,usb_keyword_end);
	}
	if(bResult)
	{
		memoMsg->Lines->Add("DisplayLink Chip Pass");
		delete NList;
		return true;
	}
	//灌驅動
	memoMsg->Lines->Add("USBIO install...");
	labMSG("USBIO驅動安裝..",clYellow);
	for(int x=0;x<NList->Count;x++)
	{

		AnsiString cmd = "devcon updateni usbio_driver\\DisplayLinkUSBIOMTT.inf \""+NList->Strings[x]+"\"";
		memoMsg->Lines->Add("devcon updateni usbio_driver\\DisplayLinkUSBIOMTT.inf \""+NList->Strings[x]+"\"");
		Resultdata = DosCommand(cmd);
		if(!strstr(Resultdata.c_str(),"Drivers installed successfully"))
		{
			memoMsg->Lines->Add(NList->Strings[x]);
			memoMsg->Lines->Add(Resultdata);
			memoMsg->Lines->Add("USBIO driver installation failed");
			return false;
		}
	}
	//重新抓裝置
	memoMsg->Lines->Add("restart device...");
	DosCommand("devcon restart \"\*usb\\vid_17e9\*\"");
	delete NList;

	//再次列舉Display產品
	memoMsg->Lines->Add("enum device...");
	Resultdata = DosCommand("devcon find \"\*usb\\vid_17e9\*\"");
	char *pch2 = strtok(Resultdata.c_str(), "\r\n");
	TStringList* NList2 = new TStringList();
	while (pch2 != NULL)
	{
		if(strstr(pch2,"MI_00")||strstr(pch2,"MI_01"))
			NList2->Add(pch2);
		pch2 = strtok(NULL, "\r\n");
	}
	//確認是否在USBIO模式
	bResult = true;
	for(int x=0;x<NList2->Count;x++)
	{
		if(!strstr(AnsiString(NList2->Strings[x]).c_str(),"USBIO"))
		{
			if(!strstr(AnsiString(NList2->Strings[x]).c_str(),"Test Mode"))
				bResult = false;
			memoMsg->Lines->Add(NList2->Strings[x]);
		}
		else memoMsg->Lines->Add(NList2->Strings[x]);
	}
	delete NList2;
	if(bResult)
	{
		memoMsg->Lines->Add("USBIO driver installation Successful");
		return true;
	}
	else
	{
		memoMsg->Lines->Add("USBIO driver installation failed");
		return false;
	}
}
AnsiString TUSBHIDForm::DosCommand(AnsiString sCmdline)
{
	PROCESS_INFORMATION proc = {0}; //關於進程資訊的一個結構
	long ret;
	bool sPipe;
	STARTUPINFO start = {0};
	SECURITY_ATTRIBUTES sa = {0};
	HANDLE hReadPipe ;
	HANDLE hWritePipe;
	AnsiString sOutput;
	AnsiString sBuffer;
	unsigned long lngBytesRead;
	char cBuffer[256];
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor=0;
	sa.bInheritHandle = TRUE;
	sPipe=::CreatePipe(&hReadPipe, &hWritePipe,&sa, 0); //創建管道
	if (!sPipe)
	{
	sOutput="CreatePipe failed. Error: "+AnsiString(GetLastError());
	memoMsg->Lines->Add("CreatePipe failed. Error: "+AnsiString(GetLastError()));
	return sOutput;
	}
	start.cb = sizeof(STARTUPINFOA);
	start.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	start.hStdOutput = hWritePipe;
	start.hStdError = hWritePipe;
	start.wShowWindow = SW_HIDE;
	sBuffer = sCmdline;
	ret =::CreateProcess(0, sBuffer.c_str(), &sa, &sa, TRUE, NORMAL_PRIORITY_CLASS, 0, 0, &start, &proc);
	if (ret == 0)
	{
	sOutput="Bad command or filename";
	memoMsg->Lines->Add("Bad command or filename");
	return sOutput;
	}
	::CloseHandle(hWritePipe);
	if(!sCmdline.Pos("DisplayLinkUSBIOMTT.inf"))
	{
		DWORD dw = WaitForSingleObject(proc.hProcess, 10000);
		if(dw == WAIT_TIMEOUT)
		{
			::CloseHandle(proc.hProcess);
			::CloseHandle(proc.hThread);
			::CloseHandle(hReadPipe);
			return "";
		}
	}
	do
	{
	memset(cBuffer,'\0',sizeof(cBuffer));
	ret = ::ReadFile(hReadPipe, &cBuffer, 255, &lngBytesRead, 0);
	//替換字串
	for(unsigned long i=0; i< lngBytesRead; i++){
		if(cBuffer[i] == '\0'){
			cBuffer[i] = ' ';
		}else if(cBuffer[i] == '\n'){
			cBuffer[i] = ' ';
		}
	}
	sBuffer=StrPas(cBuffer);
	sOutput = sOutput+sBuffer;
	Application->ProcessMessages();

	} while (ret != 0 );
	::CloseHandle(proc.hProcess);
	::CloseHandle(proc.hThread);
	::CloseHandle(hReadPipe);
	return sOutput;
}
void TUSBHIDForm::RemoveDisplayLinkDriver()
{
	btnAutoTest->Enabled = false;
	HKEY hSoftKey = NULL;
	AnsiString tempDeviceGUID;
	LONG lResult;
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\DisplayLink\\Core", 0, KEY_READ|KEY_WOW64_64KEY,&hSoftKey);
	if(lResult  == ERROR_SUCCESS)
	{
		DWORD dwType=REG_SZ;
		DWORD dwCount=0;
		LONG lResult = RegQueryValueEx(hSoftKey, "GUID", NULL, &dwType,NULL, &dwCount);
		if (lResult == ERROR_SUCCESS)
		{
			char* company_Get=new char[100];
			lResult = RegQueryValueEx(hSoftKey, "GUID", NULL, &dwType,
			company_Get, &dwCount);
			if(lResult != ERROR_SUCCESS)
				tempDeviceGUID = "";
			else
				tempDeviceGUID = (AnsiString((char*)company_Get));

			delete[] company_Get;
		}
	}

	if (hSoftKey != NULL)
		RegCloseKey(hSoftKey);
	if(tempDeviceGUID!="")
	{
		FILE * fp;
		fp = fopen("uninstall.bat","w");
		fseek( fp, 0, SEEK_END);
		tempDeviceGUID = "MsiExec.exe /X"+tempDeviceGUID+" /norestart";
		fwrite(tempDeviceGUID.c_str(),tempDeviceGUID.Length(),1,fp); //寫入一筆資料
		fclose(fp);
		labMSG("解除驅動..",clYellow);
		DosCommand("uninstall.bat");
		labMSG("等候中..",clSkyBlue);
	}
	AnsiString Resultdata = DosCommand("devcon dp_enum");
	AnsiString OemNum = "";
	char *pch = strtok(Resultdata.c_str(), "\r\n");
	TStringList* NList = new TStringList();
	while (pch != NULL)
	{
		if(strstr(pch,"oem"))
			OemNum = AnsiString(pch);
		if(strstr(pch,"DisplayLink"))
			NList->Add(OemNum);
		pch = strtok(NULL, "\r\n");
	}
	for(int i=0;i<NList->Count;i++)
	{
		memoMsg->Lines->Add(NList->Strings[i]);
		OemNum = NList->Strings[i];
		OemNum = OemNum.SubString(OemNum.Pos("oem"),OemNum.Length()-OemNum.Pos("oem")+1);
		DosCommand("devcon -f dp_delete "+OemNum);
	}
	delete NList;
	btnAutoTest->Enabled = true;
}


void TUSBHIDForm::ClearDevInfo()
{
	plChipName->Caption = "DisplayLink Chip 1";
	plChipName2->Caption = "DisplayLink Chip 2";
	plMinFWVer->Caption = "";
	plMinFWVer2->Caption = "";
	plUSBSN->Caption = "";
	plUSBSN2->Caption = "";
	plMAC->Caption = "";
	plMAC2->Caption = "";
	plMinFWVer->Font->Color = clBlue;
	plMinFWVer2->Font->Color = clBlue;
	plUSBSN->Font->Color = clBlue;
	plUSBSN2->Font->Color = clBlue;
	plMAC->Font->Color = clBlue;
	plMAC2->Font->Color = clBlue;
}


void __fastcall TUSBHIDForm::plSwitchClick(TObject *Sender)
{
	if(plSwitch->Caption == ">")
	{
		plSwitch->Caption = "<";
		USBHIDForm->Width = 1295;
	}
	else
	{
		plSwitch->Caption = ">";
		USBHIDForm->Width = 841;
	}
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::cmbNumOfChipChange(TObject *Sender)
{
	if(cmbNumOfChip->Text.ToInt()==2)
	{
		if(ckbBurnMAC->Checked) ckbBurnSN->Checked = true;
		if(ckbBurnSN->Checked) ckbBurnMAC->Checked = true;
		plChipName2->Caption = "Chip 2";
		Panel12->Height = 365;
		gbUSBHUB->Height = 283;
		USBHIDForm->Height = 615;
		Image1->Align = alNone;
		Image1->Top = 399;
	}
	else
	{
		plChipName2->Caption = "-";
		Panel12->Height = 180;
		gbUSBHUB->Height = 193;
		USBHIDForm->Height = 521;
		Image1->Align = alClient;

	}
}
//---------------------------------------------------------------------------
bool TUSBHIDForm::writeLOG(AnsiString Msg,bool bAction) {
	bool bPASS[4] = {true,true,true,true};
	AnsiString time = FormatDateTime("mm-dd-yyyy hh:nn:ss", Now());
	AnsiString SS;
	if(bAction)
	{
		AnsiString Auto = ckbAutoIncrease->Checked ? "true":"false";
		AnsiString burn_sn = ckbBurnSN->Checked ? "true":"false";
		AnsiString burn_mac = ckbBurnMAC->Checked ? "true":"false";
		AnsiString barcode_burn = ckbBarcode->Checked ? "true":"false";
		AnsiString barcode_verify = ckbBarcodeVerify->Checked ? "true":"false";
		AnsiString fw_verify = ckbFWVerify->Checked ? "true":"false";
		AnsiString sn = mskedtBurnSn->Text;
		AnsiString mac = mskedBurntMacPre->Text+mskedtBurnMac->Text;
		AnsiString min_fw = pl_Sample_MinFW->Caption;
		AnsiString min_fw2 = pl_Sample_MinFW2->Caption;
		AnsiString Setting = "\r\n===========\r\n "
							+time
							+" [Number of Chip]"+cmbNumOfChip->Text
							+" [自動跳號]"+Auto
							+" [燒錄SN]"+burn_sn
							+" [燒錄MAC] "+burn_mac+"\r\n"
							+" [SN]"+sn+" [MAC]"+mac
							+" [Barcode MAC燒錄]"+barcode_burn
							+" [Barcode MAC驗證]"+barcode_verify
							+" [驗證韌體版本]"+fw_verify;
		Setting+=" [MinFW]"+min_fw;
		if(cmbNumOfChip->Text.ToInt()==2)
			Setting+=" [MinFW2]"+min_fw2;
		Setting+=" [操作人員工號]"+edtEmployeeID->Text+"\r\n==========\r\n";
		//
		bPASS[0] = SaveUserLogLine(RDLOGFilePath,Setting);
		if(LOGDiskPath.Pos("csv") && ImgDisk->Visible)
		{
		   bPASS[1] = 	SaveUserLogLine(RDLOGDiskPath,Setting);
		}
		for(int i = 0 ; i < 2 ; i++)
			if(!bPASS[i]) return false;
	}
	else
	{
		AnsiString sn="NULL",mac="NULL",minver="NULL",Otp="NULL"
					,sn2="NULL",mac2="NULL",minver2="NULL",TestNUM="";
		double test_time = double(GetTickCount() - TestTime)/(double)1000;
		USBHIDForm->Refresh();
		minver = plMinFWVer->Caption;
		sn = plUSBSN->Caption;
		mac = plMAC->Caption;
		minver2 = plMinFWVer2->Caption;
		sn2 = plUSBSN2->Caption;
		mac2 = plMAC2->Caption;


		if(g_OTP_SN.Length()<5)
		{
			for(int i =0 ; i<3 ;i++)
			{
				AnsiString CMD_SS = DosCommand("dl_mfg_test info otp --dev "+m_dut._Dev1);
				Otp    = GetStringLine(CMD_SS, "Serial Number");
				Otp	   = Otp.SubString(19,Otp.Length()).Trim();
				if(Otp.Length()>5)
				{
					g_OTP_SN = Otp;
					break;
				}
				Delay(300);
			}
			ErrorMsg +="(!)OTP撈取失敗";
			return false;
		}
		if(cmbNumOfChip->Text.ToInt()==2&&g_OTP_SN_2.Length()<5)
		{
			for(int i =0 ; i<3 ;i++)
			{
				AnsiString CMD_SS = DosCommand("dl_mfg_test info otp --dev "+m_dut._Dev2);
				Otp    = GetStringLine(CMD_SS, "Serial Number");
				Otp	   = Otp.SubString(19,Otp.Length()).Trim();
				if(Otp.Length()>5)
				{
					g_OTP_SN_2 = Otp;
					break;
				}
				Delay(300);
			}
			ErrorMsg +="(!)OTP2撈取失敗";
			return false;
		}
		if(Msg=="PASS")
		{
			dwTestCount++;
			plCount->Caption = dwTestCount;
		}
		TestNUM.sprintf("%04d",dwTestCount);
		if(cmbNumOfChip->Text.ToInt()==1)
		{
			if(Msg=="PASS")
			{
				SS = TestNUM+",[PASS],"+time + ",[SN],"+sn+",[MAC],"+mac+",[MinFW],"+minver+",[OTP-SN],"+g_OTP_SN;
				SS+= ",[MACStartCode],"+mskedBurntMacStart->Text+",[MACLastCode],"+mskedBurntMacLast->Text+",[WorkOrder],"+edtWorkOrderNumber->Text	+",[EmployeeID],"+edtEmployeeID->Text;
				SS += ",[TEST-TIME],"+AnsiString(test_time)+"\n";
			}
			else
			{
				SS = TestNUM+",[FAIL],"+time + ",[SN],"+sn+",[MAC],"+mac+",[MinFW],"+minver+",[OTP-SN],"+g_OTP_SN;
				SS+= ",[MACStartCode],"+mskedBurntMacStart->Text+",[MACLastCode],"+mskedBurntMacLast->Text+",[WorkOrder],"+edtWorkOrderNumber->Text	+",[EmployeeID],"+edtEmployeeID->Text;
				SS += ",[TEST-TIME],"+AnsiString(test_time)+",[ERROR],"+Msg+"\n";
			}
		}
		else
		{
			if(Msg=="PASS")
			{
				SS = TestNUM+",[PASS],"+time + ",[SN],"+sn+",[MAC],"+mac+",[MinFW],"+minver+",[OTP-SN],"+g_OTP_SN
				+ ",[SN2],"+sn2+",[MAC2],"+mac2+",[MinFW2],"+minver2+",[OTP-SN2],"+g_OTP_SN_2;
				SS+= ",[MACStartCode],"+mskedBurntMacStart->Text+",[MACLastCode],"+mskedBurntMacLast->Text+",[WorkOrder],"+edtWorkOrderNumber->Text	+",[EmployeeID],"+edtEmployeeID->Text;
				SS += ",[TEST-TIME],"+AnsiString(test_time)+"\n";
			}
			else
			{
				SS = TestNUM+",[FAIL],"+time + ",[SN],"+sn+",[MAC],"+mac+",[MinFW],"+minver+",[OTP-SN],"+g_OTP_SN
				+ ",[SN2],"+sn2+",[MAC2],"+mac2+",[MinFW2],"+minver2+",[OTP SN2],"+g_OTP_SN_2;
				SS+= ",[MACStartCode],"+mskedBurntMacStart->Text+",[MACLastCode],"+mskedBurntMacLast->Text+",[WorkOrder],"+edtWorkOrderNumber->Text	+",[EmployeeID],"+edtEmployeeID->Text;
				SS += ",[TEST-TIME],"+AnsiString(test_time)+",[ERROR],"+Msg+"\n";
			}

		}
		bPASS[0] = SaveUserLogLine(RDLOGFilePath,SS);
		bPASS[1] = SaveUserLogLine(LOGFilePath,SS);
		if(LOGDiskPath.Pos("csv") && ImgDisk->Visible )
		{
			bPASS[2] = SaveUserLogLine(RDLOGDiskPath,SS);
			bPASS[3] = SaveUserLogLine(LOGDiskPath,SS);
			if(!bPASS[2] || !bPASS[3])
				ImgDisk->Visible = false;
		}
		for(int i = 0 ; i < 4 ; i++)
			if(!bPASS[i]) return false;
	}
	return true;
}

bool  TUSBHIDForm::SaveUserLogLine(AnsiString FileName,AnsiString FileLine)
{
	FILE * fp;
	fp = fopen(FileName.c_str(),"a+");
	if(NULL == fp)
		return false;
	fseek( fp, 0, SEEK_END);
	fwrite(FileLine.c_str(),FileLine.Length(),1,fp); //寫入一筆資料
	fclose(fp);
	return true;
}
AnsiString TUSBHIDForm::GetStringLine(AnsiString String,AnsiString Keyword) {
	char *pch = strtok(String.c_str(), "\r\n");
	TStringList* NList = new TStringList();
	AnsiString SS;
	while (pch != NULL)
	{
		SS = AnsiString(pch);

		if(SS.Pos(Keyword))
		{
			delete NList;
			return SS;
		}
		pch = strtok(NULL, "\r\n");
	}
	delete NList;
	return "";
}




void __fastcall TUSBHIDForm::LOGFILEClick(TObject *Sender)
{
	ShellExecute(NULL,NULL,LOGFilePath.c_str(),NULL,NULL,SW_SHOW);
}
//---------------------------------------------------------------------------


void __fastcall TUSBHIDForm::ckbBurnSNClick(TObject *Sender)
{
	TCheckBox * ckb = (TCheckBox *) Sender;
	if(cmbNumOfChip->Text == 1)
	{
		if(!ckbBurnSN->Checked&&!ckbBurnMAC->Checked)
			ckbAutoIncrease->Checked = false;
	}
	else
	{
		if(ckb->Name.Pos("SN"))
			ckbBurnMAC->Checked = ckb->Checked;
		else ckbBurnSN->Checked  = ckb->Checked;
	}
}
//---------------------------------------------------------------------------

bool TUSBHIDForm::CheckDiskName()
{
	DWORD DiskInfo = GetLogicalDrives();
	AnsiString DiskName,DiskVerifty = "DEVICE_LOG",SS;
	char Disk;
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
				LOGDiskPath = DiskName+"\\ASMP\\log\\"+edtWorkOrderNumber->Text+"\\DisplayLinkBurnTool";
				if(!DirectoryExists(LOGDiskPath.c_str())){
					_mkdir( DiskName.c_str());
					SS = DiskName + "\\ASMP";
					_mkdir( SS.c_str());
					SS = DiskName + "\\ASMP\\log";
					_mkdir( SS.c_str());
					SS = DiskName + "\\ASMP\\log\\"+edtWorkOrderNumber->Text;
					_mkdir(SS.c_str());
					_mkdir( LOGDiskPath.c_str());
				}
				return true;
			}
		}
	}
	return false;
}
void __fastcall TUSBHIDForm::edtWorkOrderNumberExit(TObject *Sender)
{
	if(edtWorkOrderNumber->Text=="")
		edtWorkOrderNumber->Text = "000-00000000000";
}
//---------------------------------------------------------------------------

int __fastcall TUSBHIDForm::HexToInt(AnsiString HexStr) {
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
//---------------------------------------------------------------------------
void TUSBHIDForm::Delay(DWORD iMilliSeconds) // 原版delay time 用在thread裡面
{
	DWORD iStart;
	iStart = GetTickCount();
	while (GetTickCount() - iStart <= iMilliSeconds)
		Application->ProcessMessages();
}
void TUSBHIDForm::SetUIEnable(bool Enable)
{
	btnLoadDutVersion->Enabled = Enable;
	cmbNumOfChip->Enabled = Enable;
	ckbFWVerify->Enabled = Enable;
	ckbOTP->Enabled = Enable;
	ckbAutoIncrease->Enabled = Enable;
	ckbBurnSN->Enabled = Enable;
	ckbBurnMAC->Enabled = Enable;
	mskedtBurnSn->Enabled = Enable;
	mskedBurntMacPre->Enabled = Enable;
	mskedtBurnMac->Enabled = Enable;
	btnGetInfo->Enabled = Enable;
	edtEmployeeID->Enabled = Enable;
	ckbBarcode->Enabled = Enable;
	ckbBarcodeVerify->Enabled = Enable;
}


void __fastcall TUSBHIDForm::ckbBarcodeClick(TObject *Sender)
{
	if(ckbBarcode->Checked)
	{
		ckbBurnSN->Checked = true;
		ckbBurnSN->Enabled = false;
		ckbBurnMAC->Checked = true;
		ckbBurnMAC->Enabled = false;
		ckbAutoIncrease->Checked = false;
		ckbAutoIncrease->Enabled = false;
	}
	else
	{
		ckbBurnSN->Enabled = true;
		ckbBurnMAC->Enabled = true;
		ckbAutoIncrease->Enabled = ckbBarcodeVerify->Checked ? false:true;
	}
}
//---------------------------------------------------------------------------


void __fastcall TUSBHIDForm::ckbBarcodeVerifyClick(TObject *Sender)
{
	if(ckbBarcodeVerify->Checked)
	{
	   ckbAutoIncrease->Checked = false;
	   ckbAutoIncrease->Enabled = false;
	   ckbFWVerify->Checked = true;
	   ckbFWVerify->Enabled = false;
	}
	else
	{
	   ckbAutoIncrease->Enabled = true;
	   ckbAutoIncrease->Enabled = ckbBarcode->Checked ? false:true;
	   ckbFWVerify->Enabled = true;
	}
}
void __fastcall TUSBHIDForm::btnBarcodeClearClick(TObject *Sender)
{
	edtBarcodeMAC->Text = "";
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::edtBarcodeMACChange(TObject *Sender)
{
	TEdit* edt=(TEdit*)Sender;
	TCHAR HexChar[15];

	edt->Text = edt->Text.UpperCase();// 把字串轉成全大寫
	_tcscpy_s(HexChar, 15, AnsiString(edt->Text).c_str());
	for(int i=0;i <edt->Text.Length();i++)
	{
		if(HexChar[i] < 0x30 || HexChar[i] > 0x39)//非數字
		{
			if(HexChar[i] < 0x41 || HexChar[i] > 0x46)//非大寫字母a~f
			{
				edtBarcodeMAC->Text =
					edtBarcodeMAC->Text.Delete(edtBarcodeMAC->Text.Length(),1);
            }
		}
	}
	if(edtBarcodeMAC->Text.Length()> 12)
	  edtBarcodeMAC->Text = "";
	edt-> SelStart=edt-> Text.Length();
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::edtBarcodeMACKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(edtBarcodeMAC->Text.Length()==12&&Key==13)
	{
		USBHIDForm->mskedtBurnSn->Text = edtBarcodeMAC->Text;
		USBHIDForm->mskedBurntMacPre->Text = edtBarcodeMAC->Text.SubString(1,6);
		USBHIDForm->mskedtBurnMac->Text = edtBarcodeMAC->Text.SubString(7,6);
		numBarcodeResult = BARCODE_FINISH;
	}
	else if(edtBarcodeMAC->Text.Length()<12&&Key==13)
		edtBarcodeMAC->Text = "";
	else if(Key == 8)
		edtBarcodeMAC->Text = "";
}
//---------------------------------------------------------------------------

void __fastcall TUSBHIDForm::btnBarcodeChanelClick(TObject *Sender)
{
	numBarcodeResult = BARCODE_CHANEL;
}
bool  TUSBHIDForm::CheckMinFW()
{
	bool bPass = true;
	if(USBHIDForm->pl_Sample_MinFW->Caption != m_dut._If1.MinFwVersion)
	{
		USBHIDForm->plMinFWVer->Font->Color = clRed;
		labMSG("(!)Min FW",clRed);
		USBHIDForm->ErrorMsg = "(!)Min FW Version 不符";
		bPass = false;
	}
	if(m_dut._Kind == 2)
	{
		if(USBHIDForm->pl_Sample_MinFW2->Caption != m_dut._If2.MinFwVersion)
		{
			USBHIDForm->plMinFWVer2->Font->Color = clRed;
			labMSG("(!)Min FW Chip 2",clRed);
			USBHIDForm->ErrorMsg = "(!)Min FW Version 不符on Chip 2";
			bPass = false;
			return bPass;
		}
	}
	return bPass;
}
void TUSBHIDForm::OnThreadTerminate()
{
   delete WORKThread;  // 釋放記憶體
   WORKThread = NULL;  // 若沒有指為 NULL，仍可讀到訊息，但讀到的訊息可能是錯誤的。
}
void TUSBHIDForm::WriteINIFile()
{
	TIniFile *ini;
	ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));

	//Thired step. 儲存所有設定
	ini->WriteString(_T("KIND"), _T("KIND"), cmbNumOfChip->Text);
	//
	ini->WriteBool(_T("DDR"), _T("DDR_CHECK"), g_bDDR_Checck);
	//BURN_SETTING
	ini->WriteBool("BURN_SETTING", "BURN_SN", ckbBurnSN->Checked);
	ini->WriteBool("BURN_SETTING", "BURN_MAC", ckbBurnMAC->Checked);
	ini->WriteBool("BURN_SETTING", "AUTO", ckbAutoIncrease->Checked);
	ini->WriteBool("BURN_SETTING", "BARCODE", ckbBarcode->Checked);
	ini->WriteBool("BURN_SETTING", "FW_VERIFY", ckbFWVerify->Checked);
	ini->WriteBool("BURN_SETTING", "BARCODE_VERIFY", ckbBarcodeVerify->Checked);
	//BURN_CONTENT
	ini->WriteString(_T("BURN_CONTENT"), _T("BURN_SN"), mskedtBurnSn->Text);
	ini->WriteString(_T("BURN_CONTENT"), _T("MAC_PREFIX"), mskedBurntMacPre->Text);
	ini->WriteString(_T("BURN_CONTENT"), _T("MAC_END"), mskedtBurnMac->Text);
	// FW Ver
	ini->WriteString(_T("FW_VERSION"), _T("MIN_FW_1"), pl_Sample_MinFW->Caption);
	ini->WriteString(_T("FW_VERSION"), _T("MIN_FW_2"), pl_Sample_MinFW2->Caption);
	ini->WriteBool(_T("FW_VERSION"), _T("OTP"), ckbOTP->Checked);
	//WorkOrder
	ini->WriteString(_T("WorkOrder"), _T("WorkOrderNumber"), edtWorkOrderNumber->Text);


	delete ini;
}
void  TUSBHIDForm::SetRegDriver(AnsiString DevVPID)
{
	if(DevVPID != "")
	{
		TRegistry *reg = new TRegistry();
		reg->RootKey = HKEY_LOCAL_MACHINE;
		AnsiString temppath = "SYSTEM\\CurrentControlSet\\Control\\usbflags";
		reg->OpenKey(temppath.c_str(), false);
		AnsiString tempIgnoreHWSerNum = "IgnoreHWSerNum"+DevVPID.SubString(5,4)+DevVPID.SubString(14,4);
		if(!reg->ValueExists (tempIgnoreHWSerNum))
		{
			BYTE buf[1];
			buf[0] = 0x01;
			reg->WriteBinaryData(tempIgnoreHWSerNum,buf,1);
		}
		reg->CloseKey();
		delete reg;
	}
}
void __fastcall TUSBHIDForm::Stop1Click(TObject *Sender)
{
	btnAutoTest->Enabled = false;
	btnAutoTestClick(NULL);
	btnAutoTest->Enabled = true;
}
//---------------------------------------------------------------------------
void TUSBHIDForm::ShowSettingForm()
{
	if(frmMsg==NULL)  frmMsg = new TfrmMsg(Application);

	frmMsg->edtSetWorkOrderNumber->Text = edtWorkOrderNumber->Text;
	frmMsg->mskedBurntMacStart->Text = mskedBurntMacStart->Text;
	frmMsg->mskedBurntMacLast->Text = mskedBurntMacLast->Text;
	frmMsg->edtSetEmployeeID->Text = edtEmployeeID->Text.Trim();
	if(ckbBarcode->Checked)
		frmMsg->plckbBarcode->Caption = "√";
	else
		frmMsg->plckbBarcode->Caption = "";

	if(plTestMode->Caption.Pos("一般"))
	{
		frmMsg->plNormalMode->Color = clLime;
		frmMsg->plReworkMode->Color = clCream;
	}
	else
	{
		frmMsg->plNormalMode->Color = clCream;
		frmMsg->plReworkMode->Color = clLime;
	}

	if(frmMsg->ShowModal()== mrOk)
	{
		edtWorkOrderNumber->Text = frmMsg->edtSetWorkOrderNumber->Text.Trim();
		edtEmployeeID->Text = frmMsg->edtSetEmployeeID->Text.Trim();
		mskedBurntMacStart->Text = frmMsg->mskedBurntMacStart->Text;
		mskedBurntMacLast->Text = frmMsg->mskedBurntMacLast->Text;
		//取得LOGFilePath、LOGDiskPath路徑
		LOGFilePath = "C:\\ASMP\\log\\"+edtWorkOrderNumber->Text+"\\DisplayLinkBurnTool";
		CheckDiskName();
		//
		if(frmMsg->plckbBarcode->Caption.Pos("√"))
		{
			ckbAutoIncrease->Checked = false;
			ckbBurnSN->Checked = true;
			ckbBurnMAC->Checked = true;
			ckbBarcode->Checked = true;
			gbBurnContent->Enabled = false;
			plTestMode->Caption = frmMsg->plNormalMode->Color == clLime ? "一般模式":"重工模式";
			if(frmMsg->plNormalMode->Color != clLime)
			{   //重工模式
				bReWork = true;
				LOGFilePath += "\\ReWork";
				if(LOGDiskPath != "")
					LOGDiskPath += "\\ReWork";
			}
			FindLogFile();
		}
		else
		{
			ckbAutoIncrease->Checked = false;
			ckbBurnSN->Checked = false;
			ckbBurnMAC->Checked = false;
			ckbBarcode->Enabled = false;
			ckbBarcode->Checked = false;
			gbBurnContent->Enabled = true;
			FindLogFile();
		}
		delete frmMsg;
		frmMsg = NULL;
		cmbNumOfChipChange(NULL);
	}
	else
	{
		delete frmMsg;
		frmMsg = NULL;
		Close();
	}
}
void TUSBHIDForm::FindLogFile()
{
	//
	NewFilePath(LOGFilePath);
	NewFilePath(LOGDiskPath);
	//
	TSearchRec Sr;
	AnsiString time = FormatDateTime("yyyymmdd-hhnnss", Now());
	if(DirectoryExists(LOGDiskPath.c_str()))
	{
		ImgDisk->Visible = true;
		if(FindFirst(LOGDiskPath+"\\*.csv",faAnyFile,Sr)==0)
		{
			do
			{
				if(Sr.Name.Pos("RD"))
				{
					if(Sr.Name.SubString(3,8) == time.SubString(1,8))
					{
						RDLOGDiskPath = LOGDiskPath + "\\"+Sr.Name;
						LOGDiskPath += "\\"+Sr.Name.SubString(3,19);
						//
						RDLOGFilePath = LOGFilePath + "\\"+Sr.Name;
						LOGFilePath += "\\"+Sr.Name.SubString(3,19);
						break;
					}
				}
				else
				{
					if(Sr.Name.SubString(1,8) == time.SubString(1,8))
					{
						RDLOGDiskPath = LOGDiskPath +"\\RD"+Sr.Name;
						LOGDiskPath += "\\"+Sr.Name;
						//
						RDLOGFilePath = LOGFilePath +"\\RD"+Sr.Name;
						LOGFilePath += "\\"+Sr.Name;
						break;
					}
				}
			}
			while(FindNext(Sr)==0);
			FindClose(Sr);
		}

		if(!LOGDiskPath.Pos(".csv"))
		{
			RDLOGDiskPath = LOGDiskPath +"\\RD"+time+".csv";
			LOGDiskPath += "\\"+time+".csv";
			RDLOGFilePath = LOGFilePath +"\\RD"+time+".csv";
			LOGFilePath += "\\"+time+".csv";
		}
		else//檔案已存在於USBDisk
		{
			if(!FileExists(LOGFilePath))
			{
				CopyFile(LOGDiskPath.c_str(),LOGFilePath.c_str(),true);
				CopyFile(RDLOGDiskPath.c_str(),RDLOGFilePath.c_str(),true);
			}
		}
	}
	else
	{
		ImgDisk->Visible = false;
		if(DirectoryExists(LOGFilePath.c_str()))
		{
			if(FindFirst(LOGFilePath+"\\*.csv",faAnyFile,Sr)==0)
			{
				do
				{
					if(Sr.Name.Pos("RD"))
					{
						if(Sr.Name.SubString(3,8) == time.SubString(1,8))
						{
							RDLOGFilePath = LOGFilePath + "\\"+Sr.Name;
							LOGFilePath += "\\"+Sr.Name.SubString(3,19);
							break;
						}
					}
					else
					{
						if(Sr.Name.SubString(1,8) == time.SubString(1,8))
						{
							RDLOGFilePath = LOGFilePath +"\\RD"+Sr.Name;
							LOGFilePath += "\\"+Sr.Name;
							break;
						}
					}
				}
				while(FindNext(Sr)==0);
				FindClose(Sr);
			}
		}
		if(!LOGFilePath.Pos(".csv"))
		{
			RDLOGFilePath = LOGFilePath +"\\RD"+time+".csv";
			LOGFilePath += "\\"+time+".csv";
		}
	}
	//
	dwTestCount = CheckMAC("");
	plCount->Caption = dwTestCount;
}
DWORD TUSBHIDForm::CheckMAC(AnsiString MAC)
{   //換人員重計數 //不綁mac範圍 //
	if(bReWork) return 0;
	AnsiString Temp_Start,Temp_Last,Temp;
	if(MAC != "")
	{
	//範圍
		Temp_Start = HexToInt(mskedBurntMacStart->Text.SubString(1,6));
		Temp_Last  = HexToInt(mskedBurntMacLast->Text.SubString(1,6));
		Temp  	   = HexToInt(MAC.SubString(1,6));
		if(Temp.ToInt() <= Temp_Last.ToInt() && Temp.ToInt() >= Temp_Start.ToInt())
		{
			Temp_Start = HexToInt(mskedBurntMacStart->Text.SubString(7,6));
			Temp_Last  = HexToInt(mskedBurntMacLast->Text.SubString(7,6));
			Temp  	   = HexToInt(MAC.SubString(7,6));
			if(Temp.ToInt() > Temp_Last.ToInt() || Temp.ToInt() < Temp_Start.ToInt())
				return ERROR_MAC_OUTRANGE;
		}
		else return ERROR_MAC_OUTRANGE;
	//重複
		Temp = LOGFilePath;
		AnsiString TempRepeat,TempRepeat1;
		if(FileExists(Temp))
		{
			ifstream lanfile(Temp.c_str());
			std::string filemsg;
			if (lanfile.is_open()) {
				while (!lanfile.eof())
				{
					getline(lanfile, filemsg);
					if (strstr(filemsg.c_str(), "PASS")
					)
					{
						TempRepeat  = "[MAC]"+MAC;
						TempRepeat1 = "[MAC2]"+MAC;
						if(strstr(filemsg.c_str(), TempRepeat.c_str())
						|| strstr(filemsg.c_str(), TempRepeat1.c_str()))
						{
							lanfile.close();
							return ERROR_MAC_REPEAT;
						}
					}
				}
				lanfile.close();
			}
		}
		Temp = LOGDiskPath;
		if(FileExists(Temp))
		{
			ifstream lanfile(Temp.c_str());
			std::string filemsg;
			if (lanfile.is_open()) {
				while (!lanfile.eof())
				{
					getline(lanfile, filemsg);
					if (strstr(filemsg.c_str(), "PASS")
					)
					{
						TempRepeat  = "[MAC]"+MAC;
						TempRepeat1 = "[MAC2]"+MAC;
						if(strstr(filemsg.c_str(), TempRepeat.c_str())
						|| strstr(filemsg.c_str(), TempRepeat1.c_str()))
						{
							lanfile.close();
							return ERROR_MAC_REPEAT;
						}
					}
				}
				lanfile.close();
			}
		}
		return VERIFTY_MAC_PASS;
	}
	else
	{   //計數
		DWORD dwCount = 0,dwCount1 = 0;
		Temp = LOGFilePath;
		AnsiString EmployeeID = "[EmployeeID]"+edtEmployeeID->Text;
		if(FileExists(Temp))
		{
			ifstream lanfile(Temp.c_str());
			std::string filemsg;
			if (lanfile.is_open()) {
				while (!lanfile.eof())
				{
					getline(lanfile, filemsg);
					if (strstr(filemsg.c_str(), "PASS"))
					{   //換工號重新計數  不綁mac範圍
						if(!AnsiString(filemsg.c_str()).Pos(EmployeeID))
							dwCount = 0;
						else
							dwCount++;
					}
				}
				lanfile.close();
			}
		}
		Temp = LOGDiskPath;
		if(FileExists(Temp))
		{
			ifstream lanfile(Temp.c_str());
			std::string filemsg;
			if (lanfile.is_open()) {
				while (!lanfile.eof())
				{
					getline(lanfile, filemsg);
					if (strstr(filemsg.c_str(), "PASS"))
					{   //換工號重新計數  不綁mac範圍
						if(!AnsiString(filemsg.c_str()).Pos(EmployeeID))
							dwCount1 = 0;
						else
							dwCount1++;
					}
				}
				lanfile.close();
				return dwCount1;
			}
		}
		return dwCount;
	}
	return 0;
}
void TUSBHIDForm::NewFilePath(AnsiString Path)
{
	Path+="\\";
	if(!DirectoryExists(Path.c_str()))
	{
		AnsiString SS,Temp = "";
		while(true)
		{
			SS = Path.SubString(1,Path.Pos("\\"));
			Path.Delete(1,Path.Pos("\\"));
			Temp+=SS;
			_mkdir( Temp.c_str());
			if(Path == "")
				break;
		}
	}
}
void __fastcall TUSBHIDForm::LogClick(TObject *Sender)
{
	if(btnAutoTest->Enabled)
	{
		ShowSettingForm();
	}
}
//---------------------------------------------------------------------------
void TUSBHIDForm::SetForegroundWindowInternal()
{
	if(!::IsWindow(USBHIDForm->Handle)) return;

	//relation time of SetForegroundWindow lock
    DWORD lockTimeOut = 0;
	HWND  hCurrWnd = ::GetForegroundWindow();
	DWORD dwThisTID = ::GetCurrentThreadId(),
		  dwCurrTID = ::GetWindowThreadProcessId(hCurrWnd,0);

    //we need to bypass some limitations from Microsoft :)
    if(dwThisTID != dwCurrTID)
	{
		::AttachThreadInput(dwThisTID, dwCurrTID, TRUE);

        ::SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT,0,&lockTimeOut,0);
		::SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT,0,0,SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);

        ::AllowSetForegroundWindow(ASFW_ANY);
	}

	::SetForegroundWindow(USBHIDForm->Handle);

	if(dwThisTID != dwCurrTID)
	{
		::SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT,0,(PVOID)lockTimeOut,SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
		::AttachThreadInput(dwThisTID, dwCurrTID, FALSE);
	}
}
