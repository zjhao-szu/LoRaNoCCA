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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "NoCCANode_impl.h"

#define MAX 99 //精度为小数点后面1位 999为小数点后面3位
#define PROBABLITY 0.1
#define DIFSWINDOWCOUNT 1000
#define LISTENRTS 30
#define DIFSLENGTH 1
namespace gr {
  namespace LoRaNoCCA {

    NoCCANode::sptr
    NoCCANode::make(uint32_t NodeID)
    {
      return gnuradio::get_initial_sptr
        (new NoCCANode_impl(NodeID));
    }


    /*
     * The private constructor
     */
    NoCCANode_impl::NoCCANode_impl(uint32_t NodeID)
      : gr::block("NoCCANode",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(0, 0, 0)),
              m_nodeId(NodeID)
    {
      m_state = STATE_RESET;
      std::cout<<"NoCCA Node "<<NodeID<<" init!!!!!"<<std::endl;
      m_outputData = pmt::mp("outputData");
      m_inputRTS = pmt::mp("InputRTS");
      m_inputData = pmt::mp("InputData");

      message_port_register_in(m_inputRTS);
      message_port_register_in(m_inputData);
      message_port_register_out(m_outputData);
      

      set_msg_handler(m_inputRTS,boost::bind(&NoCCANode_impl::receiveRTSMessage,this,_1));
      set_msg_handler(m_inputData,boost::bind(&NoCCANode_impl::receiveDataMessage,this,_1));
      srand(time(NULL));
    
      probablity = PROBABLITY;
      
      int p = rand() % (MAX + 1) / (float)(MAX + 1);
      if(p > (1- probablity)){
        m_phase = STATE_PHASE2;
      }else{
        m_phase = STATE_PHASE1;
      }

      DIFSLength = 1;
      DIFSWindowCount = DIFSWINDOWCOUNT;
      ListenRTSLength = DIFSWindowCount * DIFSLength + LISTENRTS;

      NAVLength = -1;
    }

    /*
     * Our virtual destructor.
     */
    NoCCANode_impl::~NoCCANode_impl()
    {
      
    }

    std::vector<std::string> 
    NoCCANode_impl::parseRTSMessage(std::string msgString){
      //message 格式如下：
      //Type:RTS,NodeId:1,Duration:10

      std::vector<std::string> res;
      std::string str;
      //获取TYPE
      int indexColon = msgString.find(":");
      int indexComma = msgString.find(",");
      res.push_back(msgString.substr(indexColon+1,indexComma - indexColon - 1));

      //获取NODEID
      indexColon = msgString.find(":",indexComma);
      indexComma = msgString.find(",",indexColon);
      res.push_back(msgString.substr(indexColon+1,indexComma - indexColon - 1));

      //获取Duration
      indexColon = msgString.find(":",indexComma);
      res.push_back(msgString.substr(indexColon+1,msgString.length() - indexColon - 1));
      
      return res;
    }
    void
    NoCCANode_impl::receiveDataMessage(pmt::pmt_t msg){
      //自动添加 如果messsages有数据且在reset状态，就会自动切换寻找
      messages.push_back(pmt::symbol_to_string(msg));
      std::cout<<"user need send data: "<<pmt::symbol_to_string(msg)<<std::endl;
    }
    void 
    NoCCANode_impl::receiveRTSMessage(pmt::pmt_t msg){
      if(m_state != STATE_LISTEN){
        return;
      }

      std::string msgString = pmt::symbol_to_string(msg);
      //message 格式如下：
      //Type:RTS,NodeId:1,Duration:10
      if(msgString.find("RTS") != std::string::npos){
        //HEADER 包括20bytes
        std::vector<std::string> res = parseRTSMessage(msgString);        
        NAVLength = std::stoi(res[2]) + 10;
        m_state = STATE_NAV;
      }else{
        std::cout<<"Receive DATA:: "<<msgString<<std::endl;
      }
    }

    void
    NoCCANode_impl::sendRTSPacket(){
      int totalLen = 0;
      for(int i = 0;i < messages.size();i++){
        totalLen += messages[i].length();
        totalLen += 1;//添加一个换行符号
      }

      //class A采用先来先服务策略
      std::string message = "Type:RTS,NodeId:"+std::to_string(m_nodeId)+",Duration:"+std::to_string(totalLen);
      pmt::pmt_t pmtmsg = pmt::string_to_symbol(message);
      message_port_pub(m_outputData,pmtmsg);
    }
    void
    NoCCANode_impl::sendDataPacket(){
      std::string msg;
      int totalLen = 0;
      for(int i = 0;i < messages.size();i++){
        totalLen += messages[i].length();
        msg.append(messages[i]);
        msg.push_back('\n');
      }
      messages.clear();
      pmt::pmt_t pmtmsg = pmt::string_to_symbol(msg);
      message_port_pub(m_outputData,pmtmsg);
    }

    void
    NoCCANode_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
      ninput_items_required[0] = noutput_items;
    }

    int
    NoCCANode_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      uint32_t * out = (uint32_t *) output_items[0];
      if (m_state == STATE_RESET){
        if(messages.size() > 0){
          std::cout<<"message size is "<<messages.size()<<std::endl;
          float p = rand() % (MAX + 1) / (float)( MAX + 1);
          std::cout<<"Probablity is "<<p<<std::endl;
          if(p > (1- probablity)){
            m_phase = STATE_PHASE2;
            std::cout<<"login PHASE 2"<<std::endl;
          }else{
            m_phase = STATE_PHASE1;
            std::cout<<"login PHASE 1"<<std::endl;
          } 
          NAVLength = ListenRTSLength;
          DIFSWindowCount = DIFSWINDOWCOUNT;
          DIFSLength = 1;
          ListenRTSLength = DIFSWindowCount * DIFSLength + LISTENRTS;
        }else{
           // Do <+signal processing+>
          // Tell runtime system how many input items we consumed on
          // each input stream.
          consume_each (0);

          // Tell runtime system how many output items we produced.
          return 0;
        }
      }else if(m_state == STATE_NAV){
        NAVLength--;
        if(NAVLength <= 0){
          m_state = STATE_RESET;
          NAVLength = ListenRTSLength;
        }
      }
      switch (m_phase)
      {
      
      case STATE_PHASE1:{
        m_state = STATE_LISTEN;
        switch (m_state)
        {
          case STATE_LISTEN:{
            ListenRTSLength--;
            if(ListenRTSLength <= 0){
              m_phase = STATE_PHASE2;
              m_state = STATE_DIFS;
            }
            break;
          }
          default:
            break;
        }
        break;
      }
      case STATE_PHASE2:{
        switch (m_state)
        {
          case STATE_DIFS:{
            DIFSWindowCount--;
            if(DIFSWindowCount  <=  0){
              m_state = STATE_RTS;
            }
            break;
          }
          case STATE_RTS:{
            std::cout<<"send RTS packet"<<std::endl;
            sendRTSPacket();
            m_state = STATE_LISTEN;
            ListenRTSLength = DIFSWindowCount * DIFSLength + LISTENRTS;
            break;
          }
          case STATE_LISTEN:{
            ListenRTSLength--;
            if(ListenRTSLength <= 0){
              m_phase = STATE_PHASE3;
              m_state = STATE_DIFS;
              DIFSWindowCount = DIFSWINDOWCOUNT;
              DIFSLength = DIFSLENGTH;
            }
            break;
          }
          default:
            break;
        }
        break;
      }
      case STATE_PHASE3:{
        switch (m_state)
        {
          case STATE_DIFS:{
            DIFSWindowCount--;
            if(DIFSWindowCount <= 0){
                m_state = STATE_DATA ;
            }
            break;
          }
          case STATE_DATA:{
            std::cout<<"send Data packet"<<std::endl;
            sendDataPacket();
            m_state = STATE_RESET;
          }
          default:
            break;
        }
        break;
      }
      default:
        break;
      }

      // Do <+signal processing+>
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (0);

      // Tell runtime system how many output items we produced.
      return 0;
    }

  } /* namespace LoRaNoCCA */
} /* namespace gr */

