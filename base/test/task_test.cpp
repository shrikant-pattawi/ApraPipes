#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>


#include "ExternalSourceModule.h"
#include "ExternalSinkModule.h"
#include "FileReaderModule.h"
#include "FrameMetadata.h"
#include "Frame.h"
#include "Logger.h"
#include "AIPExceptions.h"
#include "FramesMuxer.h"
#include "test_utils.h"
#include "PipeLine.h"
#include "StatSink.h"
#include "AIPExceptions.h"
#include "RotateCV.h"

BOOST_AUTO_TEST_SUITE(task_test)


void test(std::string filename, int width, int height, ImageMetadata::ImageType imageType, int type, int depth, double angle)
{

	auto fileReader = boost::shared_ptr<FileReaderModule>(new FileReaderModule(FileReaderModuleProps("./data/" + filename + ".raw")));
	auto metadata = framemetadata_sp(new RawImageMetadata(width, height, ImageMetadata::RGB, CV_8UC3, width * 3, CV_8U, FrameMetadata::HOST));
	auto rawImagePin = fileReader->addOutputPin(metadata);

	auto m1 = boost::shared_ptr<Module>(new RotateCV(RotateCVProps(angle)));
	fileReader->setNext(m1);

	auto outputPinId = m1->getAllOutputPinsByType(FrameMetadata::RAW_IMAGE)[0];

	auto sink = boost::shared_ptr<ExternalSinkModule>(new ExternalSinkModule());
	m1->setNext(sink);

	BOOST_TEST(fileReader->init());
	BOOST_TEST(m1->init());
	BOOST_TEST(sink->init());

	fileReader->step();
	m1->step();
	auto frames = sink->pop();
	BOOST_TEST((frames.find(outputPinId) != frames.end()));
	auto outFrame = frames[outputPinId];
	BOOST_TEST(outFrame->getMetadata()->getFrameType() == FrameMetadata::RAW_IMAGE);

	auto outFilename = "./data/testOutput/rotatecv_tests_" + filename + "_" + std::to_string(angle) + ".raw";
	Test_Utils::saveOrCompare(outFilename.c_str(), (const uint8_t *)outFrame->data(), outFrame->size(), 0);
}


BOOST_AUTO_TEST_CASE(task1)
{
	test("frame_1280x720_rgb", 1280, 720, ImageMetadata::ImageType::RGB, CV_8UC3, CV_8U, 90);
}
// BOOST_AUTO_TEST_CASE(task1)
// {
// 	auto fileReader = boost::shared_ptr<FileReaderModule>(new FileReaderModule(FileReaderModuleProps("./data/filenamestrategydata/?.txt")));
// 	auto metadata = framemetadata_sp(new FrameMetadata(FrameMetadata::GENERAL));
// 	auto pinId = fileReader->addOutputPin(metadata);
    
// 	bool relay = false;
// 	auto sink = boost::shared_ptr<Module>(new StatSink());	
// 	fileReader->setNext(sink, relay);
	
// 	PipeLine p("test");
// 	p.appendModule(fileReader);
// 	p.init();
// 	p.run_all_threaded();

// 	for (auto i = 0; i < 10; i++)
// 	{
// 		boost::this_thread::sleep_for(boost::chrono::milliseconds(100));  // giving time to call step 
// 		relay = !relay;
// 		fileReader->relay(sink, relay);
// 	}

// 	p.stop();
// 	p.term();
// 	p.wait_for_all();

// }

void testFrames(frame_container& frames, size_t fIndex, size_t size)
{
	BOOST_TEST(frames.size() == size);
	for (auto it = frames.cbegin(); it != frames.cend(); it++)
	{
		BOOST_TEST(fIndex == it->second->fIndex);
	}
}

BOOST_AUTO_TEST_CASE(task34)
{
	size_t readDataSize = 1024;

	auto metadata = framemetadata_sp(new FrameMetadata(FrameMetadata::ENCODED_IMAGE));

	auto m1 = boost::shared_ptr<ExternalSourceModule>(new ExternalSourceModule());
	auto pin1_1 = m1->addOutputPin(metadata);
	auto pin1_2 = m1->addOutputPin(metadata);

	auto m2 = boost::shared_ptr<ExternalSourceModule>(new ExternalSourceModule());
	auto pin2_1 = m2->addOutputPin(metadata);

	auto m3 = boost::shared_ptr<ExternalSourceModule>(new ExternalSourceModule());
	auto pin3_1 = m3->addOutputPin(metadata);

	auto muxer = boost::shared_ptr<Module>(new FramesMuxer());
	m1->setNext(muxer);
	m2->setNext(muxer);
	m3->setNext(muxer);

	auto sink = boost::shared_ptr<ExternalSinkModule>(new ExternalSinkModule());
	muxer->setNext(sink);

	BOOST_TEST(m1->init());
	BOOST_TEST(m2->init());
	BOOST_TEST(m3->init());

	BOOST_TEST(muxer->init());
	BOOST_TEST(sink->init());

	{

		{
			// basic

			auto encodedImageFrame = m1->makeFrame(readDataSize, pin1_1);
			encodedImageFrame->fIndex = 500;

			frame_container frames;
			frames.insert(make_pair(pin1_1, encodedImageFrame));
			frames.insert(make_pair(pin1_2, encodedImageFrame));
			frames.insert(make_pair(pin2_1, encodedImageFrame));
			frames.insert(make_pair(pin3_1, encodedImageFrame));

			m1->send(frames);
			muxer->step();
			BOOST_TEST(sink->try_pop().size() == 0);

			m2->send(frames);
			muxer->step();
			BOOST_TEST(sink->try_pop().size() == 0);

			m3->send(frames);
			muxer->step();
			auto outFrames = sink->try_pop();
			testFrames(outFrames, 500, 4);
		}
		{

			auto encodedImageFrame = m1->makeFrame(readDataSize, pin1_1);
			encodedImageFrame->fIndex = 600;

			frame_container frames;

			frames.insert(make_pair(pin2_1, encodedImageFrame));
			frames.insert(make_pair(pin3_1, encodedImageFrame));

			m2->send(frames);
			muxer->step();
			BOOST_TEST(sink->try_pop().size() == 0);

			m3->send(frames);
			muxer->step();
			BOOST_TEST(sink->try_pop().size() == 0);

			auto encodedImageFrame2 = m1->makeFrame(readDataSize, pin1_2);
			encodedImageFrame2->fIndex = 600;

			frame_container frames2;
			frames2.insert(make_pair(pin1_1, encodedImageFrame2));
			frames2.insert(make_pair(pin1_2, encodedImageFrame2));
			m1->send(frames2);
			muxer->step();
			auto outFrames = sink->try_pop();
			testFrames(outFrames, 600, 4);
		}
	}
	BOOST_TEST(m1->term());
	BOOST_TEST(m2->term());
	BOOST_TEST(m3->term());
	BOOST_TEST(muxer->term());
	BOOST_TEST(sink->term());

	m1.reset();
	m2.reset();
	m3.reset();
	muxer.reset();
	sink.reset();
}

BOOST_AUTO_TEST_SUITE_END()