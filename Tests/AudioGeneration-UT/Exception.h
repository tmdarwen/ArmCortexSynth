#pragma once

#include <exception>
#include <string>

class Exception : public std::exception
{
    public:
        Exception();
        Exception(const std::string& what);
        const char* what() const noexcept override;
    
    private:
        std::string what_;
};

