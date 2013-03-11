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

#ifndef INCLUDED_FS20_FS20_BIT_DECODER_BB_IMPL_H
#define INCLUDED_FS20_FS20_BIT_DECODER_BB_IMPL_H

#include <fs20/fs20_bit_decoder_bb.h>

// http://www.mikrocontroller.net/attachment/15335/fs20-protokoll.txt

// bit 0: 400µs high, 400µs low 600-1000µs period
#define FS20_BIT0_PERIOD_MIN  600000 // ns
#define FS20_BIT0_PERIOD_MAX 1000000 // ns
#define FS20_BIT0_DIFF_MAX    250000 // ns (1000us / 2 / 2) 

// bit 1: 600µs high, 600µs low 1000-1450µs period
#define FS20_BIT1_PERIOD_MIN 1000000 // ns 
#define FS20_BIT1_PERIOD_MAX 1425000 // ns
#define FS20_BIT1_DIFF_MAX    356000 // ns (1425us / 2 / 2) 

namespace gr {
  namespace fs20 {

    class fs20_bit_decoder_bb_impl : public fs20_bit_decoder_bb
    {
    private:
      unsigned long d_time_per_sample; // 10e-9 (in nanosecs)
	  unsigned int d_debug;

      unsigned long d_high_count;
      unsigned long d_low_count;

    public:
      fs20_bit_decoder_bb_impl(unsigned long samp_rate, unsigned int debug);
      ~fs20_bit_decoder_bb_impl();

      void debug(int debug_treshold, const char* format, ...);
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      // Where all the action really happens
      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace fs20
} // namespace gr

#endif /* INCLUDED_FS20_FS20_BIT_DECODER_BB_IMPL_H */

