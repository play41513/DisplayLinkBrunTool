//---------------------------------------------------------------------------

#ifndef WNetFileH
#define WNetFileH
//---------------------------------------------------------------------------
#endif
class clWNetFile
{
	private:
	bool AddWNetFilePath(AnsiString PATH);
	bool AddLocalFilePath(AnsiString PATH);

	public:
	bool WNetFileConnection();
	bool WNetWriteLOG(AnsiString PATH,AnsiString Line,bool bLocalFile);
	AnsiString WNetERROR_MSG;
};
