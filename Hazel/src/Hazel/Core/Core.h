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
#endif 

 
#define BIT(x) (1<<x) 

#define HZ_BIND_EVENT_FN(fn) std::bind(&fn,this,std::placeholders::_1)

#define HZ_RENDER(fn)   

namespace Hazel {
    template<typename T>
    using Scope = std::unique_ptr<T>;

    //允许使用多个参数创建对应的指针
    template <typename T,typename ... Args>
    //Args&&模板既可以绑定左值，又可以绑定右值
    constexpr Scope<T> CreateScope(Args&& ... args) {
        //forward和move的区别：(完美转发专用forward，右值引用绑定右值)
        return std::make_unique<T>(std::forward<Args>(args)...);
        
    }


    template<typename T>
    using Ref = std::shared_ptr<T>;

    template <typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args) {
        //forward是函数参数的完美转换，优于move（单独右值绑定）
        return std::make_shared<T>(std::forward<Args>(args)...);
    }


}