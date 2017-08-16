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
/**************************************************************************

    This is the component code. This file contains the child class where
    custom functionality can be added to the component. Custom
    functionality to the base class can be extended here. Access to
    the ports can also be done from this class

**************************************************************************/

#include "GrNbFmRx.h"

PREPARE_LOGGING(GrNbFmRx_i)

GrNbFmRx_i::GrNbFmRx_i(const char *uuid, const char *label) :
    GrNbFmRx_base(uuid, label){
	decim_factor = 1;
}

GrNbFmRx_i::~GrNbFmRx_i(){
	quad_demod.reset();
	deemph.reset();
	audio_filter.reset();
	to_short.reset();
	top_block.reset();
	float_in.reset();
	short_out.reset();
}

void GrNbFmRx_i::constructor(){

	double k1, p1, b0;
	std::vector<double> btaps;
	std::vector<double> ataps;
	std::vector<float> audio_taps;

	if(quad_rate % audio_rate != 0 || quad_rate == 0){
		quad_rate = (quad_rate/audio_rate)*audio_rate;
		if(quad_rate == 0){
			quad_rate = audio_rate;
		}
		LOG_WARN(GrNbFmRx_i,"GrNbFmRx_i: quad_rate was changed to "<<quad_rate<<" to become a multiple of audio_rate");
	}

	// FM Demodulator  input: complex; output: float
	quad_demod = gr::analog::quadrature_demod_cf::make(quad_rate/(2*M_PI*max_dev));

	// FM Deemphasis IIR filter
	k1 = -tan(1.0 / (2.0 * quad_rate * tau));
	p1 = (1.0 + k1) / (1.0 - k1);
	b0 = -k1 / (1.0 - k1);

	btaps.resize(2);
	btaps[0] = b0;
	btaps[1] = b0;
	ataps.resize(2);
	ataps[0] = 1.0;
	ataps[1] = -p1;

	deemph = gr::filter::iir_filter_ffd::make(btaps, ataps, false);

	// compute FIR taps for audio filter
	audio_taps = gr::filter::firdes::low_pass(1.0,            			// gain
											  quad_rate,      					// sampling rate
											  3.3e3,          					// Audio LPF cutoff
											  0.5e3,          					// Transition band
											  gr::filter::firdes::WIN_HAMMING);	// filter type

	// Decimating audio filter
	decim_factor = quad_rate/audio_rate;
	audio_filter = gr::filter::fir_filter_fff::make(decim_factor, audio_taps);

	to_short = gr::blocks::float_to_short::make(1,32767);

	float_in = RH_floatSource::make(fm_signal,true);

	short_out = RH_shortSink::make(audio_out, stream_id, false);

	top_block = gr::make_top_block("nbfm_rx");

	top_block->connect(float_in, 0, quad_demod, 0);
	if(deemphasis){
		top_block->connect(quad_demod, 0, deemph, 0);
		top_block->connect(deemph, 0, audio_filter, 0);
	}else{
		top_block->connect(quad_demod, 0, audio_filter, 0);
	}
	top_block->connect(audio_filter, 0, to_short, 0);
	top_block->connect(to_short, 0, short_out, 0);
}

void GrNbFmRx_i::start() throw (CF::Resource::StartError, CORBA::SystemException){
	GrNbFmRx_base::start();
	startFlowgraph();
}

void GrNbFmRx_i::stop() throw (CF::Resource::StopError, CORBA::SystemException){
	stopFlowgraph();
	GrNbFmRx_base::stop();
}


int GrNbFmRx_i::serviceFunction(){
    return FINISH;
}

void GrNbFmRx_i::startFlowgraph(){
	BULKIO::StreamSRI sri = BULKIO::StreamSRI();
	sri.xdelta = 1.0/audio_rate;
	sri.xunits = BULKIO::UNITS_TIME;
	sri.streamID = stream_id.c_str();
	sri.mode = 0;
	audio_out->pushSRI(sri);

	top_block->start(bulkio::Const::MAX_TRANSFER_BYTES*0.3);
}

void GrNbFmRx_i::stopFlowgraph(){
	top_block->stop();
	top_block->wait();
}

//TODO property Listeners
