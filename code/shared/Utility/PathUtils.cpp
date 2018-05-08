

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <string.h>
#endif

#include <Utility/PathUtils.h>

namespace Utility
{
#ifdef _WIN32
    std::wstring &GetAbsolutePathW()
    {
        static std::wstring path;

        if (path.empty())
        {
            wchar_t buf[MAX_PATH] = { 0 };
            GetModuleFileNameW(nullptr, buf, MAX_PATH);

            //append string terminator
            for (size_t i = wcslen(buf); i > 0; --i)
            {
                if (buf[i] == '\\')
                {
                    buf[i + 1] = 0;
                    break;
                }
            }

            path = std::wstring(buf);
        }

        return path;
    }

    std::string &GetAbsolutePathA()
    {
        static std::string path;

        if (path.empty())
        {
            char buf[MAX_PATH] = { 0 };
            GetModuleFileNameA(nullptr, buf, MAX_PATH);

            //append string terminator
            for (size_t i = strlen(buf); i > 0; --i)
            {
                if (buf[i] == '\\')
                {
                    buf[i + 1] = 0;
                    break;
                }
            }

            path = std::string(buf);
        }

        return path;
    }
#else

    std::wstring &GetAbsolutePathW()
    {
        static std::wstring empty = L"";
        return empty;
    }

    std::string &GetAbsolutePathA()
    {
        static std::string path;

        if (path.empty())
        {
            char buf[512];
            readlink("/proc/self/exe", buf, sizeof(buf));

            char* dirPtr = strrchr(buf, '/');

            dirPtr[1] = '\0';

            path = std::string(buf);
        }

        return path;
    }

#endif
}