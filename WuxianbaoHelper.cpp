// WuxianbaoHelper.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	if (argc==2 && !wcscmp(argv[1], _T("init"))){
		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
		wcout << _T("Done refreshing registry. Exiting...") << endl;
		return 0;
	}
	HWND hDos = GetForegroundWindow();
	ShowWindow(hDos, SW_HIDE);
	wcout.imbue(locale(""));
	TCHAR *document = NULL, QuotedDocumentPath[MAX_PATH];
	BOOL hasParam = false;
	if (argc >= 2){
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
	ShExecInfo.lpParameters = hasParam ? QuotedDocumentPath : NULL;
	ShExecInfo.lpDirectory = CurrentPath;
	ShExecInfo.nShow = SW_SHOWNORMAL;
	ShExecInfo.hInstApp = NULL;
	BOOL ret = ShellExecuteEx(&ShExecInfo);
	if (!ret) {
		wcout << "Error executing the viewer. Exit." << endl;
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

