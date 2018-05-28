#include <AudioGeneration-UT/Exception.h>

Exception::Exception()
{

}

Exception::Exception(const std::string& what) : what_(what)
{

}

const char* Exception::what() const noexcept
{
    return what_.c_str();
}
