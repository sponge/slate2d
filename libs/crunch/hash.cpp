/*
 
 MIT License
 
 Copyright (c) 2017 Chevy Ray Johnston
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 */

#include "hash.hpp"
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <physfs.h>
#include "str.hpp"

#include "console.h"
#include "files.h"

template <class T>
void HashCombine(std::size_t& hash, const T& v)
{
    std::hash<T> hasher;
    hash ^= hasher(v) + 0x9e3779b9 + (hash<<6) + (hash>>2);
}
void HashCombine(std::size_t& hash, size_t v)
{
    hash ^= v + 0x9e3779b9 + (hash<<6) + (hash>>2);
}

void HashString(size_t& hash, const string& str)
{
    HashCombine(hash, str);
}

void HashFile(size_t& hash, const string& file)
{

	char *contents;
	int sz = FS_ReadFile(file.c_str(), (void**)&contents);

	if (sz == -1) {
		Con_Printf("failed to read file: %s", file.c_str());
		exit(EXIT_FAILURE);
	}

	string fileStr = string(contents, sz);
	HashCombine(hash, fileStr);

	free(contents);
}

void HashFiles(size_t& hash, const string& root)
{   
	char **list = FS_List(root.c_str());

	char **i;
	for (i = list; *i != NULL; i++) {
		PHYSFS_Stat stat;
		int ret = PHYSFS_stat((root + "/" +*i).c_str(), &stat);
		string fullPath = (root + "/" + *i).c_str();
		if (ret == 0) {
			Con_Printf("failed to stat file: %s", fullPath.c_str());
			exit(EXIT_FAILURE);
			continue;
		}
		if (stat.filetype == PHYSFS_FILETYPE_DIRECTORY) {
			HashFiles(hash, fullPath.c_str());
		}
		else if (stat.filetype == PHYSFS_FILETYPE_REGULAR) {
			HashFile(hash, fullPath.c_str());
		}
	}

	FS_FreeList(list);
}

void HashData(size_t& hash, const char* data, size_t size)
{
    string str(data, size);
    HashCombine(hash, str);
}

bool LoadHash(size_t& hash, const string& file)
{
    ifstream stream(file);
    if (stream)
    {
        stringstream ss;
        ss << stream.rdbuf();
        ss >> hash;
        return true;
    }
    return false;
}

void SaveHash(size_t hash, const string& file)
{
    ofstream stream(file);
    stream << hash;
}
