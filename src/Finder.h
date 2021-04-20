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
    HWND GetHWND()const;
    BOOL GetItemText( INT nItem, int nSub, CString& pszText )const;
    void SetDialogSize( CRect rect );
    void SetListViewSize();
    void SetImagePreViewSize();
    CRect GetImagePreViewSize()const;
    INT xGetImageSize()const;
    INT yGetImageSize()const;
    void Redraw(CRect rect);
    void SetColumnSizes();
private:
    INT nameColumnSize;
    INT extentionColumnSize;
    INT pathColumnSize;
    LVITEM lvItem;
    CListViewCtrl myListView;
    CString path;
    CRect sizeDialogBox;
    CRect sizeListView;
    CRect sizeImagePreView;
    HWND my_hWnd;
};