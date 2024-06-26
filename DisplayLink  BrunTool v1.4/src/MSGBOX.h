//---------------------------------------------------------------------------

#ifndef MSGBOXH
#define MSGBOXH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Keyboard.hpp>
#include <Mask.hpp>
#include <tchar.h>
#include <pngimage.hpp>
//---------------------------------------------------------------------------
class TfrmMsg : public TForm
{
__published:	// IDE-managed Components
	TPanel *plWOInfo;
	TPanel *Panel39;
	TEdit *edtSetWorkOrderNumber;
	TTouchKeyboard *TouchKeyboard;
	TPanel *plSwitch;
	TLabel *Label1;
	TPanel *Panel4;
	TPanel *Panel7;
	TPanel *ckbWIP;
	TPanel *Panel8;
	TPanel *ckbFGI;
	TPanel *Panel10;
	TPanel *Panel40;
	TPanel *plBarcode;
	TPanel *Panel11;
	TPanel *plReworkMode;
	TPanel *plNormalMode;
	TPanel *Panel14;
	TPanel *Panel15;
	TMaskEdit *mskedBurntMacStart;
	TPanel *Panel16;
	TPanel *Panel17;
	TMaskEdit *mskedBurntMacLast;
	TPanel *Panel18;
	TLabel *Label2;
	TPanel *plckbBarcode;
	TPanel *Panel5;
	TEdit *edtSetEmployeeID;
	TPanel *Panel41;
	TBitBtn *btnID_ok;
	TTimer *Timer1;
	TPanel *plUSBLOG;
	TImage *ImgDisk;
	TPanel *Panel1;
	TPanel *Panel2;
	TComboBox *cboxSN;
	TPanel *plckbCustomSN;
	void __fastcall btnID_okClick(TObject *Sender);
	void __fastcall plSwitchClick(TObject *Sender);
	void __fastcall edtSetEmployeeIDKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall edtSetWorkOrderNumberKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall plckbBarcodeClick(TObject *Sender);
	void __fastcall plNormalModeClick(TObject *Sender);
	void __fastcall mskedBurntMacStartKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall mskedBurntMacLastKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall edtSetWorkOrderNumberExit(TObject *Sender);
	void __fastcall mskedBurntMacStartChange(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall cboxSNKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);









private:	// User declarations
	int TfrmMsg::HexToInt(AnsiString HexStr);
	bool TfrmMsg::CheckMACValue();
public:		// User declarations
	__fastcall TfrmMsg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMsg *frmMsg;
//---------------------------------------------------------------------------
#endif
