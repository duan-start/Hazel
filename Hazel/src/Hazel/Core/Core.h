#pragma once
#include <memory>
//这个文件一般就是全局的一些宏，进行一些全局的封装，或者预编译的条件之类
//#ifdef HZ_PLATFORM_WINDOWS
#ifdef HZ_DYNAMIC_LINK
    #ifdef HZ_BUILD_DLL
        #define HAZEL_API _declspec(dllexport)
   #else
        #define HAZEL_API _declspec(dllimport)
      #endif
#else 
#define HAZEL_API
//#endif

//#else
       //#error Hazel only support Windows !
#endif

#ifdef HZ_DEBUG
#define HZ_ENABLE_ASSETS
#endif

#ifdef HZ_ENABLE_ASSETS

#define HZ_ASSERT(x,...) { if(!(x)){HZ_ERROR("Assertion Failed:{0}",__VA_ARGS__);__debugbreak();}}
#define HZ_CORE_ASSERT(x,...) {if(!(x)){HZ_CORE_ERROR("Assertion Failed:{0}",##__VA_ARGS__);__debugbreak();}}

#else
#define HZ_CORE_ASSERT(x,...)
#define HZ_ASSERT(x,...)
#endif // HZ_ENABLE_ASSETS

 
#define BIT(x) (1<<x) 

#define HZ_BIND_EVENT_FN(fn) std::bind(&fn,this,std::placeholders::_1)

namespace Hazel {
    template<typename T>
    using Scope = std::unique_ptr<T>;

    //允许使用多个参数创建对应的指针，这里做了一个模板，但是没太明白重要性在哪
    //感觉就算不封装也没什么关系
    //可能是为了后面自己写一套新的逻辑替代吧（直接改这里面创建指针的形式）
    template <typename T,typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args) {
        //forward和move的区别：(完美转发专用forward和其他情况的泛用move&&)
        //nice Writing
        return std::make_unique<T>(std::forward<Args>(args)...);
        
    }


    template<typename T>
    using Ref = std::shared_ptr<T>;

    template <typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args) {
        //forward和move的区别：
        //forward是函数参数的完美转化，是优于多次move的
        return std::make_shared<T>(std::forward<Args>(args)...);
    }


}