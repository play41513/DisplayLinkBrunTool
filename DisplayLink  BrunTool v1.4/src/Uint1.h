//---------------------------------------------------------------------------
#ifndef Uint1H
#define Uint1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <MPlayer.hpp>
#include <Graphics.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <ImgList.hpp>
#include <jpeg.hpp>
#include <Db.hpp>
#include <DBTables.hpp>
#include "Trayicon.h"
#include <Mask.hpp>
#include <FileCtrl.hpp>

#include "SetupDi.h"
#include "DeviceNotify.h"
#include "Gauges.hpp"
#include <pngimage.hpp>
#include <vector>
#include "USB_Hub_Tree.h"
#include <fstream>
#include <iostream>
#include <queue>
#include <inifiles.hpp>
#include <systdate.h>
#include <mmsystem.h>
#include <time.h>
#include <tchar.h>
#include "ConstantString.h"
#include <Registry.hpp>
#include "Iphlpapi.h"
#include <string>
#pragma link "Iphlpapi.lib"
//---------------------------------------------------------------------------
using namespace std;

#define CALLDLL extern "C" __declspec(dllexport)
CALLDLL int  A_Get_DLL_Version(int nShowMessage);
CALLDLL int  A_GetUSBBufferLen(void);
CALLDLL int  A_EnumUSB(char *buf);
CALLDLL int  A_GetUSBDeviceInfo(int nPort,char *pDeviceName,int *pDeviceNameLen,char* pDevicePath,int *pDevicePathLen);
CALLDLL int  A_CreatePrn(int selection,AnsiString filename);
CALLDLL void A_ClosePrn(void);

CALLDLL int  A_Set_Unit(char unit_Renamed);
CALLDLL int  A_Set_Syssetting(int transfer,int cut_peel,int length,int zero,int pause);

CALLDLL int  A_Set_Darkness(unsigned int heat);
CALLDLL int  A_Del_Graphic(int mem_mode,char *graphic);
CALLDLL void A_Clear_Memory(void);
CALLDLL int  A_Set_Backfeed(int back);
CALLDLL int  A_Prn_Text(int x,int y,int ori,int font,int type_Renamed,int hor_factor,int ver_factor,char mode,int numeric,char *data);
CALLDLL int  A_Print_Out(int width,int height,int copies,int amount);

//---------------------------------------------------------------------------
#define GET_DEV_KIND	1
#define DEV_BURN		2
#define BURN_VERIFY		3
#define BURN_RESULT		4
#define BURN_END		5
#define DDR_CHECK       6
#define BURN_OVER		0
#define CHECK_FW		7
#define USB3_CHECK		8
#define BARCODE_ENTER	9

#define BARCODE_FINISH	1
#define BARCODE_CHANEL  2

#define THREAD_START	0
#define THREAD_STOP     1
#define THREAD_WIAT     2


#define MEMO_MSG(String) memoMsg->Lines->Add(String)

#define labMSG(String,sColor) USBHIDForm->lblStatus->Caption=String;\
						  USBHIDForm->lblStatus->Color=sColor;\
						  USBHIDForm->MEMO_MSG(String);\
						  USBHIDForm->Refresh()
//---------------------------------------------------------------------------
class TUSBHIDForm : public TForm
{
__published:	// IDE-managed Components
	TMemo *moMACRecord;
	TPanel *plSwitch;
	TPanel *plMain;
	TPanel *Panel2;
	TPanel *plRight;
	TLabel *lblStatus;
	TPanel *Panel12;
	TPanel *Panel22;
	TPanel *plChipName;
	TPanel *Panel11;
	TPanel *Panel10;
	TPanel *plUSBSN;
	TPanel *Panel19;
	TPanel *Panel20;
	TPanel *plMAC;
	TPanel *Panel13;
	TPanel *Panel14;
	TPanel *plMinFWVer;
	TPanel *plChip2View;
	TPanel *Panel21;
	TPanel *Panel23;
	TPanel *plMAC2;
	TPanel *Panel25;
	TPanel *Panel26;
	TPanel *plUSBSN2;
	TPanel *plChipName2;
	TPanel *Panel32;
	TPanel *Panel33;
	TPanel *plMinFWVer2;
	TPanel *Panel36;
	TMemo *memoMsg;
	TPanel *plLeft;
	TGroupBox *gbBurnContent;
	TLabel *Label1;
	TLabel *Label16;
	TLabel *Label22;
	TMaskEdit *mskedBurntMacPre;
	TMaskEdit *mskedtBurnMac;
	TCheckBox *ckbBurnSN;
	TCheckBox *ckbBurnMAC;
	TCheckBox *ckbAutoIncrease;
	TGroupBox *gbUSBHUB;
	TPanel *Panel24;
	TPanel *Panel27;
	TPanel *pl_Sample_MinFW;
	TPanel *Panel31;
	TPanel *Panel34;
	TPanel *pl_Sample_MinFW2;
	TPanel *Panel28;
	TPanel *Panel35;
	TPanel *Panel1;
	TButton *btnLoadDutVersion;
	TCheckBox *ckbOTP;
	TComboBox *cmbNumOfChip;
	TLabel *lbNumberOfChip;
	TPanel *Panel3;
	TButton *btnAutoTest;
	TPanel *Panel4;
	TButton *btnGetInfo;
	TImage *Image1;
	TPopupMenu *PopupMenu;
	TMenuItem *Log;
	TMenuItem *LOGFILE;
	TCheckBox *ckbBarcode;
	TCheckBox *ckbBarcodeVerify;
	TEdit *mskedtBurnSn;
	TPanel *edtBarcodeMain;
	TPanel *plBarcode;
	TLabel *lbBarcode;
	TPanel *Panel6;
	TBitBtn *btnBarcodeChanel;
	TBitBtn *btnBarcodeClear;
	TEdit *edtBarcodeMAC;
	TMenuItem *Stop1;
	TLabel *lbLOGStatus;
	TPanel *plBarcodeData;
	TPanel *Panel8;
	TMaskEdit *mskedBurntMacStart;
	TPanel *Panel29;
	TMaskEdit *mskedBurntMacLast;
	TPanel *Panel9;
	TPanel *plCount;
	TPanel *plBarcodeSet;
	TImage *Image2;
	TPanel *Panel15;
	TEdit *edtWorkOrderNumber;
	TPanel *Panel16;
	TEdit *edtEmployeeID;
	TPanel *plTestMode;
	TImage *ImgDisk;
	TCheckBox *ckbFWVerify;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnAutoTestClick(TObject *Sender);
	void __fastcall btnLoadDutVersionClick(TObject *Sender);
	void __fastcall plSwitchClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall cmbNumOfChipChange(TObject *Sender);
	void __fastcall LOGFILEClick(TObject *Sender);
	void __fastcall ckbBurnSNClick(TObject *Sender);
	void __fastcall edtWorkOrderNumberExit(TObject *Sender);
	void __fastcall ckbBarcodeClick(TObject *Sender);
	void __fastcall ckbBarcodeVerifyClick(TObject *Sender);
	void __fastcall btnBarcodeClearClick(TObject *Sender);
	void __fastcall edtBarcodeMACChange(TObject *Sender);
	void __fastcall edtBarcodeMACKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall btnBarcodeChanelClick(TObject *Sender);
	void __fastcall Stop1Click(TObject *Sender);
	void __fastcall LogClick(TObject *Sender);

private:	// User declarations
	AnsiString LOG_PATH;
	AnsiString DISK_PATH;
	AnsiString DISK_NAME;

	void WriteINIFile();
	void OnThreadTerminate();
	void SetUIEnable(bool Enable);
	void RemoveDisplayLinkDriver();
	void SetRegDriver(AnsiString DevVPID);
	bool SaveUserLogLine(AnsiString FileName,AnsiString FileLine);
	void ShowSettingForm();
	void TUSBHIDForm::FindLogFile();
	void TUSBHIDForm::NewFilePath(AnsiString Path);
	bool CheckDiskName();
	bool CheckWin10Driver();
	bool GetDutInfomation(bool bLOAD_DEV);
	int  __fastcall HexToInt(AnsiString HexStr);
	AnsiString GetStringLine(AnsiString String,AnsiString Keyword);

	bool bReWork;
	bool bCustomSN;
	DWORD dwTestCount;
	AnsiString strCustomSN;
	AnsiString LOGFilePath,LOGDiskPath,RDLOGDiskPath,RDLOGFilePath;
	AnsiString GetNumberFromString(AnsiString input);
protected:

public:		// User declarations
	__fastcall TUSBHIDForm(TComponent* Owner);
	TThread *WORKThread;
	AnsiString g_OTP_SN,g_OTP_SN_2;
	AnsiString ErrorMsg;
	DWORD TestTime;
	bool m_bAutoIncrease; //�N�����\
	bool g_bDDR_Checck; //DDR����;
	bool bPlugIn;
	DWORD dThreadStatus;
	int  numBarcodeResult;

	void ClearDevInfo();

	void Delay(DWORD iMilliSeconds);
	void AutoIncrease();  // �۰ʸ���
	bool CheckMinFW();
	bool writeLOG(AnsiString Msg,bool bAction);
	bool GetDutInfo_DriverCheck(bool bDEV_LOAD);
	AnsiString DosCommand(AnsiString sCmdline);
	char __fastcall DUTV(void);
	queue<AnsiString> que_log;
	void SetForegroundWindowInternal();

	DWORD CheckMAC(AnsiString MAC);

	//==============================================================
};
//---------------------------------------------------------------------------
extern PACKAGE TUSBHIDForm *USBHIDForm;
//---------------------------------------------------------------------------
#endif