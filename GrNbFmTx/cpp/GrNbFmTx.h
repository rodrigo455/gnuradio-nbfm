#ifndef GRNBFMTX_I_IMPL_H
#define GRNBFMTX_I_IMPL_H

#undef DEBUG //required to include gnuradio headers

#include <gnuradio/blocks/short_to_float.h>
#include <gnuradio/analog/frequency_modulator_fc.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/filter/interp_fir_filter_fff.h>
#include <gnuradio/filter/iir_filter_ffd.h>

#include "GrNbFmTx_base.h"

class GrNbFmTx_i : public GrNbFmTx_base
{
    ENABLE_LOGGING
public:
	GrNbFmTx_i(const char *uuid, const char *label);
	~GrNbFmTx_i();

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
	bool do_interp;
	bool sri_changed;
	int interp_factor;
	int buffersz;

	// work function interface
	gr_vector_const_void_star gr_input;
	gr_vector_void_star gr_output;
	// gr blocks shared pointers
	gr::blocks::short_to_float::sptr to_float;
	gr::filter::interp_fir_filter_fff::sptr interpolator;
	gr::filter::iir_filter_ffd::sptr preemph;
	gr::analog::frequency_modulator_fc::sptr modulator;
	// gr blocks outputs
	std::vector<float> float_out;
	std::vector<float> interp_out;
	std::vector<float> preemph_out;
	std::vector<gr_complex> mod_out;
	// redhawk output buffer
	std::vector<float> output_buffer;

	void gr_complex2float(gr_complex* input, float* output, int n);
};

#endif // GRNBFMTX_I_IMPL_H
