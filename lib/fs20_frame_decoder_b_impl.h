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

#ifndef INCLUDED_FS20_FS20_FRAME_DECODER_B_IMPL_H
#define INCLUDED_FS20_FS20_FRAME_DECODER_B_IMPL_H

#include <fs20/fs20_frame_decoder_b.h>

// SYNC: 0000000000001
#define SYNC_ZEROS 12

// HC1, HC2, ADR, CMD1, (CMD2), SUMOFBITS
#define MAX_PKT_LEN 6
#define CMD_BYTE_POS 3
#define CMD_EXT_BIT 5

#define CHECKSUM_START 6
#define CHECKSUM_DIFF 2

namespace gr {
  namespace fs20 {

    class fs20_frame_decoder_b_impl : public fs20_frame_decoder_b
    {
    private:
      enum state_t {STATE_RESET, STATE_SYNC, STATE_HEAD, STATE_TAIL};

      gr_msg_queue_sptr d_target_queue;
      int d_debug;
      state_t d_state;

      unsigned char d_packet[MAX_PKT_LEN];
      unsigned char d_nbytes;
	  unsigned char d_last_byte;
	  unsigned char d_parity;
	  unsigned int d_nbits;

      struct timeval d_timestamp;

	  void debug(int debug_treshold, const char* format, ...);
	  bool checksum(void);
	  bool fill_packet_bytes(unsigned char bit);
      void state_machine(unsigned char bit);

    public:
      fs20_frame_decoder_b_impl(gr_msg_queue_sptr target_queue, int debug);
      ~fs20_frame_decoder_b_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace fs20
} // namespace gr

#endif /* INCLUDED_FS20_FS20_FRAME_DECODER_B_IMPL_H */

