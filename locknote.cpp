// Steganos LockNote - self-modifying encrypted notepad
// Copyright (C) 2006 Steganos GmbH
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include <process.h>

#include <atlfile.h>

#include "resource.h"

#include "locknoteView.h"
#include "aboutdlg.h"
#include "passworddlg.h"
#include "MainFrm.h"

#include "aesphm.cpp"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	TCHAR szModulePath[MAX_PATH] = {'\0'};
	::GetModuleFileName(Utils::GetModuleHandle(), szModulePath, MAX_PATH);

	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	if (__argc == 3)
	{
		char* lpszCommand = __argv[1];
		char* lpszPath = __argv[2];
		if (!_tcscmp(lpszCommand, _T("-writeback")))
		{
			while (!::CopyFile(szModulePath, lpszPath, FALSE))
			{
				// wait and retry, maybe the process is still in use
				Sleep(100);
			}

			_tspawnl(_P_NOWAIT, lpszPath, Utils::Quote(lpszPath).c_str(), _T("-erase"), Utils::Quote(szModulePath).c_str(), NULL);

			return 0;
		}
		else if (!_tcscmp(lpszCommand,_T("-erase")))
		{
			while (PathFileExists(lpszPath) && !::DeleteFile(lpszPath))
			{
				// wait and retry, maybe the process is still in use
				Sleep(100);
			}

			return 0;
		}
	}

	if (__argc > 1)
	{
		int nResult = Utils::MessageBox(NULL, STR(IDS_ASK_CONVERT_FILES), MB_YESNO | MB_ICONQUESTION);
		if (nResult == IDYES)
		{
			int nConverted = 0;
			std::string encryptPassword;
			std::list<std::string> decryptPasswords;
			for (int nIndex = 1; nIndex < __argc; nIndex++)
			{
				std::string filename = __argv[nIndex];
				UINT uLength = _tcslen(filename.c_str());
				std::string extension = &filename[uLength-4];
				std::string newfilename = filename;
				strlwr((char*)extension.c_str());
				if (extension == _T(".txt"))
				{
					_tcscpy(&newfilename[uLength-4], _T(".exe"));
					std::string text;
					std::string password;
					if (LoadTextFromFile(filename, text, password))
					{
						if (SaveTextToFile(newfilename, text, encryptPassword))
						{
							nConverted += 1;
						}
					}
				}
				/*
				// convert SLIM back to text
				else if (extension == _T(".exe"))
				{
					_tcscpy(&newfilename[uLength-4], _T(".txt"));
				}
				*/
				else
				{					
					std::string text;
					text.resize(32768);
					sprintf((char*)text.c_str(), STR(IDS_CANT_CONVERT).c_str(), filename.c_str());
					Utils::MessageBox(NULL, text, MB_OK | MB_ICONERROR);
				}
			}
			if (nConverted)
			{
				std::string text;
				text.resize(32768);
				sprintf((char*)text.c_str(), STR(IDS_CONVERT_DONE).c_str(), nConverted);
				Utils::MessageBox(NULL, text, MB_OK | MB_ICONINFORMATION);
			}
		}

		return 0;
	}

	TCHAR szFileMappingName[MAX_PATH];
	strcpy(szFileMappingName, szModulePath);
	TCHAR* szChar = szFileMappingName;
	while (*szChar)
	{
		if ((*szChar == '\\')||(*szChar == ':'))
		{
			*szChar = '_';
		}
		szChar++;
	};

	CAtlFileMapping<HWND> fmSingleInstanceHWND;
	BOOL bAlreadyExisted = FALSE;
	fmSingleInstanceHWND.MapSharedMem(sizeof(HWND), szFileMappingName, &bAlreadyExisted);
	if (bAlreadyExisted)
	{
		HWND hWndMain = *fmSingleInstanceHWND;
		::SetForegroundWindow(hWndMain);
		return 0;
	}

	std::string text;
	std::string data;
	std::string password;
	Utils::LoadResource(_T("CONTENT"), _T("PAYLOAD"), data);
	if (_tcslen(data.c_str()))
	{
		password = GetPasswordDlg();
		if (!_tcslen(password.c_str()))
			return -1;
		if (!Utils::DecryptString(data, password, text))
		{
			MessageBox(NULL, STR(IDS_INVALID_PASSWORD), MB_OK | MB_ICONERROR);
			return -1;
		}		
	}
	else
	{
		text = STR(IDS_WELCOME);
	}

	CMainFrame wndMain;

	wndMain.m_password = password;
	wndMain.m_text = text;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	*fmSingleInstanceHWND = wndMain.m_hWnd;

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();

	if (((wndMain.m_text != text)||(wndMain.m_password != password)) && (_tcslen(wndMain.m_password.c_str())))
	{
		password = wndMain.m_password;

		data = _T("");
		if (_tcslen(wndMain.m_text.c_str()))
		{
			Utils::EncryptString(wndMain.m_text, password, data);
		}
		else
		{
			MessageBox(NULL, STR(IDS_TEXT_IS_ENCRYPTED), MB_OK | MB_ICONINFORMATION);
		}

		TCHAR szModulePath[MAX_PATH] = {'\0'};
		TCHAR szTempPath[MAX_PATH] = {'\0'};
		TCHAR szFileName[MAX_PATH] = {'\0'};

		::GetModuleFileName(Utils::GetModuleHandle(), szModulePath, MAX_PATH);
		::GetTempPath(MAX_PATH, szTempPath);
		::GetTempFileName(szTempPath, _T("STG"), 0, szFileName);

		::CopyFile(szModulePath, szFileName, FALSE);
		Utils::UpdateResource(szFileName, _T("CONTENT"), _T("PAYLOAD"), data);

		_tspawnl(_P_NOWAIT, szFileName, Utils::Quote(szFileName).c_str(), _T("-writeback"), Utils::Quote(szModulePath).c_str(), NULL);
	}

	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
