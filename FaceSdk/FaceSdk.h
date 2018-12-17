// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 FACESDK_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// FACESDK_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef FACESDK_EXPORTS
#define FACESDK_API __declspec(dllexport)
#else
#define FACESDK_API __declspec(dllimport)
#endif

#include "define.h"

typedef void (*Callback_OnFaceInfo)(
	int device,
	int sex,
	int age,
	int id,
	void* userParam
	);

FACESDK_API int Face_Init(int *deviceCnt);

FACESDK_API int Face_Create(int device, HWND hwnd, Callback_OnFaceInfo info, void* userParam);

FACESDK_API int Face_SetFaceWnd(int faceId, HWND hwnd);

FACESDK_API int Face_Delete(int device);

FACESDK_API int Face_InitTrajectory();

typedef void(*Callback_OnTrajectoryInfo)(
	const _TrajectoryInfo *info,
	void* userParam
	);

FACESDK_API int Face_StartTrajectory(HWND hwnd, Callback_OnTrajectoryInfo info, void* userParam);

FACESDK_API void Face_StopTrajectory();
