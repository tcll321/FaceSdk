#include "FaceDetector.h"

#ifdef USE_SEETA_DET
#include "SeetaDetector.h"
#endif
#ifdef USE_IMI_DET
#include "ImiDetector.h"
#endif
#ifdef USE_DEPTH_DET
#include "DepthDetector.h"
#endif
FaceBaseDetector* FaceBaseDetector::create(std::string type, std::string data_path /* = "" */)
{
	FaceBaseDetector* p = 0;
#ifdef USE_SEETA_DET
	if (type == "seeta")
		p = new SeetaDetector;
#endif
#ifdef USE_IMI_DET
	if (type == "imi")
		p = new ImiDetector;
#endif
#ifdef USE_DEPTH_DET
	if (type == "depth")
	{
		p = new DepthDetector;
	}
#endif
	if (!p)
		p = new FaceBaseDetector;

	p->Init(data_path);
	return p;
}