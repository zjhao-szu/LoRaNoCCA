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

#ifndef INCLUDED_LORANOCCA_NOCCANODE_IMPL_H
#define INCLUDED_LORANOCCA_NOCCANODE_IMPL_H

#include <LoRaNoCCA/NoCCANode.h>
#include <vector>
#include <iostream>

namespace gr {
  namespace LoRaNoCCA {

    class NoCCANode_impl : public NoCCANode
    {
     private:
      NODEPHASE m_phase;
      NODESTATE m_state;
      pmt::pmt_t m_outputData;
      pmt::pmt_t m_inputRTS;
      pmt::pmt_t m_inputData;
      int DIFSLength;
      int DIFSWindowCount;
      int ListenRTSLength;
      float probablity;
      int NAVLength;
      int m_nodeId;
      std::vector<std::string> messages;
     public:
      NoCCANode_impl(uint32_t NodeID);
      ~NoCCANode_impl();

      void receiveDataMessage(pmt::pmt_t msg);
      void receiveRTSMessage(pmt::pmt_t msg);
      std::vector<std::string> parseRTSMessage(std::string msgString);
      void sendRTSPacket();
      void sendDataPacket();
      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

    };

  } // namespace LoRaNoCCA
} // namespace gr

#endif /* INCLUDED_LORANOCCA_NOCCANODE_IMPL_H */

