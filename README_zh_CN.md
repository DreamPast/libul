# libul

[English](README.md)	[简体中文](README_zh_CN.md)

一些实用的纯头文件，无需配置，开箱即用。

大部分头文件都可以在C89/C++98下进行使用，部分功能需要一些平台支持，这些将通过宏进行判断。

| 头文件     | 介绍                                       |
| ---------- | ------------------------------------------ |
| ulatomic.h | 原子操作                                   |
| uldate.h   | 日期时间（类似于JS中的`Date`）             |
| uldecode.h | 文本编码                                   |
| uldl.h     | 动态链接库                                 |
| ulendian.h | 字节序                                     |
| ullist.h   | 双向链表                                   |
| ulmtx.h    | 互斥锁                                     |
| ulrb.h     | 红黑树（快速但受限的版本）                 |
| ulsarr.h   | 只读共享数组（加速切片、拼接等操作）       |
| ulstdint.h | <stdint.h>的兼容头文件（用于部分老编译器） |
| ulutf.h    | UTF相关操作                                |

## 协议

> The MIT License (MIT)
> 
> Copyright (C) 2023-2024 Jin Cai
> 
> Permission is hereby granted, free of charge, to any person obtaining a copy
> of this software and associated documentation files (the "Software"), to deal
> in the Software without restriction, including without limitation the rights
> to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
> copies of the Software, and to permit persons to whom the Software is
> furnished to do so, subject to the following conditions:
> 
> The above copyright notice and this permission notice shall be included in all
> copies or substantial portions of the Software.
> 
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
> FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
> AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
> LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
> OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
> SOFTWARE.