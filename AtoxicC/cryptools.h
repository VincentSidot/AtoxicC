#pragma once
#include <Windows.h>
#define print_to_stdout "CONOUT$"

using byte = unsigned char;

void XOR(void* const input, void* output, size_t size, void* const key, size_t key_len)
{
	DWORD oldProtec = 0,test = 0;
	VirtualProtect(output, size, PAGE_READWRITE, &oldProtec);
	for (int i = 0; i < size; i++)
	{
		*((byte*)output + i) = *((byte*)input + i) ^ *((byte*)key + i%key_len);
	}
	VirtualProtect(output, size, oldProtec,&test);
}

void Dump(void* const adrr, void* output, size_t size)
{
	DWORD oldProtecOut, oldProtecIn, temp;
	VirtualProtect(output, size, PAGE_READWRITE, &oldProtecOut);
	VirtualProtect(adrr, size, PAGE_READONLY, &oldProtecIn);
	CopyMemory(output, adrr, size);
	VirtualProtect(output, size, oldProtecOut, &temp);
	VirtualProtect(adrr, size, oldProtecIn, &temp);
}

void DispDump(void* const dump, size_t size,size_t byte_per_line)
{
	for (int i = 0; i < size; i++)
	{
		if (i%byte_per_line == 0 && i != 0)
		{
			printf("\n");
		}

		printf("0x%x ", *((byte*)dump+i));
	}
}

void DumpAndDisp(void* const addr, size_t size, size_t byte_per_line)
{
	void* buffer;
	buffer = malloc(size);
	char* test = (char*)buffer;
	Dump(addr, buffer, size);
	DispDump(buffer, size, byte_per_line);
	free(buffer);
}

void PrintDump(LPCSTR lpFileName, void* const dump, size_t size, size_t byte_per_line,LPCSTR title = NULL)
{
	HANDLE hFile = CreateFileA(lpFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "Error opening file : %s\n",lpFileName);
		return;
	}
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		SetFilePointer(hFile, 0, NULL, FILE_END);
		char* temp = "\n\n";
		WriteFile(hFile, temp, 2, NULL, NULL);
	}
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	char* date;
	size_t len = 255 + size;
	date = new char[len];

	if (title != NULL)
	{
		sprintf_s(date, len, "\nName : \"%s\"", title);
	}
	sprintf_s(date + strlen(date), len, "\nDate : \n%02d-%02d-%d  %02d:%02d:%02d", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);
	sprintf_s(date + strlen(date), len, "\n-- Dump --\n");
	for (int i = 0; i < size; i++)
	{
		if (i%byte_per_line == 0 && i != 0 && i!=size-1)
		{
			sprintf_s(date+strlen(date),len,"\n");
		}

		sprintf_s(date + strlen(date), len, "0x%x ", *((byte*)dump + i));
	}
	sprintf_s(date + strlen(date), len, "\n-- End --\n");
	WriteFile(hFile, date, strlen(date), NULL, NULL);
	
	CloseHandle(hFile);
	delete date;
}

void DumpAndPrint(LPCSTR lpFileName, void* const addr, size_t size, size_t byte_per_line, LPCSTR title = NULL)
{
	byte* buffer;
	buffer = new byte[size];
	Dump(addr, buffer, size);
	PrintDump(lpFileName, buffer, size, byte_per_line,title);
	delete buffer;
}

