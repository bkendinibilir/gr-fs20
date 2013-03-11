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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_io_signature.h>
#include "fs20_bit_decoder_bb_impl.h"

#include <stdio.h>
#include <stdarg.h>

namespace gr {
  namespace fs20 {

	void fs20_bit_decoder_bb_impl::debug(int debug_treshold,
      const char* format, ...)
    {   
      va_list ap; 

      if(d_debug >= debug_treshold) {
        fprintf(stderr, "DEBUG(%d): fs20_bit_decoder: ", debug_treshold);
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        va_end(ap);
        fprintf(stderr, "\n");
      }   
    } 

    fs20_bit_decoder_bb::sptr
    fs20_bit_decoder_bb::make(unsigned long samp_rate, unsigned int debug)
    {
      return gnuradio::get_initial_sptr (new fs20_bit_decoder_bb_impl(samp_rate, debug));
    }

    /*
     * The private constructor
     */
    fs20_bit_decoder_bb_impl::fs20_bit_decoder_bb_impl(unsigned long samp_rate, 
      unsigned int debug)
      : gr_block("fs20_bit_decoder_bb",
		      gr_make_io_signature(1, 1, sizeof (char)),
		      gr_make_io_signature(1, 1, sizeof (char))),
	  d_debug(debug)
    {
      // init counters
	  d_low_count = 0;
      d_high_count = 0;

      // time per sample in nanoseconds
      d_time_per_sample = (1000000000 / samp_rate);
    }

    /*
     * Our virtual destructor.
     */
    fs20_bit_decoder_bb_impl::~fs20_bit_decoder_bb_impl()
    {
    }

    void
    fs20_bit_decoder_bb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      // FS20_BIT1_PERIOD_MAX / d_time_per_sample = avg. input bits needed for one output bit
      ninput_items_required[0] = int(noutput_items * (FS20_BIT1_PERIOD_MAX / d_time_per_sample));

      debug(9, "forecast(): %d x (%d  / %d) = %d input items needed", 
        noutput_items, FS20_BIT1_PERIOD_MAX, d_time_per_sample, ninput_items_required[0]);
    }

    int
    fs20_bit_decoder_bb_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];

	  unsigned int ipos = 0;
      unsigned int opos = 0;

      // iterate over all new input items, dont output more data then possible
	  while(ipos < ninput_items[0] && opos < noutput_items) {
   
        // high value (output of binary slicer) 
        if (in[ipos] == 1) {
       
          // end of a period, if we have high and low values
          if(d_high_count > 0 && d_low_count > 0) {

            // calculate timings 
            unsigned long high_time = d_high_count * d_time_per_sample;
           	unsigned long low_time = d_low_count * d_time_per_sample;
			unsigned long period_time = high_time + low_time;
			unsigned long diff_time = abs(high_time - low_time);

            // compare timings with FS20 data transmittion specs
            if(period_time >= FS20_BIT0_PERIOD_MIN && 
              period_time <= FS20_BIT0_PERIOD_MAX && diff_time <= FS20_BIT0_DIFF_MAX) {

              // bit 0 decoded
              out[opos++] = 0;
				
              debug(1, "DATA: bit %d -> high: %ld = %ldns, low: %ld = %ldns, period: %ldns", 
                0, d_high_count, high_time, d_low_count, low_time, period_time);

			} else if(period_time >= FS20_BIT1_PERIOD_MIN && 
              period_time <= FS20_BIT1_PERIOD_MAX && diff_time <= FS20_BIT1_DIFF_MAX) {

              // bit 1 decoded
              out[opos++] = 1;

              debug(1, "DATA: bit %d -> high: %ld = %ldns, low: %ld = %ldns, period: %ldns", 
                1, d_high_count, high_time, d_low_count, low_time, period_time);

			} else if(high_time >= FS20_BIT0_PERIOD_MIN / 2 && 
			  high_time <= FS20_BIT0_PERIOD_MAX / 2 && low_time >= FS20_BIT0_PERIOD_MIN / 2) {

              // end of transmission (EOT)
              debug(1, "DATA: EOT -> high: %ld = %ldns, low: %ld = %ldns, period: %ldns", 
                d_high_count, high_time, d_low_count, low_time, period_time);

            } else {
              // no valid fs20 data
              debug(1, "INVALID DATA -> high: %ld = %ldns, low: %ld = %ldns, period: %ldns", 
                d_high_count, high_time, d_low_count, low_time, period_time);
            } 

            // reset counter, new period, first high value
            d_high_count = 1;
            d_low_count = 0;

          // no low values in this period yet...
          } else {

            // counting high values
            d_high_count++;
		  }
  
        // low value (output of binary slicer)
        } else if(in[ipos] == 0) {

          // start counting low values if we have high values
          if(d_high_count > 0) {
            d_low_count++;
          }   

        // binary slicer should only output 0 and 1, ignore other values
        } else {
          debug(0, "ERROR in general_work(): no valid binary_sliced data!");
        }    

		// next input value
		ipos++;

      } // while()

      debug(5, "general_work(): %d inputs consumed, %d outputs produced", 
        ipos, opos);
    
      // Tell runtime system how many input items we consumed on each input stream.
      consume_each(ipos);

      // Tell runtime system how many output items we produced.
      return (opos);

    } /* general_work() */

  } /* namespace fs20 */
} /* namespace gr */

