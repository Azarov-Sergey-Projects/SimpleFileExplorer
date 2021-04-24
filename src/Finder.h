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


static int CALLBACK CompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );

static BOOL bReverse = TRUE;


class Finder 
{
public:
    void create( HWND m_hWnd );
    void findFile( CString szPath,int i );
    std::tuple<CString, CString> Split( CString buf );
    HWND GetHWND()const;
    BOOL GetItemText( INT nItem, int nSub, CString& pszText )const;
    void SetDialogSize( CRect rect );
    CRect GetImagePreViewSize()const;
    INT xGetImageSize()const;
    INT yGetImageSize()const;
    void Redraw(CRect rect);
    void Sort( LPARAM func );
private:
    CImageList hSmall;
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
    void SetListViewSize();
    void SetImagePreViewSize();
    void SetColumnSizes();
    void view_List( CString buf, int i, CString path );
};



