#pragma once
#include "..\AtoxicC\cryptools.h"
#include "..\AtoxicC\atoxicc.h"
#include "XORbruteforce.h"


namespace Cracker {

	inline void OpenFile(char const *path, FILE_DATA *rep)
	{
		WIN32_FIND_DATAA fd;
		HANDLE hFile;
		FindFirstFileA(path, &fd);
		strcpy_s(rep->cFileName, MAX_PATH, fd.cFileName);
		strcpy_s(rep->cFilePath, MAX_PATH, path);
		rep->nFileSize.HighPart = fd.nFileSizeHigh;
		rep->nFileSize.LowPart = fd.nFileSizeLow;
		rep->dwFileAttributes = fd.dwFileAttributes;
		rep->ftCreationTime = fd.ftCreationTime;
		rep->ftLastAccessTime = fd.ftLastAccessTime;
		rep->ftLastWriteTime = fd.ftLastWriteTime;
		return;
	}
	inline void ADecryptFile(FILE_DATA *fd)
	{
		GUID signature_crypted;
		pbyte key = new byte[sizeof(signature_crypted)];
		size_t keylen;
		HANDLE hFile = CreateFileA(fd->cFilePath, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, NULL, NULL);
		SetFilePointer(hFile, strlen(header), NULL, FILE_BEGIN);
		ReadFile(hFile, &signature_crypted, sizeof(signature_crypted), NULL, NULL);
		BruteForce(&signature_crypted, (void*)&signature, key, sizeof(signature_crypted));
		keylen = ReduceKey(&signature_crypted, (void*)&signature, key, sizeof(signature_crypted));
		pbyte data = new byte[fd->nFileSize.QuadPart - sizeof(signature) - strlen(header)];
		ReadFile(hFile, data, fd->nFileSize.QuadPart - sizeof(signature) - strlen(header), NULL, NULL);
		CloseHandle(hFile);
		XOR(data, data, sizeof(data), key, keylen);
		char newPath[MAX_PATH];
		strcpy_s(newPath, MAX_PATH, fd->cFilePath);
		newPath[searchstr(newPath, extention_crypted)] = '\0';
		hFile = CreateFileA(newPath, GENERIC_WRITE, NULL, NULL, CREATE_NEW, NULL, NULL);
		WriteFile(hFile, data, fd->nFileSize.QuadPart - sizeof(signature) - strlen(header), NULL, NULL);
		CloseHandle(hFile);
		DeleteFileA(fd->cFilePath);
		delete[] key;
		delete[] data;
		return;
	}
}