/******************* Processor-specific glue  *****************************/

#ifndef _MSC_VER

#if defined(__i686__) && !defined(_M_IX86)
#define _M_IX86 600
#elif defined(__i586__) && !defined(_M_IX86)
#define _M_IX86 500
#elif defined(__i486__) && !defined(_M_IX86)
#define _M_IX86 400
#elif defined(__i386__) && !defined(_M_IX86)
#define _M_IX86 300
#elif defined(__x86_64__) && !defined(_M_AMD64)
#define _M_AMD64 100
#elif defined(__arm__) && !defined(_M_ARM)
#define _M_ARM 7
#elif defined(__aarch64__) && !defined(_M_ARM64)
#define _M_ARM64 1
#elif defined(__loongarch64) && !defined(_M_LOONGARCH64)
#define _M_LOONGARCH64 1
#elif defined(__s390x__) && !defined(_M_S390X)
#define _M_S390X 1
#elif defined(__powerpc__) && !defined(_M_PPC64)
#define _M_PPC64 1
#endif
#endif

#if defined(_M_IX86) && !defined(HOST_X86)
#define HOST_X86
#elif defined(_M_AMD64) && !defined(HOST_AMD64)
#define HOST_AMD64
#elif defined(_M_ARM) && !defined(HOST_ARM)
#define HOST_ARM
#elif defined(_M_ARM64) && !defined(HOST_ARM64)
#define HOST_ARM64
#elif defined(_M_LOONGARCH64) && !defined(HOST_LOONGARCH64)
#define HOST_LOONGARCH64
#elif defined(_M_S390X) && !defined(HOST_S390X)
#define HOST_S390X
#elif defined(_M_PPC64) && !defined(HOST_POWERPC64)
#define HOST_POWERPC64
#endif

#ifdef HOST_X86
#define TARGET_X86 1
#elif defined(HOST_AMD64)
#define TARGET_AMD64 1
#elif defined(HOST_ARM)
#define TARGET_ARM 1
#elif defined(HOST_ARM64)
#define TARGET_ARM64 1
#elif defined(HOST_LOONGARCH64)
#define TARGET_LOONGARCH64 1
#elif defined(HOST_S390X)
#define TARGET_S390X 1
#elif defined(HOST_POWERPC64)
#define TARGET_POWERPC64 1
#endif

#include <windows.h>
