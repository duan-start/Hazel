#pragma once
//预编译头文件,通用的东西（统一编译，而且不会改变），避免每次都编译（保留在那里）
//被所有的cpp文件包含，因为声明几乎不会影响cpu的编译时间
#include <iostream>

#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <cstddef>

#include <string>
#include <array>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include "Hazel/Core/Log.h"
#include "Debug/Instrumentor.h"


#ifdef HZ_PLATFORM_WINDOWS
#include <Windows.h>
#endif