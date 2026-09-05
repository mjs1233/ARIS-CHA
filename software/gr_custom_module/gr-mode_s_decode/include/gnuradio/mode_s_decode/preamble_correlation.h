/* -*- c++ -*- */
/*
 * Copyright 2026 Takakura.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_MODE_S_DECODE_PREAMBLE_CORRELATION_H
#define INCLUDED_MODE_S_DECODE_PREAMBLE_CORRELATION_H

#include <gnuradio/mode_s_decode/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace mode_s_decode {

    /*!
     * \brief <+description of block+>
     * \ingroup mode_s_decode
     *
     */
    class MODE_S_DECODE_API preamble_correlation : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<preamble_correlation> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of mode_s_decode::preamble_correlation.
       *
       * To avoid accidental use of raw pointers, mode_s_decode::preamble_correlation's
       * constructor is in a private implementation
       * class. mode_s_decode::preamble_correlation::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace mode_s_decode
} // namespace gr

#endif /* INCLUDED_MODE_S_DECODE_PREAMBLE_CORRELATION_H */
