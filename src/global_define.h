//
// Created by noccolor on 16/4/30.
//

#ifndef CAPPUCCINO_GLOBAL_DEFINE_H
#define CAPPUCCINO_GLOBAL_DEFINE_H

#ifdef __USE_DLL__
#define USE_DLL __attribute__ ((visibility("default")))
#else
#define USE_DLL
#endif

#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#define NC_DEPRECATED __attribute__((deprecated))
#elif _MSC_VER >= 1400 //vs 2005 or higher
#define NC_DEPRECATED __declspec(deprecated)
#else
    #define CC_DEPRECATED_ATTRIBUTE
#endif

#endif //CAPPUCCINO_GLOBAL_DEFINE_H
