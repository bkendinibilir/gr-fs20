/* -*- c++ -*- */
/* 
 * Copyright 2013 Benjamin Kendinibilir.
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


#ifndef INCLUDED_FS20_FS20_BIT_DECODER_BB_H
#define INCLUDED_FS20_FS20_BIT_DECODER_BB_H

#include <fs20/api.h>
#include <gr_block.h>

namespace gr {
  namespace fs20 {

    /*!
     * \brief <+description of block+>
     * \ingroup fs20
     *
     */
    class FS20_API fs20_bit_decoder_bb : virtual public gr_block
    {
    public:
       typedef boost::shared_ptr<fs20_bit_decoder_bb> sptr;

       /*!
        * \brief Return a shared_ptr to a new instance of fs20::fs20_bit_decoder_bb.
        *
        * To avoid accidental use of raw pointers, fs20::fs20_bit_decoder_bb's
        * constructor is in a private implementation
        * class. fs20::fs20_bit_decoder_bb::make is the public interface for
        * creating new instances.
        */
       static sptr make(unsigned long samp_rate, unsigned int debug);
    };

  } // namespace fs20
} // namespace gr

#endif /* INCLUDED_FS20_FS20_BIT_DECODER_BB_H */

