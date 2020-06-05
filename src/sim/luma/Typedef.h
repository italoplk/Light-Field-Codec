
#ifndef TYPEDEF_H
#define TYPEDEF_H

#define USE_YCbCr 1 // 0: no, 1: mule, 2: other

#define LFSample short

#define TRANSF_QUANT                true
#define QUANTIZATION                false
#define DPCM_DC                     QUANTIZATION
#define STATISTICS_LOCAL            true
#define STATISTICS_GLOBAL           false
#define STATISTICS_TIME             false
#define TRACE_TRANSF                false
#define TRACE_QUANT                 false
#define TRACE_LRE                   false
#define LFCODEC_USE_PREDICTION      false
#define LFCODEC_FORCE_DCT_NON_LUMA  true
#define LFCODEC_USE_BLOCK_SEG       false
#define LFCODEC_BLOCK_SEG_SIZE      1


#endif //TYPEDEF_H
