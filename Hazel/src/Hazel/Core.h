#pragma once

#ifdef HZ_PLATFORM_WINDOWS
#ifdef HZ_DYNAMIC_LINK
    #ifdef HZ_BUILD_DLL
        #define HAZEL_API _declspec(dllexport)
   #else
        #define HAZEL_API _declspec(dllimport)
      #endif
#else 
#define HAZEL_API
#endif

#else
       #error Hazel only support Windows !
#endif

#ifdef HZ_DEBUG
#define HZ_ENABLE_ASSETS
#endif

#ifdef HZ_ENABLE_ASSETS

#define HZ_ASSERT(x,...) { if(!(x)){HZ_ERROR("Assertion Failed:{0}",__VA_ARGS__);__debugbreak();}}
#define HZ_CORE_ASSERT(x,...) {if(!(x)){HZ_CORE_ERROR("Assertion Failed:{0}",__VA_ARGS__);__debugbreak();}}

#else
#define HZ_CORE_ASSERT(x,...)
#define HZ_ASSERT(x,...)
#endif // HZ_ENABLE_ASSETS

 
#define BIT(x) (1<<x) 

#define HZ_BIND_EVENT_FN(fn) std::bind(&fn,this,std::placeholders::_1)