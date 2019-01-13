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
 
 crunch - command line texture packer
 ====================================
 
 usage:
    crunch [OUTPUT] [INPUT1,INPUT2,INPUT3...] [OPTIONS...]
 
 example:
    crunch bin/atlases/atlas assets/characters,assets/tiles -p -t -v -u -r
 
 options:
    -p  --premultiply       premultiplies the pixels of the bitmaps by their alpha channel
    -t  --trim              trims excess transparency off the bitmaps
    -v  --verbose           print to the debug console as the packer works
    -f  --force             ignore the hash, forcing the packer to repack
    -u  --unique            remove duplicate bitmaps from the atlas
    -r  --rotate            enabled rotating bitmaps 90 degrees clockwise when packing
    -s# --size#             max atlas size (# can be 4096, 2048, 1024, 512, 256, 128, or 64)
    -p# --pad#              padding between images (# can be from 0 to 16)
 
 binary format:
    [int16] num_textures (below block is repeated this many times)
        [string] name
        [int16] num_images (below block is repeated this many times)
            [string] img_name
            [int16] img_x
            [int16] img_y
            [int16] img_width
            [int16] img_height
            [int16] img_frame_x         (if --trim enabled)
            [int16] img_frame_y         (if --trim enabled)
            [int16] img_frame_width     (if --trim enabled)
            [int16] img_frame_height    (if --trim enabled)
            [byte] img_rotated          (if --rotate enabled)
 */

#include <iostream>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <physfs.h>
#include "bitmap.hpp"
#include "packer.hpp"
#include "binary.hpp"
#include "hash.hpp"
#include "str.hpp"

#include "console/console.h"
#include "files.h"

using namespace std;

static int optSize;
static int optPadding;
static bool optPremultiply;
static bool optTrim;
static bool optVerbose;
static bool optForce;
static bool optUnique;
static bool optRotate;
static vector<Bitmap*> bitmaps;
static vector<Packer*> packers;

static void SplitFileName(const string& path, string* dir, string* name, string* ext)
{
    size_t si = path.rfind('/') + 1;
    if (si == string::npos)
        si = 0;
    size_t di = path.rfind('.');
    if (dir != nullptr)
    {
        if (si > 0)
            *dir = path.substr(0, si);
        else
            *dir = "";
    }
    if (name != nullptr)
    {
        if (di != string::npos)
            *name = path.substr(si, di - si);
        else
            *name = path.substr(si);
    }
    if (ext != nullptr)
    {
        if (di != string::npos)
            *ext = path.substr(di);
        else
            *ext = "";
    }
}

static string GetFileName(const string& path)
{
    string name;
    SplitFileName(path, nullptr, &name, nullptr);
    return name;
}

static void LoadBitmap(const string& prefix, const string& path)
{
    if (optVerbose) {
        Com_Printf("\t %s\n", path.c_str());
    }
    
    bitmaps.push_back(new Bitmap(path, prefix + GetFileName(path), optPremultiply, optTrim));
}

static void LoadBitmaps(const string& root, const string& prefix)
{
    int err;
    PHYSFS_Stat stat;

    char **files = PHYSFS_enumerateFiles(root.c_str());
    char **i;
    for (i = files; *i != NULL; i++) {
        const string fullPath = root + "/" + *i;

        err = PHYSFS_stat(fullPath.c_str(), &stat);
        if (err == 0) {
            Com_Printf("can't stat file %s", fullPath.c_str());
            return;
        }


        int len = strlen(fullPath.c_str());

        string virtPath = prefix + "/" + fullPath;

        if (stat.filetype == PHYSFS_FILETYPE_DIRECTORY) {
            LoadBitmaps(virtPath, "");
        }
        else if (strncmp(fullPath.c_str() + len - 4, ".png", 4) == 0) {
            const char *realPath = PHYSFS_getRealDir(virtPath.c_str());

            if (realPath == nullptr) {
                Com_Printf("realpath returning nullptr for virtual path %s\n", virtPath.c_str());
                continue;
            }

            LoadBitmap("", string(realPath) + "/" + root + "/" + *i);
        } 
    }

    PHYSFS_freeList(files);
}

static void RemoveFile(string file)
{
    remove(file.data());
}

static int GetPackSize(const string& str)
{
    if (str == "4096")
        return 4096;
    if (str == "2048")
        return 2048;
    if (str == "1024")
        return 1024;
    if (str == "512")
        return 512;
    if (str == "256")
        return 256;
    if (str == "128")
        return 128;
    if (str == "64")
        return 64;
    Com_Printf("invalid size: %s", str.c_str());
    exit(EXIT_FAILURE);
    return 0;
}

static int GetPadding(const string& str)
{
    for (int i = 0; i <= 16; ++i)
        if (str == to_string(i))
            return i;
    Com_Printf("invalid padding value: %s\n", str.c_str());
    exit(EXIT_FAILURE);
    return 1;
}

int crunch_main(int argc, const char* argv[])
{
    packers.clear();
    bitmaps.clear();

    if (argc < 2)
    {
        Com_Printf("invalid input, expected: \"crunch [INPUT DIRECTORY] [OUTPUT PREFIX] [OPTIONS...]\"\n");
        return EXIT_FAILURE;
    }
    
    //Get the output directory and name
    string outputDir, name, scriptsDir;
    SplitFileName(argv[1], &outputDir, &name, nullptr);
    outputDir = outputDir;
    scriptsDir = "/scripts/sprites/";

    const char *oldWriteDir = PHYSFS_getWriteDir();

    PHYSFS_setWriteDir((fs_basepath->string + string("/") + string(fs_game->string)).c_str());
    PHYSFS_mkdir(outputDir.c_str());
    PHYSFS_mkdir(scriptsDir.c_str());

    PHYSFS_setWriteDir(oldWriteDir);

    auto err = PHYSFS_getLastErrorCode();

    outputDir = fs_basepath->string + string("/") + string(fs_game->string) + string("/") + outputDir;
    scriptsDir = fs_basepath->string + string("/") + string(fs_game->string) + string("/scripts/sprites/");

    //Get all the input files and directories
    vector<string> inputs;
    stringstream ss(argv[2]);
    while (ss.good())
    {
        string inputStr;
        getline(ss, inputStr, ',');
        inputs.push_back(inputStr);
    }
    
    //Get the options
    optSize = 4096;
    optPadding = 1;
    optPremultiply = false;
    optTrim = false;
    optVerbose = false;
    optForce = false;
    optUnique = false;
    if (argc < 3) {
        optPremultiply = optTrim = optUnique = true;
    }
    else {
        for (int i = 3; i < argc; ++i)
        {
            string arg = argv[i];
            if (arg == "-p" || arg == "--premultiply")
                optPremultiply = true;
            else if (arg == "-t" || arg == "--trim")
                optTrim = true;
            else if (arg == "-v" || arg == "--verbose")
                optVerbose = true;
            else if (arg == "-f" || arg == "--force")
                optForce = true;
            else if (arg == "-u" || arg == "--unique")
                optUnique = true;
            else if (arg == "-r" || arg == "--rotate")
                optRotate = true;
            else if (arg.find("--size") == 0)
                optSize = GetPackSize(arg.substr(6));
            else if (arg.find("-s") == 0)
                optSize = GetPackSize(arg.substr(2));
            else if (arg.find("--pad") == 0)
                optPadding = GetPadding(arg.substr(5));
            else if (arg.find("-p") == 0)
                optPadding = GetPadding(arg.substr(2));
            else
            {
                Com_Printf("unexpected argument: %s\n", arg.c_str());
                return EXIT_FAILURE;
            }
        }
    }

    
    //Hash the arguments and input directories
    size_t newHash = 0;
    for (int i = 1; i < argc; ++i)
        HashString(newHash, argv[i]);
    for (size_t i = 0; i < inputs.size(); ++i)
    {
        if (inputs[i].rfind('.') == string::npos)
            HashFiles(newHash, inputs[i]);
        else
            HashFile(newHash, inputs[i]);
    }
    
    //Load the old hash
    size_t oldHash;
    if (LoadHash(oldHash, outputDir + name + ".hash"))
    {
        if (!optForce && newHash == oldHash)
        {
            Com_Printf("atlas is unchanged: %s\n", name.c_str());
            return EXIT_SUCCESS;
        }
    }
    
    /*
    -p  --premultiply       premultiplies the pixels of the bitmaps by their alpha channel
    -t  --trim              trims excess transparency off the bitmaps
    -v  --verbose           print to the debug console as the packer works
    -f  --force             ignore the hash, forcing the packer to repack
    -u  --unique            remove duplicate bitmaps from the atlas
    -r  --rotate            enabled rotating bitmaps 90 degrees clockwise when packing
    -s# --size#             max atlas size (# can be 4096, 2048, 1024, 512, or 256)
    -p# --pad#              padding between images (# can be from 0 to 16)*/
    
    if (optVerbose)
    {
        Com_Printf("options...\n");
        Com_Printf("\t--premultiply: %s\n", optPremultiply ? "true" : "false");
        Com_Printf("\t--trim: %s\n", optTrim ? "true" : "false");
        Com_Printf("\t--verbose: %s\n", optVerbose ? "true" : "false");
        Com_Printf("\t--force: %s\n", optForce ? "true" : "false");
        Com_Printf("\t--unique: %s\n", optUnique ? "true" : "false");
        Com_Printf("\t--rotate: %s\n", optRotate ? "true" : "false");
        Com_Printf("\t--size: %i\n", optSize);
        Com_Printf("\t--pad: %i\n", optPadding);
    }
    
    //Remove old files

    RemoveFile(outputDir + name + ".hash");
    RemoveFile(outputDir + name + ".bin");
    RemoveFile(outputDir + name + ".xml");
    RemoveFile(outputDir + name + ".json");
    for (size_t i = 0; i < 16; ++i)
        RemoveFile(outputDir + name + to_string(i) + ".png");
    
    //Load the bitmaps from all the input files and directories
    if (optVerbose)
        Com_Printf("loading images...\n");
    for (size_t i = 0; i < inputs.size(); ++i)
    {
        if (inputs[i].rfind('.') != string::npos)
            LoadBitmap("", inputs[i]);
        else
            LoadBitmaps(inputs[i], "");
    }
    
    //Sort the bitmaps by area
    sort(bitmaps.begin(), bitmaps.end(), [](const Bitmap* a, const Bitmap* b) {
        return (a->width * a->height) < (b->width * b->height);
    });
    
    //Pack the bitmaps
    while (!bitmaps.empty())
    {
        if (optVerbose)
            Com_Printf("packing %i images...\n", bitmaps.size());
        auto packer = new Packer(optSize, optSize, optPadding);
        packer->Pack(bitmaps, optVerbose, optUnique, optRotate);
        packers.push_back(packer);
        if (optVerbose) {
            Com_Printf("finished packing: %s%s (%i x %i)\n", name.c_str(), to_string(packers.size() - 1).c_str(), packer->width, packer->height);
        }

        if (packer->bitmaps.empty())
        {
            Com_Printf("packing failed, could not fit bitmap: %s\n", (bitmaps.back())->name.c_str());
            return EXIT_FAILURE;
        }
    }
    
    //Save the atlas image
    for (size_t i = 0; i < packers.size(); ++i)
    {
        Com_Printf("writing png: %s%s%s.png\n", outputDir.c_str(), name.c_str(), to_string(i).c_str());
        packers[i]->SavePng(outputDir + name + to_string(i) + ".png");
    }
    
    //Save the atlas binary
    Com_Printf("writing bin: %s%s.bin\n", outputDir.c_str(), name.c_str());
        
    ofstream bin(outputDir + name + ".bin", ios::binary);
    WriteShort(bin, (int16_t)packers.size());
    for (size_t i = 0; i < packers.size(); ++i)
        packers[i]->SaveBin(name + to_string(i), bin, optTrim, optRotate);
    bin.close();

    //Save the atlas binary
    Com_Printf("writing wren: %s%s.wren\n", scriptsDir.c_str(), name.c_str());

    ofstream wren(scriptsDir + name + ".wren", ios::binary);
    for (size_t i = 0; i < packers.size(); ++i)
        packers[i]->SaveWren(name + (packers.size() == 1 ? "" : to_string(i)), wren);
    wren.close();

    //Save the new hash
    SaveHash(newHash, outputDir + name + ".hash");
    
    return EXIT_SUCCESS;
}
