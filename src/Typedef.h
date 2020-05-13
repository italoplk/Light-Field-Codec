
#ifndef TYPEDEF_H
#define TYPEDEF_H

#ifndef USE_YCbCr
#define USE_YCbCr 1 // 0: no, 1: mule, 2: other
#endif

#define LFSample short

#ifdef DONT_COMPILE
#error "Should not compile"
#endif


#ifndef TRANSF_QUANT
#define TRANSF_QUANT true
#endif

#ifndef QUANTIZATION
#define QUANTIZATION true
#endif

#ifndef STATISTICS_LOCAL
#define STATISTICS_LOCAL true
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

#ifndef DPCM_DC
#define DPCM_DC true
#endif

#ifndef CALCULATE_PSNR
#define CALCULATE_PSNR false
#endif

// #define STR_HELPER(x) #x
// #define STR(x) #x " " STR_HELPER(x)
//
// #pragma message "DEFINITIONS: " STR(TRANSF_QUANT)
// #pragma message "DEFINITIONS: " STR(QUANTIZATION)
// #pragma message "DEFINITIONS: " STR(STATISTICS_LOCAL)
// #pragma message "DEFINITIONS: " STR(STATISTICS_GLOBAL)
// #pragma message "DEFINITIONS: " STR(STATISTICS_TIME)
// #pragma message "DEFINITIONS: " STR(TRACE_TRANSF)
// #pragma message "DEFINITIONS: " STR(TRACE_QUANT)
// #pragma message "DEFINITIONS: " STR(TRACE_LRE)
// #pragma message "DEFINITIONS: " STR(DPCM_DC)
// #pragma message "DEFINITIONS: " STR(CALCULATE_PSNR)

#endif // TYPEDEF_H
