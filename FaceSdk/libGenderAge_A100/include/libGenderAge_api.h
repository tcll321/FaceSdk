#pragma once
/**************************************************************************
@File Name     : libGenderAge_api.h
@Author        : fuwenyan@hjimi.com
@Date          : 2018-12-04
@Description   : get person's Gender and age
@Version       : 0.2.20181204
@note		   :
@Dependency    :
pthreadVC2
@History       :
1.2018-12-04 Fu Wenyan Created file
***************************************************************************/
#include "imiCV.h"

#define IMI_API extern "C"
namespace imi
{
	/// @brief  face GenderAge initialization
	/// @return if run normally return IMI_OK£¬otherwise return -1
	IMI_API int imi_face_init_GenderAge(const char* model_path="../model");

	/// @brief  get face GenderAge version
	/// @return version information
	IMI_API char* imi_face_GenderAge_get_version();


	/// @brief  get face gender and age
	/// @param[in]	in_image		3 channels BGR image, data flow as BRGBGRBRG...BRG
	/// @param[in+out]	face_info		input face info returned by libfacedetect,output gender age
	/// @return if run normally return IMI_OK£¬otherwise return -1
	IMI_API int imi_face_get_GenderAge(const imi::imiMat* in_image, imi::FaceInfo* face_info);


	/// @brief  get face gender and age for video tracking
	/// @param[in]	in_image		3 channels BGR image, data flow as BRGBGRBRG...BRG
	/// @param[in+out]	face_info		input face info returned by libfacedetect,output gender age
	/// @return if run normally return IMI_OK£¬otherwise return -1
	IMI_API int imi_face_get_GenderAge_refine(const imi::imiMat* in_image, std::vector<imi::FaceInfo>& face_info);


	/// @brief  face GenderAge release
	/// @return NULL
	IMI_API void imi_face_release_GenderAge();

}
