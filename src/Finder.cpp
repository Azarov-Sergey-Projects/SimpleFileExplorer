#include "Finder.h"

void Finder::create( HWND m_hWnd )
{
	my_hWnd = myListView.Create( m_hWnd, sizeListView, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL |
								 WS_SIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
								 LVS_REPORT | LVS_AUTOARRANGE | DS_ABSALIGN |
								 LVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS );
	
	myListView.InsertColumn(0, TEXT( "Название" ), LVCFMT_LEFT, nameColumnSize );
	myListView.InsertColumn(1, TEXT( ".*" ), LVCFMT_LEFT, extentionColumnSize );
	myListView.InsertColumn(2,  TEXT( "Полный путь" ), LVCFMT_LEFT, pathColumnSize );
	hSmall.Create( GetSystemMetrics( SM_CXSMICON ),
				   GetSystemMetrics( SM_CYSMICON ),
				   ILC_MASK | ILC_COLOR32, 10, 1 );
}

void Finder::findFile( CString szPath,int i )
{
	CFindFile F;
	CString S = szPath + TEXT( "\\*.*" );
	BOOL bFlag = F.FindFile( S );
	if( !bFlag )
	{
		MessageBox(my_hWnd, TEXT( "Error" ), TEXT( "File not found" ), 0 );
	}
	else
	{
		do
		{
			if( F.IsDots() )
			{
				continue;
			}
			else
			{
				view_List( F.GetFileName(), i, F.GetFilePath() );
				i++;
				if( F.IsDirectory() )
				{
					CFindFile G;
					G.FindFile( F.GetFilePath() + TEXT( "\\*.*" ) );
					do
					{
						if( G.IsDots() )
						{
							continue;
						}
						view_List( G.GetFileName(), i, G.GetFilePath() );
						i++;
					} while( G.FindNextFileW() );
					G.Close();
				}
			}
		} while( F.FindNextFileW() );
		F.Close();
	}
	myListView.SetImageList( hSmall, 1 );
	//initListViewImage(S.GetString() );
	//myListView.SetImageList( hSmall, 1 );
}

void Finder::view_List( CString name, int i, CString path )
{
	SHFILEINFOW lp{};
	CString nameWithoutEx = std::get<0>( Split( name ) );
	CString extention = std::get<1>( Split( name ) );
	lvItem.mask = LVIF_IMAGE | LVIF_TEXT;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iItem = i;
	lvItem.iImage = i;
	lvItem.iSubItem = 0;
	lvItem.pszText = const_cast< LPWSTR >( nameWithoutEx.GetString() );
	lvItem.cchTextMax = nameWithoutEx.GetLength();
	myListView.InsertItem( &lvItem );
	myListView.SetItemText( i, 1, extention.GetString() );
	myListView.SetItemText( i, 2, path.GetString() );
	DWORD num = GetFileAttributesW( path.GetString() );
	SHGetFileInfoW( path.GetString(), num, &lp, sizeof( lp ),
					SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES );
	hSmall.AddIcon( lp.hIcon );
	DestroyIcon( lp.hIcon );
}

std::tuple<CString, CString> Finder::Split( CString buf )
{
	CString tmp = buf;
	if( buf.Find( TEXT( "." ) ) == -1 )//папка
	{
		return { buf.GetString(),TEXT( "Папка" ) };
	}
	else
	{
		int index = buf.Find( TEXT( "." ) ) + 1;
		if( buf.Find( TEXT( "." ), index ) != -1 )
		{
			while( buf.Find( TEXT( "." ), index ) != -1 )
			{
				index = buf.Find( TEXT( "." ), index ) + 1;
			}
		}
		buf.Delete( index - 1, buf.GetLength() );
		tmp.Delete( 0, index - 1 );
		return { buf.GetString(), tmp.GetString() };
	}
}

HWND Finder::GetHWND()const
{
	return my_hWnd;
}

BOOL Finder::GetItemText( INT nItem, int nSub,CString& pszText )const
{
	return myListView.GetItemText(nItem,nSub,pszText );
}

void Finder::SetDialogSize( CRect rect )
{
	sizeDialogBox = rect;
	SetListViewSize();
	SetImagePreViewSize();
	SetColumnSizes();
}

void Finder::SetListViewSize()
{
	sizeListView.bottom= sizeDialogBox.bottom-sizeDialogBox.bottom/8;
	sizeListView.top =sizeDialogBox.top;
	sizeListView.right =sizeDialogBox.right-sizeDialogBox.right/2.5;
	sizeListView.left =sizeDialogBox.left;
}

void Finder::SetImagePreViewSize()
{
	sizeImagePreView.top = sizeListView.top;
	sizeImagePreView.left = sizeListView.right;
	sizeImagePreView.bottom = sizeDialogBox.bottom-sizeDialogBox.bottom/8;
	sizeImagePreView.right = sizeDialogBox.right;
}

CRect Finder::GetImagePreViewSize()const
{
	return sizeImagePreView;
}

INT Finder::xGetImageSize()const
{
	return sizeDialogBox.right - sizeListView.right;
}

INT Finder::yGetImageSize()const
{
	return sizeListView.bottom;
}

void Finder::Redraw( CRect rect )
{
	SetDialogSize( rect );
	myListView.SetWindowPos( HWND_BOTTOM, sizeListView, NULL );
}

void Finder::SetColumnSizes()
{
	 nameColumnSize = sizeListView.right / 3;
	 extentionColumnSize =  50;
	 pathColumnSize = sizeListView.right - nameColumnSize - extentionColumnSize;
}


 int CALLBACK CompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
	CListViewCtrl* myListView = reinterpret_cast<CListViewCtrl*>(lParamSort);
	CString FirstFile;
	CString SecondFile;
	myListView->GetItemText( static_cast< int >( lParam1 ),columnInd, FirstFile );
	myListView->GetItemText( static_cast<int>(lParam2),columnInd, SecondFile );
	if( bReverse )
	{
		return  StrCmpW( SecondFile.GetString(), FirstFile.GetString() );
	}
	else
	{
		return StrCmpW( FirstFile.GetString(),SecondFile.GetString() );
	}
}


void Finder::Sort( LPNMHDR func )
{
	bReverse = !bReverse;
	LPNMLISTVIEW list = ( LPNMLISTVIEW )func;
	columnInd = list->iSubItem;
	myListView.SortItemsEx( &CompareFunc, (LPARAM)func );
}
