#ifndef GRNBFMTX_BASE_IMPL_BASE_H
#define GRNBFMTX_BASE_IMPL_BASE_H

#include <boost/thread.hpp>
#include <ossie/Component.h>
#include <ossie/ThreadedComponent.h>

#include <bulkio/bulkio.h>

class GrNbFmTx_base : public Component, protected ThreadedComponent
{
    public:
        GrNbFmTx_base(const char *uuid, const char *label);
        ~GrNbFmTx_base();

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
        /// Property: fh
        float fh;
        /// Property: stream_id
        std::string stream_id;

        // Ports
        /// Port: audio
        bulkio::InFloatPort *audio;
        /// Port: fm_signal
        bulkio::OutFloatPort *fm_signal;

    private:
};
#endif // GRNBFMTX_BASE_IMPL_BASE_H
