#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "FileReaderModule.h"
#include "FileWriterModule.h"
#include
BOOST_AUTO_TEST_SUITE(task_test)

BOOST_AUTO_TEST_CASE(check1)
{
    Logger::getLogger()->setLogLevel(boost::log::trivial::severity_level::info);
	FileReaderModuleProps props("./data/1280x960.jpg");
	auto fileReader = boost::shared_ptr<FileReaderModule>(new FileReaderModule(props));
	
}
BOOST_AUTO_TEST_SUITE_END()