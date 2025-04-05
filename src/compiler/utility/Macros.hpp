#pragma once

#if defined(_MSC_VER)
#define FORCE_INLINE [[msvc::forceinline]]
#elif defined(__GNUC__) || defined(__clang__)
#define FORCE_INLINE __attribute__((always_inline))
#else
#define FORCE_INLINE
#endif

#ifdef _MSC_VER
#define STDCALL __stdcall
#define CDECL __cdecl
#define FASTCALL __fastcall
#define VECTORCALL __vectorcall
#else
#define STDCALL __attribute__((stdcall))
#define CDECL __attribute__((cdecl))
#define FASTCALL __attribute__((fastcall))
#define VECTORCALL __attribute__((vectorcall))
#endif