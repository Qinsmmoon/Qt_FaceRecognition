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

#include "mat.h"

#if NCNN_PIXEL
#if __ANDROID_API__ >= 39

#include <string.h>

#include <jni.h>
#include <android/bitmap.h>

namespace ncnn {

static Mat get_continous_pixels(const unsigned char* data, int w, int h, int elempack, int stride)
{
    if (stride == w * elempack)
        return Mat(w, h, (void*)data, (size_t)elempack, elempack);

    Mat m(w, h, (size_t)elempack, elempack);

    unsigned char* ptr = m;
    for (int y=0; y<h; y++)
    {
        memcpy(ptr, data, w * elempack);

        ptr += w * elempack;
        data += stride;
    }

    return m;
}

static void set_continous_pixels(const Mat& m, unsigned char* data, int stride)
{
    int w = m.w;
    int h = m.h;
    int elempack = m.elempack;

    if (stride == w * elempack)
    {
        if (data != m.data)
            memcpy(data, m, w * h * elempack);
        return;
    }

    const unsigned char* ptr = m;
    for (int y=0; y<h; y++)
    {
        memcpy(data, ptr, w * elempack);

        ptr += w * elempack;
        data += stride;
    }
}

Mat Mat::from_android_bitmap(JNIEnv* env, jobject bitmap, int type_to, Allocator* allocator)
{
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env, bitmap, &info);

    int type_from;
    int elempack;

    if (info.format == ANDROID_BITMAP_FORMAT_A_8)
    {
        type_from = PIXEL_GRAY;
        elempack = 1;
    }
    else if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888)
    {
        type_from = PIXEL_RGBA;
        elempack = 4;
    }
    else
    {
        // unsuppored android bitmap format
        return Mat();
    }

    // let PIXEL_RGBA2XXX become PIXEL_XXX
    type_to = (type_to & PIXEL_CONVERT_MASK) ? (type_to >> PIXEL_CONVERT_SHIFT) : (type_to & PIXEL_FORMAT_MASK);

    void* data;
    AndroidBitmap_lockPixels(env, bitmap, &data);

    Mat continous_pixels = get_continous_pixels((const unsigned char*)data, info.width, info.height, elempack, info.stride);

    int type = type_to == type_from ? type_from : (type_from | (type_to << PIXEL_CONVERT_SHIFT));

    Mat m = Mat::from_pixels(continous_pixels, type, info.width, info.height, allocator);

    AndroidBitmap_unlockPixels(env, bitmap);

    return m;
}

Mat Mat::from_android_bitmap_resize(JNIEnv* env, jobject bitmap, int type_to, int target_width, int target_height, Allocator* allocator)
{
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env, bitmap, &info);

    int type_from;
    int elempack;

    if (info.format == ANDROID_BITMAP_FORMAT_A_8)
    {
        type_from = PIXEL_GRAY;
        elempack = 1;
    }
    else if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888)
    {
        type_from = PIXEL_RGBA;
        elempack = 4;
    }
    else
    {
        // unsuppored android bitmap format
        return Mat();
    }

    // let PIXEL_RGBA2XXX become PIXEL_XXX
    type_to = (type_to & PIXEL_CONVERT_MASK) ? (type_to >> PIXEL_CONVERT_SHIFT) : (type_to & PIXEL_FORMAT_MASK);

    void* data;
    AndroidBitmap_lockPixels(env, bitmap, &data);

    Mat continous_pixels = get_continous_pixels((const unsigned char*)data, info.width, info.height, elempack, info.stride);

    int type = type_to == type_from ? type_from : (type_from | (type_to << PIXEL_CONVERT_SHIFT));

    Mat m = Mat::from_pixels_resize(continous_pixels, type, info.width, info.height, target_width, target_height, allocator);

    AndroidBitmap_unlockPixels(env, bitmap);

    return m;
}

void Mat::to_android_bitmap(JNIEnv* env, jobject bitmap, int type_from) const
{
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env, bitmap, &info);

    int type_to;
    int elempack;

    if (info.format == ANDROID_BITMAP_FORMAT_A_8)
    {
        type_to = PIXEL_GRAY;
        elempack = 1;
    }
    else if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888)
    {
        type_to = PIXEL_RGBA;
        elempack = 4;
    }
    else
    {
        // unsuppored android bitmap format
        return;
    }

    // let PIXEL_XXX2RGBA become PIXEL_XXX
    type_from = (type_from & PIXEL_CONVERT_MASK) ? (type_from & PIXEL_FORMAT_MASK) : type_from;

    void* data;
    AndroidBitmap_lockPixels(env, bitmap, &data);

    Mat continous_pixels = get_continous_pixels((const unsigned char*)data, info.width, info.height, elempack, info.stride);

    int type = type_from == type_to ? type_to : (type_from | (type_to << PIXEL_CONVERT_SHIFT));

    to_pixels_resize(continous_pixels, type, info.width, info.height);

    set_continous_pixels(continous_pixels, (unsigned char*)data, info.stride);

    AndroidBitmap_unlockPixels(env, bitmap);
}

} // namespace ncnn

#endif // __ANDROID_API__ >= 9
#endif // NCNN_PIXEL
