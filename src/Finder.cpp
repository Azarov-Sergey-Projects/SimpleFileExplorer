#include "Finder.h"
#include "DialogBar.h"





void Finder::create( HWND m_hWnd )
{
	ListView_hWnd = Tmp.ListView.Create( m_hWnd, sizeListView, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL |
								 WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
								 LVS_REPORT | LVS_AUTOARRANGE | DS_ABSALIGN |
								 LVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS );
	
	Tmp.ListView.InsertColumn(0, TEXT( "FileName" ), LVCFMT_LEFT, nameColumnSize );
	Tmp.ListView.InsertColumn(1, TEXT( ".*" ), LVCFMT_LEFT, extentionColumnSize );
	Tmp.ListView.InsertColumn(2,  TEXT( "Path" ), LVCFMT_LEFT, pathColumnSize );
	hSmall.Create( GetSystemMetrics( SM_CXSMICON ),
				   GetSystemMetrics( SM_CYSMICON ),
				   ILC_MASK | ILC_COLOR32, 10, 1 );
}

void Finder::findFile( CString szPath,int i )
{
	CFindFile FileSearch;
	CString S = szPath + TEXT( "\\*.*" );
	BOOL bFlag = FileSearch.FindFile( S );
	if( !bFlag )
	{
		MessageBox(ListView_hWnd, TEXT( "Error" ), TEXT( "File not found" ), 0 );
	}
	else
	{
		
		do
		{
			if( FileSearch.IsDots() )
			{
				continue;
			}
			else
			{
				view_List( FileSearch.GetFileName(), i, FileSearch.GetFilePath() );
				i++;
				if( FileSearch.IsDirectory() )
				{
					CFindFile DirectoryFileSeach;
					DirectoryFileSeach.FindFile( FileSearch.GetFilePath() + TEXT( "\\*.*" ) );
					do
					{
						if( DirectoryFileSeach.IsDots() )
						{
							continue;
						}
						view_List( DirectoryFileSeach.GetFileName(), i, DirectoryFileSeach.GetFilePath() );
						i++;
						if( StopThread)
						{
							return;
						}
					} while( DirectoryFileSeach.FindNextFileW() );
					DirectoryFileSeach.Close();
				}
			}
			if( StopThread)
			{
				return;
			}
		} while( FileSearch.FindNextFileW() );
		FileSearch.Close();
	}
	Tmp.ListView.SetImageList( hSmall, 1 );
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
	Tmp.ListView.InsertItem( &lvItem );
	Tmp.ListView.SetItemText( i, 1, extention.GetString() );
	Tmp.ListView.SetItemText( i, 2, path.GetString() );
	DWORD num = GetFileAttributesW( path.GetString() );
	SHGetFileInfoW( path.GetString(), num, &lp, sizeof( lp ),
					SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES );
	hSmall.AddIcon( lp.hIcon );
	DestroyIcon( lp.hIcon );
}

std::tuple<CString, CString> Finder::Split( CString buf )
{
	CString tmp = buf;
	if( buf.Find( TEXT( "." ) ) == -1 )//folder
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
	return ListView_hWnd;
}

BOOL Finder::GetItemText( INT nItem, int nSub,CString& pszText )const
{
	return Tmp.ListView.GetItemText(nItem,nSub,pszText );
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
	Tmp.ListView.SetWindowPos( HWND_BOTTOM, sizeListView, NULL );
}

void Finder::SetColumnSizes()
{
	 nameColumnSize = sizeListView.right / 3;
	 extentionColumnSize =  50;
	 pathColumnSize = sizeListView.right - nameColumnSize - extentionColumnSize;
}

 int CALLBACK CompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
	Finder::AdditionalTmp* ListViewCast =reinterpret_cast<Finder::AdditionalTmp*>(lParamSort);
	CString FirstFile;
	CString SecondFile;
	ListViewCast->ListView.GetItemText( static_cast< int >( lParam1 ),ListViewCast->columnInd, FirstFile );
	ListViewCast->ListView.GetItemText( static_cast<int>(lParam2),ListViewCast->columnInd, SecondFile );
	if( ListViewCast->bReverse )
	{
		return  StrCmpW( SecondFile.GetString(), FirstFile.GetString() );
	}
	else
	{
		return StrCmpW( FirstFile.GetString(),SecondFile.GetString() );
	}
}


void Finder::Sort( LPNMHDR lParamSort )
{
	SetReverse();
	LPNMLISTVIEW list = reinterpret_cast< LPNMLISTVIEW >(lParamSort);
	Tmp.columnInd = list->iSubItem;
	Tmp.ListView.SortItemsEx( &CompareFunc, reinterpret_cast<LPARAM>(&Tmp) );
}

void Finder::SetReverse()
{
	Tmp.bReverse = !Tmp.bReverse;
}

BOOL Finder::GetReverse()const
{
	return Tmp.bReverse;
}