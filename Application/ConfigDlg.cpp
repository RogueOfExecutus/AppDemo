// ConfigDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "Application.h"
#include "ConfigDlg.h"
#include "afxdialogex.h"


// ConfigDlg 对话框

IMPLEMENT_DYNAMIC(ConfigDlg, CDialogEx)

ConfigDlg::ConfigDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CONFIG, pParent)
{

}

ConfigDlg::~ConfigDlg()
{
}

void ConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LOGLEVEL, logList);
	DDX_Control(pDX, IDC_CHECK_EOFCR, checkIsCR);
}


BEGIN_MESSAGE_MAP(ConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &ConfigDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// ConfigDlg 消息处理程序


BOOL ConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CFileFind finder;   //查找是否存在ini文件，若不存在，则生成一个新的默认设置的ini文件，这样就保证了我们更改后的设置每次都可用   
	if (finder.FindFile(parameter))
	{
		logLevel = GetPrivateProfileInt(_T("log"), _T("LogLevel"), 20000, parameter);
		isCR = GetPrivateProfileInt(_T("config"), _T("EOFCR"), 0, parameter);

		logList.InsertString(0, _T("TRACE"));
		logList.InsertString(1, _T("DEBUG"));
		logList.InsertString(2, _T("INFO"));
		logList.InsertString(3, _T("WARN"));
		logList.InsertString(4, _T("ERROR"));
		logList.InsertString(5, _T("FATAL"));
		logList.SetCurSel(logLevel / 10000);

		checkIsCR.SetCheck(isCR);
	}
	else
	{
		MessageBox(_T("配置文件未找到，请检查！"));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void ConfigDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString temp;
	temp.Format(_T("%d"), logList.GetCurSel() * 10000);
	WritePrivateProfileString(_T("log"), _T("LogLevel"), temp, parameter);
	temp.Format(_T("%d"), checkIsCR.GetCheck());
	WritePrivateProfileString(_T("config"), _T("EOFCR"), temp, parameter);
	CDialogEx::OnOK();
}
