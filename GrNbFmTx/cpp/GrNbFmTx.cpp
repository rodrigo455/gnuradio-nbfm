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
	do_interp = true;
	sri_changed = true;
	interp_factor = 1;
	gr_input.resize(1);
	gr_output.resize(1);
}

GrNbFmTx_i::~GrNbFmTx_i(){
	interpolator.reset();
	preemph.reset();
	modulator.reset();
}

void GrNbFmTx_i::constructor(){
    /***********************************************************************************
     This is the RH constructor. All properties are properly initialized before this function is called 
    ***********************************************************************************/
	double kl, kh, z1, p1, b0, g;
	std::vector<double> btaps;
	std::vector<double> ataps;
	std::vector<float> interp_taps;

	if(quad_rate % audio_rate != 0){
		LOG_ERROR(GrNbFmTx_i,"GnuRadioNBFM_TX_i: quad_rate is not an integer multiple of audio_rate");
		quad_rate = (quad_rate/audio_rate)*audio_rate;
	}

	do_interp = (quad_rate == audio_rate);
	interp_factor = quad_rate/audio_rate;

	if(do_interp){
		interp_taps = gr::filter::firdes::low_pass(interp_factor,            		// gain
												quad_rate,      					// sampling rate
												4500,          						// cutoff
												2500,          						// Transition band
												gr::filter::firdes::WIN_HAMMING);	// filter type

		interpolator = gr::filter::interp_fir_filter_fff::make(interp_factor, interp_taps);
	}

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
}


int GrNbFmTx_i::serviceFunction(){
	LOG_TRACE(GrNbFmTx_i,__PRETTY_FUNCTION__);
	int n;

	bulkio::InFloatPort::dataTransfer *packet = audio->getPacket(0);

	if (packet == NULL)
		return NOOP;

	if (packet->inputQueueFlushed){
		LOG_WARN(GrNbFmTx_i,"Input Queue Flushed");
	}

	if(packet->SRI.mode != 0){
		LOG_DEBUG(GrNbFmTx_i,"GrNbFmTx_i::serviceFunction| GrNbFmTx requires real input signal");
		return NOOP;
	}

	n = (int)packet->dataBuffer.size()*interp_factor;
	//LOG_INFO(GnuRadioNBFM_TX_i,"packet size: "<<n);
	gr_complex complex_buf[n];
	float out_1[n];
	float out_2[n];
	float out_3[2*n];

	gr_input[0] = (const void *)&packet->dataBuffer.front();
	gr_output[0] = (void *)&out_1[0];

	if(do_interp){
		(*interpolator).work(n, gr_input, gr_output);

		gr_input[0] = gr_output[0];
		gr_output[0] = (void *)&out_2[0];
	}

	(*preemph).work(n, gr_input, gr_output);

	gr_input[0] = gr_output[0];
	gr_output[0] = (void *)&complex_buf[0];

	(*modulator).work(n, gr_input, gr_output);

	for(int i=0; i < n; i++){
		out_3[2*i] =  real(complex_buf[i]);
		out_3[2*i+1] =  imag(complex_buf[i]);
	}

	if(sri_changed){
		BULKIO::StreamSRI sri = packet->SRI;
		sri.xdelta = sri.xdelta/interp_factor;
		sri.streamID = stream_id.c_str();
		sri.mode = 1;
		fm_signal->pushSRI(sri);
		sri_changed = false;
	}

	if(fm_signal->isActive()){
		fm_signal->pushPacket(out_3,2*n, bulkio::time::utils::now(), false, stream_id);
	}

	return NORMAL;
}
