// Copyright 2018 S520
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and / or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef BASE_H_
#define BASE_H_
#include <stdexcept>
#include <boost/container/vector.hpp>
#include <boost/array.hpp>

/// <summary>
/// テンプレートを記述する基底クラス
/// </summary>
class Base {
 protected:
    /// <summary>
    /// vectorコンテナへの範囲外アクセス時にデフォルト値を返す関数
    /// </summary>
    /// <remarks>vectorコンテナが空の場合は0を、インデックスが負の場合は0番目の要素を、インデックスが要素数以上の場合は最後の要素を返す</remarks>
    /// <param name="v">vectorオブジェクト</param>
    /// <param name="index">インデックス</param>
    /// <param name="file">(Debug用)呼び出し元ファイル</param>
    /// <param name="func">(Debug用)呼び出し元関数</param>
    /// <param name="line">(Debug用)呼び出し元行数</param>
    /// <returns>vectorコンテナの要素またはデフォルト値</returns>
    template<typename T>T VectorGetOrDefault(const boost::container::vector<T> &v, int index, char* file, char* func, int line) {
        char debugstr[1000];
        if (!v.empty()) {
            if (index < 0) {
                sprintf_s(debugstr, sizeof(debugstr), "Out of Renge!!! (Vector index: %d < 0) at File: %s / Func: %s / Line: %d\n", index, file, func, line);
                OutputDebugString(debugstr);
                return v[0];
            } else if (index > static_cast<int>(v.size() - 1)) {
                sprintf_s(debugstr, sizeof(debugstr), "Out of Renge!!! (Vector index: %d > %d) at File: %s / Func: %s / Line: %d\n", index, static_cast<int>(v.size() - 1), file, func, line);
                OutputDebugString(debugstr);
                return v.back();
            } else {
                return v[index];
            }
        } else {
            sprintf_s(debugstr, sizeof(debugstr), "Out of Renge!!! (Tihs Vector is Empty) at File: %s / Func: %s / Line: %d\n", file, func, line);
            OutputDebugString(debugstr);
            return static_cast<T>(0.0);
        }
    }

    /// <summary>
    /// arrayコンテナへの範囲外アクセス時にデフォルト値を返す関数
    /// </summary>
    /// <remarks>arrayコンテナが空の場合は0を、インデックスが負の場合は0番目の要素を、インデックスが要素数以上の場合は最後の要素を返す</remarks>
    /// <param name="a">arrayオブジェクト</param>
    /// <param name="index">インデックス</param>
    /// <param name="file">(Debug用)呼び出し元ファイル</param>
    /// <param name="func">(Debug用)呼び出し元関数</param>
    /// <param name="line">(Debug用)呼び出し元行数</param>
    /// <returns>vectorコンテナの要素またはデフォルト値</returns>
    template<typename T, std::size_t N>T ArrayGetOrDefault(const boost::array<T, N>&a, int index, char* file, char* func, int line) {
        char debugstr[1000];
        if (!a.empty()) {
            if (index < 0) {
                sprintf_s(debugstr, sizeof(debugstr), "Out of Renge!!! (Array index: %d < 0) at File: %s / Func: %s / Line: %d\n", index, file, func, line);
                OutputDebugString(debugstr);
                return a[0];
            } else if (index > static_cast<int>(a.size() - 1)) {
                sprintf_s(debugstr, sizeof(debugstr), "Out of Renge!!! (Array index: %d > %d) at File: %s / Func: %s / Line: %d\n", index, static_cast<int>(a.size() - 1), file, func, line);
                OutputDebugString(debugstr);
                return a.back();
            } else {
                return a[index];
            }
        } else {
            sprintf_s(debugstr, sizeof(debugstr), "Out of Renge!!! (Tihs Array is Empty) at File: %s / Func: %s / Line: %d\n", file, func, line);
            OutputDebugString(debugstr);
            return static_cast<T>(0.0);
        }
    }

    template<typename T>
    T VectorTryGet(const boost::container::vector<T> &v, int index, char* file, char* func, int line) {
        try {
            return v.at(index);
        }
        catch (std::out_of_range) {
            char debugstr[1000];
            if (!v.empty()) {
                if (index < 0) {
                    sprintf_s(debugstr, sizeof(debugstr), "Out of Renge!!! (Vector index: %d < 0) at File: %s / Func: %s / Line: %d\n", index, file, func, line);
                    OutputDebugString(debugstr);
                } else if (index > static_cast<int>(v.size() - 1)) {
                    sprintf_s(debugstr, sizeof(debugstr), "Out of Renge!!! (Vector index: %d > %d) at File: %s / Func: %s / Line: %d\n", index, static_cast<int>(v.size() - 1), file, func, line);
                    OutputDebugString(debugstr);
                }
            } else {
                sprintf_s(debugstr, sizeof(debugstr), "Out of Renge!!! (Tihs Vector is Empty) at File: %s / Func: %s / Line: %d\n", file, func, line);
                OutputDebugString(debugstr);
            }
            throw;
        }
    }

    template<typename T, std::size_t N>
    T ArrayTryGet(const boost::array<T, N> &a, int index, char* file, char* func, int line) {
        try {
            return a.at(index);
        }
        catch (std::out_of_range) {
            char debugstr[1000];
            if (!a.empty()) {
                if (index < 0) {
                    sprintf_s(debugstr, sizeof(debugstr), "Out of Renge!!! (Array index: %d < 0) at File: %s / Func: %s / Line: %d\n", index, file, func, line);
                    OutputDebugString(debugstr);
                } else if (index > static_cast<int>(a.size() - 1)) {
                    sprintf_s(debugstr, sizeof(debugstr), "Out of Renge!!! (Array index: %d > %d) at File: %s / Func: %s / Line: %d\n", index, static_cast<int>(a.size() - 1), file, func, line);
                    OutputDebugString(debugstr);
                }
            } else {
                sprintf_s(debugstr, sizeof(debugstr), "Out of Renge!!! (Tihs Array is Empty) at File: %s / Func: %s / Line: %d\n", file, func, line);
                OutputDebugString(debugstr);
            }
            throw;
        }
    }
};

#endif  // BASE_H_
