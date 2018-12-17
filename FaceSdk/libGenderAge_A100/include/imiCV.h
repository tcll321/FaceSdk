#ifndef __IMICV__H
#define __IMICV__H
#include <assert.h>
#include <math.h>
#include <iostream>
#include <vector>
typedef unsigned char uchar;
typedef unsigned short ushort;
#define IMI_PI 3.1415926535897932384626433832795
//#define USE_OPENMP

//#ifndef USE_OPENMP
//#define USE_TBB
//#include<tbb\tbb.h>
//
//#endif
// exchange-add operation for atomic operations on reference counters
#if defined __INTEL_COMPILER && !(defined WIN32 || defined _WIN32)
// atomic increment on the linux version of the Intel(tm) compiler
#  define IMICV_XADD(addr, delta) (int)_InterlockedExchangeAdd(const_cast<void*>(reinterpret_cast<volatile void*>(addr)), delta)
#elif defined __GNUC__
#  if defined __clang__ && __clang_major__ >= 3 && !defined __ANDROID__ && !defined __EMSCRIPTEN__ && !defined(__CUDACC__)
#    ifdef __ATOMIC_ACQ_REL
#      define IMICV_XADD(addr, delta) __c11_atomic_fetch_add((_Atomic(int)*)(addr), delta, __ATOMIC_ACQ_REL)
#    else
#      define IMICV_XADD(addr, delta) __atomic_fetch_add((_Atomic(int)*)(addr), delta, 4)
#    endif
#  else
#    if defined __ATOMIC_ACQ_REL && !defined __clang__
// version for gcc >= 4.7
#      define IMICV_XADD(addr, delta) (int)__atomic_fetch_add((unsigned*)(addr), (unsigned)(delta), __ATOMIC_ACQ_REL)
#    else
#      define IMICV_XADD(addr, delta) (int)__sync_fetch_and_add((unsigned*)(addr), (unsigned)(delta))
#    endif
#  endif
#elif defined _MSC_VER && !defined RC_INVOKED
#  include <intrin.h>
#  define IMICV_XADD(addr, delta) (int)_InterlockedExchangeAdd((long volatile*)addr, delta)
#else
static inline void IMICV_XADD(int* addr, int delta) { int tmp = *addr; *addr += delta; return tmp; }
#endif
namespace imi{
#define FACE_LEFT_HALF 0x101
#define FACE_RIGHT_HALF 0x102
#define FACE_FRONTAL 0x100
#define FACE_LEFT_FULL 0x111
#define FACE_RIGHT_FULL 0x112
	//////////////////////////////// imiPoint2_ ////////////////////////////////
	template<typename _Tp> struct imiPoint_
	{
		// various constructors
		imiPoint_() : x(0), y(0){}
		imiPoint_(_Tp _x, _Tp _y) :x(_x), y(_y){  }
		imiPoint_(const imiPoint_& pt) : x(pt.x), y(pt.y){ }
		_Tp x;
		_Tp y; //< the point coordinates
	};
	typedef imiPoint_<int> imiPoint2i;
	typedef imiPoint_<float> imiPoint2f;
	typedef imiPoint_<double> imiPoint2d;
	typedef imiPoint2i imiPoint;

	//////////////////////////////// imiPoint3_ ////////////////////////////////
	template<typename _Tp> struct imiPoint3_
	{
		//typedef _Tp value_type;
		// various constructors
		imiPoint3_() : x(0), y(0),z(0){}
		imiPoint3_(_Tp _x, _Tp _y,_Tp _z) :x(_x), y(_y),z(_z){  }
		imiPoint3_(const imiPoint3_& pt) : x(pt.x), y(pt.y),z(pt.z){ }

		_Tp x;
		_Tp y; //
		_Tp z;
	};
	typedef imiPoint3_<int> imiPoint3i;
	typedef imiPoint3_<float> imiPoint3f;
	typedef imiPoint3_<double> imiPoint3d;
	typedef imiPoint3i imiPoint3;

	//////////////////////////////// imiSize_ ////////////////////////////////
	struct imiSize
	{
		//! various constructors
		imiSize() : width(0), height(0){}
		imiSize(int _width, int _height) : width(_width), height(_height){}
		imiSize(const imiSize& sz) :width(sz.width), height(sz.height){}

		int width;
		int height; // the width and the height
	};

	//////////////////////////////// imiRect_ ////////////////////////////////
	/*!
	The 2D up-right rectangle class
	The class represents a 2D rectangle with coordinates of the specified data type.
	*/
	template<typename _Tp> struct imiRect_
	{
		//typedef _Tp value_type;
		//! various constructors
		imiRect_() :x(0), y(0), width(0), height(0){};
		imiRect_(_Tp _x, _Tp _y, _Tp _width, _Tp _height) : x(_x), y(_y), width(_width), height(_height) {}
		imiRect_(const imiRect_& r) : x(r.x), y(r.y), width(r.width), height(r.height) {}
		imiRect_(const imiPoint_<_Tp>& org, const imiSize sz) :x(org.x), y(org.y), width(sz.width), height(sz.height) { }
		imiRect_(const imiPoint_<_Tp>& pt1, const imiPoint_<_Tp>& pt2) : x(pt1.x), y(pt1.y), width(pt2.x - pt1.x), height(pt2.y - pt1.y){ }

		_Tp x;
		_Tp y;
		_Tp width;
		_Tp height; //< the top-left corner, as well as width and height of the rectangle
	};
	typedef imiRect_<int> imiRect;

	typedef struct FaceInfo {
		imiRect bbox;
		float roll;
		float pitch;
		float yaw;
		float depth;// 
		float score;   /* confidential values, i.e. Larger score means higher confidence. */
		imiPoint landmark[5];
		int profile = 0;
		int gender;// added on Mar.1,2018
		int age;
		float faceFeature[256];

	} FaceInfo;


	//  type defintions of imi follows that of opencv. 
#define IMI_CV_8U   0
#define IMI_CV_8S   1
#define IMI_CV_16U  2
#define IMI_CV_16S  3
#define IMI_CV_32S  4
#define IMI_CV_32F  5
#define IMI_CV_64F  6

#define CV_CN_MAX     512
#define CV_CN_SHIFT   3
#define CV_DEPTH_MAX  (1 << CV_CN_SHIFT)
#define CV_MAT_DEPTH_MASK       (CV_DEPTH_MAX - 1)
#define CV_MAT_DEPTH(flags)     ((flags) & CV_MAT_DEPTH_MASK)
#define CV_MAKETYPE(depth,cn) (CV_MAT_DEPTH(depth) + (((cn)-1) << CV_CN_SHIFT))

#define IMI_CV_8UC1 CV_MAKETYPE(IMI_CV_8U,1)
#define IMI_CV_8UC3 CV_MAKETYPE(IMI_CV_8U,3)

#define IMI_CV_8SC1 CV_MAKETYPE(IMI_CV_8S,1)
#define IMI_CV_8SC3 CV_MAKETYPE(IMI_CV_8S,3)

#define IMI_CV_16UC1 CV_MAKETYPE(IMI_CV_16U,1)
#define IMI_CV_16UC3 CV_MAKETYPE(IMI_CV_16U,3)

#define IMI_CV_16SC1 CV_MAKETYPE(IMI_CV_16S,1)
#define IMI_CV_16SC3 CV_MAKETYPE(IMI_CV_16S,3)

#define IMI_CV_32SC1 CV_MAKETYPE(IMI_CV_32S,1)
#define IMI_CV_32SC3 CV_MAKETYPE(IMI_CV_32S,3)

#define IMI_CV_32FC1 CV_MAKETYPE(IMI_CV_32F,1)
#define IMI_CV_32FC3 CV_MAKETYPE(IMI_CV_32F,3)

#define IMI_CV_64FC1 CV_MAKETYPE(IMI_CV_64F,1)
#define IMI_CV_64FC3 CV_MAKETYPE(IMI_CV_64F,3)


	struct imiMat
	{
		imiMat() : data(0), refcount(0), rows(0), cols(0), c(0), m_type(-1) {}
		imiMat(int _rows, int _cols, int _type/*flags*/) : data(0), refcount(0)
		{
#if 0
			create(_rows, _cols, flags);
#else
			create(_rows, _cols, _type);
#endif
		}

		// copy
		imiMat(const imiMat& m) : data(m.data), refcount(m.refcount)
		{
			if (refcount)
				IMICV_XADD(refcount, 1);
			//*refcount++;

			rows = m.rows;
			cols = m.cols;
			c = m.c;
			m_type = m.m_type;
		}

		imiMat(int _rows, int _cols, int _type/*flags*/, void* _data) : data((unsigned char*)_data), refcount(0)
		{
			rows = _rows;
			cols = _cols;
			//
			int channels = 0;
			switch (_type) {
			case IMI_CV_8UC1: case IMI_CV_16UC1: case IMI_CV_32SC1: case IMI_CV_32FC1: case IMI_CV_64FC1:
				channels = 1;
				break;
			case IMI_CV_8UC3: case IMI_CV_16UC3: case IMI_CV_32SC3: case IMI_CV_32FC3: case IMI_CV_64FC3:
				channels = 3;
				break;
			default:
				assert(false);// , "Unkonwn element type in channels()!", FA_ERROR_PARAMETER);
				break;
			}
			c = channels/*flags*/;
			m_type = _type;
		}

		~imiMat()
		{
			release();
		}

		// assign
		imiMat& operator=(const imiMat& m)
		{
			if (this == &m)
				return *this;

			if (m.refcount)
				IMICV_XADD(m.refcount, 1);
			//(*m.refcount)++;

			release();

			data = m.data;
			refcount = m.refcount;

			rows = m.rows;
			cols = m.cols;
			c = m.c;
			m_type = m.m_type;
			return *this;
		}

		void create(int _rows, int _cols, int _type/*flags*/) 
		{

		
			release();

			rows = _rows;
			cols = _cols;
			int channels = 0;
			switch (_type) {
			case IMI_CV_8UC1: case IMI_CV_16UC1: case IMI_CV_32SC1: case IMI_CV_32FC1: case IMI_CV_64FC1:
				channels = 1;
				break;
			case IMI_CV_8UC3: case IMI_CV_16UC3: case IMI_CV_32SC3: case IMI_CV_32FC3: case IMI_CV_64FC3:
				channels = 3;
				break;
			default:
				assert(false);// , "Unkonwn element type in channels()!", FA_ERROR_PARAMETER);
				break;
			}

			c = channels/*flags*/;
			m_type = _type;
			if (total() > 0)
			{
				// refcount address must be aligned, so we expand totalsize here
				size_t totalsize = (total() + 3) >> 2 << 2;
				//std::cout << "*refcount" << *refcount << std::endl;
				//std::cout << " (int)sizeof(*refcount): " << (int)sizeof(*refcount) << std::endl;
				data = (unsigned char*)malloc(totalsize + (int)sizeof(*refcount));

				refcount = (int*)(((unsigned char*)data) + totalsize);
				*refcount = 1;
			}
		}

		void release()
		{
			if (refcount && IMICV_XADD(refcount, -1) == 1)
				free(data);

			data = 0;

			rows = 0;
			cols = 0;
			c = 0;
			m_type = -1;
			refcount = 0;
		}

		bool empty() const { return data == 0 || total() == 0; }
		int channels() const { return c; }
		int type() const{ return m_type; }
		inline int getpixelsize(){

			int pixelsize = 1;
			switch (m_type) {
			case IMI_CV_8UC1:	case IMI_CV_8UC3:
				pixelsize = sizeof(uchar);
				break;
			case IMI_CV_16UC1:	case IMI_CV_16UC3:
				pixelsize = sizeof(ushort);
				break;
			case IMI_CV_32SC1:	case IMI_CV_32SC3:
				pixelsize = sizeof(int);
				break;
			case IMI_CV_32FC1:	case IMI_CV_32FC3:
				pixelsize = sizeof(float);
				break;
			case IMI_CV_64FC1:	case IMI_CV_64FC3:
				pixelsize = sizeof(double);
				break;
			default:
				assert(false);// "Unkonwn element type in getPixelSize()!", FA_ERROR_PARAMETER);
				break;
			}
			return pixelsize;
		}
		size_t total() const {
			int pixelsize = 1;
			switch (m_type) {
			case IMI_CV_8UC1:	case IMI_CV_8UC3:
				pixelsize = sizeof(uchar);
				break;
			case IMI_CV_16UC1:	case IMI_CV_16UC3:
				pixelsize = sizeof(ushort);
				break;
			case IMI_CV_32SC1:	case IMI_CV_32SC3:
				pixelsize = sizeof(int);
				break;
			case IMI_CV_32FC1:	case IMI_CV_32FC3:
				pixelsize = sizeof(float);
				break;
			case IMI_CV_64FC1:	case IMI_CV_64FC3:
				pixelsize = sizeof(double);
				break;
			default:
				assert(false);// "Unkonwn element type in getPixelSize()!", FA_ERROR_PARAMETER);
				break;
			}
			return cols * rows * c*pixelsize; }
		const unsigned char* ptr(int y) const {
			int pixelsize = 1;
			switch (m_type) {
			case IMI_CV_8UC1:	case IMI_CV_8UC3:
				pixelsize = sizeof(uchar);
				break;
			case IMI_CV_16UC1:	case IMI_CV_16UC3:
				pixelsize = sizeof(ushort);
				break;
			case IMI_CV_32SC1:	case IMI_CV_32SC3:
				pixelsize = sizeof(int);
				break;
			case IMI_CV_32FC1:	case IMI_CV_32FC3:
				pixelsize = sizeof(float);
				break;
			case IMI_CV_64FC1:	case IMI_CV_64FC3:
				pixelsize = sizeof(double);
				break;
			default:
				assert(false);// "Unkonwn element type in getPixelSize()!", FA_ERROR_PARAMETER);
				break;
			}
			return data + y * cols * c*pixelsize; }
		unsigned char* ptr(int y) {
			int pixelsize = 1;
			switch (m_type) {
			case IMI_CV_8UC1:	case IMI_CV_8UC3:
				pixelsize = sizeof(uchar);
				break;
			case IMI_CV_16UC1:	case IMI_CV_16UC3:
				pixelsize = sizeof(ushort);
				break;
			case IMI_CV_32SC1:	case IMI_CV_32SC3:
				pixelsize = sizeof(int);
				break;
			case IMI_CV_32FC1:	case IMI_CV_32FC3:
				pixelsize = sizeof(float);
				break;
			case IMI_CV_64FC1:	case IMI_CV_64FC3:
				pixelsize = sizeof(double);
				break;
			default:
				assert(false);// "Unkonwn element type in getPixelSize()!", FA_ERROR_PARAMETER);
				break;
			}
			return data + y * cols * c*pixelsize; }

		// roi
		imiMat operator()(const imiRect& roi) const
		{
			if (empty())
				return imiMat();

			imiMat m(roi.height, roi.width, this->m_type/*c*/);
			int pixelsize = 1;
			switch (m_type) {
			case IMI_CV_8UC1:	case IMI_CV_8UC3:
				pixelsize = sizeof(uchar);
				break;
			case IMI_CV_16UC1:	case IMI_CV_16UC3:
				pixelsize = sizeof(ushort);
				break;
			case IMI_CV_32SC1:	case IMI_CV_32SC3:
				pixelsize = sizeof(int);
				break;
			case IMI_CV_32FC1:	case IMI_CV_32FC3:
				pixelsize = sizeof(float);
				break;
			case IMI_CV_64FC1:	case IMI_CV_64FC3:
				pixelsize = sizeof(double);
				break;
			default:
				assert(false);// "Unkonwn element type in getPixelSize()!", FA_ERROR_PARAMETER);
				break;
			}
			int sy = roi.y;
			for (int y = 0; y < roi.height; ++y)
			{
				const unsigned char* sptr = ptr(sy) + roi.x * c*pixelsize;
				unsigned char* dptr = m.ptr(y);
				memcpy(dptr, sptr, roi.width * c*pixelsize);
				sy++;
			}
			return m;
		}


		bool operator()(imiMat* input, const imiRect& roi) const
		{
			if (empty())
				return false;

			if (input->rows != roi.height || input->cols != roi.width || this->c != input->c){
				//std::cout << input->cols << " " << roi.height << " " << input->rows << " " << roi.width << " " << this->c << " " << input->c << std::endl;
				return false;
			}
			int pixelsize = 1;
			switch (m_type) {
			case IMI_CV_8UC1:	case IMI_CV_8UC3:
				pixelsize = sizeof(uchar);
				break;
			case IMI_CV_16UC1:	case IMI_CV_16UC3:
				pixelsize = sizeof(ushort);
				break;
			case IMI_CV_32SC1:	case IMI_CV_32SC3:
				pixelsize = sizeof(int);
				break;
			case IMI_CV_32FC1:	case IMI_CV_32FC3:
				pixelsize = sizeof(float);
				break;
			case IMI_CV_64FC1:	case IMI_CV_64FC3:
				pixelsize = sizeof(double);
				break;
			default:
				assert(false);// "Unkonwn element type in getPixelSize()!", FA_ERROR_PARAMETER);
				break;
			}
			int sy = roi.y;
			for (int y = 0; y < roi.height; y++)
			{
				const unsigned char* sptr = ptr(sy) + roi.x * c*pixelsize;
				unsigned char* dptr = input->ptr(y);
				memcpy(dptr, sptr, roi.width * c*pixelsize);
				sy++;
			}
			return true;
		}
		unsigned char* data = NULL;

		// pointer to the reference counter;
		// when points to user-allocated data, the pointer is NULL
		int* refcount;
		int rows;
		int cols;
		int m_type;// new adding, Jan.23,2018
		int c;//channels
	};

#if 0
	void ParallelIMIresize_new(const imiMat* matSrc, imiMat* matDst1, const imiSize &dsize);
	//void IMIresize_new(const imiMat* matSrc, imiMat* matDst1, const imiSize &dsize);
	int borderInterpolate(int p, int len, int borderType);
#endif
#if 0
	void resize_bilinear_c1(const unsigned char* src, int srcw, int srch, unsigned char* dst, int w, int h);
	void resize_bilinear_c3(const unsigned char* src, int srcw, int srch, unsigned char* dst, int w, int h);
	void imi_resize(const imiMat& src, imiMat& dst, const imiSize& size, float sw, float sh, int flags);
#endif

#define IMI_MAX(a, b)		((a)>(b)?(a):(b))
#define IMI_MIN(a, b)		((a)<(b)?(a):(b))
#define IMI_SQR(x) ((x)*(x))
}
#endif