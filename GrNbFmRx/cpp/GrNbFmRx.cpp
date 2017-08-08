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
	sri_changed =  true;
	gr_input.resize(1);
	gr_output.resize(1);
	buffersz = 0;
}

GrNbFmRx_i::~GrNbFmRx_i(){
	quad_demod.reset();
	deemph.reset();
	audio_filter.reset();
	to_short.reset();
}

void GrNbFmRx_i::constructor(){
    /***********************************************************************************
     This is the RH constructor. All properties are properly initialized before this function is called 
    ***********************************************************************************/
}

void GrNbFmRx_i::start() throw (CF::Resource::StartError, CORBA::SystemException){

	double k1, p1, b0;
    std::vector<double> btaps;
    std::vector<double> ataps;
    std::vector<float> audio_taps;

    GrNbFmRx_base::start();

	if(quad_rate % audio_rate != 0){
		LOG_ERROR(GrNbFmRx_i,"GnuRadioNBFM_RX_i: quad_rate is not an integer multiple of audio_rate");
		stop();
		return;
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
											  2.7e3,          					// Audio LPF cutoff
											  0.5e3,          					// Transition band
											  gr::filter::firdes::WIN_HAMMING);	// filter type

	// Decimating audio filter
	decim_factor = quad_rate/audio_rate;
	audio_filter = gr::filter::fir_filter_fff::make(decim_factor, audio_taps);

	to_short = gr::blocks::float_to_short::make(1,32767);

	buffersz = (buffer_size/decim_factor)*decim_factor;
	complex_in.resize(buffersz);
	quad_out.resize(buffersz);
	filter_out.resize(buffersz/decim_factor);
	short_out.resize(buffersz/decim_factor);
	deemph_out.resize(buffersz);

	sri_changed = true;
}

void GrNbFmRx_i::stop() throw (CF::Resource::StopError, CORBA::SystemException){
	GrNbFmRx_base::stop();
}

void GrNbFmRx_i::validate(CF::Properties property, CF::Properties& validProps, CF::Properties& invalidProps){
    for (CORBA::ULong ii = 0; ii < property.length (); ++ii) {
        std::string id((const char*)property[ii].id);
        // properties cannot be set while the component is running
        if (_started) {
			LOG_WARN(GrNbFmRx_i, "'"<<id<<"' cannot be changed while component is running.")
			CORBA::ULong count = invalidProps.length();
			invalidProps.length(count + 1);
			invalidProps[count].id = property[ii].id;
			invalidProps[count].value = property[ii].value;
        }
    }

}

void GrNbFmRx_i::configure(const CF::Properties& configProperties)
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


int GrNbFmRx_i::serviceFunction(){
	LOG_TRACE(GrNbFmRx_i,__PRETTY_FUNCTION__);
	int blocksz;

	bulkio::InFloatStream stream = fm_signal->getCurrentStream(0);

	if(!stream)
		return NOOP;

	bulkio::FloatDataBlock block = stream.read(buffersz);

	if(block.inputQueueFlushed()){
		LOG_WARN(GrNbFmRx_i,"Input Queue Flushed");
	}

	if(!block.complex()){
		LOG_DEBUG(GrNbFmRx_i,"GrNbFmTx_i::serviceFunction| GrNbFmTx requires complex input signal");
		return NOOP;
	}

	blocksz = block.cxsize();
	assert(buffersz >= blocksz);

	float2gr_complex(block.data(), &complex_in[0], blocksz);

	gr_input[0] = (const void *)&complex_in[0];

	gr_output[0] = (void *)&quad_out[0];
	(*quad_demod).work(blocksz, gr_input, gr_output);
	gr_input[0] = gr_output[0];

	if(deemphasis_enable){
		gr_output[0] = (void *)&deemph_out[0];
		(*deemph).work(blocksz, gr_input, gr_output);
		gr_input[0] = gr_output[0];
	}

	if(decim_factor!=1){
		gr_output[0] = (void *)&filter_out[0];
		(*audio_filter).work(blocksz/decim_factor, gr_input, gr_output);
		gr_input[0] = gr_output[0];
	}

	gr_output[0] = (void *)&short_out[0];
	(*to_short).work(blocksz/decim_factor, gr_input, gr_output);

	if(sri_changed){
		BULKIO::StreamSRI sri = block.sri();
		sri.xdelta = sri.xdelta*decim_factor;
		sri.streamID = stream_id.c_str();
		sri.mode = 0;
		audio_out->pushSRI(sri);
		sri_changed = false;
	}

	if(audio_out->isActive()){
		audio_out->pushPacket(&short_out[0], blocksz/decim_factor, bulkio::time::utils::now(), false, stream_id);
	}
    
    return NORMAL;
}

void GrNbFmRx_i::float2gr_complex(float* input, gr_complex* output, int n){
	int _n = n;
	while(_n--){
		*(output++) =  gr_complex(input[0],input[1]);
		input+=2;
	}
}

