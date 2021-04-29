#pragma once
#include <atlbase.h>
#include <atlwin.h>
#include <atlapp.h>

#include <thread>

#include <Commctrl.h>
#include "resource.h"
#include "resource2.h"
#include "Finder.h"


//static std::atomic_bool StopThread = FALSE;


class DialogBar :public CDialogImpl<DialogBar>
{
public:
	enum
	{
		IDD = IDD_MYDIALOGBAR
	};

	BEGIN_MSG_MAP( DialogBar )
		MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		MESSAGE_HANDLER( WM_COMMAND, OnCommand )
		NOTIFY_CODE_HANDLER( NM_CLICK, OnItemClick )
		NOTIFY_CODE_HANDLER( LVN_COLUMNCLICK, OnColumn )
		MESSAGE_HANDLER( WM_CLOSE, OnCloseCmd )
	END_MSG_MAP()

	LRESULT OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );


	LRESULT OnColumn( int n, LPNMHDR pnmh, BOOL& );


	LRESULT OnCommand( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	

	LRESULT OnItemClick( int, LPNMHDR pnmh, BOOL& );
	

	LRESULT OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ );
	

	LRESULT OnCloseCmd( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ );
	


private:
	CString FilePath;
	CString fileName;
	Finder ListView;
	std::thread FindThread;
	CStatic imageFile;
	HBITMAP Image;
	int i=0;
};