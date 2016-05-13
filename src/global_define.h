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

#endif //CAPPUCCINO_GLOBAL_DEFINE_H
