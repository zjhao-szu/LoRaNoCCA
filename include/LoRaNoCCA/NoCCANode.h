/* -*- c++ -*- */
/*
 * Copyright 2022 zjhao.
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

#ifndef INCLUDED_LORANOCCA_NOCCANODE_H
#define INCLUDED_LORANOCCA_NOCCANODE_H

#include <LoRaNoCCA/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace LoRaNoCCA {

    /*!
     * \brief <+description of block+>
     * \ingroup LoRaNoCCA
     *
     */
    class LORANOCCA_API NoCCANode : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<NoCCANode> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of LoRaNoCCA::NoCCANode.
       *
       * To avoid accidental use of raw pointers, LoRaNoCCA::NoCCANode's
       * constructor is in a private implementation
       * class. LoRaNoCCA::NoCCANode::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace LoRaNoCCA
} // namespace gr

#endif /* INCLUDED_LORANOCCA_NOCCANODE_H */

