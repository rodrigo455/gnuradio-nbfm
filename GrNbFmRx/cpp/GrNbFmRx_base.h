#ifndef GRNBFMRX_BASE_IMPL_BASE_H
#define GRNBFMRX_BASE_IMPL_BASE_H

#include <boost/thread.hpp>
#include <ossie/Component.h>
#include <ossie/ThreadedComponent.h>

#include <bulkio/bulkio.h>

class GrNbFmRx_base : public Component, protected ThreadedComponent
{
    public:
        GrNbFmRx_base(const char *uuid, const char *label);
        ~GrNbFmRx_base();

        void start() throw (CF::Resource::StartError, CORBA::SystemException);

        void stop() throw (CF::Resource::StopError, CORBA::SystemException);

        void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

        void loadProperties();

    protected:
        // Member variables exposed as properties
        /// Property: audio_rate
        CORBA::Long audio_rate;
        /// Property: quad_rate
        CORBA::Long quad_rate;
        /// Property: tau
        float tau;
        /// Property: max_dev
        float max_dev;
        /// Property: stream_id
        std::string stream_id;
        /// Property: deemphasis_enable
        bool deemphasis_enable;
        /// Property: buffer_size
        CORBA::Long buffer_size;
        /// Property: audio_gain
        float audio_gain;

        // Ports
        /// Port: fm_signal
        bulkio::InFloatPort *fm_signal;
        /// Port: audio
        bulkio::OutFloatPort *audio;

    private:
};
#endif // GRNBFMRX_BASE_IMPL_BASE_H
