/* -*- c++ -*- */
/*
 * Copyright 2026 Takakura.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_MODE_S_DECODE_PREAMBLE_CORRELATION_IMPL_H
#define INCLUDED_MODE_S_DECODE_PREAMBLE_CORRELATION_IMPL_H

#include <gnuradio/mode_s_decode/preamble_correlation.h>

namespace gr {
  namespace mode_s_decode {

    class preamble_correlation_impl : public preamble_correlation
    {
     private:
      // Nothing to declare in this block.
      constexpr std::array<float, 32> preamble = {
        +1,+1,-1,-1,
        +1,+1,-1,-1,
        -1,-1,-1,-1,
        -1,-1,+1,+1,
        -1,-1,+1,+1,
        -1,-1,-1,-1,
        -1,-1,-1,-1,
        -1,-1,-1,-1
      };
     public:
      preamble_correlation_impl();
      ~preamble_correlation_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace mode_s_decode
} // namespace gr

#endif /* INCLUDED_MODE_S_DECODE_PREAMBLE_CORRELATION_IMPL_H */
