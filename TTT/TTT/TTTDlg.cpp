// TTTDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TTT.h"
#include "TTTDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTTTDlg dialog

CTTTDlg::CTTTDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TTT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hiX = AfxGetApp()->LoadIcon(IDI_X);
	m_hiO = AfxGetApp()->LoadIcon(IDI_O);
	m_cbBoard.LoadBitmap(IDB_BOARD);
}

void CTTTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTTTDlg, CDialogEx)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_SQUARE1, IDC_SQUARE9, &OnBnClickedAny)
	ON_WM_QUERYDRAGICON()
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CTTTDlg message handlers
// Clear all game-relevant members, reset icons
void CTTTDlg::ResetBoard()
{
	UINT x = IDC_SQUARE1;
	UINT y = IDC_SQUARE9;
	for (UINT i = x; i <= y; i++) {
		CMFCButton* pButton = (CMFCButton*)(this->GetDlgItem(i));
		pButton->CleanUp();
		pButton->SetFaceColor(RGB(255, 255, 255), TRUE);
		pButton->Invalidate();
	}
	m_bmO = m_bmX = m_bGameWon = 0;
	m_bXtoPlay = 1;
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTTTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CTTTDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


// Winning game condition bitmaps, initialized in octal
CTTTDlg::BOARDMAP CTTTDlg::bmWinCondition[8] = {
	0007,  // 000 000 111 , 3-in-a-row across
	0070,  // 000 111 000
	0700,  // 111 000 000
	0111,  // 001 001 001 , 3-in-a-row vertical
	0222,  // 010 010 010
	0444,  // 100 100 100
	0421,  // 100 010 001 , diagonal
	0124   // 001 010 100 , diagonal
};


afx_msg void CTTTDlg::OnBnClickedAny(UINT nID)
{
	// TODO: Add your control notification handler code here

	if (this->m_bGameWon)
		return;
	const UINT8 SQUARE = nID - IDC_SQUARE1;  // button is 0-8
	CMFCButton* pButton = (CMFCButton*)(this->GetDlgItem(nID));
	CMFCButton& button = *pButton;
	BOARDMAP& boardMap = (m_bXtoPlay ? m_bmX : m_bmO);
	HICON& playericon = (m_bXtoPlay ? m_hiX : m_hiO);
	const BOARDMAP myMap = (0001 << SQUARE);
	const BOARDMAP (&bmWin)[8] = CTTTDlg::bmWinCondition;

	if ((m_bmX | m_bmO) == 0777) {  // All squares full, reset game.
		ResetBoard();
		return;
	}
	if (myMap & (m_bmX | m_bmO)) {  // Square already taken.
		return;
	}
	else {
		boardMap |= myMap;  // Mark square as taken

		// Set square's icon
		button.SetImage(playericon, FALSE, playericon, NULL);

		for (int i = 0; i <= 7; i++) {
			if ((boardMap & bmWin[i]) == bmWin[i]) {
				m_bGameWon = bmWin[i];
				m_pTimer = SetTimer(1, 150, NULL);
				return;
			}
		}
		m_bXtoPlay = !m_bXtoPlay;  // Set next player's turn
	}
}

void CTTTDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	static int count = 15;
	BOARDMAP pattern = m_bGameWon;
	for (int i = 0; i <= 8; i++) {
		CMFCButton *button = (CMFCButton *)GetDlgItem(IDC_SQUARE1+i);
		if (pattern & 1)
			button->EnableWindow(!(count % 2));
		pattern = pattern >> 1;
	}
	if (count-- == 0) {
		// Clear board and reset
		KillTimer(nIDEvent);
		count = 15;
		ResetBoard();
	}


	CDialogEx::OnTimer(nIDEvent);
}


BOOL CTTTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	SetBackgroundImage(m_cbBoard, BACKGR_TOPLEFT, FALSE, TRUE);
	ResetBoard();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
