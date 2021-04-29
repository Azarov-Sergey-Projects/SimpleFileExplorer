#pragma once
#include <atlbase.h>
#include <atlwin.h>
#include <atlapp.h>
#include <atlmisc.h>

#include <atlctrls.h>
#include <atlfile.h>
#include <atlstr.h>

#include <atomic>
#include <tuple>
#include "resource2.h"


int CALLBACK CompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );


class Finder 
{
public:
    void create( HWND m_hWnd );
    void findFile( CString szPath,int &i );
    std::tuple<CString, CString> split( CString buf );
    HWND getHWND()const;
    BOOL getItemText( INT nItem, int nSub, CString& pszText )const;
    void setDialogSize( CRect rect );
    CRect getImagePreViewSize()const;
    INT xGetImageSize()const;
    INT yGetImageSize()const;
    void redraw(CRect rect);
    void sort( LPNMHDR func );
    void setReverse();
    BOOL getReverse()const;
    void SetAtomic()
    {
   //     StopThread = !StopThread;
    }
    struct AdditionalTmp
    {
         BOOL bReverse;
         CListViewCtrl ListView;
         int columnInd;
    };
private:
   // std::atomic<BOOL> StopThread=FALSE;
    AdditionalTmp Tmp;
    CImageList hSmall;
    INT nameColumnSize;
    INT extentionColumnSize;
    INT pathColumnSize;
    LVITEM lvItem;
    CString path;
    CRect sizeDialogBox;
    CRect sizeListView;
    CRect sizeImagePreView;
    HWND ListView_hWnd;
    void setListViewSize();
    void setImagePreViewSize();
    void setColumnSizes();
    void view_List( CString buf, int i, CString path );
};



