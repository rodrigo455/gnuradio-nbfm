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
	sri_changed = true;
	interp_factor = 1;
	gr_input.resize(1);
	gr_output.resize(1);
}

GrNbFmTx_i::~GrNbFmTx_i(){
	to_float.reset();
	interpolator.reset();
	preemph.reset();
	modulator.reset();
}

void GrNbFmTx_i::constructor(){
    /***********************************************************************************
     This is the RH constructor. All properties are properly initialized before this function is called 
    ***********************************************************************************/
}

void GrNbFmTx_i::start() throw (CF::Resource::StartError, CORBA::SystemException){

	double kl, kh, z1, p1, b0, g;
	std::vector<double> btaps;
	std::vector<double> ataps;
	std::vector<float> interp_taps;

	GrNbFmTx_base::start();

	if(quad_rate % audio_rate != 0){
		LOG_ERROR(GrNbFmTx_i,"GnuRadioNBFM_TX_i: quad_rate is not an integer multiple of audio_rate");
		stop();
		return;
	}

	interp_factor = quad_rate/audio_rate;

	to_float = gr::blocks::short_to_float::make(1,32767);


	interp_taps = gr::filter::firdes::low_pass(interp_factor,            		// gain
											quad_rate,      					// sampling rate
											2.7e3,          					// Audio LPF cutoff
											0.5e3,          					// Transition band
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

	to_float_in.resize(buffer_size+to_float->history()-1);
	interp_in.resize(buffer_size+interpolator->history()-1);
	preemph_in.resize(buffer_size*interp_factor+preemph->history()-1);
	mod_in.resize(buffer_size*interp_factor+modulator->history()-1);;
	output_buf.resize(buffer_size*interp_factor);

	sri_changed = true;
}

void GrNbFmTx_i::stop() throw (CF::Resource::StopError, CORBA::SystemException){
	GrNbFmTx_base::stop();
}

void GrNbFmTx_i::validate(CF::Properties property, CF::Properties& validProps, CF::Properties& invalidProps){
    for (CORBA::ULong ii = 0; ii < property.length (); ++ii) {
        std::string id((const char*)property[ii].id);
        // properties cannot be set while the component is running
        if (_started) {
			LOG_WARN(GrNbFmTx_i, "'"<<id<<"' cannot be changed while component is running.")
			CORBA::ULong count = invalidProps.length();
			invalidProps.length(count + 1);
			invalidProps[count].id = property[ii].id;
			invalidProps[count].value = property[ii].value;
        }
    }

}

void GrNbFmTx_i::configure(const CF::Properties& configProperties)
		throw (CF::PropertySet::PartialConfiguration,
		CF::PropertySet::InvalidConfiguration, CORBA::SystemException) {
    CF::Properties validProperties;
    CF::Properties invalidProperties;
    validate(configProperties, validProperties, invalidProperties);

    if (invalidProperties.length() > 0) {
        throw CF::PropertySet::InvalidConfiguration("Properties failed validation.  See log for details.", invalidProperties);
    }

    PropertySet_impl::configure(configProperties);
}

int GrNbFmTx_i::serviceFunction(){
	LOG_TRACE(GrNbFmTx_i,__PRETTY_FUNCTION__);
	int blocksz;

	bulkio::InShortStream stream = audio->getCurrentStream(0);

	if(!stream)
		return NOOP;

	bulkio::ShortDataBlock block = stream.read(buffer_size);

	if(block.inputQueueFlushed()){
		LOG_WARN(GrNbFmTx_i,"Input Queue Flushed");
	}

	if(block.complex()){
		LOG_DEBUG(GrNbFmTx_i,"GrNbFmTx_i::serviceFunction| GrNbFmTx requires real input signal");
		return NOOP;
	}

	blocksz = block.size();
	memcpy(&to_float_in[to_float->history()-1],block.data(), sizeof(short)*blocksz);

	gr_input[0] = (const void *)&to_float_in[0];

	if(interp_factor != 1){
		gr_output[0] = (void *)&interp_in[interpolator->history()-1];
	}else if(preemphasis_enable){
		gr_output[0] = (void *)&preemph_in[preemph->history()-1];
	}else{
		gr_output[0] = (void *)&mod_in[modulator->history()-1];
	}

	to_float->work(blocksz, gr_input, gr_output);

	if(interp_factor != 1){
		gr_input[0] = (const void *)&interp_in[0];
		if(preemphasis_enable){
			gr_output[0] = (void *)&preemph_in[preemph->history()-1];
		}else{
			gr_output[0] = (void *)&mod_in[modulator->history()-1];
		}
		interpolator->work(blocksz*interp_factor, gr_input, gr_output);
	}

	if(preemphasis_enable){
		gr_input[0] = (const void *)&preemph_in[0];
		gr_output[0] = (void *)&mod_in[modulator->history()-1];
		preemph->work(blocksz*interp_factor, gr_input, gr_output);
	}

	gr_input[0] = (const void *)&mod_in[0];

	gr_output[0] = (void *)&output_buf[0];

	modulator->work(blocksz*interp_factor, gr_input, gr_output);

	if(sri_changed){
		BULKIO::StreamSRI sri = block.sri();
		sri.xdelta = sri.xdelta/interp_factor;
		sri.streamID = stream_id.c_str();
		sri.mode = 1;
		fm_signal->pushSRI(sri);
		sri_changed = false;
	}

	if(fm_signal->isActive()){
		fm_signal->pushPacket((float *)&output_buf[0],2*blocksz*interp_factor, bulkio::time::utils::now(), false, stream_id);
	}

	//update history
	memcpy(&to_float_in[0],&to_float_in[blocksz-(to_float->history()-1)], sizeof(short)*(to_float->history()-1));
	if(interp_factor != 1){
		memcpy(&interp_in[0],&interp_in[blocksz-(interpolator->history()-1)], sizeof(float)*(interpolator->history()-1));
	}
	if(preemphasis_enable){
		memcpy(&preemph_in[0],&preemph_in[blocksz*interp_factor-(preemph->history()-1)], sizeof(float)*(preemph->history()-1));
	}
	memcpy(&mod_in[0],&mod_in[blocksz*interp_factor-(modulator->history()-1)], sizeof(gr_complex)*(modulator->history()-1));


	return NORMAL;
}

