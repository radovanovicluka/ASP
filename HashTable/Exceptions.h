
#ifndef DZ3_EXCEPTIONS_H
#define DZ3_EXCEPTIONS_H

#include <exception>

using namespace std;

class FileError : public exception {
public:
    const char * what() const noexcept override{
        return "File Error";
    }
};

class InvalidValue : public exception {
public:
    const char * what() const noexcept override {
        return "Uneta je nevalidna vrednost";
    }
};

#endif //DZ3_EXCEPTIONS_H
