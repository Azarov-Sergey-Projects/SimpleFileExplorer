#pragma once
#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>
#include <atlwin.h>
#include <atlctrls.h>
#include <atlfile.h>
#include <atlstr.h>

#include <tuple>

#include "resource2.h"
class Finder 
{
public:
  /*  static int CALLBACK CompareProc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
    {
        CListViewCtrl* pListCtrl = ( CListViewCtrl* )lParamSort;
        CString    strItem1;
        pListCtrl->GetItemText( lParam1, 0, strItem1 );
        CString    strItem2;
        pListCtrl->GetItemText( lParam2, 0, strItem2 );
        return wcscmp( strItem2.GetString(), strItem1.GetString() );
    }*/
    void create( HWND m_hWnd );
    void findFile ( CString szPath );
    void view_List( CString buf, int i, CString path );
    BOOL initListViewImage( int size, CString path );
    std::tuple<CString, CString> Split( CString buf );
    HWND GetHWND()const
    {
        return my_hWnd;
    }
    DWORD_PTR GetItemData( INT nItem )const
    {
        return myListView.GetItemData( nItem );
    }
private:
    LVITEM lvItem;
    CListViewCtrl myListView;
    CString path;
    HWND my_hWnd;
};