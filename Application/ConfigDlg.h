#pragma once


// ConfigDlg 对话框

class ConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ConfigDlg)

public:
	ConfigDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ConfigDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CONFIG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedOk();

private:
	const CString parameter = _T(".\\parameter.ini");
	int logLevel;
	int isCR;
	CComboBox logList;
	CButton checkIsCR;
};
