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
#include <stdio.h>
#include <stdarg.h>
#include "fs20_frame_decoder_b_impl.h"

namespace gr {
  namespace fs20 {

	void fs20_frame_decoder_b_impl::debug(int debug_treshold,
		const char* format, ...)
	{
		va_list ap;

		if(d_debug >= debug_treshold) {
			fprintf(stderr, "DEBUG(%d): fs20_frame_decoder: ", debug_treshold);
			va_start(ap, format);
			vfprintf(stderr, format, ap);
			va_end(ap);
			fprintf(stderr, "\n");
		}
	}

	bool fs20_frame_decoder_b_impl::checksum(void) 
	{
		unsigned char checksum = CHECKSUM_START;

		for (int i = 0; i < d_nbytes; i++)
			checksum += d_packet[i];

		if (abs(d_packet[d_nbytes] - checksum) >= CHECKSUM_DIFF) {
			debug(1, "dropped packet - checksum error: packet "
				"0x%02x != calculated 0x%02x", d_packet[d_nbytes], checksum);
			return false;
		}

		return true;
	}

	bool fs20_frame_decoder_b_impl::fill_packet_bytes(unsigned char bit) 
	{
		// sum bits
		d_parity += bit;

		debug(7, "data: bit %d (parity of byte=%d)", bit, d_parity);

		// one byte with a parity bit is complete
		if (++d_nbits == 9) {
			// check even parity
			if (d_parity % 2 != 0) {
				debug(1, "dropped packet - parity error at "
					"byte position %d!", d_nbytes);
				return false;
			}
			debug(3, "byte complete at pos: %d > 0x%02x", 
				d_nbytes, d_packet[d_nbytes]);
			d_nbits = 0;
			d_parity = 0;
			d_nbytes++;


		// collect bits in the packet, MSB first
		} else {
			d_packet[d_nbytes] = (d_packet[d_nbytes] << 1) | (bit & 1);
		}

		return true;
	}

	void fs20_frame_decoder_b_impl::state_machine(unsigned char bit)
    {
		switch(d_state) {
			case STATE_RESET:
				d_nbits = 0;
				d_nbytes = 0;
				d_parity = 0;
				d_last_byte = 0;
				d_state = STATE_SYNC;

			case STATE_SYNC:
				if (bit == 0) {
					d_nbits++;
					debug(9, "sync: bit 0");

				} else if(bit == 1) {
					// if we have enough zeros before, SYNC is complete
					if (d_nbits >= SYNC_ZEROS) {
						gettimeofday(&d_timestamp, NULL);
						d_nbits = 0;
						d_state = STATE_HEAD;
						debug(5, "sync: successful, reading data...");
					} else {
						debug(9, "sync: aborted!");
						d_state = STATE_RESET;
					}
				}
				break;

			case STATE_HEAD:
				// collect bits, check parity of byte, reset on errors
				if (!fill_packet_bytes(bit)) {
					d_state = STATE_RESET;
					break;
				}

				// is CMD byte finished?
				if (d_nbytes > CMD_BYTE_POS) {
					// is extension bit set in CMD byte?
					if (d_packet[CMD_BYTE_POS] & (1 << CMD_EXT_BIT)) {
						// read max byte
						d_last_byte = MAX_PKT_LEN;
					} else {
						d_last_byte = MAX_PKT_LEN - 1;
					}
					d_state = STATE_TAIL;
				}
				break;

			case STATE_TAIL:
				if (!fill_packet_bytes(bit)) {
					d_state = STATE_RESET;
					break;
				}

				// finished reading?
				if (d_nbytes >= d_last_byte) {
					// set byte count to last byte
					d_nbytes--;

					if(checksum()) {
						// post complete packet without checksum as message to queue
						double time = (double)d_timestamp.tv_usec * 1.0e-6 + 
							(double)d_timestamp.tv_sec;
						gr_message_sptr msg = gr_make_message(0, time, 0, d_nbytes);
						memcpy(msg->msg(), d_packet, d_nbytes);
						d_target_queue->insert_tail(msg);
						msg.reset();

						debug(1, "frame successful decoded and send as "
							"message to queue: %d bytes", d_nbytes);
					}

					// restart state machine
					d_state = STATE_RESET;
				}
				break;
		}
    }

    fs20_frame_decoder_b::sptr
    fs20_frame_decoder_b::make(gr_msg_queue_sptr target_queue, int debug)
    {
      return gnuradio::get_initial_sptr (
        new fs20_frame_decoder_b_impl(target_queue, debug));
    }

    /*
     * The private constructor
     */
    fs20_frame_decoder_b_impl::fs20_frame_decoder_b_impl(
		gr_msg_queue_sptr target_queue, int debug)
      : gr_sync_block("fs20_frame_decoder_b",
		      gr_make_io_signature(1, 1, sizeof (char)),
		      gr_make_io_signature(0, 0, 0)),
	  	d_target_queue(target_queue),
		d_debug(debug),
		d_state(STATE_RESET)
    {
	}

    /*
     * Our virtual destructor.
     */
    fs20_frame_decoder_b_impl::~fs20_frame_decoder_b_impl()
    {
    }

    int
    fs20_frame_decoder_b_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];
		
		for(int i = 0; i < noutput_items; i++) {
			state_machine(in[i] & 0x01);
		}

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }


  } /* namespace fs20 */
} /* namespace gr */

