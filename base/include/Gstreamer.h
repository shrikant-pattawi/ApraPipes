#prgama once

#include"Module.h"
#include<gst/gst.h>

class GstreamerProps : public ModuleProps
{
    public:
	GstreamerProps( uint32_t _width, uint32_t _height) : ModuleProps()
	{
		height = _height;
		width = _width;
	}
    uint32_t width;
    uint32_t height;
}

class Gstreamer : public Module
{
    public:
        Gstreamer(GstreamerProps props);
        ~Gstreamer();

        bool init();
        bool term();
    protected:
        bool process(frame_container& frames);
        bool processSOS(frame_sp& frame);
        bool validateInputPins();
        bool shouldTriggersSOS();
    private:
        class Detail;
        boost::shared_ptr<Detail> mdetail;
}