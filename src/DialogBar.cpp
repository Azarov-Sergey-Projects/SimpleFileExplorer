#include "DialogBar.h"



LRESULT DialogBar::OnCommand( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	
	switch( wParam )
	{
		case IDC_BUTTON_EXIT:
			OnCloseCmd( uMsg, wParam, lParam, bHandled );
			return 0;
		case IDC_BUTTON_APPLY:
			ListView.DeleteAllItems();
			GetDlgItemTextW( IDC_SEARCH_TEXT_BAR, FilePath );
			if( FilePath.IsEmpty() )
			{
				MessageBox( TEXT( "Empty string, cant start searching" ), TEXT( "Error" ), MB_OK );
				return 0;
			}
			else
			{
				//MutexForThread.lock();
				if( FindThread.joinable() )
				{
					ListView.SetAtomic();
					FindThread.join();
					ListView.SetAtomic();
				}
				imageIndex = 0;
				FindThread = std::thread( ( &Finder::findFile ), this->ListView, FilePath, std::ref(imageIndex) );
				//MutexForThread.unlock();
				return 0;
			}
			return 0;
	}
}

LRESULT DialogBar::OnItemClick( int, LPNMHDR pnmh, BOOL& )
{
	if( !imageFile )
	{
		imageFile.Create( this->m_hWnd,static_cast<ATL::_U_RECT>(ListView.getImagePreViewSize()),NULL, WS_CHILD | WS_VISIBLE|SS_BITMAP,NULL,1,NULL);
	}
	LPNMITEMACTIVATE lpItem = reinterpret_cast< LPNMITEMACTIVATE >( pnmh );
	ListView.getItemText( lpItem->iItem, 1, fileName );


	if( fileName == TEXT( ".bmp" ) )
	{

		ListView.getItemText( lpItem->iItem, 2, fileName );
		Image = reinterpret_cast< HBITMAP >( LoadImageW( NULL, fileName.GetString(),
														 IMAGE_BITMAP, ListView.xGetImageSize(),
														 ListView.yGetImageSize(), LR_LOADFROMFILE ) );
		imageFile.SetBitmap( Image );
		return 0;
	}
	else
	{
		imageFile.DestroyWindow();
		return 0;
	}
}

LRESULT DialogBar::OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
{
	CRect dialogRect;
	GetClientRect( &dialogRect );
	ListView.setDialogSize( dialogRect );
	ListView.create( m_hWnd );
	imageFile.Create( this->m_hWnd,static_cast<ATL::_U_RECT>(ListView.getImagePreViewSize()),NULL, WS_CHILD | WS_VISIBLE|SS_BITMAP,NULL,1,NULL);
	return 0;
}

LRESULT DialogBar::OnCloseCmd( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
{
	if( FindThread.joinable() )
	{
		//StopThread = TRUE;
		FindThread.join();
	}
	EndDialog( NULL );
	return 0;
}

LRESULT DialogBar::OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	CRect rect;
	GetClientRect( &rect );
	ListView.redraw(rect);
	if( imageFile )
	{
		imageFile.DestroyWindow();
		imageFile.Create( this->m_hWnd,static_cast<ATL::_U_RECT>(ListView.getImagePreViewSize()),NULL, WS_CHILD | WS_VISIBLE|SS_BITMAP,NULL,1,NULL);
		Image = reinterpret_cast< HBITMAP >( LoadImageW( NULL, fileName.GetString(), IMAGE_BITMAP, 
														 ListView.xGetImageSize(), ListView.yGetImageSize(),
														 LR_LOADFROMFILE ) );
		imageFile.SetBitmap( Image );
	}
	GetDlgItem( IDC_SEARCH_TEXT ).SetWindowPos( HWND_BOTTOM, rect.left, rect.bottom - 30, rect.right/8, rect.bottom/10, NULL );
	GetDlgItem( IDC_SEARCH_TEXT_BAR ).SetWindowPos( HWND_BOTTOM, rect.left + rect.right / 8, rect.bottom - 30, rect.right / 4, rect.bottom - 30, NULL );
	GetDlgItem( IDC_BUTTON_APPLY ).SetWindowPos( HWND_BOTTOM, rect.right-rect.right/8, rect.bottom - 30, 50, 30,NULL );
	GetDlgItem( IDC_BUTTON_EXIT ).SetWindowPos( HWND_BOTTOM, rect.right - rect.right / 20, rect.bottom - 30, 50, 30, NULL );

	return 0;
}

LRESULT DialogBar::OnColumn( int n, LPNMHDR pnmh, BOOL& )
{
	ListView.sort(pnmh);
	return 0;
}

