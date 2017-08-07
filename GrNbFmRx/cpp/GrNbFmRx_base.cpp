#include "GrNbFmRx_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/

GrNbFmRx_base::GrNbFmRx_base(const char *uuid, const char *label) :
    Component(uuid, label),
    ThreadedComponent()
{
    setThreadName(label);

    loadProperties();

    fm_signal = new bulkio::InFloatPort("fm_signal");
    addPort("fm_signal", fm_signal);
    audio = new bulkio::OutFloatPort("audio");
    addPort("audio", audio);
}

GrNbFmRx_base::~GrNbFmRx_base()
{
    fm_signal->_remove_ref();
    fm_signal = 0;
    audio->_remove_ref();
    audio = 0;
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void GrNbFmRx_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    Component::start();
    ThreadedComponent::startThread();
}

void GrNbFmRx_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    Component::stop();
    if (!ThreadedComponent::stopThread()) {
        throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
    }
}

void GrNbFmRx_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    // This function clears the component running condition so main shuts down everything
    try {
        stop();
    } catch (CF::Resource::StopError& ex) {
        // TODO - this should probably be logged instead of ignored
    }

    Component::releaseObject();
}

void GrNbFmRx_base::loadProperties()
{
    addProperty(audio_rate,
                16000,
                "audio_rate",
                "",
                "readwrite",
                "Hz",
                "external",
                "property");

    addProperty(quad_rate,
                16000,
                "quad_rate",
                "",
                "readwrite",
                "Hz",
                "external",
                "property");

    addProperty(tau,
                75e-6,
                "tau",
                "",
                "readwrite",
                "",
                "external",
                "property");

    addProperty(max_dev,
                5e3,
                "max_dev",
                "",
                "readwrite",
                "Hz",
                "external",
                "property");

    addProperty(stream_id,
                "fm_audio",
                "stream_id",
                "",
                "readwrite",
                "",
                "external",
                "property");

    addProperty(deemphasis_enable,
                false,
                "deemphasis_enable",
                "",
                "readwrite",
                "",
                "external",
                "property");

    addProperty(buffer_size,
                1024,
                "buffer_size",
                "",
                "readwrite",
                "",
                "external",
                "property");

    addProperty(audio_gain,
                10.0,
                "audio_gain",
                "",
                "readwrite",
                "",
                "external",
                "property");

}

