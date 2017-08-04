#include <iostream>
#include <Windows.h>
#include "cracker.h"

#define MAX_LEN 1024

const char *path;

void randomize(void*, size_t);

using namespace std;
int inline strToInt(char*);

int main(int argc, char** argv)
{
// ---------------------------------------------------------------------------
	if (argc == 1)
	{
		printf("Please use : %s [path1] [path2] [path3] [path4] ...\n", argv[0]);
		getchar();
		exit(EXIT_FAILURE);
	}

// ---------------------------------------------------------------------------
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][strlen(argv[i]) - 1] == '*')
		{
			list<FILE_DATA> fd;
			ListPotentialEncryptedFile(argv[i], &fd);
// ---------------------------------------------------------------------------
			GUID signature_crypted;
			pbyte key = new byte[sizeof(signature_crypted)];
			size_t keylen;
			HANDLE hFile = CreateFileA(fd.at(0).cFilePath, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, NULL, NULL);
			SetFilePointer(hFile, strlen(header), NULL, FILE_BEGIN);
			ReadFile(hFile, &signature_crypted, sizeof(signature_crypted), NULL, NULL);
			BruteForce(&signature_crypted, (void*)&signature, key, sizeof(signature_crypted));
			keylen = ReduceKey(&signature_crypted, (void*)&signature, key, sizeof(signature_crypted));
// ---------------------------------------------------------------------------
			ADecryptFileList(&fd, key, keylen);
		}
		else
		{
			FILE_DATA fd;
			Cracker::OpenFile(argv[i], &fd);
			if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				Cracker::ADecryptFile(&fd);
		}
	}
// ---------------------------------------------------------------------------
	return 0;
}

int inline pow10(size_t n)
{
	if (n == 1)
		return n;
	else if (n % 2 == 0)
		return pow10(n / 2)*pow10(n / 2);
	else
		return pow10(n / 2)*pow10(n / 2)*n;
}

int inline strToInt(char* number)
{
	int len = strlen(number);
	int rep = 0;
	for (int i = 0; i < len; i++)
	{
		rep += (number[i] - '0')*pow10(len - i);
	}
	return rep;
}

void randomize(void* data, size_t len)
{
	for (size_t i = 0; i < len; i++)
		*((pbyte)data + i) = rand();
}