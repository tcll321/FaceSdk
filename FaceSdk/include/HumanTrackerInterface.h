#pragma once
#include "common_type.h"
#include <ImiSkeleton.h>

#ifdef _WIN32 
#ifdef HUMANTRACKER_EXPORT
#define DLL_EXPORT _declspec(dllexport)
#else
#define DLL_EXPORT _declspec(dllimport)
#endif
#else
#define DLL_EXPORT __attribute__(visibility("default"))
#endif

#ifdef __cplusplus
extern "C"
{
#endif
	//handle 是用户注册, 同cb_hdl
	typedef void(*humantracker_cb)(void* handle, tracker_t* vTracker, int nTracker);

	DLL_EXPORT void* CreateHumanTracker(const char* cfgFile);
	//vRes[out]
	//nRes[out]
	//blocked
	DLL_EXPORT void ProcessHumanTracker(void* hdl, shop_image_t * vImage, int nImageCount, ImiSkeletonFrame* vSkeleton, int nSkeletonCount);//1 2
	//DLL_EXPORT void SetCallbackHumanTracker(void* hdl, humantracker_cb cb, void* cb_hdl);//cb_hdl类 cb(cb_hdl,...);
	//DLL_EXPORT void UpdateFaceHumanTracker(void* hdl, const face_info_t* feat, long long stamp);
	DLL_EXPORT void ProcessHumanTrackerSync(void* hdl, shop_image_t * vImage, int nImageCount, ImiSkeletonFrame* vSkeleton, int nSkeletonCount, tracker_t* vTracker, int& nTracker, tracker_stat_t* stat);//1 2

	DLL_EXPORT void ReleaseHumanTracker(void* hdl);

#ifdef __cplusplus
}
#endif
