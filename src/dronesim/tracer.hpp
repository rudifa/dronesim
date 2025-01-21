#pragma once

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <type_traits>

// Helper template to check if a type is a pointer to member function
template <typename T>
struct is_member_function_pointer : std::false_type
{
};

template <typename R, typename C, typename... Args>
struct is_member_function_pointer<R (C::*)(Args...)> : std::true_type
{
};

template <typename R, typename C, typename... Args>
struct is_member_function_pointer<R (C::*)(Args...) const> : std::true_type
{
};

// Helper function to get class name
template <typename T>
std::string get_class_name(T)
{
    if constexpr (is_member_function_pointer<T>::value)
    {
        return typeid(typename std::remove_pointer<typename std::remove_const<
                          typename std::remove_reference<
                              decltype(std::declval<T>().operator->())>::type>::
                                                       type>::type)
            .name();
    }
    return "";
}

class Tracer
{
   public:
    static int indent;
    static std::ofstream& getLogFile();
    static std::chrono::high_resolution_clock::time_point operationStartTime;

    explicit Tracer(const char* prettyFunction)
        : startTime(std::chrono::high_resolution_clock::now())
    {
        parseFunctionName(prettyFunction);
        trace(true);
        indent++;
    }

    ~Tracer()
    {
        indent--;
        trace(false);
    }

   private:
    std::string className;
    std::string functionName;
    std::chrono::high_resolution_clock::time_point startTime;

    void trace(bool enter);

    void parseFunctionName(const char* prettyFunction);

    long long get_nanoseconds();
};

#define TRACE Tracer tracer(__PRETTY_FUNCTION__)
