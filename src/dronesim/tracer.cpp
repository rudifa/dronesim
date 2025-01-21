#include "tracer.hpp"

int Tracer::indent = 0;
std::chrono::high_resolution_clock::time_point Tracer::operationStartTime =
    std::chrono::high_resolution_clock::now();

std::ofstream& Tracer::getLogFile()
{
    static std::ofstream logFile("Tracer.log");
    return logFile;
}

long long Tracer::get_nanoseconds()
{
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               now - operationStartTime)
        .count();
}

void Tracer::trace(bool enter)
{
    auto& logFile = getLogFile();
    logFile << std::string(indent, ' ');

    logFile << (enter ? "Enter " : "Exit ");

    logFile << (className.empty() ? "" : className + "::") << functionName
            << " at " << get_nanoseconds() << " ns";

    if (!enter)
    {
        auto duration =
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::high_resolution_clock::now() - startTime)
                .count();
        logFile << " (duration: " << duration << " ns)";
    }

    logFile << std::endl;
}

void Tracer::parseFunctionName(const char* prettyFunction)
{
    std::string pretty(prettyFunction);

    // Extract the function signature
    size_t start = pretty.find_first_of('(');
    if (start != std::string::npos)
    {
        std::string signature = pretty.substr(0, start);

        // Find the last space before the function name
        size_t lastSpace = signature.find_last_of(' ');
        if (lastSpace != std::string::npos)
        {
            std::string fullName = signature.substr(lastSpace + 1);

            // Split the full name into class name and function name
            size_t colonPos = fullName.find("::");
            if (colonPos != std::string::npos)
            {
                className = fullName.substr(0, colonPos);
                functionName = fullName.substr(colonPos + 2);
            }
            else
            {
                className = "";
                functionName = fullName;
            }
        }
        else
        {
            functionName = signature;
        }
    }
    else
    {
        functionName = pretty;
    }
}
