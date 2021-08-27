#include "Gstreamer.h"
#include "Logger.h"

class Gstreamer::Detail
{
    public:
    Detail(uint32_t _width,uint32_t _height) : width(_width) ,height(_height)
    {

    }
    ~Detail()
    {

    }
    bool init(uint32_t _height,uint32_t _width)
    {

    }
    bool shouldTriggerSOS()
	{
		
	}
    uint32_t width,height;
    GstElement* m_appsrc;
	GstElement* m_sourceBin;
	GstBuffer* m_gstBuffer;
};

Gstreamer:: Gstreamer(GstreamerProps props) : Module(SINK, "Gstreamer", props)
{
    
}

Gstreamer:: ~Gstreamer()
{
}

bool Gstreamer:: init()
{
    m_appsrc =gst_element_factory_make("appsrc","source");
    g_object_set(G_OBJECT(m_appsrc),
    "stream-type",0,
    "format",GST_FORMAT_TIME,
    "is-live",TRUE,
    "num-buffers",m_numFramesToGrab,
    NULL);    
    g_object_set(G_OBJECT(m_appsrc), "caps",
			gst_caps_new_simple("video/x-raw",
			"format", G_TYPE_STRING, format.c_str(),
			"width", G_TYPE_INT, 600, 
			"height", G_TYPE_INT, 600,
			"framerate", GST_TYPE_FRACTION, 20, 1, NULL), NULL);
    g_signal_connect(m_appsrc, "need-data", G_CALLBACK(cb_need_data), this);
}

bool Gstreamer:: process(frame_container& frames)
{

}

bool Gstreamer:: validateInputPins()
{

}

bool Gstreamer:: term()
{

}

bool Gstreamer:: processSOS(frame_sp& frame)
{

}

bool Gstreamer:: shouldTriggerSOS()
{
    return mDetail->shouldTriggerSOS();
}