#ifndef _ICEBERG_CORE
#define _ICEBERG_CORE

#ifdef ICEBERG_BUILD_DLL
//#define ICEBERG_API __declspec(dllexport)
#define ICEBERG_API // Not using dynamic library at the moment
#else
//#define ICEBERG_API __declspec(dllimport)
#define ICEBERG_API // Not using dynamic library at the moment
#endif

#endif