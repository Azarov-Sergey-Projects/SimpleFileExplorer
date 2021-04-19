#pragma once

#include <thread>
#include <atomic>
#include <Commctrl.h>
#include "resource.h"
#include "resource2.h"
#include "Finder.h"

class MyDialogBar :public CDialogImpl<MyDialogBar>
{
public:
	enum
	{
		IDD = IDD_MYDIALOGBAR
	};

	BEGIN_MSG_MAP( MyDialogBar )
		MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
		MESSAGE_HANDLER( WM_COMMAND, OnCommand )
		NOTIFY_CODE_HANDLER( NM_CLICK, OnItemClick )
		NOTIFY_CODE_HANDLER( LVN_COLUMNCLICK, OnColumn )
		MESSAGE_HANDLER( WM_CLOSE, OnCloseCmd )
	END_MSG_MAP()

	LRESULT OnColumn( int n, LPNMHDR pnmh, BOOL& )
	{
		//myListView.m_hWnd = this->m_hWnd;
		//myListView.SortItems(myListView.CompareProc, (LPARAM)pnmh);
		//ListView_SortItems(this->m_hWnd, CompareProc, (LPARAM)pnmh);
	//	MessageBox(TEXT("I am gere"), TEXT("Here"), NULL);
		return 0;
	}

	LRESULT OnCommand( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{

		switch( wParam )
		{
			case IDC_BUTTON_EXIT:
				OnCloseCmd( uMsg, wParam, lParam, bHandled );
				return 0;
			case IDC_BUTTON_APPLY:
				ListView_DeleteAllItems(myListView.GetHWND());
				GetDlgItemTextW( IDC_SEARCH_TEXT_BAR, FilePath );
				if( FilePath.IsEmpty() )
				{
					MessageBox( TEXT( "Empty string, cant start searching" ), TEXT( "Error" ), MB_OK );
					return 0;
				}
				else
				{
					if( FindThread.joinable() )
					{
						FindThread.join();
						FindThread = std::thread( ( &Finder::findFile ), this->myListView, FilePath );
					}
					else
					{
						FindThread = std::thread( ( &Finder::findFile ), this->myListView, FilePath );
					}
				}
				return 0;
		}
		return 0;
	}

	LRESULT OnItemClick( int, LPNMHDR pnmh, BOOL& )
	{
		CString fileName;
		LPNMITEMACTIVATE lpItem = ( LPNMITEMACTIVATE )pnmh;
		LVITEM ImageInfo{};
		ImageInfo.iItem = lpItem->iItem;
		ImageInfo.mask = LVIF_PARAM | LVIF_TEXT;
		fileName.SetString(reinterpret_cast<LPCWSTR>(myListView.GetItemData( lpItem->iItem )));
		CString text;
		text = std::get<1>( myListView.Split( fileName ) );

		if( text.GetString() == TEXT( ".bmp" ) || text.GetString() == TEXT( ".jpeg" ) )
		{
			HBITMAP Image = reinterpret_cast< HBITMAP >( LoadImage( NULL, ImageInfo.pszText, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) );
			MessageBox( TEXT( "I am here" ), TEXT( "Here" ) );
			return 0;
		}
		else
		{
			//MessageBox(TEXT("I am here ELSE"), TEXT("Here"));
			return 0;
		}
		return 0;
	}


	LRESULT OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
	{
		myListView.create( m_hWnd );
		return 0;
	}

	LRESULT OnCloseCmd( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
	{
		if( FindThread.joinable() )
		{
			FindThread.join();
		}
		EndDialog( NULL );
		return 0;
	}


private:
	std::atomic <BOOL> atom=false;
	CString FilePath;
	Finder myListView;
	std::thread FindThread;
};