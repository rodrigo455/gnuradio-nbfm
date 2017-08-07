#ifndef GRNBFMRX_I_IMPL_H
#define GRNBFMRX_I_IMPL_H

#undef DEBUG //required to include gnuradio headers

#include <gnuradio/analog/quadrature_demod_cf.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/filter/iir_filter_ffd.h>
#include <gnuradio/filter/fir_filter_fff.h>

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

	void validate(CF::Properties property, CF::Properties& validProps,
			CF::Properties& invalidProps);
	void configure(const CF::Properties& configProperties)
			throw (CF::PropertySet::PartialConfiguration,
			CF::PropertySet::InvalidConfiguration, CORBA::SystemException);

	int serviceFunction();

private:
	bool sri_changed;
	int decim_factor;
	int buffersz;

	gr_vector_const_void_star gr_input;
	gr_vector_void_star gr_output;

	gr::analog::quadrature_demod_cf::sptr quad_demod;
	gr::filter::iir_filter_ffd::sptr deemph;
	gr::filter::fir_filter_fff::sptr audio_filter;

	std::vector<gr_complex> complex_in;
	// gr blocks outputs
	std::vector<float> quad_out;
	std::vector<float> deemph_out;

	// redhawk output buffer
	std::vector<float> output_buffer;

	void float2gr_complex(float* input, gr_complex* output, int n);
};

#endif // GRNBFMRX_I_IMPL_H
