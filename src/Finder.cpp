#include "Finder.h"
void Finder::Create(HWND m_hWnd)
{
	CRect myRect{ 0,0,600,400 };
	myListView.Create(m_hWnd, myRect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		LVS_REPORT | LVS_AUTOARRANGE | LVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS);
	myListView.InsertColumn(0, TEXT("Название"), LVCFMT_LEFT, 290);
	myListView.InsertColumn(1, TEXT(".*"), LVCFMT_LEFT, 50);
	myListView.InsertColumn(2, TEXT("Полный путь"), LVCFMT_LEFT, 290);

}
void Finder::FindFile(CString szPath)
{
	HIMAGELIST hSmall{};
	CFindFile F;
	CString S = szPath +TEXT("\\*.*");
	int i = 0;
	BOOL bFlag = F.FindFile(S);
	if (!bFlag)
	{
		MessageBox(TEXT("Error"), TEXT("File not found"), 0);
	}
	else
	{
		do
		{
			if (F.IsDots() == TRUE)
			{
				continue;
			}
			else
			{
				if (F.IsDirectory() == TRUE)
				{
					View_List(F.GetFileName(), i, F.GetFilePath());
					i++;
					FindFile(F.GetFilePath());
					
				}
				else
				{
					View_List(F.GetFileName(), i, F.GetFilePath());
					i++;
				}
			}
		} while (F.FindNextFileW());
		F.Close();
		InitListViewImage(myListView.GetItemCount(), S);
	}
}

void Finder::View_List(CString name, int i, CString path)
{
	CString nameWithoutEx=std::get<0>(Split(name));
	CString extention= std::get<1>(Split(name));
	lvItem.mask = LVIF_IMAGE | LVIF_TEXT;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iItem = i;
	lvItem.iImage = i;
	lvItem.iSubItem = 0;
	lvItem.pszText = (LPWSTR)nameWithoutEx.GetString();
	lvItem.cchTextMax = nameWithoutEx.GetLength();
	myListView.InsertItem(&lvItem);
	myListView.SetItemText(i, 1, extention.GetString());
	myListView.SetItemText(i, 2, path.GetString());
}

std::tuple<CString, CString> Finder::Split(CString buf)
{
	CString tmp = buf;
	if (buf.Find(TEXT(".")) == -1)//папка
	{
		return { buf.GetString(),TEXT("Папка") };
	}
	else
	{
		int index = buf.Find(TEXT("."))+1;
		if (buf.Find(TEXT("."),index) != -1)
		{
			while (buf.Find(TEXT("."), index) != -1)
			{
				index = buf.Find(TEXT("."), index)+1;
			}
		}
		buf.Delete(index-1, buf.GetLength());
		tmp.Delete(0, index-1);
		return{ buf.GetString(),tmp.GetString() };
	}
}


BOOL Finder::InitListViewImage(int size, CString path)
{
	CFindFile F;
	HIMAGELIST hSmall;
	SHFILEINFO lp{};
	hSmall = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_MASK | ILC_COLOR32, size, 1);
	bool hFind = F.FindFile(path);
	if (!hFind)
	{
		MessageBox(TEXT("Error"), TEXT("File not found"), MB_OK | MB_ICONWARNING);
	}
	else
	{
		do
		{
			if (F.IsDots() == TRUE)
			{
				continue;
			}
			DWORD num = GetFileAttributesW(F.GetFilePath());
			SHGetFileInfoW(F.GetFilePath(), num, &lp, sizeof(lp), SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES);
			ImageList_AddIcon(hSmall, lp.hIcon);
			DestroyIcon(lp.hIcon);
		} while (F.FindNextFileW());
		F.Close();
	}
	myListView.SetImageList(hSmall, 1);
	return TRUE;
}
