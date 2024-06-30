# libul

[English](README.md)	[简体中文](README_zh_CN.md)

一些实用的纯头文件，无需配置，开箱即用。

在每个文件夹下，包含以下文件：

| 文件  | 介绍                                                         |
| ----- | ------------------------------------------------------------ |
| *.h   | C头文件，大部分可在C89/C++98下使用，部分功能依赖于平台支持（这些将通过宏进行自动判断） |
| *.hpp | C++头文件，提供对C头文件的简单包装，大多数需要C++11或者更高的版本 |
| *.c   | C有关例子                                                    |
| *.cpp | C++有关例子                                                  |

## 简览

| 文件夹   | 介绍                                                         |
| -------- | ------------------------------------------------------------ |
| ulatomic | 原子操作                                                     |
| uldate   | 日期时间（类似于JS中的`Date`）                               |
| uldbuf   | 动态缓冲区                                                   |
| uldecode | 文本编码                                                     |
| uldl     | 动态链接库                                                   |
| ulendian | 字节序                                                       |
| ulfd     | 文件描述符                                                   |
| ullist   | 双向链表                                                     |
| ulmtx    | 互斥锁                                                       |
| ulrand   | 随机数生成器（使用[PCG随机数生成器](https://www.pcg-random.org/)） |
| ulrb     | 红黑树（快速但受限的版本）                                   |
| ulsarr   | 只读共享数组（加速切片、拼接等操作）                         |
| ulstdint | <stdint.h>的兼容头文件（用于部分老编译器）                   |
| ulutf    | UTF相关操作                                                  |

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
