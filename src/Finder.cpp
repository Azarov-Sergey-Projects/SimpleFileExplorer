#include "Finder.h"



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


void Finder::findFile( CString szPath,int& i )
{
	CString ExtentionFILE;
	CFindFile FileSearch;
	CString S = szPath + TEXT( "\\*.*" );
	BOOL bFlag = FileSearch.FindFile( S );
	if( !bFlag )
	{
		MessageBox( ListView_hWnd, TEXT( "Error" ), TEXT( "File not found" ), 0 );
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
				if( FileSearch.IsDirectory() )
				{
					findFile( FileSearch.GetFilePath(),i );
				}
				view_List( FileSearch.GetFileName(), i, FileSearch.GetFilePath() );
				i++;
			}
		} while( FileSearch.FindNextFileW() );
		FileSearch.Close();
	}
	Tmp.ListView.SetImageList( hSmall, 1 );
}
				
void Finder::view_List( CString name, int i, CString path )
{
	SHFILEINFOW lp{};
	CString nameWithoutEx = std::get<0>( split( name ) );
	CString extention = std::get<1>( split( name ) );
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

std::tuple<CString, CString> Finder::split( CString buf )
{
	CString tmp = buf;
	if( buf.Find( TEXT( "." ),0 ) == -1)
	{
		return { buf.GetString(),TEXT( "Directory" ) };
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

HWND Finder::getHWND()const
{
	return ListView_hWnd;
}

BOOL Finder::getItemText( INT nItem, int nSub,CString& pszText )const
{
	return Tmp.ListView.GetItemText(nItem,nSub,pszText );
}

void Finder::setDialogSize( CRect rect )
{
	sizeDialogBox = rect;
	setListViewSize();
	setImagePreViewSize();
	setColumnSizes();
}

void Finder::setListViewSize()
{
	sizeListView.bottom= sizeDialogBox.bottom-sizeDialogBox.bottom/8;
	sizeListView.top =sizeDialogBox.top;
	sizeListView.right =sizeDialogBox.right-sizeDialogBox.right/2.5;
	sizeListView.left =sizeDialogBox.left;
}

void Finder::setImagePreViewSize()
{
	sizeImagePreView.top = sizeListView.top;
	sizeImagePreView.left = sizeListView.right;
	sizeImagePreView.bottom = sizeDialogBox.bottom-sizeDialogBox.bottom/8;
	sizeImagePreView.right = sizeDialogBox.right;
}

CRect Finder::getImagePreViewSize()const
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

void Finder::redraw( CRect rect )
{
	setDialogSize( rect );
	Tmp.ListView.SetWindowPos( HWND_BOTTOM, sizeListView, NULL );
}

void Finder::setColumnSizes()
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

void Finder::sort( LPNMHDR lParamSort )
{
	setReverse();
	LPNMLISTVIEW list = reinterpret_cast< LPNMLISTVIEW >(lParamSort);
	Tmp.columnInd = list->iSubItem;
	Tmp.ListView.SortItemsEx( &CompareFunc, reinterpret_cast<LPARAM>(&Tmp) );
}

void Finder::setReverse()
{
	Tmp.bReverse = !Tmp.bReverse;
}

BOOL Finder::getReverse()const
{
	return Tmp.bReverse;
}