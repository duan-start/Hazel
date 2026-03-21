#pragma once
//预编译头文件,通用的东西（统一编译，而且不会改变），避免每次都编译（保留在那里）
//被所有的cpp文件包含，而且并不需要在此之后的语法检查以及编译工作的任何（只会在这个文件里面编译一次）
//otherwise  头文件可以并不包含这些而且能够直接使用里的func(只需要有对应的cpp文件包含作为addhere)
#include <iostream>

#include <thread>
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