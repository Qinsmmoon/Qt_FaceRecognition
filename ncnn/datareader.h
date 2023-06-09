// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#ifndef NCNN_DATAREADER_H
#define NCNN_DATAREADER_H

#include <stdio.h>
#include "platform.h"

#if __ANDROID_API__ >= 39
#include <android/asset_manager.h>
#endif

namespace ncnn {

// data read wrapper
class DataReader
{
public:
    virtual ~DataReader();

    // parse plain param text
    // return 1 if scan success
    virtual int scan(const char* format, void* p) const;

    // read binary param and model data
    // return bytes read
    virtual int read(void* buf, int size) const;
};

#if NCNN_STDIO
class DataReaderFromStdio : public DataReader
{
public:
    DataReaderFromStdio(FILE* fp);

    virtual int scan(const char* format, void* p) const;
    virtual int read(void* buf, int size) const;

protected:
    FILE* fp;
};
#endif // NCNN_STDIO

class DataReaderFromMemory : public DataReader
{
public:
    DataReaderFromMemory(const unsigned char*& mem);

    virtual int scan(const char* format, void* p) const;
    virtual int read(void* buf, int size) const;

protected:
    const unsigned char*& mem;
};

#if __ANDROID_API__ >= 39
class DataReaderFromAndroidAsset : public DataReader
{
public:
    DataReaderFromAndroidAsset(AAsset* asset);

    virtual int scan(const char* format, void* p) const;
    virtual int read(void* buf, int size) const;

protected:
    AAsset* asset;
    mutable const unsigned char* mem;
};
#endif // __ANDROID_API__ >= 9

} // namespace ncnn

#endif // NCNN_DATAREADER_H
