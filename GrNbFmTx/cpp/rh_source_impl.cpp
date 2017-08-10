/* -*- c++ -*- */
/* 
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
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

#include <gnuradio/io_signature.h>
#include "rh_source_impl.h"

rh_source::sptr rh_source::make(bulkio::InShortPort *audio) {
	return gnuradio::get_initial_sptr(new rh_source_impl(audio));
}

/*
 * The private constructor
 */
rh_source_impl::rh_source_impl(bulkio::InShortPort *_audio) :
		gr::sync_block("rh_source", gr::io_signature::make(0, 0, 0),
				gr::io_signature::make(1, 1, sizeof(short))) {
	audio = _audio;
}

/*
 * Our virtual destructor.
 */
rh_source_impl::~rh_source_impl() {
}

int rh_source_impl::work(int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items) {
	int blocksz;
	short *out = (short *) output_items[0];

	bulkio::InShortStream stream = audio->getCurrentStream(0);

	if(!stream)
		return 0;

	bulkio::ShortDataBlock block = stream.read(noutput_items);

	if(block.inputQueueFlushed()){
		std::cout << "audio port Input Queue Flushed" << std::endl;
	}

	if(block.complex()){
		std::cerr << "audio port requires real input signal" << std::endl;
		return 0;
	}

	blocksz = block.size();

	memcpy(&out[0],block.data(), sizeof(short)*blocksz);

	return blocksz;
}

