#pragma once

#include <thread>
#include <atomic>
#include <Commctrl.h>
#include "resource.h"
#include "resource2.h"
#include "Finder.h"


#define IDC_SEARCH_BAR_1 1500
class MyDialogBar :public CDialogImpl<MyDialogBar>
{
public:
	enum
	{
		IDD = IDD_MYDIALOGBAR
	};

	BEGIN_MSG_MAP( MyDialogBar )
		MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		MESSAGE_HANDLER( WM_COMMAND, OnCommand )
		NOTIFY_CODE_HANDLER( NM_CLICK, OnItemClick )
		NOTIFY_CODE_HANDLER( LVN_COLUMNCLICK, OnColumn )
		MESSAGE_HANDLER( WM_CLOSE, OnCloseCmd )
	END_MSG_MAP()

	LRESULT OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		CRect rect;
		GetClientRect( &rect );
		myListView.Redraw(rect);
		if( imageFile )
		{
			imageFile.DestroyWindow();
			imageFile.Create( this->m_hWnd,static_cast<ATL::_U_RECT>(myListView.GetImagePreViewSize()),NULL, WS_CHILD | WS_VISIBLE|SS_BITMAP,NULL,1,NULL);
			Image = reinterpret_cast< HBITMAP >( LoadImageW( NULL, fileName.GetString(), IMAGE_BITMAP, 
															 myListView.xGetImageSize(), myListView.yGetImageSize(),
															 LR_LOADFROMFILE ) );
			imageFile.SetBitmap( Image );
		}
		GetDlgItem( IDC_SEARCH_TEXT ).SetWindowPos( HWND_BOTTOM, rect.left, rect.bottom - 30, rect.right/8, rect.bottom/10, NULL );
		GetDlgItem( IDC_SEARCH_TEXT_BAR ).SetWindowPos( HWND_BOTTOM, rect.left + rect.right / 8, rect.bottom - 30, rect.right / 4, rect.bottom - 30, NULL );
		GetDlgItem( IDC_BUTTON_APPLY ).SetWindowPos( HWND_BOTTOM, rect.right-rect.right/8, rect.bottom - 30, 50, 30,NULL );
		GetDlgItem( IDC_BUTTON_EXIT ).SetWindowPos( HWND_BOTTOM, rect.right - rect.right / 20, rect.bottom - 30, 50, 30, NULL );
		
		return 0;
	}

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
		if( !imageFile )
		{
			imageFile.Create( this->m_hWnd,static_cast<ATL::_U_RECT>(myListView.GetImagePreViewSize()),NULL, WS_CHILD | WS_VISIBLE|SS_BITMAP,NULL,1,NULL);
		}
		LPNMITEMACTIVATE lpItem = reinterpret_cast< LPNMITEMACTIVATE >( pnmh );
		myListView.GetItemText( lpItem->iItem, 1, fileName );//получаю имя файла с расширением
		CString text;
		text = std::get<1>( myListView.Split( fileName ) );

		if( text == TEXT( ".bmp" ) )
		{
			
			myListView.GetItemText( lpItem->iItem, 2, fileName );//получаю путь к файлу файла
			Image = reinterpret_cast< HBITMAP >( LoadImageW( NULL, fileName.GetString(), IMAGE_BITMAP, 
																	 myListView.xGetImageSize(), myListView.yGetImageSize(),
																	 LR_LOADFROMFILE ) );
			imageFile.SetBitmap( Image );
			return 0;
		}
		else
		{
			return 0;
		}
		return 0;
	}

	LRESULT OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
	{
		CRect dialogRect;
		GetClientRect( &dialogRect );
		myListView.SetDialogSize( dialogRect );
		myListView.create( m_hWnd );
		imageFile.Create( this->m_hWnd,static_cast<ATL::_U_RECT>(myListView.GetImagePreViewSize()),NULL, WS_CHILD | WS_VISIBLE|SS_BITMAP,NULL,1,NULL);
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
	CString fileName;
	Finder myListView;
	std::thread FindThread;
	CStatic imageFile;
	HBITMAP Image;
};