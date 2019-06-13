
// ApplicationDlg.h: 头文件
//

#pragma once

#include "log4cplus/log4cplus.h"
#include <condition_variable>
#include <thread>
#include <mutex>
#include <string.h>
#include "CMSComm.h"
#include <vector>
#include "ConfigDlg.h"
#include <io.h>
#include <direct.h>

// CApplicationDlg 对话框
class CApplicationDlg : public CDialogEx
{
// 构造
public:
	CApplicationDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APPLICATION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonConfig();
	DECLARE_EVENTSINK_MAP()
	void OnCommPlc();
	void OnCommScanner();
private:
	int logLevel;
	int isCR;
	CString plcPort;
	CString scanPort;
	std::string plcData;
	std::string scanData;
	bool runFlag;
	void workOneThread();
	bool workOneFlag;
	std::string workOneValue;//工位寄存器
	void workPlcThread();
	bool workPlcFlag;
	bool trigFlagOne;
	void Add2SendMSG(CString msg);
	bool sendFlag;
	char returnMsg[256];
	std::vector<CString> msgs;
	const CString checkTrigger = _T("");
	const CString resetTriggerOne = _T("");
	const CString sendOKOne = _T("");
	const CString sendNGOne = _T("");
	const CString scanTrigger = _T("\x16\x54\x0D");
	const CString scanCloser = _T("\x16\x55\x0D");
	bool plcCRFlag;
	bool scanCRFlag;
	std::mutex mtx1;
	std::condition_variable cv1;
	std::mutex mtxPlc;
	std::condition_variable cvPlc;
	std::mutex mtxScan;
	std::condition_variable cvScan;
	std::mutex mtxPLCMsg;
	std::condition_variable cvPLCMsg;
	const CString iniPath = _T(".\\parameter.ini");
	CMSComm plcComm;
	CMSComm scanComm;
	CEdit msgBox;
	void ShowMsg(CString msg);
	void initAll();
	void checkFile(const char* path);
	void initConfig();
	void readConfig();
	void initSerial();
	int GetPCPort(std::vector<CString>& comms);
	void initLog4cplus(std::string name, std::string path);
	char * UnicodeToUtf8(const wchar_t * unicode);
	CString UTF82WCS(const char * szU8, size_t len);
	bool OpenComm(CMSComm & comm, LPCTSTR setter, CString portName);
	void closeComm();
	CString getXOR(char * data);
	CComboBox plcList;
	CComboBox scannerList;
	CButton BtOK;
};
