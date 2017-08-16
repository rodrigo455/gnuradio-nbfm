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

#ifndef _RH_SINK_BULKIO_H
#define _RH_SINK_BULKIO_H

#include <gnuradio/sync_block.h>
#include <gnuradio/io_signature.h>
#include <bulkio/bulkio.h>


template<typename PORT_TYPE, typename DATA_TYPE>
class rh_sink_bulkio: virtual public gr::sync_block {

private:
	bulkio::OutNumericPort<PORT_TYPE>* port;
	std::string stream_id;
	bool complex;

public:
	typedef boost::shared_ptr<rh_sink_bulkio<PORT_TYPE, DATA_TYPE> > sptr;

	rh_sink_bulkio(bulkio::OutNumericPort<PORT_TYPE>* _port, const std::string& _stream_id, bool _complex) :
		gr::sync_block("rh_sink_bulkio",
				gr::io_signature::make(1, 1, (_complex? 2:1)*sizeof(DATA_TYPE)),
				gr::io_signature::make(0, 0, 0)),
		port(_port),
		stream_id(_stream_id),
		complex(_complex){
	}
	~rh_sink_bulkio();

	sptr get_sptr();

	int work(int noutput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items);
};

template<typename PORT_TYPE, typename DATA_TYPE>
typename rh_sink_bulkio<PORT_TYPE, DATA_TYPE>::sptr
rh_sink_bulkio<PORT_TYPE,DATA_TYPE>::get_sptr() {
	return gnuradio::get_initial_sptr(this);
}

template<typename PORT_TYPE, typename DATA_TYPE>
rh_sink_bulkio<PORT_TYPE, DATA_TYPE>::~rh_sink_bulkio() {
}

template<typename PORT_TYPE, typename DATA_TYPE>
int rh_sink_bulkio<PORT_TYPE, DATA_TYPE>::work(int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items) {
	const DATA_TYPE *in = (const DATA_TYPE *) input_items[0];

	if (port->isActive()) {
		if(complex){
			port->pushPacket(in, 2*noutput_items, bulkio::time::utils::now(), false, stream_id);
		}else{
			port->pushPacket(in, noutput_items, bulkio::time::utils::now(), false, stream_id);
		}
	}

	consume_each(noutput_items);

	return 0;
}

typedef rh_sink_bulkio<BULKIO::dataChar, CORBA::Char>     			RH_charSink;
typedef rh_sink_bulkio<BULKIO::dataOctet, CORBA::Octet> 			RH_octetSink;
typedef RH_charSink                           						RH_int8Sink;
typedef RH_octetSink                          						RH_uint8Sink;
typedef rh_sink_bulkio<BULKIO::dataShort, CORBA::Short> 			RH_shortSink;
typedef rh_sink_bulkio<BULKIO::dataUshort, CORBA::UShort> 			RH_ushortSink;
typedef RH_shortSink                          						RH_int16Sink;
typedef RH_ushortSink                         						RH_uint16Sink;
typedef rh_sink_bulkio<BULKIO::dataLong, CORBA::Long> 				RH_longSink;
typedef rh_sink_bulkio<BULKIO::dataUlong, CORBA::ULong> 			RH_ulongSink;
typedef RH_longSink                           						RH_int32Sink;
typedef RH_ulongSink                          						RH_uint32Sink;
typedef rh_sink_bulkio<BULKIO::dataLongLong, CORBA::LongLong> 		RH_longlongSink;
typedef rh_sink_bulkio<BULKIO::dataUlongLong, CORBA::ULongLong> 	RH_ulonglongSink;
typedef RH_longlongSink                       						RH_int64Sink;
typedef RH_int64Sink                      							RH_uint64Sink;
typedef rh_sink_bulkio<BULKIO::dataFloat, CORBA::Float> 			RH_floatSink;
typedef rh_sink_bulkio<BULKIO::dataDouble, CORBA::Double> 			RH_doubleSink;

#endif /* _RH_SINK_BULKIO_H */

