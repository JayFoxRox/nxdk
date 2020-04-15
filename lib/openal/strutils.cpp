
#include "config.h"

#include "openal-soft/common/strutils.h"

#include <cstdlib>

typedef unsigned short WCHAR;

std::string wstr_to_utf8(const WCHAR *wstr)
{
    std::string ret;
    return ret;
}

std::wstring utf8_to_wstr(const char *str)
{
    std::wstring ret;
    return ret;
}

namespace al {

al::optional<std::string> getenv(const char *envname)
{
    return al::nullopt;
}

} // namespace al
