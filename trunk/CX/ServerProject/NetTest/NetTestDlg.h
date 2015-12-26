
// NetTestDlg.h : 头文件
//

#pragma once

#include <string>
#include "MsgInfo.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"

#define CONNECTION_PASSWORD	 "~Game@9494~"



// CNetTestDlg 对话框
class CNetTestDlg : public CDialogEx
{
// 构造
public:
	CNetTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_NETTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	// 初始化List控件
	void InitListCtrl();

public:

	// 当前客户端有新消息到来的时候，在主界面中显示新到来的信息
	// 为了减少界面代码对效率的影响，此处使用了内联
	inline void AddInformation(const CString strInfo)
	{
		CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_DATA);
		pList->InsertItem(0,strInfo);
	}
	afx_msg void OnBnClickedButton2();


	unsigned char 		m_packetIdentifier;
	RakNet::RakPeerInterface* m_peer;
	const char*			m_ServerIP;
	int					m_curPort;

	void InitCon();

	HANDLE		m_hServerThread;	
	HANDLE		m_hEvtStop;
	static DWORD WINAPI	ThreadFunc(LPVOID lpParam);	
	bool	subThreadRecv();
	void	Start();
	void	Stop();

	void	OnRecv(RakNet::Packet* pData);
	int		Send(MsgBase* msg);
	void	SendRegister();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
};
