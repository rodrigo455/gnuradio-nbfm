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

#ifndef _RH_SOURCE_BULKIO_H
#define _RH_SOURCE_BULKIO_H

#include <gnuradio/io_signature.h>
#include <gnuradio/sync_block.h>
#include <bulkio/bulkio.h>

template<typename PORT_TYPE, typename DATA_TYPE>
class rh_source_bulkio: virtual public gr::sync_block {

private:
	bulkio::InNumericPort<PORT_TYPE>* port;
	bool complex;

public:
	typedef boost::shared_ptr<rh_source_bulkio<PORT_TYPE, DATA_TYPE> > sptr;

	rh_source_bulkio(bulkio::InNumericPort<PORT_TYPE>* _port, bool _complex):
		gr::sync_block("rh_source_bulkio",
				gr::io_signature::make(0, 0, 0),
				gr::io_signature::make(1, 1, (_complex? 2:1)*sizeof(DATA_TYPE))),
		port(_port),
		complex(_complex){
	}
	~rh_source_bulkio();

	static sptr make(bulkio::InNumericPort<PORT_TYPE>* port, bool complex);

	int work(int noutput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items);

};

template<typename PORT_TYPE, typename DATA_TYPE>
typename rh_source_bulkio<PORT_TYPE, DATA_TYPE>::sptr
rh_source_bulkio<PORT_TYPE, DATA_TYPE>::make(bulkio::InNumericPort<PORT_TYPE>* port, bool complex) {
	return gnuradio::get_initial_sptr(new rh_source_bulkio<PORT_TYPE, DATA_TYPE>(port, complex));
}

template<typename PORT_TYPE, typename DATA_TYPE>
rh_source_bulkio<PORT_TYPE, DATA_TYPE>::~rh_source_bulkio() {
}

template<typename PORT_TYPE, typename DATA_TYPE>
int rh_source_bulkio<PORT_TYPE, DATA_TYPE>::work(int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items) {
	int block_size;
	DATA_TYPE *out = (DATA_TYPE *) output_items[0];

	bulkio::BufferedInputStream<PORT_TYPE> stream = port->getCurrentStream(0);

	if (!stream)
		return 0;

	bulkio::SampleDataBlock<DATA_TYPE> block = stream.read(noutput_items);

	if (block.inputQueueFlushed()) {
		std::cout << "rh_source_bulkio::work| Input Queue Flushed" << std::endl;
	}

	if(complex != block.complex()){
		std::cerr << "rh_source_bulkio::work| Port requires "<< (complex? "complex":"real") <<" input signal" << std::endl;
		return 0;
	}

	block_size = block.size();

	memcpy(out, block.data(), sizeof(DATA_TYPE) * block_size);

	if(complex)
		return block_size/2;
	else
		return block_size;
}

typedef rh_source_bulkio<BULKIO::dataChar, CORBA::Char>     		RH_charSource;
typedef rh_source_bulkio<BULKIO::dataOctet, CORBA::Octet> 		RH_octetSource;
typedef RH_charSource                           			RH_int8Source;
typedef RH_octetSource                          			RH_uint8Source;
typedef rh_source_bulkio<BULKIO::dataShort, CORBA::Short> 		RH_shortSource;
typedef rh_source_bulkio<BULKIO::dataUshort, CORBA::UShort> 		RH_ushortSource;
typedef RH_shortSource                          			RH_int16Source;
typedef RH_ushortSource                         			RH_uint16Source;
typedef rh_source_bulkio<BULKIO::dataLong, CORBA::Long> 		RH_longSource;
typedef rh_source_bulkio<BULKIO::dataUlong, CORBA::ULong> 		RH_ulongSource;
typedef RH_longSource                           			RH_int32Source;
typedef RH_ulongSource                          			RH_uint32Source;
typedef rh_source_bulkio<BULKIO::dataLongLong, CORBA::LongLong> 	RH_longlongSource;
typedef rh_source_bulkio<BULKIO::dataUlongLong, CORBA::ULongLong> 	RH_ulonglongSource;
typedef RH_longlongSource                       			RH_int64Source;
typedef RH_int64Source                      				RH_uint64Source;
typedef rh_source_bulkio<BULKIO::dataFloat, CORBA::Float> 		RH_floatSource;
typedef rh_source_bulkio<BULKIO::dataDouble, CORBA::Double> 		RH_doubleSource;

#endif /* _RH_SOURCE_BULKIO_H */

