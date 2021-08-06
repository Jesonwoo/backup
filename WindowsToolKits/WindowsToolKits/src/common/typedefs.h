/*
 * Filename:  typedefs.h
 * Project :  LMPCore
 * Created by Jeson on 4/15/2019.
 * Copyright © 2019 Guangzhou AiYunJi Inc. All rights reserved.
*/
#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <stdio.h>            //define NULL.
#include <vector>
#include <map>
#include <list>
#include <olectl.h>
#include <ddraw.h>
#include <mmsystem.h>

#ifdef	_MSC_VER
// disable some level-4 warnings, use #pragma warning(enable:###) to re-enable
#pragma warning(disable:4100) // warning C4100: unreferenced formal parameter
#pragma warning(disable:4201) // warning C4201: nonstandard extension used : nameless struct/union
#pragma warning(disable:4511) // warning C4511: copy constructor could not be generated
#pragma warning(disable:4512) // warning C4512: assignment operator could not be generated
#pragma warning(disable:4514) // warning C4514: "unreferenced inline function has been removed"
#pragma warning(disable:4996)
#pragma warning(disable:4068)
#pragma warning(disable:4146)
#pragma warning(disable:4819)
#pragma warning(disable:4251)
#pragma warning(disable:4172) // warning C4514: returns the address of a local variable or temporary

#if _MSC_VER>=1900
#define AM_NOVTABLE __declspec(novtable)

#define PI32        "zd"
#define PI64        "lld"
#define PU64        "llu"
#define snprintf    snprintf
#else

#define AM_NOVTABLE
#define PI32        "d"
#define PI64        "I64d"
#define PU64        "I64u"
#define snprintf    _snprintf
#endif
#endif	// MSC_VER

#include <strmif.h>     // Generated IDL header file for streams interfaces
#include <intsafe.h>    // required by amvideo.h
#include <amvideo.h>    // ActiveMovie video interfaces and definitions
#include <comlite.h>    // Light weight com function prototypes
#include <control.h>    // generated from control.odl
#include <edevdefs.h>   // External device control interface defines
#include <audevcod.h>   // audio filter device error event codes


typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed __int64      int64_t;
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned __int64    uint64_t;

#define CORE_EXPORT          __declspec(dllexport)
#if _DEBUG
#define CORE_DEBUG_EXPORT    __declspec(dllexport)
#else
#define CORE_DEBUG_EXPORT    
#endif

#define SAFE_DELETE(ptr)        \
    do {                        \
        if (ptr != NULL) {      \
            delete ptr;         \
            ptr = NULL;         \
        }                       \
    } while (0)

#define SAFE_FREE(ptr)          \
    do {                        \
        if (ptr != NULL) {      \
            free(ptr);          \
            ptr = NULL;         \
        }                       \
    } while (0)

#define SAFE_RELEASE(ptr)       \
    do {                        \
        if (ptr != NULL) {		\
            ptr->Release();     \
        }                       \
    } while(0)

#define SAFE_STOP(ptr)          \
    do {                        \
        if (ptr != NULL) {		\
            ptr->Stop();        \
        }                       \
    } while(0)

#define SAFE_CLOSE(ptr)         \
    do {                        \
        if (ptr != NULL) {		\
            ptr->Close();       \
        }                       \
    } while(0)

#define SAFE_CLOSE_FILE(file)   \
    do {                        \
        if (file != NULL) {		\
            fclose(file);       \
            file = NULL;        \
        }                       \
    } while(0)


// The state of the algorithm's results
enum ResultStatus
{
    kResultNone,
    kResultNormal,
    kResultSuspected,
    kResultAbnormal
};

/*
 * I420: YYYYYYYY UU VV    =>YUV420P
 * YV12: YYYYYYYY VV UU    =>YUV420P
 * NV12: YYYYYYYY UVUV     =>YUV420SP
 * NV21: YYYYYYYY VUVU     =>YUV420SP
 * YUYV/YUY2: YU YV YU YV  =>YUV422
 */

enum ImageFormat
{
    kImageNone = -1,
    kImageI420,
    kImageYUY2,
    kImageYV12,
    kImageNV21,
    kImageNV12,
    kImageYVYU,
    kImageYUYV,
    kImageUYVY,

    kImageRGB32,
    kImageRGB24,
    kImageRGB565,
    kImageRGB555,
    kImageMJPG
};

struct CORE_EXPORT CoordinatePoint
{
    int32_t x = 0;
    int32_t y = 0;
    CoordinatePoint & operator=(const CoordinatePoint & other)
    {
        if (this == &other) {
            return *this;
        }
        x = other.x;
        y = other.y;
        return *this;
    }
};

struct CORE_EXPORT FeatureRect
{
    int32_t w = 0;
    int32_t h = 0;
    CoordinatePoint lefttop;
};

// Measurement results of the model
struct CORE_EXPORT MeasurementResult
{
    uint32_t id = 0;
    enum MeasureType
    {
        kLine = 1,
        kEllipse,
        kLineEllipse,
    } type;
    FeatureRect rect;
    struct MeasurementLine
    {
        bool drawLine;
        double value;
        std::string label;
        CoordinatePoint point1;
        CoordinatePoint point2;
    }line;
    struct MeasurementEllipse
    {
        double angle;
        double value;
        std::string label;
        double longAxis;
        double minorAxis;
        CoordinatePoint center;
    }ellipse;
};

// The information contained in the results of the algorithm
struct CORE_EXPORT DetectionResult
{
    bool            plane = false;       // Identifies whether the current feature is plane
    uint32_t        id = 0;              // feature of the id
    float           weight = 0.0;        // Indicates the severity of the result, max is 1.0
    int             planeScore = -1;     // Plane score
    ResultStatus    status = kResultNone;// There can be three types of results
    std::string     name;                // Result's name, maybe Chinese
    std::vector<CoordinatePoint> * areas;
    void InitRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
    {
        areas->at(0).x = x;
        areas->at(0).y = y;
        areas->at(1).x = x + w;
        areas->at(1).y = y;
        areas->at(2).x = x + w;
        areas->at(2).y = y + h;
        areas->at(3).x = x;
        areas->at(3).y = y + h;
    }
    DetectionResult(void)
    {
        areas = new std::vector<CoordinatePoint>(4); // The default is a rectangular region
    }
    ~DetectionResult(void)
    {
        SAFE_DELETE(areas);
    }
    DetectionResult(const DetectionResult& other)
    {
        this->id = other.id;
        this->plane = other.plane;
        this->weight = other.weight;
        this->status = other.status;
        this->name = other.name;
        this->planeScore = other.planeScore;
        this->areas = new std::vector<CoordinatePoint>(other.areas->size());
        *(this->areas) = *(other.areas);
    }
    DetectionResult & operator=(const DetectionResult & other)
    {
        if (this == &other) {
            return *this;
        }
        this->id = other.id;
        this->plane = other.plane;
        this->weight = other.weight;
        this->status = other.status;
        this->name = other.name;
        this->planeScore = other.planeScore;
        *(this->areas) = *(other.areas);

        return *this;
    }
};

struct CORE_EXPORT DetectedFrame
{
    bool          isDetected = false;      // Indicates whether the frame has been detected
    ImageFormat   format = kImageNone;     // Format of frame data 
    uint32_t      size = 0;                // Size of frame data
    uint32_t      width = 1600;
    uint32_t      height = 900;
    uint64_t      seq = 0;
    uint64_t      pts = 0;
    uint8_t *     data = nullptr;
    std::vector<DetectionResult> * detectedInfos = nullptr; // AI model detection results
    std::vector<DetectionResult> * renderInfos = nullptr;   // Preview the box you need to draw
    std::vector<MeasurementResult> * measurement = nullptr;
    std::string   filename;

    DetectedFrame(void)
    {
        renderInfos = new std::vector<DetectionResult>();
        detectedInfos = new std::vector<DetectionResult>();
        measurement = new std::vector<MeasurementResult>();
    }
    ~DetectedFrame(void)
    {
        format = kImageNone;
        size = 0;
        width = 0;
        height = 0;
        pts = 0;
        seq = 0;
        isDetected = false;
        filename.clear();
        SAFE_DELETE(data);
        SAFE_DELETE(renderInfos);
        SAFE_DELETE(detectedInfos);
        SAFE_DELETE(measurement);
    }
    DetectedFrame(const DetectedFrame & other)
    {
        if (other.size > 0) {
            data = new uint8_t[other.size];
        }
        memcpy(data, other.data, other.size);

        isDetected = other.isDetected;
        format = other.format;
        width = other.width;
        height = other.height;
        pts = other.pts;
        seq = other.seq;
        size = other.size;
        filename = other.filename;
        *renderInfos = *other.renderInfos;
        *detectedInfos = *other.detectedInfos;
        this->measurement = new std::vector<MeasurementResult>();
        *(this->measurement) = *(other.measurement);
    }
    DetectedFrame(ImageFormat fmt, uint32_t w, uint32_t h)
    {
        isDetected = false;
        data = NULL;
        pts = 0;
        seq = 0;
        SetFrameSize(fmt, w, h);

        renderInfos = new std::vector<DetectionResult>();
        detectedInfos = new std::vector<DetectionResult>();
        measurement = new std::vector<MeasurementResult>();
    }
    DetectedFrame & operator=(const DetectedFrame& other)
    {
        if (this == &other) {
            return *this;
        }
        if (size != other.size) {
            SAFE_DELETE(data);
            data = new uint8_t[other.size];
        }

        memcpy(data, other.data, other.size);

        isDetected = other.isDetected;
        format = other.format;
        width = other.width;
        height = other.height;
        pts = other.pts;
        seq = other.seq;
        size = other.size;
        filename = other.filename;
        renderInfos->clear();
        detectedInfos->clear();
        measurement->clear();
        *renderInfos = *other.renderInfos;
        *detectedInfos = *other.detectedInfos;
        *measurement = *other.measurement;

        return *this;
    }
    int SetFrameSize(ImageFormat fmt, uint32_t w, uint32_t h)
    {
        if (fmt == format && w == width && h == height) {
            return 0;
        }
        if (size != 0) {
            SAFE_DELETE(data);
        }
        int frameSize = 0;
        switch (fmt) {
        case kImageI420:
            frameSize = w * h * 3 >> 1;
            break;
        case kImageYUY2:
        case kImageYUYV:
            frameSize = w * h * 2;
            break;
        case kImageRGB24:
            frameSize = w * h * 3;
            break;
        }
        if (frameSize == 0) {
            return -1;
        }

        data = new uint8_t[frameSize];
        size = frameSize;
        format = fmt;
        width = w;
        height = h;
        return 0;
    }
};

enum FrameType
{
    kUnkownFrame = -1,
    kIFrame,
    kPFrame,
    kBFrame
};

struct H264Packet
{
    FrameType type = kUnkownFrame;
    size_t    size = 0;
    uint64_t  pts = 0;
    uint8_t * data = nullptr;

    H264Packet(void)
        : type(kUnkownFrame)
        , size(0)
        , pts(0)
        , data(NULL){}
    ~H264Packet(void)
    {
        SAFE_DELETE(data);
    }
    int SetPacket(uint8_t * data, size_t size, FrameType type, uint64_t pts)
    {
        if (size != size) {
            SAFE_DELETE(data);
            size = size;
            data = new uint8_t[size];
        }

        memcpy(data, data, size);

        type = type;
        pts = pts;
        return 0;
    }
    H264Packet & operator=(const H264Packet & other)
    {
        if (&other == this) {
            return *this;
        }
        if (size != other.size) {
            SAFE_DELETE(data);
            size = other.size;
            data = new uint8_t[other.size];
        }
        type = other.type;
        pts = other.pts;
        memcpy(data, other.data, other.size);
        return *this;
    }
};

#endif // __TYPEDEF_H__
