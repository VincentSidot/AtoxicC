#pragma once
#include <Windows.h>
#include <cstring>
#include "list.h"
#include "cryptools.h"

#define MAX_PATH 1024

typedef struct
{
	char cFilePath[MAX_PATH];
	char cFileName[MAX_PATH];
	DWORD dwFileAttributes;
	FILETIME ftCreationTime;
	FILETIME ftLastAccessTime;
	FILETIME ftLastWriteTime;
	LARGE_INTEGER nFileSize;
}FILE_DATA,*PFILE_DATA;

void ConvertToFileData(PWIN32_FIND_DATAA const in,LPCSTR const path, PFILE_DATA out)
{
	strcpy_s(out->cFileName, MAX_PATH, in->cFileName);
	strcpy_s(out->cFilePath, MAX_PATH, path);
	strcpy_s(out->cFilePath+strlen(out->cFilePath)-1, MAX_PATH-strlen(out->cFilePath), in->cFileName);
	if (in->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		sprintf_s(out->cFilePath + strlen(out->cFilePath), MAX_PATH - strlen(out->cFilePath), "\\");
	out->nFileSize.HighPart = in->nFileSizeHigh;
	out->nFileSize.LowPart = in->nFileSizeLow;
	out->dwFileAttributes = in->dwFileAttributes;
	out->ftCreationTime = in->ftCreationTime;
	out->ftLastAccessTime = in->ftLastAccessTime;
	out->ftLastWriteTime = in->ftLastWriteTime;
}

int searchstr(const char* m_str1, const char* m_str2)
{
	size_t strlen1 = strlen(m_str1);
	size_t strlen2 = strlen(m_str2);
	char* str1 = new char[strlen1];
	char* str2 = new char[strlen2];
	for (int i = 0; i < strlen1; i++)
	{
		if (i < strlen2)
		{
			str2[i] = (m_str2[i] >= 'A' && m_str2[i] <= 'Z') ? m_str2[i] - 'A' + 'a' : m_str2[i];
		}
		str1[i] = (m_str1[i] >= 'A' && m_str1[i] <= 'Z') ? m_str1[i] - 'A' + 'a' : m_str1[i];
	}
	if (strlen1 > strlen2)
	{
		for (int i = strlen1-1; i >= 0; i--)
		{
			if (str1[i] == str2[strlen2-1])
			{
				bool test = true;
				for(int j = 1;j<strlen2;j++)
				{
					if (str1[i - j] != str2[strlen2 - 1 - j])
					{
						test = false;
						break;
					}
				}
				if (test)
				{
					return i-strlen2+1;
				}
			}
		}
		return -1;
	}
}
bool CheckExtention(const char* str1, const char* str2)
{
	return !((searchstr(str1, str2) + strlen(str2))<strlen(str1));
}
void ListFile(LPCSTR const path, list<FILE_DATA> *rep)
{
	if (path[strlen(path) - 1] != '*')
		return;
	HANDLE hFind;
	WIN32_FIND_DATAA FindFileData;
	FILE_DATA FileData;
	hFind = FindFirstFileA(path, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}
	do
	{
		if (strcmp(FindFileData.cFileName,".") && strcmp(FindFileData.cFileName, ".."))
		{
			ConvertToFileData(&FindFileData, path, &FileData);
			rep->add(FileData);
		}
	} while (FindNextFileA(hFind, &FindFileData) != 0);
	FindClose(hFind);
}
void RecursiveListFile(LPCSTR const path, list<FILE_DATA> *rep)
{
	if (path[strlen(path) - 1] != '*')
		return;
	HANDLE hFind;
	WIN32_FIND_DATAA FindFileData;
	FILE_DATA FileData;
	hFind = FindFirstFileA(path, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}
	do
	{
		if (strcmp(FindFileData.cFileName, ".") && strcmp(FindFileData.cFileName, ".."))
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				char newPath[MAX_PATH];

				strcpy_s(newPath, MAX_PATH, path);
				strcpy_s(newPath + strlen(newPath) - 1, MAX_PATH - strlen(newPath), FindFileData.cFileName);
				sprintf_s(newPath + strlen(newPath), MAX_PATH - strlen(newPath), "\\*");
				RecursiveListFile(newPath, rep);
			}
			else
			{
				ConvertToFileData(&FindFileData, path, &FileData);
				rep->add(FileData);
			}
		}
	} while (FindNextFileA(hFind, &FindFileData) != 0);
	FindClose(hFind);
}
void RecursiveListFileWithMask(LPCSTR const path, list<FILE_DATA> *rep, LPCSTR const *extentionList, size_t count)
{
	if (path[strlen(path) - 1] != '*')
		return;
	HANDLE hFind;
	WIN32_FIND_DATAA FindFileData;
	FILE_DATA FileData;
	hFind = FindFirstFileA(path, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}
	do
	{
		if (strcmp(FindFileData.cFileName, ".") && strcmp(FindFileData.cFileName, ".."))
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				char newPath[MAX_PATH];

				strcpy_s(newPath, MAX_PATH, path);
				strcpy_s(newPath + strlen(newPath) - 1, MAX_PATH - strlen(newPath), FindFileData.cFileName);
				sprintf_s(newPath + strlen(newPath), MAX_PATH - strlen(newPath), "\\*");
				RecursiveListFileWithMask(newPath, rep,extentionList,count);
			}
			else
			{
				for (int i = 0; i < count; i++)
				{
					if (CheckExtention(FindFileData.cFileName, extentionList[i]))
					{
						ConvertToFileData(&FindFileData, path, &FileData);
						rep->add(FileData);
						break;
					}
				}
			}
		}
	} while (FindNextFileA(hFind, &FindFileData) != 0);
	FindClose(hFind);
}

bool CanAccessFolder(LPCSTR folderName, DWORD genericAccessRights)
{
	bool bRet = false;
	DWORD length = 0;
	if (!::GetFileSecurityA(folderName, OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION
		| DACL_SECURITY_INFORMATION, NULL, NULL, &length) &&
		ERROR_INSUFFICIENT_BUFFER == ::GetLastError()) {
		PSECURITY_DESCRIPTOR security = static_cast< PSECURITY_DESCRIPTOR >(::malloc(length));
		if (security && ::GetFileSecurityA(folderName, OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION
			| DACL_SECURITY_INFORMATION, security, length, &length)) {
			HANDLE hToken = NULL;
			if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_IMPERSONATE | TOKEN_QUERY |
				TOKEN_DUPLICATE | STANDARD_RIGHTS_READ, &hToken)) {
				HANDLE hImpersonatedToken = NULL;
				if (::DuplicateToken(hToken, SecurityImpersonation, &hImpersonatedToken)) {
					GENERIC_MAPPING mapping = { 0xFFFFFFFF };
					PRIVILEGE_SET privileges = { 0 };
					DWORD grantedAccess = 0, privilegesLength = sizeof(privileges);
					BOOL result = FALSE;

					mapping.GenericRead = FILE_GENERIC_READ;
					mapping.GenericWrite = FILE_GENERIC_WRITE;
					mapping.GenericExecute = FILE_GENERIC_EXECUTE;
					mapping.GenericAll = FILE_ALL_ACCESS;

					::MapGenericMask(&genericAccessRights, &mapping);
					if (::AccessCheck(security, hImpersonatedToken, genericAccessRights,
						&mapping, &privileges, &privilegesLength, &grantedAccess, &result)) {
						bRet = (result == TRUE);
					}
					::CloseHandle(hImpersonatedToken);
				}
				::CloseHandle(hToken);
			}
			::free(security);
		}
	}

	return bRet;
}

