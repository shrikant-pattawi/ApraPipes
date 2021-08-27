#include "OpencvWebcam.h"
#include "FrameMetadata.h"

class OpencvWebcam::Detail
{
 public:
	cv::VideoCapture vid_capture;
	typedef std::function<void(frame_sp &)> SendFrame;
	Detail(SendFrame sendFrame, std::function<frame_sp(size_t)> _makeFrame) : mMakeFrame(_makeFrame), mSendFrame(sendFrame)
	{
		try
		{
			    int deviceID = 0;             // 0 = open default camera
    			int apiID = cv::CAP_ANY;
				vid_capture.open(deviceID+apiID);

		}
		catch (std::exception &e)
		{
			LOG_ERROR << "An exception occurred in CameraHelper(): " << endl
					  << e.what() << endl;
		}
	}

	~Detail()
	{
		try
		{
			
		}
		catch (std::exception &e)
		{
			LOG_ERROR << "An exception occurred in CloseCamera(): " << endl
					  << e.what() << endl;
		}
	}
	bool stopCamera()
	{
		try
		{
            vid_capture.release();
			LOG_INFO << "Stopping Camera image acquistion and image grabbing..." << endl;
		}
		catch (std::exception &e)
		{
			LOG_ERROR << "An exception occurred in StopCamera(): " << endl
					  << e.what() << endl;
			return false;
		}
		return true;
	}

	bool retrieveImage(int width,int height)
	{
		try
		{
			if(vid_capture.isOpened())
            {
				vid_capture >> img;
                {
					resize(img, resized_down, cv::Size(width, height), cv::INTER_LINEAR);
					size_t t = static_cast<size_t>(resized_down.total() * resized_down.elemSize());
					auto frame = mMakeFrame(t);
					memcpy(frame->data(), &resized_down.data[0], frame->size());
                    mSendFrame(frame);
                }
			}
		}
		catch (std::exception &e)
		{
			LOG_ERROR << "An exception occurred in retrieve_image(): " << endl
					  << e.what() << endl;
			return false;
		}
		return true;
	}

private:
	std::function<frame_sp(size_t)> mMakeFrame;
	SendFrame mSendFrame;
	cv::Mat img;
	cv::Mat resized_down;
};

OpencvWebcam::OpencvWebcam(OpencvWebcamProps _props): Module(SOURCE, "OpencvWebcam", _props), mProps(_props)
{
	auto outputMetadata = framemetadata_sp(new RawImageMetadata(mProps.width, mProps.height, ImageMetadata::BGR, CV_8UC3, size_t(0), CV_8U, FrameMetadata::MemType::HOST, true));
	std::string mOutputPinId = addOutputPin(outputMetadata);
	mDetail.reset(new Detail([&, mOutputPinId](frame_sp &frame) -> void
							 {
								 frame_container frames;
								 frames.insert(make_pair(mOutputPinId, frame));
								 send(frames);
							 },
							 [&](size_t size) -> frame_sp
							 {
								 return makeFrame(size);
							 }));
}

OpencvWebcam::~OpencvWebcam() {}

bool OpencvWebcam::validateOutputPins()
{
	if (getNumberOfOutputPins() != 1)
	{
		return false;
	}

	return true;
}

bool OpencvWebcam::init()
{
	if (!Module::init())
	{
		return false;
	}
	return true;
}

bool OpencvWebcam::term()
{
	auto ret = mDetail->stopCamera();
	auto moduleRet = Module::term();

	return ret && moduleRet;
}

bool OpencvWebcam::produce()
{
	mDetail->retrieveImage(mProps.width, mProps.height);
	return true;
}