// Add frequently used header files or remove header files that are less frequently
// used during project evolvement.

#if defined(CXBASE_BUILD_PCH_PRECOMPILE_H_)
#error Include the precompiled header file more than once is prohibited.
#endif

#define CXBASE_BUILD_PCH_PRECOMPILE_H_

#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <deque>
#include <set>

#include <random>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <regex>

#include <exception>
#include <typeinfo>
#include <type_traits>


#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <stdarg.h>
#include <assert.h>
#include <stdint.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <nlohmann/json.hpp>
#include <NESupport.h>