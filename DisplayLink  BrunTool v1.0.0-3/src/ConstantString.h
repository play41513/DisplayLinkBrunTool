#ifndef ConstantStringH
#define ConstantStringH
//---------------------------------------------------------------------------
//標題
const TCHAR APP_TITLE[] = _T("DisplayLink BurnTool v1.0.0 (ActionStar revised.3)");
const TCHAR USB_HUB_FIRMWARE_VERSION[] = _T("USB HUB 韌體版本");


//Hardware ID
//TCHAR HID_V2101P1406M01C02[] = _T("\\\\?\\HID#VID_2101&PID_1406&MI_01&COL02");
const TCHAR V17E9P4306R0036[] = _T("USB\\VID_17E9&PID_4306&REV0036");


const char REFERENCE_XML[] = "reference.xml";
const char CHAR_UNKNOW[] = "UNKNOW";
const TCHAR UNKNOW[] = _T("UNKNOW");
const TCHAR PASSWORD[] = _T("set");
const TCHAR SETTING[] = _T("設定");
const TCHAR EXIT[] = _T("結束");
const TCHAR INVALID_PASSWORD[] = _T("Invalid password");
const TCHAR REWORK[] = _T("重工作業");
const TCHAR PRODUCTION_OPERATION[] = _T("生產作業");
const TCHAR BURN_MIN[] = _T("燒錄最小數量為 1");
const TCHAR BURN_MAX[] = _T("燒錄最大數量為 16777215");
const TCHAR ERR_SERIAL_RANGE[] = _T("序號 範圍異常");
const TCHAR ERR_MAC_RANGE[] = _T("MAC 範圍異常");
//---------------------------------------------------------------------------
//測試訊息
const TCHAR VERIFY_USB_HUB_VERSION[] = _T("驗證 USB HUB 韌體版本");
const TCHAR VERIFY_DL_FIRMWARE_VERSION[] = _T("驗證 DisplayLink 韌體版本");
const TCHAR BURN_DUT[] = _T("燒錄 序號 MAC");
const TCHAR VERIFY_DUT_SERIAL[] = _T("驗證 序號");
const TCHAR VERIFY_DUT_MAC[] = _T("驗證 MAC");
//MessageBox
const TCHAR MB_CAPTIONWARNING[] = _T("WARNING");
const TCHAR MB_UNKNOW_DEVICE[] = _T("無法識別裝置類型!");
const TCHAR MB_NOT_SET_KIND[] = _T("Number of chip 值未設定!");
const TCHAR MB_FAILED_GET_DUT_INFORMATION[] = _T("獲取裝置資訊失敗!");
const TCHAR MB_FAILED_LOAD_DUT_INFORMATION[] = _T("載入裝置資訊失敗!");
const TCHAR MB_FAILED_ENTER_MTT[] = _T("進入 MTT 模式失敗!");
const TCHAR MB_FAILED_GET_DUT_ID[] = _T("獲取待測物ID失敗，請檢查驅動程式和裝置是否正常掛載及Number of chip欄位是否設定正確?");


#define MB_WARNING(handle, msg) MessageBox(handle, msg, MB_CAPTIONWARNING, MB_ICONWARNING)
//---------------------------------------------------------------------------
#endif
