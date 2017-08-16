/*
 * Author: Rodrigo Rolim Mendes de Alencar <alencar.fmce@imbel.gov.br>
 *
 * Copyright 2017 IMBEL/FMCE.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef GRNBFMTX_I_IMPL_H
#define GRNBFMTX_I_IMPL_H

#undef DEBUG //required to include gnuradio headers

#include <gnuradio/top_block.h>
#include <gnuradio/blocks/short_to_float.h>
#include <gnuradio/analog/frequency_modulator_fc.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/filter/interp_fir_filter_fff.h>
#include <gnuradio/filter/iir_filter_ffd.h>

#include "rh_source_bulkio.h"
#include "rh_sink_bulkio.h"

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

	int serviceFunction();

private:
	int interp_factor;

	gr::blocks::short_to_float::sptr to_float;
	gr::filter::interp_fir_filter_fff::sptr interpolator;
	gr::filter::iir_filter_ffd::sptr preemph;
	gr::analog::frequency_modulator_fc::sptr modulator;

	RH_shortSource* short_in;
	RH_floatSink* float_out;

    gr::top_block_sptr top_block;

    void startFlowgraph();
    void stopFlowgraph();
//    void quadRateChanged(const std::string& propid);
};

#endif // GRNBFMTX_I_IMPL_H
