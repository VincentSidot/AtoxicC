#pragma once
#include <Windows.h>
#include <iostream>

using byte = unsigned char;
using pbyte = unsigned char*;


void inline BruteForce(void* const crypted,void* const uncryted,void* key, size_t size)
{
	pbyte cd = (pbyte)crypted;
	pbyte ud = (pbyte)uncryted;
	ZeroMemory(key, size);
	bool *Table = new bool[size];
	memset(Table, false, size * sizeof(bool));
	for (int i = 0; i < size; i++)
	{
		if (!Table[i])
		{
			for (byte b = 0; b <= 255; b++)
			{
				if ((cd[i] ^ b) == ud[i])
				{
					*((pbyte)key + i) = b;
					Table[i] = true;
					for (int j = i; j < size; j++)
					{
						if (!Table[j] && ((cd[j] ^ b) == ud[j]))
						{
							*((pbyte)key + j) = b;
							Table[i] = true;
						}
					}
					break;
				}
			}
		}
	}
	delete[] Table;
}
size_t inline ReduceKey(void* const crypted,void* const decrypted,void* key, size_t size)
{
	size_t newLen = size;
	pbyte temp = new byte[size];
	for (int i = size; i > 0; i--)
	{
		XOR(crypted, temp, size, key, i);
		if (memcmp(temp, decrypted, size) == 0)
			newLen = i;
	}
	return newLen;
}