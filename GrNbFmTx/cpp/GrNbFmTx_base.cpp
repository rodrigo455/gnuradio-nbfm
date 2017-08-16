#include "GrNbFmTx_base.h"

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

    The following class functions are for the base class for the component class. To
    customize any of these functions, do not modify them here. Instead, overload them
    on the child class

******************************************************************************************/

GrNbFmTx_base::GrNbFmTx_base(const char *uuid, const char *label) :
    Component(uuid, label),
    ThreadedComponent()
{
    setThreadName(label);

    loadProperties();

    audio = new bulkio::InShortPort("audio");
    addPort("audio", audio);
    fm_signal = new bulkio::OutFloatPort("fm_signal");
    addPort("fm_signal", fm_signal);
}

GrNbFmTx_base::~GrNbFmTx_base()
{
    audio->_remove_ref();
    audio = 0;
    fm_signal->_remove_ref();
    fm_signal = 0;
}

/*******************************************************************************************
    Framework-level functions
    These functions are generally called by the framework to perform housekeeping.
*******************************************************************************************/
void GrNbFmTx_base::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    Component::start();
    ThreadedComponent::startThread();
}

void GrNbFmTx_base::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    Component::stop();
    if (!ThreadedComponent::stopThread()) {
        throw CF::Resource::StopError(CF::CF_NOTSET, "Processing thread did not die");
    }
}

void GrNbFmTx_base::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    // This function clears the component running condition so main shuts down everything
    try {
        stop();
    } catch (CF::Resource::StopError& ex) {
        // TODO - this should probably be logged instead of ignored
    }

    Component::releaseObject();
}

void GrNbFmTx_base::loadProperties()
{
    addProperty(audio_rate,
                8000,
                "audio_rate",
                "",
                "readwrite",
                "Hz",
                "external",
                "property");

    addProperty(quad_rate,
                8000,
                "quad_rate",
                "",
                "readwrite",
                "Hz",
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

    addProperty(preemphasis,
                false,
                "preemphasis",
                "",
                "readwrite",
                "",
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

    addProperty(fh,
                -1.0,
                "fh",
                "",
                "readwrite",
                "Hz",
                "external",
                "property");

    addProperty(stream_id,
                "fm_signal",
                "stream_id",
                "",
                "readwrite",
                "",
                "external",
                "property");

}


