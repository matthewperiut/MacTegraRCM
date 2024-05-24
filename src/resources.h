#pragma once

#include <string>

#ifdef __APPLE__
    #include <mach-o/dyld.h>

    std::string GetResourcePath()
    {
        static std::string pathStr;

        if (pathStr.empty())
        {
            char path[1024];
            uint32_t size = sizeof(path);
            if (_NSGetExecutablePath(path, &size) == 0)
                pathStr = path;
            else
                printf("buffer too small; need size %u\n", size);

            pathStr.erase(pathStr.begin()+pathStr.find("MacOS/MacTegraRCM"), pathStr.end());
            pathStr += "Resources/";
        }

        return pathStr;
    }
#endif
#if defined(_WIN32) || defined(__linux__)
std::string GetResourcePath()
    {
        return "./";
    }
#endif