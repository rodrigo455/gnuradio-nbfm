#ifndef GRNBFMRX_I_IMPL_H
#define GRNBFMRX_I_IMPL_H

#undef DEBUG //required to include gnuradio headers

#include <gnuradio/top_block.h>
#include <gnuradio/blocks/float_to_short.h>
#include <gnuradio/analog/quadrature_demod_cf.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/filter/iir_filter_ffd.h>
#include <gnuradio/filter/fir_filter_fff.h>

#include "rh_source_bulkio.h"
#include "rh_sink_bulkio.h"

#include "GrNbFmRx_base.h"

class GrNbFmRx_i : public GrNbFmRx_base
{
	ENABLE_LOGGING
public:
	GrNbFmRx_i(const char *uuid, const char *label);
	~GrNbFmRx_i();

	void constructor();
	void start() throw (CF::Resource::StartError, CORBA::SystemException);
	void stop() throw (CF::Resource::StopError, CORBA::SystemException);

	int serviceFunction();

private:
	int decim_factor;

	gr::analog::quadrature_demod_cf::sptr quad_demod;
	gr::filter::iir_filter_ffd::sptr deemph;
	gr::filter::fir_filter_fff::sptr audio_filter;
	gr::blocks::float_to_short::sptr to_short;

	RH_floatSource* float_in;
	RH_shortSink* short_out;

    gr::top_block_sptr top_block;

    void startFlowgraph();
    void stopFlowgraph();

};

#endif // GRNBFMRX_I_IMPL_H
