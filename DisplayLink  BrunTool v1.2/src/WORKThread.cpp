//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "WORKThread.h"
#include "Uint1.h"
#include "DUT.h"
#include "USBDevConnectionInfo.h"
#include "ConstantString.h"
#pragma package(smart_init)
extern DUT m_dut;
USBDevConnectionInfo *USBCONInfo = new USBDevConnectionInfo();
int autoSTEP,UIStatus = UI_WAIT;
char autoERRC;
bool bStart,getDUT;
AnsiString SS;

//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TWORKThread::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TWORKThread::TWORKThread(bool CreateSuspended)
	: TThread(CreateSuspended)
{

}
//---------------------------------------------------------------------------
void __fastcall TWORKThread::Execute()
{
	//NameThreadForDebugging("WORKThread");
	//---- Place thread code here ----
	int kind;
	bStart = false;
	FreeOnTerminate = true;

	while(!Terminated)
	{
		if(!USBHIDForm->dThreadStatus)
		{
			if(bStart)
			{
				Synchronize(MainTest);
			}
			else
			{
				kind = m_dut.GetKind();
				if(kind == 0)
				{
					if(UIStatus != UI_CLEAR)
					{
						UIStatus = UI_CLEAR;
						Synchronize(UIClear);
						USBHIDForm->bPlugIn = false;
					}
				}
				else if(USBHIDForm->cmbNumOfChip->Text.ToInt()!=kind)
				{
					if(UIStatus != UI_CHIP_ERROR)
					{
						UIStatus = UI_CHIP_ERROR;
						SS = "(!)Chip num Error";
						Synchronize(UIUpdate);
					}
				}
				else if(USBHIDForm->cmbNumOfChip->Text.ToInt()== kind)
				{
					if(!USBHIDForm->bPlugIn)
					{
						USBHIDForm->g_OTP_SN = "";
						USBHIDForm->TestTime = GetTickCount();
						getDUT = false;
						m_dut.SetMtt(false);
						if(CheckDisplayLinkChipUSB3())
						{
							Synchronize(CheckDUTInfomation);
							if(getDUT)
							{
								USBHIDForm->PopupMenu->Items->Items[2]->Visible = false;
								USBHIDForm->m_bAutoIncrease = false;
								bStart = true;
								autoSTEP = GET_DEV_KIND;
								autoERRC = 0x00;
								Synchronize(MainTest);
							}
							else
							{
								SS = "(!)讀取資料失敗";
								UIStatus = UI_WAIT;
								Synchronize(UIUpdate);
							}
						}
						else
						{
							SS = "(!)USB2.0";
							UIStatus = UI_WAIT;
							Synchronize(UIUpdate);
						}
					}
					USBHIDForm->bPlugIn = true;
				}
			}
		}
		if(USBHIDForm->dThreadStatus == THREAD_STOP)
			USBHIDForm->dThreadStatus = THREAD_WIAT;
		Sleep(200);
	}
}
//---------------------------------------------------------------------------
void __fastcall TWORKThread::MainTest()
{
	int i ;
	int kind;
	bool bOTPError = false;
	switch(autoSTEP)
	{
	case GET_DEV_KIND:
		USBHIDForm->numBarcodeResult = 0;
		USBHIDForm->btnAutoTest->Enabled = false;
		USBHIDForm->ErrorMsg = "PASS";
		kind = m_dut.GetKind();
		m_dut.SetKind(kind);

		autoSTEP = USB3_CHECK;

		USBHIDForm->memoMsg->Lines->Clear();
		labMSG("開始作業",clYellow);

		break;
	case USB3_CHECK:
		labMSG("檢測USB",clYellow);
		if(CheckDisplayLinkChipUSB3())
		{
			if(USBHIDForm->g_bDDR_Checck)
				autoSTEP = DDR_CHECK;
			else if(USBHIDForm->ckbBurnSN->Checked||USBHIDForm->ckbBurnMAC->Checked)
				autoSTEP = USBHIDForm->ckbFWVerify->Checked ? CHECK_FW:DEV_BURN;
			else if(USBHIDForm->ckbFWVerify->Checked)
				autoSTEP = BURN_VERIFY;
			else BURN_RESULT;
		}
		else
		{
			autoERRC = 0x40;
			labMSG("(!)USB2.0 Mode",clYellow);
			USBHIDForm->ErrorMsg = "(!)USB2.0 Mode";
			autoSTEP = BURN_RESULT;
		}
		break;
	case DDR_CHECK:
			labMSG("DDR BUS TEST",clYellow);
			if(!m_dut.DataBusTest()){
				autoERRC = 0x40;
				labMSG("(!)DDR Error",clRed);
				USBHIDForm->ErrorMsg = "(!)DDR BUS Error";
				autoSTEP = BURN_RESULT;
				break;
			}
			USBHIDForm->memoMsg->Lines->Add("DDR::Address Bus test");
			if(!m_dut.AddressBusTest()) {
				autoERRC = 0x40;
				labMSG("(!)DDR Error",clRed);
				USBHIDForm->ErrorMsg = "(!)DDR Address BUS Error";
				autoSTEP = BURN_RESULT;
				break;
			}
			USBHIDForm->memoMsg->Lines->Add("DDR::Checkerboard test");
			if (!m_dut.CheckerboardTest()) {
				autoERRC = 0x40;
				labMSG("(!)DDR Error",clRed);
				USBHIDForm->ErrorMsg = "(!)DDR Checkerboard Error";
				autoSTEP = BURN_RESULT;
				break;
			}
			USBHIDForm->memoMsg->Lines->Add("DDR::Byte Count Data test");
			if (!m_dut.ByteCountDataTest()) {
				autoERRC = 0x40;
				labMSG("(!)DDR Error",clRed);
				USBHIDForm->ErrorMsg = "(!)DDR Byte Count Data Error";
				autoSTEP = BURN_RESULT;
				break;
			}
			if(autoERRC) autoSTEP = BURN_RESULT;
			else if(USBHIDForm->ckbBurnSN->Checked||USBHIDForm->ckbBurnMAC->Checked)
			{
				autoSTEP = USBHIDForm->ckbFWVerify->Checked ? CHECK_FW:DEV_BURN;
			}
			else if(USBHIDForm->ckbFWVerify->Checked)
				autoSTEP = BURN_VERIFY;
			else autoSTEP = BURN_RESULT;
		break;
	case CHECK_FW:
			labMSG("驗證韌體版本..",clYellow);
			if(!USBHIDForm->CheckMinFW()) autoERRC = 0x40;
			USBHIDForm->edtBarcodeMAC->Text = "";
			autoSTEP = autoERRC ? BURN_RESULT : DEV_BURN;
			if(autoSTEP == DEV_BURN && USBHIDForm->ckbBarcode->Checked)
			{
				labMSG("燒錄程序",clYellow);
			   	autoSTEP = BARCODE_ENTER;
				USBHIDForm->edtBarcodeMain->Height = 160;
				USBHIDForm->edtBarcodeMain->Top = USBHIDForm->Height/2 - 80;
				USBHIDForm->lbBarcode->Caption = "請掃描MAC燒錄條碼";
				USBHIDForm->Refresh();
				USBHIDForm->edtBarcodeMAC->SetFocus();
			}
			break;
	case BARCODE_ENTER:
			if(USBHIDForm->numBarcodeResult == BARCODE_FINISH)
			{
				USBHIDForm->numBarcodeResult = 0;
				USBHIDForm->edtBarcodeMain->Height = 0;
				switch(USBHIDForm->CheckMAC(USBHIDForm->mskedBurntMacPre->Text+USBHIDForm->mskedtBurnMac->Text))
				{
					case ERROR_MAC_OUTRANGE:
						autoERRC =  0x40;
						labMSG("(!)MAC超出範圍設定",clRed);
						USBHIDForm->ErrorMsg = "(!)MAC超出範圍設定";
						autoSTEP = BURN_RESULT;
						break;
					case ERROR_MAC_REPEAT:
						autoERRC =  0x40;
						labMSG("(!)此MAC已燒錄過",clRed);
						USBHIDForm->ErrorMsg = "(!)此MAC已燒錄過";
						autoSTEP = BURN_RESULT;
						break;
					default:
						USBHIDForm->Refresh();
						autoSTEP = DEV_BURN;
						break;
				}
			}
			if(USBHIDForm->numBarcodeResult == BARCODE_CHANEL)
			{
				USBHIDForm->numBarcodeResult = 0;
				USBHIDForm->edtBarcodeMain->Height = 0;
				autoERRC =  0x40;
				labMSG("(!)燒錄取消",clRed);
				USBHIDForm->ErrorMsg = "(!)燒錄取消";
				autoSTEP = BURN_RESULT;
			}
			break;
	case DEV_BURN:
			labMSG("燒錄程序",clYellow);
			autoERRC = DUTB();

			if (autoERRC) {
				autoSTEP = BURN_RESULT;
				break;
			}
			autoSTEP = BURN_VERIFY;
		break;
	case BURN_VERIFY:
		if(USBHIDForm->ckbFWVerify->Checked){
			labMSG("驗證程序",clYellow);
			autoERRC = USBHIDForm->DUTV();
		}
		autoSTEP = BURN_RESULT;
		break;
	case BURN_RESULT:
		for(i = 0;i<=5;i++)
		{
			USBHIDForm->memoMsg->Lines->Add("確認DUT..."+AnsiString(i));
			if(m_dut.GetKind())
				break;
			else if(i==5)
			{
				labMSG("(!)找不到裝置",clRed);
				USBHIDForm->ErrorMsg = "(!)找不到裝置";
				autoERRC = 0x40;
				break;
			}
			Sleep(1000);
		}
		//
		if(USBHIDForm->ckbOTP->Checked)
		{
			if(USBHIDForm->cmbNumOfChip->Text == "1")
			{
				//僅用於SSA1 舊IC篩選用
				if(USBHIDForm->g_OTP_SN.Length()==8)
				{
					if(USBHIDForm->g_OTP_SN.ToInt() < 14800000)
					{
						autoERRC =  0x40;
						labMSG("(!)OTP範圍異常",clRed);
						USBHIDForm->ErrorMsg = "(!)OTP範圍異常";
						USBHIDForm->memoMsg->Lines->Add("OTP:"+USBHIDForm->g_OTP_SN);
					}
				}
				else
				{
					autoERRC =  0x40;
					labMSG("(!)OTP範圍異常",clRed);
					USBHIDForm->ErrorMsg = "(!)OTP範圍異常";
					USBHIDForm->memoMsg->Lines->Add("OTP:"+USBHIDForm->g_OTP_SN);
				}
			}
		}
		//
		if(!USBHIDForm->writeLOG(USBHIDForm->ErrorMsg,false))
		{
			labMSG("(!)LOG寫入失敗",clRed);
			autoERRC = 0x40;
		}
		if (autoERRC) {
			if(USBHIDForm->lblStatus->Color!= clRed)
			{
				if(!USBHIDForm->ErrorMsg.Pos("USB2.0 Mode"))
				{
					labMSG("!!! 異常發生 !!!",clRed);
					USBHIDForm->ErrorMsg = "(!)異常發生";
				}
			}
			USBHIDForm->m_bAutoIncrease = false;
		}
		else {
			labMSG("PASS",clGreen);
			USBHIDForm->m_bAutoIncrease = true;
		}
		autoSTEP = BURN_END;

		break;
	case BURN_END:
		USBHIDForm->PopupMenu->Items->Items[2]->Visible = true;
		autoSTEP = BURN_OVER;
		UIStatus = UI_WAIT;
		bStart = false;
		break;
	} //End switch case.
	//}
}
void __fastcall TWORKThread::CheckDUTInfomation()
{
	getDUT = USBHIDForm->GetDutInfo_DriverCheck(false);
}
void __fastcall TWORKThread::UIClear()
{
	if(USBHIDForm->m_bAutoIncrease)
		USBHIDForm->AutoIncrease();
	USBHIDForm->ClearDevInfo();
	labMSG("等候中",clSkyBlue);
	USBHIDForm->memoMsg->Text = "No matching devices found";
}
void __fastcall TWORKThread::UIUpdate()
{
	if(!SS.Pos("USB2.0") && SS.Pos("Chip num Error"))
	{
		labMSG(SS,clRed);
	}
	{
		labMSG(SS,clYellow);
		USBHIDForm->writeLOG(USBHIDForm->ErrorMsg,false);
    }
	USBHIDForm->ErrorMsg = SS;
}
bool TWORKThread::CheckDisplayLinkChipUSB3()
{
	AnsiString Resultdata = USBHIDForm->DosCommand("devcon find \"\*usb\\vid_17e9\*\"");
	char *pch = strtok(Resultdata.c_str(), "\r\n");
	TStringList* NList = new TStringList();
	AnsiString VPID = "";
	bool bHave = false;
	bool bPass = false;
	while (pch != NULL)
	{
		VPID = AnsiString(pch);
		VPID = VPID.SubString(VPID.Pos("VID"),17);
		for(int x =0;x<NList->Count;x++)
		{
			if(strstr(AnsiString(NList->Strings[x]).c_str(),VPID.c_str()))
			{
				bHave = true;
				break;
			}
		}
		if(!bHave)
		{
			NList->Add(VPID);
			bPass = USBCONInfo->GetDevUSB(VPID.SubString(5,4),VPID.SubString(14,4));
		}
		if(!bPass) break;
		pch = strtok(NULL, "\r\n");
	}
	delete NList;
	return bPass;
}
char __fastcall TWORKThread::DUTB(void)
{
	int ERRC = 0;
	AnsiString SN, MAC;
	AnsiString updcode, SS;
	char macbuf[18];
	USBHIDForm->memoMsg->Lines->Add(BURN_DUT);
	// -----------------------------------------------------upd flash
	if (!ERRC) {
		labMSG("燒錄開始..",clYellow);
		updcode = " upd";
		SN = "\t" + USBHIDForm->mskedtBurnSn->Text;

		if(m_dut._Kind == 2 && m_dut._Dev1.Pos(_T("4301_")) != 0){
			updcode += " -s " + AnsiString(USBHIDForm->mskedtBurnSn->Text);
			MAC = AnsiString("\t") + _T("000000000000");;
		}
		else
		{
			if(USBHIDForm->ckbBurnSN->Checked)
				updcode += " -s " +AnsiString(USBHIDForm->mskedtBurnSn->Text);
			if(USBHIDForm->ckbBurnMAC->Checked)
			{
				updcode += " -a ";
				SS = USBHIDForm->mskedBurntMacPre->Text;
				strncpy(&macbuf[0], SS.c_str(), 6);
				SS = USBHIDForm->mskedtBurnMac->Text;
				strncpy(&macbuf[6], SS.c_str(), 6);
				macbuf[17] = 0x00;
				macbuf[16] = macbuf[11];
				macbuf[15] = macbuf[10];
				macbuf[14] = ':';
				macbuf[13] = macbuf[9];
				macbuf[12] = macbuf[8];
				macbuf[11] = ':';
				macbuf[10] = macbuf[7];
				macbuf[9] = macbuf[6];
				macbuf[8] = ':';
				macbuf[7] = macbuf[5];
				macbuf[6] = macbuf[4];
				macbuf[5] = ':';
				macbuf[4] = macbuf[3];
				macbuf[3] = macbuf[2];
				macbuf[2] = ':';
				macbuf[1] = macbuf[1];
				macbuf[0] = macbuf[0];
				updcode += macbuf;
				MAC = AnsiString("\t") + macbuf;
			}
		}
		if(!ERRC)
		{
			if(!m_dut.ProgramUPD(updcode))
			{
				ERRC = 0x01;
			}else{
				ERRC = 0x00;
			}
		}
	}

	if (ERRC)
	{
		labMSG("(!)燒錄失敗",clRed);
		USBHIDForm->ErrorMsg = "(!)燒錄失敗";
	}
	return(ERRC);
}
