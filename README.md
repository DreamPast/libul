# libul

[English](README.md)	[简体中文](README_zh_CN.md)

Some header-only utility files, no configuration required, ready to use.

Every folders may include following files:

| File type | Introduction                                                 |
| --------- | ------------------------------------------------------------ |
| *.h       | C header files. Most of which can be used under C89/C++98, some components depend on platform support (automatically determined by macros). |
| *.hpp     | C++ header files. These provide simple wrappers for C header file, most require C++11 or higher. |
| *.c       | Example in C.                                                |
| *.cpp     | Example in C++.                                              |

## Overview

| Folder   | Introduction                                                 |
| -------- | ------------------------------------------------------------ |
| ulatomic | Atomic operations                                            |
| uldate   | Date and time (like `Date` in Javascript)                    |
| uldbuf   | Dynamic buffer                                               |
| uldecode | Text encoding                                                |
| uldl     | Dynamic shared library                                       |
| ulendian | Endianness                                                   |
| ulfd     | File descriptor                                              |
| ullist   | Double linked list                                           |
| ulmtx    | Mutex                                                        |
| ulrand   | Random number generator (uses [PCG Random Number Generators](https://www.pcg-random.org/)) |
| ulrb     | Red-black tree (quick but restricted version)                |
| ulsarr   | Read-only shared array (speeding up slicing, concatenating, etc.) |
| ulstdint | Compatibility header file for <stdint.h> (for some older compilers) |
| ulutf    | UTF related operations                                       |

## License

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
