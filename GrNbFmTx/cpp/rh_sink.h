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


#ifndef INCLUDED_REDHAWK_RH_SINK_H
#define INCLUDED_REDHAWK_RH_SINK_H

#include <gnuradio/sync_block.h>

#include "GrNbFmTx.h"


namespace gr {
  namespace redhawk {

    /*!
     * \brief <+description of block+>
     * \ingroup redhawk
     *
     */
    class rh_sink : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<rh_sink> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of redhawk::rh_sink.
       *
       * To avoid accidental use of raw pointers, redhawk::rh_sink's
       * constructor is in a private implementation
       * class. redhawk::rh_sink::make is the public interface for
       * creating new instances.
       */
      static sptr make(GrNbFmTx_i* component);
    };

  } // namespace redhawk
} // namespace gr

#endif /* INCLUDED_REDHAWK_RH_SINK_H */

