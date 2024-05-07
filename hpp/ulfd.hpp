/*
File descriptor (Unix style) (C++ wrapper)


# Dependence
  C++11


# License
  The MIT License (MIT)

  Copyright (C) 2023-2024 Jin Cai

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
#ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS 1 /* disable warnings of `strerror` */
#endif
#include "ulfd.h"
#include <stdexcept>
#include <string>
#include <utility>
#include <functional>

namespace ul {
    namespace fd {
        class Exception : public std::runtime_error {
        public:
            inline Exception(int err) : std::runtime_error(strerror(err)), error(err) { }
            inline Exception(const Exception& exp) = default;
            inline Exception(Exception&& exp) = default;
            inline Exception& operator=(const Exception& exp) = default;
            inline Exception& operator=(Exception&& exp) = default;
            inline int get_error_code() const{ return error; }

            int error;
        };

        typedef std::basic_string<ulfd_uchar_t> String;
        class NativeStringView {
        public:
            inline NativeStringView() : ustr(nullptr), should_free(false) { }
            inline NativeStringView(const NativeStringView& other) {
                should_free = other.should_free;
                if(other.should_free) ustr = ulfd_ucsdup(other.ustr);
                else ustr = other.ustr;
            }
            inline NativeStringView(NativeStringView&& other) {
                should_free = other.should_free; other.should_free = false;
                ustr = other.ustr; other.ustr = nullptr;
            }
            inline NativeStringView(const char* str) { _set(str); }
            inline NativeStringView(const std::string& str) { _set(str.c_str());}
            inline NativeStringView(const wchar_t* wstr) { _set(wstr); }
            inline NativeStringView(const std::wstring& wstr) { _set(wstr.c_str()); }

            inline ~NativeStringView() { _destroy(); }
            inline NativeStringView& operator=(const NativeStringView& other) {
                _destroy();
                should_free = other.should_free;
                if(other.should_free) ustr = ulfd_ucsdup(other.ustr);
                else ustr = other.ustr;
                return *this;
            }
            inline NativeStringView& operator=(NativeStringView&& other) {
                swap(other); return *this;
            }

            inline operator const ulfd_uchar_t*() const{ return ustr; }
            inline const ulfd_uchar_t* get() const{ return ustr; }

            inline void swap(NativeStringView& o) {
                std::swap(ustr, o.ustr);
                std::swap(should_free, o.should_free);
            }

        private:
            inline void _set(const char* str) {
            #ifdef _WIN32
                wchar_t* out;
                int err = ulfd_str_to_wstr_alloc(&out, str);
                if(err) throw Exception(err);
                ustr = out; should_free = true;
            #else
                ustr = str; should_free = false;
            #endif
            }
            inline void _set(const wchar_t* wstr) {
            #ifdef _WIN32
                ustr = wstr; should_free = false;
            #else
                char* out;
                int err = ulfd_wstr_to_str_alloc(&out, wstr);
                if(err) throw Exception(err);
                ustr = out; should_free = true;
            #endif
            }
            inline void _destroy() noexcept{
                if(should_free) {
                    should_free = false;
                    ul_free(const_cast<ulfd_uchar_t*>(ustr));
                    ustr = nullptr;
                }
            }

            const ulfd_uchar_t* ustr;
            bool should_free;
        };

        static inline void _throw_if_error(int err) {
            if(err) throw Exception(err);
        }

        static inline std::string _copy_string(const ulfd_uchar_t* str) {
        #ifdef _WIN32
            std::string ret;
            size_t len = ul_os_wstr_to_str_len(str);
            if(ul_unlikely(len == 0)) throw Exception(EILSEQ);
            ret.resize(len - 1);
            ul_os_wstr_to_str(&ret[0], str);
            return ret;
        #else
            return std::string(str);
        #endif
        }
        static inline std::wstring _copy_wstring(const ulfd_uchar_t* str) {
        #ifdef _WIN32
            return std::wstring(str);
        #else
            std::wstring ret;
            size_t len = ul_os_str_to_wstr_len(str);
            if(ul_unlikely(len == 0)) throw Exception(EILSEQ);
            ret.resize(len - 1);
            ul_os_str_to_wstr(&ret[0], str);
            return ret;
        #endif
        }
        static inline String _copy_ustring(const ulfd_uchar_t* str) {
            return String(str);
        }

        static inline std::string _pass_string(ulfd_uchar_t* str) {
            struct StringGuard {
                ~StringGuard() { ul_free(str); }
                ulfd_uchar_t* str;
            } guard = { str };
            return _copy_string(str);
        }
        static inline std::wstring _pass_wstring(ulfd_uchar_t* str) {
            struct StringGuard {
                ~StringGuard() { ul_free(str); }
                ulfd_uchar_t* str;
            } guard = { str };
            return _copy_wstring(str);
        }
        static inline String _pass_ustring(ulfd_uchar_t* str) {
            struct StringGuard {
                ~StringGuard() { ul_free(str); }
                ulfd_uchar_t* str;
            } guard = { str };
            return _copy_ustring(str);
        }

        class FileDescriptorGuard {
        public:
            inline explicit FileDescriptorGuard() : fd(ULFD_INVALID_VALUE) { }
            inline explicit FileDescriptorGuard(ulfd_t nfd) : fd(nfd) { }
            inline FileDescriptorGuard(const FileDescriptorGuard& other) = delete;
            inline FileDescriptorGuard(FileDescriptorGuard&& other) {
                fd = other.fd; other.fd = ULFD_INVALID_VALUE;
            }
            inline ~FileDescriptorGuard() {
                if(fd != ULFD_INVALID_VALUE) {
                    ulfd_close(fd); fd = ULFD_INVALID_VALUE;
                }
            }

            inline FileDescriptorGuard& operator=(const FileDescriptorGuard& other) = delete;
            inline FileDescriptorGuard& operator=(FileDescriptorGuard&& other) {
                close();
                fd = other.fd; other.fd = ULFD_INVALID_VALUE;
                return *this;
            }

            inline operator ulfd_t() const{ return fd; }
            inline ulfd_t get() const{ return fd; }
            inline ulfd_t release(ulfd_t nfd = ULFD_INVALID_VALUE) {
                ulfd_t ret = fd;
                fd = nfd;
                return ret;
            }
            inline void close() {
                if(fd == ULFD_INVALID_VALUE) return;
                int err = ulfd_close(fd); fd = ULFD_INVALID_VALUE;
                if(err) throw Exception(err);
            }
            inline void set(ulfd_t nfd) {
                close(); fd = nfd;
            }
            inline void swap(FileDescriptorGuard& other) noexcept{
                std::swap(fd, other.fd);
            }

            ulfd_t fd;
        };

        inline ulfd_t open(const NativeStringView& path, long oflag, ulfd_mode_t mode) {
            ulfd_t fd;
            _throw_if_error(ulfd_open_u(&fd, path, oflag, mode));
            return fd;
        }
        inline ulfd_t open(const NativeStringView& path, long oflag) {
            ulfd_t fd;
            if(oflag & ULFD_O_CREAT) throw Exception(EINVAL);
            _throw_if_error(ulfd_open_u(&fd, path, oflag, 0664));
            return fd;
        }
        inline ulfd_t creat(const NativeStringView& path, ulfd_mode_t mode) {
            ulfd_t fd;
            _throw_if_error(ulfd_creat_u(&fd, path, mode));
            return fd;
        }
        inline void close(ulfd_t fd) {
            _throw_if_error(ulfd_close(fd));
        }
        inline void pipe(ulfd_t& rfd, ulfd_t& wfd) {
            ulfd_t fds[2];
            _throw_if_error(ulfd_pipe(fds));
            rfd = fds[0]; wfd = fds[1];
        }
        inline void pipe(ulfd_t fds[2]) {
            _throw_if_error(ulfd_pipe(fds));
        }

        inline size_t read(ulfd_t fd, void* buf, size_t count) {
            size_t read_bytes;
            _throw_if_error(ulfd_read(fd, buf, count, &read_bytes));
            return read_bytes;
        }
        inline size_t write(ulfd_t fd, const void* buf, size_t count) {
            size_t write_bytes;
            _throw_if_error(ulfd_write(fd, buf, count, &write_bytes));
            return write_bytes;
        }
        inline size_t pread(ulfd_t fd, void* buf, size_t count, ulfd_int64_t off) {
            size_t read_bytes;
            _throw_if_error(ulfd_pread(fd, buf, count, off, &read_bytes));
            return read_bytes;
        }
        inline size_t pwrite(ulfd_t fd, const void* buf, size_t count, ulfd_int64_t off) {
            size_t write_bytes;
            _throw_if_error(ulfd_pwrite(fd, buf, count, off, &write_bytes));
            return write_bytes;
        }

        inline ulfd_int64_t seek(ulfd_t fd, ulfd_int64_t off, int origin = ULFD_SEEK_SET) {
            ulfd_int64_t r;
            _throw_if_error(ulfd_seek(fd, off, origin, &r));
            return r;
        }
        inline ulfd_int64_t tell(ulfd_t fd) {
            ulfd_int64_t r;
            _throw_if_error(ulfd_tell(fd, &r));
            return r;
        }

        inline size_t copy_file_range(ulfd_t fd_in, ulfd_int64_t* off_in, ulfd_t fd_out, ulfd_int64_t* off_out, size_t len) {
            size_t r;
            _throw_if_error(ulfd_copy_file_range(fd_in, off_in, fd_out, off_out, len, &r));
            return r;
        }
        inline size_t copy_file_range_user(
            ulfd_t fd_in, ulfd_int64_t* off_in, ulfd_t fd_out, ulfd_int64_t* off_out,
            size_t len, void* buf = nullptr, size_t buf_len = 0
        ) {
            size_t r;
            _throw_if_error(ulfd_copy_file_range_user(fd_in, off_in, fd_out, off_out, len, &r, buf, buf_len));
            return r;
        }

        inline void ffullsync(ulfd_t fd) { _throw_if_error(ulfd_ffullsync(fd)); }
        inline void fsync(ulfd_t fd) { _throw_if_error(ulfd_fsync(fd)); }
        inline void fdatasync(ulfd_t fd) { _throw_if_error(ulfd_fdatasync(fd)); }

        inline void lock(ulfd_t fd, ulfd_int64_t off, ulfd_int64_t len, int mode) {
            _throw_if_error(ulfd_lock(fd, off, len, mode));
        }
        inline void lockw(ulfd_t fd, ulfd_int64_t off, ulfd_int64_t len, int mode) {
            _throw_if_error(ulfd_lockw(fd, off, len, mode));
        }

        inline void ftruncate(ulfd_t fd, ulfd_int64_t length) {
            _throw_if_error(ulfd_ftruncate(fd, length));
        }
        inline ulfd_int64_t ffilelength(ulfd_t fd) {
            ulfd_int64_t r;
            _throw_if_error(ulfd_ffilelength(fd, &r));
            return r;
        }
        inline void fchmod(ulfd_t fd, ulfd_mode_t mode) {
            _throw_if_error(ulfd_fchmod(fd, mode));
        }
        inline void fchown(ulfd_t fd, ulfd_uid_t uid = static_cast<ulfd_uid_t>(-1), ulfd_gid_t gid = static_cast<ulfd_gid_t>(-1)) {
            _throw_if_error(ulfd_fchown(fd, uid, gid));
        }
        inline void futime(ulfd_t fd, ulfd_int64_t atime, ulfd_int64_t mtime) {
            _throw_if_error(ulfd_futime(fd, atime, mtime));
        }

        inline bool isatty(ulfd_t fd) {
            int r;
            _throw_if_error(ulfd_isatty(fd, &r));
            return r != 0;
        }

        inline ulfd_t dup(ulfd_t fd) {
            ulfd_t r;
            _throw_if_error(ulfd_dup(&r, fd));
            return r;
        }
        inline FILE* fdopen(ulfd_t fd, const NativeStringView& mode) {
            FILE* fp;
            _throw_if_error(ulfd_fdopen_u(&fp, fd, mode));
            return fp;
        }
        inline ulfd_t fileno(FILE* fp) {
            ulfd_t fd;
            _throw_if_error(ulfd_fileno(&fd, fp));
            return fd;
        }

        inline void* mmap(ulfd_t fd, void* addr, size_t len, ulfd_int64_t off, int prot_and_flags) {
            void* ret;
            _throw_if_error(ulfd_mmap(&ret, fd, addr, len, off, prot_and_flags));
            return ret;
        }
        inline void munmap(void* addr, size_t len) {
            _throw_if_error(ulfd_munmap(addr, len));
        }
        inline void mprotect(void* addr, size_t len, int prot) {
            _throw_if_error(ulfd_mprotect(addr, len, prot));
        }
        inline void msync(void* addr, size_t len, int flags) {
            _throw_if_error(ulfd_msync(addr, len, flags));
        }
        inline void mlock(const void* addr, size_t len) {
            _throw_if_error(ulfd_mlock(addr, len));
        }
        inline void munlock(const void* addr, size_t len) {
            _throw_if_error(ulfd_munlock(addr, len));
        }
        inline void madvise(void* addr, size_t len, int advise) {
            _throw_if_error(ulfd_madvise(addr, len, advise));
        }

        inline void chdir(const NativeStringView& path) {
            _throw_if_error(ulfd_chdir_u(path));
        }
        inline std::string getcwd() {
            ulfd_uchar_t* up;
            _throw_if_error(ulfd_getcwd_alloc_u(&up));
            return _pass_string(up);
        }
        inline std::wstring getcwd_w() {
            ulfd_uchar_t* up;
            _throw_if_error(ulfd_getcwd_alloc_u(&up));
            return _pass_wstring(up);
        }
        inline String getcwd_u() {
            ulfd_uchar_t* up;
            _throw_if_error(ulfd_getcwd_alloc_u(&up));
            return _pass_ustring(up);
        }

        inline ulfd_mode_t umask(ulfd_mode_t mask) { return ulfd_umask(mask); }
        inline void chmod(const NativeStringView& path, ulfd_mode_t mode) {
            _throw_if_error(ulfd_chmod_u(path, mode));
        }
        inline void lchmod(const NativeStringView& path, ulfd_mode_t mode) {
            _throw_if_error(ulfd_lchmod_u(path, mode));
        }
        inline void chown(const NativeStringView& path, ulfd_uid_t uid = static_cast<ulfd_uid_t>(-1), ulfd_gid_t gid = static_cast<ulfd_gid_t>(-1)) {
            _throw_if_error(ulfd_chown_u(path, uid, gid));
        }
        inline void lchown(const NativeStringView& path, ulfd_uid_t uid = static_cast<ulfd_uid_t>(-1), ulfd_gid_t gid = static_cast<ulfd_gid_t>(-1)) {
            _throw_if_error(ulfd_lchown_u(path, uid, gid));
        }
        inline void utime(const NativeStringView& path, ulfd_int64_t atime, ulfd_int64_t mtime) {
            _throw_if_error(ulfd_utime_u(path, atime, mtime));
        }
        inline void lutime(const NativeStringView& path, ulfd_int64_t atime, ulfd_int64_t mtime) {
            _throw_if_error(ulfd_lutime_u(path, atime, mtime));
        }

        inline ulfd_stat_t& stat(const NativeStringView& path, ulfd_stat_t& state) {
            _throw_if_error(ulfd_stat_u(path, &state));
            return state;
        }
        inline ulfd_stat_t stat(const NativeStringView& path) {
            ulfd_stat_t state;
            _throw_if_error(ulfd_stat_u(path, &state));
            return state;
        }
        inline ulfd_stat_t& lstat(const NativeStringView& path, ulfd_stat_t& state) {
            _throw_if_error(ulfd_lstat_u(path, &state));
            return state;
        }
        inline ulfd_stat_t lstat(const NativeStringView& path) {
            ulfd_stat_t state;
            _throw_if_error(ulfd_lstat_u(path, &state));
            return state;
        }
        inline ulfd_stat_t& fstat(ulfd_t fd, ulfd_stat_t& state) {
            _throw_if_error(ulfd_fstat(fd, &state));
            return state;
        }
        inline ulfd_stat_t fstat(ulfd_t fd) {
            ulfd_stat_t state;
            _throw_if_error(ulfd_fstat(fd, &state));
            return state;
        }

        inline ulfd_mode_t type(const NativeStringView& path) {
            ulfd_mode_t mode;
            _throw_if_error(ulfd_type_u(path, &mode));
            return mode;
        }
        inline ulfd_mode_t ltype(const NativeStringView& path) {
            ulfd_mode_t mode;
            _throw_if_error(ulfd_ltype_u(path, &mode));
            return mode;
        }
        inline ulfd_mode_t ftype(ulfd_t fd) {
            ulfd_mode_t mode;
            _throw_if_error(ulfd_ftype(fd, &mode));
            return mode;
        }

        inline bool access(const NativeStringView& path, ulfd_mode_t mode) {
            int err = ulfd_access_u(path, mode);
            if(err == EACCES) return false;
            else if(err == 0) return true;
            else throw Exception(err);
        }
        inline bool empty(const NativeStringView& path) {
            int r;
            _throw_if_error(ulfd_empty_u(path, &r));
            return r != 0;
        }

        inline void mkdir(const NativeStringView& path, ulfd_mode_t mode) {
            _throw_if_error(ulfd_mkdir_u(path, mode));
        }
        inline void rename(const NativeStringView& newpath, const NativeStringView& oldpath) {
            _throw_if_error(ulfd_rename_u(newpath, oldpath));
        }
        inline void unlink(const NativeStringView& path) {
            _throw_if_error(ulfd_unlink_u(path));
        }
        inline void remove(const NativeStringView& path) {
            _throw_if_error(ulfd_remove_u(path));
        }
        inline void rmdir(const NativeStringView& path) {
            _throw_if_error(ulfd_rmdir_u(path));
        }

        inline void link(const NativeStringView& newpath, const NativeStringView& oldpath) {
            _throw_if_error(ulfd_link_u(newpath, oldpath));
        }
        inline void symlink(const NativeStringView& target, const NativeStringView& source) {
            _throw_if_error(ulfd_symlink_u(target, source));
        }
        inline std::string readlink(const NativeStringView& path) {
            ulfd_uchar_t* up;
            _throw_if_error(ulfd_readlink_alloc_u(&up, path));
            return _pass_string(up);
        }
        inline std::wstring readlink_w(const NativeStringView& path) {
            ulfd_uchar_t* up;
            _throw_if_error(ulfd_readlink_alloc_u(&up, path));
            return _pass_wstring(up);
        }
        inline String readlink_u(const NativeStringView& path) {
            ulfd_uchar_t* up;
            _throw_if_error(ulfd_readlink_alloc_u(&up, path));
            return _pass_ustring(up);
        }
        inline std::string realpath(const NativeStringView& path) {
            ulfd_uchar_t* up;
            _throw_if_error(ulfd_realpath_alloc_u(&up, path));
            return _pass_string(up);
        }
        inline std::wstring realpath_w(const NativeStringView& path) {
            ulfd_uchar_t* up;
            _throw_if_error(ulfd_realpath_alloc_u(&up, path));
            return _pass_wstring(up);
        }
        inline String realpath_u(const NativeStringView& path) {
            ulfd_uchar_t* up;
            _throw_if_error(ulfd_realpath_alloc_u(&up, path));
            return _pass_ustring(up);
        }

        inline void truncate(const NativeStringView& path, ulfd_int64_t size) {
            _throw_if_error(ulfd_truncate_u(path, size));
        }

        inline void opendir(ulfd_dir_t& dir, const NativeStringView& path) {
            _throw_if_error(ulfd_opendir_u(&dir, path));
        }
        inline void rewinddir(ulfd_dir_t& dir) {
            _throw_if_error(ulfd_rewinddir(&dir));
        }
        inline void closedir(ulfd_dir_t& dir) {
            _throw_if_error(ulfd_closedir(&dir));
        }
        inline std::string readdir(ulfd_dir_t& dir) {
            const ulfd_uchar_t* up;
            _throw_if_error(ulfd_readdir_u(&dir, &up));
            return up ? _copy_string(up) : std::string();
        }
        inline std::wstring readdir_w(ulfd_dir_t& dir) {
            const ulfd_uchar_t* up;
            _throw_if_error(ulfd_readdir_u(&dir, &up));
            return up ? _copy_wstring(up) : std::wstring();
        }
        inline String readdir_u(ulfd_dir_t& dir) {
            const ulfd_uchar_t* up;
            _throw_if_error(ulfd_readdir_u(&dir, &up));
            return up ? _copy_ustring(up) : String();
        }

        inline ulfd_spaceinfo_t& space(const NativeStringView& path, ulfd_spaceinfo_t& info) {
            _throw_if_error(ulfd_space_u(&info, path));
            return info;
        }
        inline ulfd_spaceinfo_t space(const NativeStringView& path) {
            ulfd_spaceinfo_t info;
            _throw_if_error(ulfd_space_u(&info, path));
            return info;
        }

        class DirectoryReader {
        public:
            inline DirectoryReader(const NativeStringView& path) { opendir(dir, path.get()); }
            inline ~DirectoryReader() { ulfd_closedir(&dir); }

            inline std::string next() { return readdir(dir); }
            inline std::wstring next_w() { return readdir_w(dir); }
            inline String next_u() { return readdir_u(dir); }

            inline void rewind() { rewinddir(dir); }
        private:
            ulfd_dir_t dir;
        };

        struct DirectoryReaderIterator {
        public:
            inline DirectoryReaderIterator(const NativeStringView& path) : up(nullptr) {
                dir = new ulfd_dir_t;
                _throw_if_error(ulfd_opendir_u(dir, path));
                _throw_if_error(ulfd_readdir_u(dir, &up));
            }
            inline DirectoryReaderIterator() : dir(nullptr), up(nullptr) { }
            inline ~DirectoryReaderIterator() {
                if(dir) { ulfd_closedir(dir); delete dir; dir = nullptr; }
                up = nullptr;
            }

            inline DirectoryReaderIterator(const DirectoryReaderIterator&) = delete;
            inline DirectoryReaderIterator(DirectoryReaderIterator&& other) : dir(other.dir), up(other.up) {
                other.dir = nullptr; other.up = nullptr;
            }
            inline DirectoryReaderIterator& operator=(const DirectoryReaderIterator&) = delete;
            inline DirectoryReaderIterator& operator=(DirectoryReaderIterator&& other) {
                closedir(*dir); delete dir;
                dir = other.dir; up = other.up;
                other.dir = nullptr; other.up = nullptr;
                return *this;
            }

            inline DirectoryReaderIterator& operator++() {
                _throw_if_error(ulfd_readdir_u(dir, &up));
                return *this;
            }
            inline std::string operator*() const{ return up ? _copy_string(up) : std::string(); }

            inline bool operator==(const DirectoryReaderIterator& other) const {
                return up == other.up;
            }
            inline bool operator!=(const DirectoryReaderIterator& other) const {
                return up != other.up;
            }
        private:
            ulfd_dir_t* dir;
            const ulfd_uchar_t* up;
        };

        struct DirectoryReaderIteratorW {
        public:
            inline DirectoryReaderIteratorW(const NativeStringView& path) : up(nullptr) {
                dir = new ulfd_dir_t;
                _throw_if_error(ulfd_opendir_u(dir, path));
                _throw_if_error(ulfd_readdir_u(dir, &up));
            }
            inline DirectoryReaderIteratorW() : dir(nullptr), up(nullptr) { }
            inline ~DirectoryReaderIteratorW() {
                if(dir) { ulfd_closedir(dir); delete dir; dir = nullptr; }
                up = nullptr;
            }

            inline DirectoryReaderIteratorW(const DirectoryReaderIteratorW&) = delete;
            inline DirectoryReaderIteratorW(DirectoryReaderIteratorW&& other) : dir(other.dir), up(other.up) {
                other.dir = nullptr; other.up = nullptr;
            }
            inline DirectoryReaderIteratorW& operator=(const DirectoryReaderIteratorW&) = delete;
            inline DirectoryReaderIteratorW& operator=(DirectoryReaderIteratorW&& other) {
                closedir(*dir); delete dir;
                dir = other.dir; up = other.up;
                other.dir = nullptr; other.up = nullptr;
                return *this;
            }

            inline DirectoryReaderIteratorW& operator++() {
                _throw_if_error(ulfd_readdir_u(dir, &up));
                return *this;
            }
            inline std::wstring operator*() const{ return up ? _copy_wstring(up) : std::wstring(); }

            inline bool operator==(const DirectoryReaderIteratorW& other) const {
                return up == other.up;
            }
            inline bool operator!=(const DirectoryReaderIteratorW& other) const {
                return up != other.up;
            }
        private:
            ulfd_dir_t* dir;
            const ulfd_uchar_t* up;
        };

        struct DirectoryReaderIteratorU {
        public:
            inline DirectoryReaderIteratorU(const NativeStringView& path) : up(nullptr) {
                dir = new ulfd_dir_t;
                _throw_if_error(ulfd_opendir_u(dir, path));
                _throw_if_error(ulfd_readdir_u(dir, &up));
            }
            inline DirectoryReaderIteratorU() : dir(nullptr), up(nullptr) { }
            inline ~DirectoryReaderIteratorU() {
                if(dir) { ulfd_closedir(dir); delete dir; dir = nullptr; }
                up = nullptr;
            }

            inline DirectoryReaderIteratorU(const DirectoryReaderIteratorU&) = delete;
            inline DirectoryReaderIteratorU(DirectoryReaderIteratorU&& other) : dir(other.dir), up(other.up) {
                other.dir = nullptr; other.up = nullptr;
            }
            inline DirectoryReaderIteratorU& operator=(const DirectoryReaderIteratorU&) = delete;
            inline DirectoryReaderIteratorU& operator=(DirectoryReaderIteratorU&& other) {
                closedir(*dir); delete dir;
                dir = other.dir; up = other.up;
                other.dir = nullptr; other.up = nullptr;
                return *this;
            }

            inline DirectoryReaderIteratorU& operator++() {
                _throw_if_error(ulfd_readdir_u(dir, &up));
                return *this;
            }
            inline String operator*() const{ return up ? _copy_ustring(up) : String(); }

            inline bool operator==(const DirectoryReaderIteratorU& other) const {
                return up == other.up;
            }
            inline bool operator!=(const DirectoryReaderIteratorU& other) const {
                return up != other.up;
            }
        private:
            ulfd_dir_t* dir;
            const ulfd_uchar_t* up;
        };

        struct DirectoryReaderRange {
        public:
            DirectoryReaderRange(const NativeStringView& path) : hold_path(path) { }
            DirectoryReaderIterator begin() { return DirectoryReaderIterator(hold_path); }
            DirectoryReaderIterator end() { return DirectoryReaderIterator(); }
        private:
            NativeStringView hold_path;
        };

        struct DirectoryReaderRangeW {
        public:
            DirectoryReaderRangeW(const NativeStringView& path) : hold_path(path) { }
            DirectoryReaderIteratorW begin() { return DirectoryReaderIteratorW(hold_path); }
            DirectoryReaderIteratorW end() { return DirectoryReaderIteratorW(); }
        private:
            NativeStringView hold_path;
        };

        struct DirectoryReaderRangeU {
        public:
            DirectoryReaderRangeU(const NativeStringView& path) : hold_path(path) { }
            DirectoryReaderIteratorU begin() { return DirectoryReaderIteratorU(hold_path); }
            DirectoryReaderIteratorU end() { return DirectoryReaderIteratorU(); }
        private:
            NativeStringView hold_path;
        };
    }
}
