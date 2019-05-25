
// ApplicationDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "Application.h"
#include "ApplicationDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;
using namespace log4cplus;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CApplicationDlg 对话框



CApplicationDlg::CApplicationDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_APPLICATION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSCOMM_PLC, plcComm);
	DDX_Control(pDX, IDC_MSCOMM_SCANNER, scanComm);
	DDX_Control(pDX, IDC_EDIT_SHOW, msgBox);
	DDX_Control(pDX, IDC_COMBO_PLC, plcList);
	DDX_Control(pDX, IDC_COMBO_SCANNER, scannerList);
	DDX_Control(pDX, IDOK, BtOK);
}

BEGIN_MESSAGE_MAP(CApplicationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CApplicationDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CApplicationDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG, &CApplicationDlg::OnBnClickedButtonConfig)
END_MESSAGE_MAP()


// CApplicationDlg 消息处理程序

BOOL CApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	initAll();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CApplicationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CApplicationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CApplicationDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!runFlag)
	{
		if (workOneFlag || workPlcFlag)
		{
			MessageBox(_T("工作线程未结束，请稍后重试"));
			return;
		}
		if (plcList.GetCurSel() == -1 || scannerList.GetCurSel() == -1)
		{
			MessageBox(_T("请选择端口号"));
			return;
		}

		plcList.GetWindowText(plcPort);
		scannerList.GetWindowText(scanPort);

		WritePrivateProfileString(_T("serial"), _T("plc"), plcPort, iniPath);
		WritePrivateProfileString(_T("serial"), _T("scanner"), scanPort, iniPath);

		if (!OpenComm(plcComm, _T("115200,e,7,2"), plcPort) ||
			!OpenComm(scanComm, _T("115200,n,8,1"), scanPort))
		{
			MessageBox(_T("打开串口失败，请检查"));
			return;
		}
		runFlag = true;
		BtOK.SetWindowText(_T("停止"));
		std::thread th1(&CApplicationDlg::workOneThread, this);
		th1.detach();
		std::thread thPlc(&CApplicationDlg::workPlcThread, this);
		thPlc.detach();
	}
	else
	{
		runFlag = false;
		cv1.notify_all();
		BtOK.SetWindowText(_T("启动"));
	}
}


void CApplicationDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	if (runFlag)
		ShowMsg(_T("软件正在运行中，请停止运行后在退出。。。"));
	else
	{
		if (MessageBox(_T("确定要退出程序吗？"), _T("退出提示！"), MB_ICONINFORMATION | MB_YESNO) != IDYES)
			return;
		CDialogEx::OnCancel();
	}
}


void CApplicationDlg::OnBnClickedButtonConfig()
{
	// TODO: 在此添加控件通知处理程序代码
	ConfigDlg dlg;
	dlg.DoModal();
	readConfig();
}
BEGIN_EVENTSINK_MAP(CApplicationDlg, CDialogEx)
	ON_EVENT(CApplicationDlg, IDC_MSCOMM_PLC, 1, CApplicationDlg::OnCommPlc, VTS_NONE)
	ON_EVENT(CApplicationDlg, IDC_MSCOMM_SCANNER, 1, CApplicationDlg::OnCommScanner, VTS_NONE)
END_EVENTSINK_MAP()


void CApplicationDlg::OnCommPlc()
{
	// TODO: 在此处添加消息处理程序代码
	if (!plcCRFlag)
		plcData.clear();
	COleSafeArray safearray_inp;
	long len, k;
	if (plcComm.get_CommEvent() == 2) //值为 2 表示接收缓冲区内有字符
	{
		safearray_inp = plcComm.get_Input();; ///变量转换
		len = safearray_inp.GetOneDimSize(); //得到有效的数据长度
		char *rxdata; //设置 BYTE 数组
		rxdata = new char[len + 1];
		for (k = 0; k < len; k++)
			safearray_inp.GetElement(&k, rxdata + k);
		rxdata[len] = '\0';
		plcData += string(rxdata);
		if (!(plcCRFlag = rxdata[len - 1] != '\r'))
		{
			//唤醒PLC线程继续发送
			cvPlc.notify_all();
			//处理
		}

		LOG4CPLUS_DEBUG(Logger::getInstance(LOG4CPLUS_TEXT("serial")),
			LOG4CPLUS_STRING_TO_TSTRING("获取PLC回复数据：" + string(rxdata)));
		delete[] rxdata;
	}
}


void CApplicationDlg::OnCommScanner()
{
	// TODO: 在此处添加消息处理程序代码
	if (!scanCRFlag)
		scanData.clear();
	COleSafeArray safearray_inp;
	long len, k;
	if (scanComm.get_CommEvent() == 2) //值为 2 表示接收缓冲区内有字符
	{
		safearray_inp = scanComm.get_Input();; ///变量转换
		len = safearray_inp.GetOneDimSize(); //得到有效的数据长度
		char *rxdata; //设置 BYTE 数组
		rxdata = new char[len + 1];
		for (k = 0; k < len; k++)
			safearray_inp.GetElement(&k, rxdata + k);
		//scanCRFlag = rxdata[len - 1] != '\r';
		rxdata[len] = '\0';
		scanData += string(rxdata);
		if (isCR != 1 || rxdata[len - 1] == '\r')
		{
			cvScan.notify_all();
			scanCRFlag = false;
		}
		else
			scanCRFlag = true;

		LOG4CPLUS_INFO(Logger::getInstance(LOG4CPLUS_TEXT("serial")),
			LOG4CPLUS_STRING_TO_TSTRING("获取条码：" + string(rxdata)));
		delete[] rxdata;
	}
}


void CApplicationDlg::ShowMsg(CString msg)
{
	// TODO: 在此处添加实现代码.
	if (msgBox.GetLineCount() > 50)
	{
		CString allMSG;
		msgBox.GetWindowText(allMSG);
		int firstLineLen = allMSG.Find(_T("\r\n"), 0);
		msgBox.SetSel(0, firstLineLen + 2);
		msgBox.ReplaceSel(_T(""));
	}
	CTime time1 = CTime::GetCurrentTime();
	CString cstrTime = time1.Format("[%Y/%m/%d %H:%M:%S] ");

	int len = msgBox.GetWindowTextLengthW();
	msgBox.SetSel(len, len);
	msgBox.ReplaceSel(cstrTime + msg + _T("\r\n"));

}


void CApplicationDlg::initAll()
{
	// TODO: 在此处添加实现代码.
	checkFile("log");
	initConfig();
	initSerial();
	initLog4cplus(workLog, "work", ".\\log\\work.log");
	initLog4cplus(serialLog, "serial", ".\\log\\serial.log");
	initLog4cplus(mesLog, "mes", ".\\log\\mes.log");
}


void CApplicationDlg::checkFile(const char* path)
{
	// TODO: 在此处添加实现代码.
	if (_access(path, 0) == -1)
		_mkdir(path);
}


void CApplicationDlg::initConfig()
{
	// TODO: 在此处添加实现代码.
	CFileFind finder;   //查找是否存在ini文件，若不存在，则生成一个新的默认设置的ini文件，这样就保证了我们更改后的设置每次都可用   
	if (!finder.FindFile(iniPath))
	{
		WritePrivateProfileString(_T("log"), _T("LogLevel"), _T("20000"), iniPath);
		WritePrivateProfileString(_T("serial"), _T("plc"), _T("COM3"), iniPath);
		WritePrivateProfileString(_T("serial"), _T("scanner"), _T("COM4"), iniPath);
		WritePrivateProfileString(_T("config"), _T("EOFCR"), _T("0"), iniPath);
	}
	readConfig();
}

void CApplicationDlg::readConfig()
{
	// TODO: 在此处添加实现代码.
	logLevel = GetPrivateProfileInt(_T("log"), _T("LogLevel"), INFO_LOG_LEVEL, iniPath);
	GetPrivateProfileString(_T("serial"), _T("plc"), _T("COM3"), plcPort.GetBuffer(6), 6, iniPath);
	GetPrivateProfileString(_T("serial"), _T("scanner"), _T("COM4"), scanPort.GetBuffer(6), 6, iniPath);
	isCR = GetPrivateProfileInt(_T("config"), _T("EOFCR"), 0, iniPath);

	plcPort.ReleaseBuffer();
	scanPort.ReleaseBuffer();
}

void CApplicationDlg::initSerial()
{
	// TODO: 在此处添加实现代码.
	vector<CString> comms;
	int len = GetPCPort(comms);
	if (len)
	{
		for (int i = 0; i < len; i++)
		{
			plcList.AddString(comms[i]);
			scannerList.AddString(comms[i]);
		}
		plcList.SetCurSel(plcList.FindStringExact(0, plcPort));
		scannerList.SetCurSel(scannerList.FindStringExact(0, scanPort));
	}
	else
	{
		//无串口
	}
}

int CApplicationDlg::GetPCPort(vector<CString> &comms)
{
	// TODO: 在此处添加实现代码.
	long lReg;
	HKEY hKey;
	DWORD MaxValueLength;
	DWORD dwValueNumber;

	lReg = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"),
		0, KEY_QUERY_VALUE, &hKey);

	if (lReg != ERROR_SUCCESS) //成功时返回ERROR_SUCCESS，
	{
		MessageBox(_T("Open Registry Error!"));
		return 0;
	}

	lReg = RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL,
		&dwValueNumber, &MaxValueLength, NULL, NULL, NULL);

	if (lReg != ERROR_SUCCESS) //没有成功
	{
		MessageBox(_T("Getting Info Error!"));
		return 0;
	}

	TCHAR *pValueName, *pCOMNumber;
	DWORD cchValueName, dwValueSize = 254;

	comms = vector<CString>(dwValueNumber);

	for (DWORD i = 0; i < dwValueNumber; i++)
	{
		cchValueName = MaxValueLength + 1;
		dwValueSize = 254;  //端口数
		pValueName = (TCHAR*)VirtualAlloc(NULL, cchValueName, MEM_COMMIT, PAGE_READWRITE);
		lReg = RegEnumValue(hKey, i, pValueName,
			&cchValueName, NULL, NULL, NULL, NULL);

		if ((lReg != ERROR_SUCCESS) && (lReg != ERROR_NO_MORE_ITEMS))
		{
			MessageBox(_T("Enum Registry Error or No More Items!"));
			return FALSE;
		}

		pCOMNumber = (TCHAR*)VirtualAlloc(NULL, 6, MEM_COMMIT, PAGE_READWRITE);
		lReg = RegQueryValueEx(hKey, pValueName, NULL,
			NULL, (LPBYTE)pCOMNumber, &dwValueSize);

		if (lReg != ERROR_SUCCESS)
		{
			MessageBox(_T("Can not get the name of the port"));
			return FALSE;
		}

		CString str(pCOMNumber);
		comms[i] = str;
		VirtualFree(pValueName, 0, MEM_RELEASE);
		VirtualFree(pCOMNumber, 0, MEM_RELEASE);
	}

	return dwValueNumber;
}


void CApplicationDlg::initLog4cplus(Logger &logger, string name, string path)
{
	// TODO: 在此处添加实现代码.
	SharedAppenderPtr fileAppender(new RollingFileAppender(
		LOG4CPLUS_STRING_TO_TSTRING(path),
		5 * 1024 * 1024,
		5
	)
	);

	fileAppender->setName(LOG4CPLUS_TEXT("test"));
	tstring pattern = LOG4CPLUS_TEXT("%D{%y/%m/%d %H:%M:%S,%Q} [%t] %-5p %c - %m [%l]%n");
	fileAppender->setLayout(unique_ptr<Layout>(new PatternLayout(pattern)));

	Logger logger = Logger::getInstance(LOG4CPLUS_STRING_TO_TSTRING(name));
	logger.setLogLevel(logLevel);

	logger.addAppender(fileAppender);
}

//后面要delete
char *CApplicationDlg::UnicodeToUtf8(const wchar_t *unicode)
{
	int len;
	len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);
	char *szUtf8 = new char[len + 1];
	//memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, unicode, -1, szUtf8, len, NULL, NULL);
	szUtf8[len] = '\0';
	return szUtf8;
}


CString CApplicationDlg::UTF82WCS(const char *szU8, size_t len)
{
	//预转换，得到所需空间的大小;
	int wcsLen = MultiByteToWideChar(CP_UTF8, NULL, szU8, len, NULL, 0);

	//分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间
	wchar_t* wszString = new wchar_t[wcsLen + 1];

	//转换
	MultiByteToWideChar(CP_UTF8, NULL, szU8, len, wszString, wcsLen);

	//最后加上'\0'
	wszString[wcsLen] = '\0';

	CString unicodeString(wszString);

	delete[] wszString;
	wszString = NULL;

	return unicodeString;
}

bool CApplicationDlg::OpenComm(CMSComm &comm, LPCTSTR setter, CString portName)
{
	// TODO: 在此处添加实现代码.
	portName.Delete(0, 3);
	int portNum = _ttoi(portName);//将其转化为数值类型
	if (comm.get_PortOpen())
		comm.put_PortOpen(FALSE);
	comm.put_CommPort(portNum); //选择COM
	comm.put_InBufferSize(1024); //接收缓冲区
	comm.put_OutBufferSize(1024);//发送缓冲区
	comm.put_InputLen(0);//设置当前接收区数据长度为0,表示全部读取
	comm.put_InputMode(1);//以二进制方式读写数据
	comm.put_RThreshold(1);//接收缓冲区有1个及1个以上字符时，将引发接收数据的OnComm
	comm.put_Settings(setter);//波特率，检验位，数据位，停止位
	if (!comm.get_PortOpen())
	{
		try
		{
			comm.put_PortOpen(TRUE);//打开串口
			LOG4CPLUS_INFO(Logger::getInstance(LOG4CPLUS_TEXT("serial")),
				LOG4CPLUS_STRING_TO_TSTRING("打开串口 COM" + to_string(portNum)));
			return true;
		}
		catch (...)
		{
			LOG4CPLUS_INFO(Logger::getInstance(LOG4CPLUS_TEXT("serial")),
				LOG4CPLUS_STRING_TO_TSTRING("打开串口 COM" + to_string(portNum) + "失败"));
			return false;
		}
	}
	return false;
}


CString CApplicationDlg::getXOR(char *data)
{
	// TODO: 在此处添加实现代码.
	int len = strlen(data);
	char xor = data[0];
	for (int i = 1; i < len; i++)
	{
		xor ^= data[i];
	}
	CString XOR;
	XOR.Format(_T("%02X"), xor);
	return CString(data) + XOR + _T("*\r");
}

void CApplicationDlg::workOneThread()
{
	// TODO: 在此处添加实现代码.
	workOneFlag = true;
	LOG4CPLUS_INFO(Logger::getInstance(LOG4CPLUS_TEXT("work")),
		LOG4CPLUS_TEXT("工作线程启动。。"));
	while (runFlag)
	{
		unique_lock<mutex> lock(mtx1);
		cv1.wait(lock);
		lock.unlock();
		if (!runFlag)
			break;

		//工位1触发，先扫码，情况不同再具体修改
		unique_lock<mutex> lockScan(mtxScan);
		scanComm.put_Output(COleVariant(scanTrigger));
		cv_status result = cvScan.wait_for(lockScan, chrono::milliseconds(1000));
		scanComm.put_Output(COleVariant(scanCloser));
		lockScan.unlock();
		if (!runFlag)
			break;

		if (cv_status::timeout == result)
		{
			//超时
			ShowMsg(_T("扫码超时异常，请检查"));

		}
		else
		{

		}
	}
	LOG4CPLUS_INFO(Logger::getInstance(LOG4CPLUS_TEXT("work")),
		LOG4CPLUS_TEXT("工作线程停止。。"));
	workOneFlag = false;
}

void CApplicationDlg::workPlcThread()
{
	workPlcFlag = true;
	LOG4CPLUS_INFO(Logger::getInstance(LOG4CPLUS_TEXT("serial")),
		LOG4CPLUS_TEXT("PLC线程启动。。"));
	int timeoutTimes = 0;
	while (runFlag)
	{
		unique_lock<mutex> lock(mtxPlc);
		if (trigFlagOne)
		{
			//复位
			plcComm.put_Output(COleVariant(resetTriggerOne));
			trigFlagOne = false;
		}
		else if (sendFlag)
		{
			//发送特殊数据
			CString msg = _T("");
			unique_lock<mutex> lockMsg(mtxPLCMsg);
			if (msgs.empty())
			{
				sendFlag = false;
			}
			else
			{
				msg = msgs[0];
				msgs.erase(msgs.begin());
				if (msgs.empty())
					sendFlag = false;
			}
			lockMsg.unlock();
			//sendFlag = false;
			if (msg == _T(""))
			{
				LOG4CPLUS_INFO(Logger::getInstance(LOG4CPLUS_TEXT("serial")),
					LOG4CPLUS_TEXT("发送PLC数据为空"));
				continue;
			}
			plcComm.put_Output(COleVariant(msg));

			char *umsg = UnicodeToUtf8(msg);
			LOG4CPLUS_INFO(Logger::getInstance(LOG4CPLUS_TEXT("serial")),
				LOG4CPLUS_STRING_TO_TSTRING("发送PLC数据：" + string(umsg)));
			delete[] umsg;
		}
		else
		{
			//检查触发
			plcComm.put_Output(COleVariant(checkTrigger));
		}
		//等待PLC回复
		if (cv_status::timeout == cvPlc.wait_for(lock, chrono::milliseconds(1000)))
		{
			timeoutTimes++;
			if (timeoutTimes >= 3)
			{
				ShowMsg(_T("PLC连续3次以上回复超时，请检查通信！！"));
			}
		}
		else
			timeoutTimes = 0;
		lock.unlock();
	}
	LOG4CPLUS_INFO(Logger::getInstance(LOG4CPLUS_TEXT("serial")),
		LOG4CPLUS_TEXT("PLC线程停止。。"));
	workPlcFlag = false;
}

void CApplicationDlg::Add2SendMSG(CString msg)
{
	// TODO: 在此处添加实现代码.
	unique_lock<mutex> lockMsg(mtxPLCMsg);
	msgs.push_back(msg);
	sendFlag = true;
	lockMsg.unlock();
}
