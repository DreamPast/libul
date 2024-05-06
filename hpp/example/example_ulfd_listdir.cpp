#include "ulfd.hpp"
#include <iostream>
#include <fstream>
int main(int argc, char** argv) {
#ifdef _WIN32
    if(!SetConsoleOutputCP(65001)) {
        fputs("cannot set UTF-8 in Windows", stderr);
        exit(1);
    }
#endif

    using namespace ul;
    try {
        if(argc > 1) fd::chdir(argv[1]);
        for(auto s : fd::DirectoryReaderRange(".")) {
            std::cout << s << std::endl;
        }
    } catch(const fd::Exception& e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }
    return 0;
}
