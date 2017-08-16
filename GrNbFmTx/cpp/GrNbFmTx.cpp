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

#include "GrNbFmTx.h"

PREPARE_LOGGING(GrNbFmTx_i)

GrNbFmTx_i::GrNbFmTx_i(const char *uuid, const char *label) :
    GrNbFmTx_base(uuid, label){
	interp_factor = 1;
}

GrNbFmTx_i::~GrNbFmTx_i(){
	to_float.reset();
	interpolator.reset();
	preemph.reset();
	modulator.reset();
	top_block.reset();
	short_in.reset();
	float_out.reset();
}

void GrNbFmTx_i::constructor(){
	double kl, kh, z1, p1, b0, g;
	std::vector<double> btaps;
	std::vector<double> ataps;
	std::vector<float> interp_taps;

	//setPropertyChangeListener(static_cast<std::string>("quad_rate"), this, &GrNbFmTx_i::quadRateChanged);

	if(quad_rate % audio_rate != 0 || quad_rate == 0){
		quad_rate = (quad_rate/audio_rate)*audio_rate;
		if(quad_rate == 0){
			quad_rate = audio_rate;
		}
		LOG_WARN(GrNbFmTx_i,"GnuRadioNBFM_TX_i: quad_rate was changed to "<<quad_rate<<" to become a multiple of audio_rate");
	}

	interp_factor = quad_rate/audio_rate;

	to_float = gr::blocks::short_to_float::make(1,32767);

	interp_taps = gr::filter::firdes::low_pass(interp_factor,            		// gain
						quad_rate,      			// sampling rate
						3.3e3,          			// Audio LPF cutoff
						0.5e3,          			// Transition band
						gr::filter::firdes::WIN_HAMMING);	// filter type

	interpolator = gr::filter::interp_fir_filter_fff::make(interp_factor, interp_taps);


	if(fh <= 0.0 || fh >= quad_rate/2.0)
		fh = 0.925 * quad_rate/2.0;

	kl = -tan(1.0 / (2.0 * quad_rate * tau));
	kh = -tan(M_PI * fh / quad_rate);
	z1 = (1.0 + kl) / (1.0 - kl);
	p1 = (1.0 + kh) / (1.0 - kh);
	b0 = (1.0 - kl) / (1.0 - kh);

	g = fabs(1.0-p1)/(b0*fabs(1.0-z1));

	btaps.resize(2);
	btaps[0] = g*b0;
	btaps[1] = -g*b0*z1;
	ataps.resize(2);
	ataps[0] = 1.0;
	ataps[1] = -p1;

	preemph = gr::filter::iir_filter_ffd::make(btaps, ataps, false);

	modulator = gr::analog::frequency_modulator_fc::make(2*M_PI*max_dev/quad_rate);

	short_in = RH_shortSource::make(audio, false);
	float_out = RH_floatSink::make(fm_signal, stream_id ,true);

	top_block = gr::make_top_block("nbfm_tx");

	top_block->connect(short_in, 0, to_float, 0);
	if(interp_factor != 1){
		top_block->connect(to_float, 0, interpolator, 0);
		if(preemphasis){
			top_block->connect(interpolator, 0, preemph, 0);
			top_block->connect(preemph, 0, modulator, 0);
		}else{
			top_block->connect(interpolator, 0, modulator, 0);
		}
	}else{
		if(preemphasis){
			top_block->connect(to_float, 0, preemph, 0);
			top_block->connect(preemph, 0, modulator, 0);
		}else{
			top_block->connect(to_float, 0, modulator, 0);
		}
	}
	top_block->connect(modulator, 0, float_out, 0);
}

void GrNbFmTx_i::start() throw (CF::Resource::StartError, CORBA::SystemException){
	GrNbFmTx_base::start();
	startFlowgraph();
}

void GrNbFmTx_i::stop() throw (CF::Resource::StopError, CORBA::SystemException){
	stopFlowgraph();
	GrNbFmTx_base::stop();
}

void GrNbFmTx_i::startFlowgraph(){
	BULKIO::StreamSRI sri = BULKIO::StreamSRI();
	sri.xdelta = 1.0/quad_rate;
	sri.xunits = BULKIO::UNITS_TIME;
	sri.streamID = stream_id.c_str();
	sri.mode = 1;
	fm_signal->pushSRI(sri);

	top_block->start(bulkio::Const::MAX_TRANSFER_BYTES*0.3);
}

void GrNbFmTx_i::stopFlowgraph(){
	top_block->stop();
	top_block->wait();
}

int GrNbFmTx_i::serviceFunction(){
	return FINISH;
}

//TODO property listeners
//void GrNbFmTx_i::quadRateChanged(const std::string& propid){
//
//	double kl, kh, z1, p1, b0, g;
//	std::vector<double> btaps;
//	std::vector<double> ataps;
//	std::vector<float> interp_taps;
//
//	if(quad_rate % audio_rate != 0 || quad_rate == 0){
//		quad_rate = (quad_rate/audio_rate)*audio_rate;
//		if(quad_rate == 0){
//			quad_rate = audio_rate;
//		}
//		LOG_WARN(GrNbFmTx_i,"GnuRadioNBFM_TX_i: quad_rate was changed to "<<quad_rate<<" to become a multiple of audio_rate");
//	}
//
//	interp_factor = quad_rate/audio_rate;
//
//	interp_taps = gr::filter::firdes::low_pass(interp_factor,            		// gain
//						quad_rate,      			// sampling rate
//						3.3e3,          			// Audio LPF cutoff
//						0.5e3,          			// Transition band
//						gr::filter::firdes::WIN_HAMMING);	// filter type
//
//	interpolator->set_interpolation(interp_factor);
//	interpolator->set_taps(interp_taps);
//
//	if(fh <= 0.0 || fh >= quad_rate/2.0)
//		fh = 0.925 * quad_rate/2.0;
//
//	kl = -tan(1.0 / (2.0 * quad_rate * tau));
//	kh = -tan(M_PI * fh / quad_rate);
//	z1 = (1.0 + kl) / (1.0 - kl);
//	p1 = (1.0 + kh) / (1.0 - kh);
//	b0 = (1.0 - kl) / (1.0 - kh);
//
//	g = fabs(1.0-p1)/(b0*fabs(1.0-z1));
//
//	btaps.resize(2);
//	btaps[0] = g*b0;
//	btaps[1] = -g*b0*z1;
//	ataps.resize(2);
//	ataps[0] = 1.0;
//	ataps[1] = -p1;
//
//	preemph->set_taps(btaps, ataps);
//
//	modulator->set_sensitivity(2*M_PI*max_dev/quad_rate);
//
//	top_block->lock();
//	top_block->disconnect_all();
//
//	top_block->connect(short_in, 0, to_float, 0);
//	if(interp_factor != 1){
//		top_block->connect(to_float, 0, interpolator, 0);
//		if(preemphasis){
//			top_block->connect(interpolator, 0, preemph, 0);
//			top_block->connect(preemph, 0, modulator, 0);
//		}else{
//			top_block->connect(interpolator, 0, modulator, 0);
//		}
//	}else{
//		if(preemphasis){
//			top_block->connect(to_float, 0, preemph, 0);
//			top_block->connect(preemph, 0, modulator, 0);
//		}else{
//			top_block->connect(to_float, 0, modulator, 0);
//		}
//	}
//	top_block->connect(modulator, 0, float_out, 0);
//
//	top_block->unlock();
//}



