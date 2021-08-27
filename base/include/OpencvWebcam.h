#pragma once
#include "Module.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <memory>
#include "Logger.h"
class OpencvWebcamProps : public ModuleProps
{
    public:
        OpencvWebcamProps(uint32_t _width,uint32_t _height): ModuleProps(), width(_width), height(_height)
        {
        }
    
    uint32_t width;
    uint32_t height;
};

class OpencvWebcam : public Module
{
    public:
        OpencvWebcam(OpencvWebcamProps props);
        virtual ~OpencvWebcam();
        bool init();
        bool term();
        bool produce();
        bool validateOutputPins();
        // void setMetadata(framemetadata_sp &metadata);
        // bool processSOS(frame_sp &frame);
    
    private:
        class Detail;
        std::string mOutputPinId;
        std::shared_ptr<Detail> mDetail;
        OpencvWebcamProps mProps;
};