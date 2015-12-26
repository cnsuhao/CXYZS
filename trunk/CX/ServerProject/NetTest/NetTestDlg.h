
// NetTestDlg.h : ͷ�ļ�
//

#pragma once

#include <string>
#include "MsgInfo.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"

#define CONNECTION_PASSWORD	 "~Game@9494~"



// CNetTestDlg �Ի���
class CNetTestDlg : public CDialogEx
{
// ����
public:
	CNetTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_NETTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	// ��ʼ��List�ؼ�
	void InitListCtrl();

public:

	// ��ǰ�ͻ���������Ϣ������ʱ��������������ʾ�µ�������Ϣ
	// Ϊ�˼��ٽ�������Ч�ʵ�Ӱ�죬�˴�ʹ��������
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
