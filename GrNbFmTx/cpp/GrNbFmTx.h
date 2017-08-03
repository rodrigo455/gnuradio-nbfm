#ifndef GRNBFMTX_I_IMPL_H
#define GRNBFMTX_I_IMPL_H

#undef DEBUG

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

        int serviceFunction();

    private:
        bool do_interp;
        bool sri_changed;
		int interp_factor;

        gr_vector_const_void_star gr_input;
		gr_vector_void_star gr_output;
		gr::filter::interp_fir_filter_fff::sptr interpolator;
		gr::filter::iir_filter_ffd::sptr preemph;
		gr::analog::frequency_modulator_fc::sptr modulator;
};

#endif // GRNBFMTX_I_IMPL_H
