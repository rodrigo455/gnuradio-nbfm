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

	RH_floatSource::sptr float_in;
	RH_shortSink::sptr short_out;

    gr::top_block_sptr top_block;

    void startFlowgraph();
    void stopFlowgraph();

};

#endif // GRNBFMRX_I_IMPL_H
