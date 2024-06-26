//---------------------------------------------------------------------------


#pragma hdrstop
#include <vcl.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "wininet.h"
#include <direct.h>
#pragma link "wininet.lib"
#include "Uint1.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)


bool clWNetFile::WNetFileConnection()
{
	bool bPASS = false;
	if(!DirectoryExists("\\\\192.168.20.1\\RDUser"))
	{
		//目標根目錄
		AnsiString MapSharedPath = "\\\\192.168.20.1\\RDUser";
		//目標用戶名
		AnsiString MapUser = "RDUser";
		//目標密碼
		AnsiString MapPassword = "RDUser";

		NETRESOURCEA nr;
		DWORD res;
		nr.dwType = RESOURCETYPE_ANY;
		nr.lpLocalName = "";
		nr.lpRemoteName = "\\\\192.168.20.1\\RDUser";
		nr.lpProvider = NULL;

		res = WNetCancelConnection2A(nr.lpRemoteName,0,false);
		if(NO_ERROR!=res)
		{
			//WinExec("NET USE * /DELETE /Y",SW_HIDE);
		}

		res = WNetAddConnection2A(&nr, MapPassword.c_str(), MapUser.c_str(), CONNECT_UPDATE_PROFILE);
		switch(res)
		{
			case NO_ERROR:
				WNetERROR_MSG = "Connection Succeeded\n";
				bPASS = true;
				break;
			case ERROR_BAD_PROFILE:
				WNetERROR_MSG = "ERROR_BAD_PROFILE\n";
				break;
			case ERROR_CANNOT_OPEN_PROFILE:
				WNetERROR_MSG = "ERROR_CANNOT_OPEN_PROFILE\n";
				break;
			case ERROR_DEVICE_IN_USE:
				WNetERROR_MSG = "ERROR_DEVICE_IN_USE\n";
				break;
			case ERROR_EXTENDED_ERROR:
				WNetERROR_MSG = "ERROR_EXTENDED_ERROR\n";
				break;
			case ERROR_NOT_CONNECTED:
				WNetERROR_MSG = "ERROR_NOT_CONNECTED\n";
				break;
			case ERROR_LOGON_FAILURE:
				WNetERROR_MSG = "ERROR_LOGON_FAILURE\n";
				break;
			case ERROR_NO_NETWORK:
				WNetERROR_MSG = "ERROR_NO_NETWORK\n";
				break;
			case ERROR_OPEN_FILES:
				WNetERROR_MSG = "ERROR_OPEN_FILES\n";
				break;
			case ERROR_INVALID_PASSWORD:
				WNetERROR_MSG = "ERROR_INVALID_PASSWORD\n";
				break;
			default:
				WNetERROR_MSG = "未知錯誤 可能需要帳密 或路徑不存在";
				break;
		}
		return bPASS;
	}
	else return true;
}
bool clWNetFile::WNetWriteLOG(AnsiString PATH,AnsiString Line,bool bLocalFile)
{
	WNetERROR_MSG = "";
	if(bLocalFile)
	{
		AddLocalFilePath(PATH);

		AnsiString FileName = "C:\\ASMP\\"+PATH;
		AnsiString FileLine = (FormatDateTime("hh:nn:ss", Now()))+"  "+Line+"\r\n";
		FILE * fp;
		fp = fopen(FileName.c_str(),"a+");
		fseek( fp, 0, SEEK_END);
		fwrite(FileLine.c_str(),FileLine.Length(),1,fp); //寫入一筆資料
		fclose(fp);
	}
	else if(AddWNetFilePath(PATH))
	{
		DEBUG("Write LOG...");
		AnsiString FileName = "\\\\192.168.20.1\\RDUser\\"+PATH;
		AnsiString FileLine = (FormatDateTime("hh:nn:ss", Now()))+"  "+Line+"\r\n";
		FILE * fp;
		fp = fopen(FileName.c_str(),"a+");
		fseek( fp, 0, SEEK_END);
		fwrite(FileLine.c_str(),FileLine.Length(),1,fp); //寫入一筆資料
		fclose(fp);
		DEBUG("Write PASS");
		return true;
	}
	else
		return false;
}
bool clWNetFile::AddWNetFilePath(AnsiString PATH)
{
	bool bPASS = false;
	if(WNetFileConnection())
	{
		AnsiString FileName = "\\\\192.168.20.1\\RDUser\\"+PATH;
		if(!FileExists(FileName.c_str())){
			TStringList *sList = new TStringList();
			sList->Delimiter = '\\';
			sList->DelimitedText = AnsiString(PATH);
			FileName = "\\\\192.168.20.1\\RDUser\\"+sList->Strings[0];
			_mkdir(FileName.c_str());
			FileName = FileName+"\\"+sList->Strings[1];
			_mkdir(FileName.c_str());
			FileName = FileName+"\\"+sList->Strings[2];
			_mkdir(FileName.c_str());
			FileName = FileName+"\\"+sList->Strings[3];
			_mkdir(FileName.c_str());
			FileName = FileName+"\\"+sList->Strings[4];
			_mkdir(FileName.c_str());
			delete  sList;
			if(DirectoryExists(FileName))
				bPASS = true;
			else
			{
				bPASS = false;
				DEBUG("(!)Add WNetFile FAIL");
			}
		}
		else bPASS = true;
	}
	else bPASS = false;
	return bPASS;
}
bool clWNetFile::AddLocalFilePath(AnsiString PATH)
{
	bool bPASS = false;

	AnsiString FileName = "C:\\ASMP\\"+PATH;
	if(!FileExists(FileName.c_str())){
		TStringList *sList = new TStringList();
		sList->Delimiter = '\\';
		sList->DelimitedText = AnsiString(PATH);

		_mkdir("C:\\ASMP");
		FileName = "C:\\ASMP\\"+sList->Strings[0];
		_mkdir(FileName.c_str());
		FileName = FileName+"\\"+sList->Strings[1];
		_mkdir(FileName.c_str());
		FileName = FileName+"\\"+sList->Strings[2];
		_mkdir(FileName.c_str());
		FileName = FileName+"\\"+sList->Strings[3];
		_mkdir(FileName.c_str());
		FileName = FileName+"\\"+sList->Strings[4];
		_mkdir(FileName.c_str());
		delete  sList;
		if(DirectoryExists(FileName))
		   bPASS = true;
		else bPASS = false;
	}
	else bPASS = true;
	return bPASS;
}
