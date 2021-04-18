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
class Finder : public CListViewCtrl
{
private:
    LVITEM lvItem;
    CListViewCtrl myListView;
    CString path;
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
    void Create( HWND m_hWnd );
    void FindFile( CString szPath );
    void View_List( CString buf, int i, CString path );
    BOOL InitListViewImage( int size, CString path );
    std::tuple<CString, CString> Split( CString buf );
};