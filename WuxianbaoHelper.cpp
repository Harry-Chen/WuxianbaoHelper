/*
* Copyright (C) 2015 Harry Chen
*
* This file is part of WuxianbaoHelper
*
* WuxianbaoHelper is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* WuxianbaoHelper is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with WuxianbaoHelper.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	//用于在批处理改变.und文件关联后通知系统刷新
	if (argc == 2 && !wcscmp(argv[1], _T("init"))){
		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
		wcout << _T("Done refreshing registry. Exiting...") << endl;
		return 0;
	}

	HWND hDos = GetForegroundWindow();
	ShowWindow(hDos, SW_HIDE); //隐藏窗口
	wcout.imbue(locale("")); //This is so evil.

	TCHAR *document = NULL, QuotedDocumentPath[MAX_PATH];
	BOOL hasParam = false; //是否传入了文档路径
	if (argc >= 2){
		//在命令行最后加'-d'打开调试模式
		int isDifferent = wcscmp(argv[1], _T("-d"));
		if (!isDifferent || argc == 3){
			ShowWindow(hDos, SW_NORMAL);
			wcout << "WuXianBao Helper" << endl << "Copyright Harry Chen 2015" << endl << endl;
			wcout << "Debug mode enabled. Console window not hidden." << endl << endl;
		}
		if (isDifferent || argc == 3){
			hasParam = true;
			document = argv[1];
			wcout << "Document Path: " << document << endl;
			wcscpy_s(QuotedDocumentPath, _T("\""));
			wcscat_s(QuotedDocumentPath, document);
			wcscat_s(QuotedDocumentPath, _T("\""));
		}
	}
	WIN32_FIND_DATA FindFileData;
	HANDLE hListFile;
	TCHAR CurrentPath[MAX_PATH];

	GetModuleFileName(NULL, CurrentPath, sizeof(CurrentPath));
	wcout << "Helper Path: " << CurrentPath << endl;

	//黑科技，截去exe的文件名和多余的'\'
	*wcsrchr(CurrentPath, '\\') = 0;
	wcout << "Working Path: " << CurrentPath << endl;

	TCHAR ExePath[MAX_PATH];
	wcscpy_s(ExePath, CurrentPath);
	wcscat_s(ExePath, _T("\\ViewDocument.exe"));
	wcout << "Viewer Path: " << ExePath << endl;
	wcout << endl;

	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = hDos;
	ShExecInfo.lpVerb = _T("open");
	ShExecInfo.lpFile = ExePath;
	ShExecInfo.lpParameters = hasParam ? QuotedDocumentPath : NULL; //防止阅读器动作诡异
	ShExecInfo.lpDirectory = CurrentPath;
	ShExecInfo.nShow = SW_SHOWNORMAL;
	ShExecInfo.hInstApp = NULL;
	BOOL ret = ShellExecuteEx(&ShExecInfo);
	if (!ret) {
		wcout << "Error executing the viewer. Exit." << endl; //Will this happen?
		return 255;
	}
	wcout << "Waiting for the viewer to quit..." << endl;
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

	TCHAR FilePath[MAX_PATH];
	wcscpy_s(FilePath, CurrentPath);
	wcscat_s(FilePath, _T("\\*.log"));
	wcout << "Logs to delete: " << FilePath << endl;
	hListFile = FindFirstFile(FilePath, &FindFileData);
	if (hListFile == INVALID_HANDLE_VALUE)
	{
		wcout << GetLastError() << endl;
		return 1;
	}
	//循环查找并删除log
	do{
		TCHAR FullPath[MAX_PATH];
		wcscpy_s(FullPath, CurrentPath);
		wcscat_s(FullPath, _T("\\"));
		wcscat_s(FullPath, FindFileData.cFileName);
		wcout << "Found " << FindFileData.cFileName;
		if (DeleteFile(FullPath)){
			wcout << " ...Deleted.";
		}
		else{
			wcout << " ...Delete Failed.";
		}
		wcout << endl;
	} while (FindNextFile(hListFile, &FindFileData));

	return 0;
}

