/** @file inject_dll.h
  * @brief inject_dll
  * @author fei.xu
  * @date 2021/03/19
  */

#ifndef _INJECT_DLL_7A9EAC2E_51C2_44CA_8627_E8E4A8AB06E9_
#define _INJECT_DLL_7A9EAC2E_51C2_44CA_8627_E8E4A8AB06E9_

// 定义导出宏
#ifdef INJECT_DLL_EXPORTS
#define	INJECT_DLL_API __declspec(dllexport)
#else
#define	INJECT_DLL_API __declspec(dllimport)
#endif

#endif//_INJECT_DLL_7A9EAC2E_51C2_44CA_8627_E8E4A8AB06E9_