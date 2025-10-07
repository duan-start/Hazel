#pragma once
#include <memory>
//����ļ�һ�����ȫ�ֵ�һЩ�꣬����һЩȫ�ֵķ�װ������Ԥ���������֮��
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

    //����ʹ�ö������������Ӧ��ָ�룬��������һ��ģ�壬����û̫������Ҫ������
    //�о����㲻��װҲûʲô��ϵ
    //������Ϊ�˺����Լ�дһ���µ��߼�����ɣ�ֱ�Ӹ������洴��ָ�����ʽ��
    template <typename T,typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args) {
        //forward��move������(����ת��ר��forward����������ķ���move&&)
        //nice Writing
        return std::make_unique<T>(std::forward<Args>(args)...);
        
    }


    template<typename T>
    using Ref = std::shared_ptr<T>;

    template <typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args) {
        //forward��move������
        //forward�Ǻ�������������ת���������ڶ��move��
        return std::make_shared<T>(std::forward<Args>(args)...);
    }


}