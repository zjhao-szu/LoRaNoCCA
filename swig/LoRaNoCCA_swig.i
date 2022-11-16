/* -*- c++ -*- */

#define LORANOCCA_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "LoRaNoCCA_swig_doc.i"

%{
#include "LoRaNoCCA/NoCCANode.h"
%}

%include "LoRaNoCCA/NoCCANode.h"
GR_SWIG_BLOCK_MAGIC2(LoRaNoCCA, NoCCANode);
