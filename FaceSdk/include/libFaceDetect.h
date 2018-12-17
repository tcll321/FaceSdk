#ifndef IMI_FACE_DETECT_API_H_
#define IMI_FACE_DETECT_API_H_
#include"imiCV.h"
#ifdef IMI_EXPORTS
#define IMI_API __declspec(dllexport)
#else
#define IMI_API __declspec(dllexport)
#endif

namespace imi
{
	
		IMI_API bool FaceDetectionInitialization();
		IMI_API std::vector<imi::FaceInfo> detectFace(const imiMat *pImg);
		IMI_API bool FaceDetectionRelease();

}  // namespace imi

#endif  
