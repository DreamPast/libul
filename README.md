# libul

[English](README.md)	[简体中文](README_zh_CN.md)

Some header-only utility files, no configuration required, ready to use.

Most of the header files can be used under C89/C++98, some features require some platform support, these will be determined through macros.

| Header Files | Introduction                                                 |
| ------------ | ------------------------------------------------------------ |
| ulatomic.h   | Atomic operations                                            |
| uldate.h     | Date and time (like `Date` in Javascript)                    |
| uldbuf.h     | Dynamic buffer                                               |
| uldecode.h   | Text encoding                                                |
| uldl.h       | Dynamic shared library                                       |
| ulendian.h   | Endianness                                                   |
| ulfd.h       | File descriptor                                              |
| ullist.h     | Double linked list                                           |
| ulmtx.h      | Mutex                                                        |
| ulrand.h     | Random number generator (uses [PCG Random Number Generators](https://www.pcg-random.org/)) |
| ulrb.h       | Red-black tree (quick but restricted version)                |
| ulsarr.h     | Read-only shared array (speeding up slicing, concatenating, etc.) |
| ulstdint.h   | Compatibility header file for <stdint.h> (for some older compilers) |
| ulutf.h      | UTF related operations                                       |

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
