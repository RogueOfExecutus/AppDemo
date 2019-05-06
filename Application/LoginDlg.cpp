// LoginDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "Application.h"
#include "LoginDlg.h"
#include "afxdialogex.h"


// LoginDlg 对话框

IMPLEMENT_DYNAMIC(LoginDlg, CDialogEx)

LoginDlg::LoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LOGIN, pParent)
{

}

LoginDlg::~LoginDlg()
{
}

void LoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_USER, userEdit);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, passwordEdit);
}


BEGIN_MESSAGE_MAP(LoginDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &LoginDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// LoginDlg 消息处理程序


void LoginDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString password;
	passwordEdit.GetWindowText(password);
	if (password == _T("9999"))
	{
		//
	}
	else if (password == _T("000000"))
	{

	}
	else
	{
		//登录失败
		MessageBox(_T("用户名或密码错误"));
		return;
	}
	CDialogEx::OnOK();
}
