//---------------------------------------------------------------------------

#ifndef WORKThreadH
#define WORKThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
#define UI_WAIT				0
#define UI_CLEAR			1
#define UI_CHIP_ERROR     	2
class TWORKThread : public TThread
{
protected:
	void __fastcall Execute();
public:
	__fastcall TWORKThread(bool CreateSuspended);
	void __fastcall MainTest();
	char __fastcall DUTB(void);
	bool TWORKThread::CheckDisplayLinkChipUSB3();
	void __fastcall CheckDUTInfomation();
	void __fastcall UIClear();
	void __fastcall UIUpdate();
};
//---------------------------------------------------------------------------
#endif
