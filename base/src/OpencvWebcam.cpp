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

	bool initCamera(int width, int height)
	{
		try
		{
            vid_capture.set(cv::CAP_PROP_FRAME_WIDTH, width);//Setting the width of the video
            vid_capture.set(cv::CAP_PROP_FRAME_HEIGHT, height);//Setting the height of the video
		}
		catch (std::exception &e)
		{
			LOG_ERROR << "An exception occurred in InitCamera(): " << endl
					  << e.what() << endl;
			return false;
		}
		return true;
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

	bool retrieveImage()
	{
		try
		{
			while(vid_capture.isOpened())
            {
                cv::Mat img;
                bool isSuccess=vid_capture.read(img);
                if(isSuccess==false)
                {
                    cout << "Stream disconnected" << endl;
                    break;
                }
                if(isSuccess==true)
                {
					size_t t = static_cast<size_t>(img.total() * img.elemSize());
					auto frame = mMakeFrame(t);
					memcpy(frame->data(), &img.data[0], frame->size());
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
};

OpencvWebcam::OpencvWebcam(OpencvWebcamProps _props)
	: Module(SOURCE, "OpencvWebcam", _props), mProps(_props)
{
	auto outputMetadata = framemetadata_sp(new RawImageMetadata(_props.width, _props.height, ImageMetadata::BGR, CV_8UC3, size_t(0), CV_8U, FrameMetadata::MemType::HOST, true));
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
	return mDetail->initCamera(mProps.width, mProps.height);
}

bool OpencvWebcam::term()
{
	auto ret = mDetail->stopCamera();
	auto moduleRet = Module::term();

	return ret && moduleRet;
}

bool OpencvWebcam::produce()
{
	mDetail->retrieveImage();
	return true;
}