#pragma once


#ifndef AMBITION_API
#ifdef _MSC_VER
#ifdef AMBITION_EXPORTS
#define AMBITION_API __declspec(dllexport)
#else
#define AMBITION_API __declspec(dllimport)
#endif
#else
#define AMBITION_API
#endif
#endif

extern void AMBITION_API Test(bool fullscreen);