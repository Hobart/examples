
// TTTDlg.h : header file
//

#pragma once


// CTTTDlg dialog
class CTTTDlg : public CDialogEx
{
// Construction
public:
	CTTTDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TTT_DIALOG };
#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	CBitmap m_cbBoard;
	UINT_PTR m_pTimer;
	typedef UINT16 BOARDMAP;  // bitmap of tic-tac-toe board, 9 bits used
	static BOARDMAP bmWinCondition[8];
	BOARDMAP m_bmX, m_bmO;
	BOARDMAP m_bGameWon;  // set to the winning pattern for victory blink
	BOOL m_bXtoPlay;
	void ResetBoard();

// Implementation
protected:
	// Generated message map functions
	afx_msg HCURSOR OnQueryDragIcon();
	HICON m_hIcon;
	HICON m_hiX;
	HICON m_hiO;

public:
	afx_msg void OnPaint();
	afx_msg void OnBnClickedAny(UINT nID);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
};
