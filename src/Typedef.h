
#ifndef TYPEDEF_H
#define TYPEDEF_H

#define USE_YCbCr 1 // 0: no, 1: mule, 2: other

#define LFSample short

#ifndef TRANSF_QUANT
#define TRANSF_QUANT true
#endif
#ifndef QUANTIZATION
#define QUANTIZATION true
#endif
#ifndef DPCM_DC
#define DPCM_DC true
#endif
#ifndef STATISTICS_LOCAL
#define STATISTICS_LOCAL false
#endif
#ifndef STATISTICS_GLOBAL
#define STATISTICS_GLOBAL false
#endif
#ifndef STATISTICS_TIME
#define STATISTICS_TIME false
#endif
#ifndef TRACE_TRANSF
#define TRACE_TRANSF false
#endif
#ifndef TRACE_QUANT
#define TRACE_QUANT false
#endif
#ifndef TRACE_LRE
#define TRACE_LRE false
#endif
#ifndef LFCODEC_USE_PREDICTION
#define LFCODEC_USE_PREDICTION true
#endif
#ifndef LFCODEC_FORCE_DCT_NON_LUMA
#define LFCODEC_FORCE_DCT_NON_LUMA true
#endif
#ifndef LFCODEC_TRANSFORM_HISTOGRAM
#define LFCODEC_TRANSFORM_HISTOGRAM false
#endif



#endif // TYPEDEF_H
