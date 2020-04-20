#include <iostream>

#include "EncoderParameters.h"
#include "Quantization.h"
#include "Transform.h"
#include "LightField.h"
#include "Statistics.h"
#include "LRE.h"
#include "DpcmDC.h"
#include "EncBitstreamWriter.h"
#include "Typedef.h"
#include "Time.h"

using namespace std;

template <class T>
void printVector(const std::string &msg, T *vet, int size)
{
    std::cout << std::endl
              << msg << std::endl;
    for (int i = 0; i < size; ++i)
    {
        std::cout << vet[i] << "\t";
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    EncoderParameters encoderParameters;
    encoderParameters.parse_cli(argc, argv);
    encoderParameters.report();

    LightField lf(encoderParameters.dim_LF, encoderParameters.getPathInput(),
                  encoderParameters.isLytro());

    float orig4D[encoderParameters.dim_block.getNSamples()],
        ti4D[encoderParameters.dim_block.getNSamples()],
        qf4D[encoderParameters.dim_block.getNSamples()],
        tf4D[encoderParameters.dim_block.getNSamples()],
        qi4D[encoderParameters.dim_block.getNSamples()];

    int temp_lre[encoderParameters.dim_block.getNSamples()];
    uint bits_per_4D_Block = 0;

    Transform transform(encoderParameters.dim_block);
    Quantization quantization(encoderParameters.dim_block, encoderParameters.getQp(),
                              encoderParameters.quant_weight_100);
    // TODO: Entropy entropy(...)
    LRE lre(encoderParameters.dim_block.getNSamples() ==
            15 * 15 * 15 * 15); //todo: fixed block size (15x15x15x15) or (15x15x13x13)
#if DPCM_DC
    DpcmDC dpcmDc[3]{{encoderParameters.dim_LF.x},
                     {encoderParameters.dim_LF.x},
                     {encoderParameters.dim_LF.x}};
#endif

    EncBitstreamWriter encoder(&encoderParameters, 10000000);
    //encoder.writeHeader();

    std::string sep = ",";



    const Point4D dimLF = encoderParameters.dim_LF;

    Point4D it_pos, dimBlock, stride_lf, stride_block;

    for (it_pos.v = 0; it_pos.v < dimLF.v; it_pos.v += dimBlock.v)
    { // angular
        for (it_pos.u = 0; it_pos.u < dimLF.u; it_pos.u += dimBlock.u)
        {

            for (it_pos.y = 0; it_pos.y < dimLF.y; it_pos.y += dimBlock.y)
            { // spatial
                for (it_pos.x = 0; it_pos.x < dimLF.x; it_pos.x += dimBlock.x)
                {

                    dimBlock = Point4D(std::min(encoderParameters.dim_block.x, dimLF.x - it_pos.x),
                                       std::min(encoderParameters.dim_block.y, dimLF.y - it_pos.y),
                                       std::min(encoderParameters.dim_block.u, dimLF.u - it_pos.u),
                                       std::min(encoderParameters.dim_block.v, dimLF.v - it_pos.v));

                    stride_lf = Point4D(1, dimLF.x - dimBlock.x,
                                        dimLF.x * (dimLF.y - dimBlock.y),
                                        dimLF.x * dimLF.y * (dimLF.u - dimBlock.u));

                    stride_block = Point4D(1, encoderParameters.dim_block.x - dimBlock.x,
                                           (encoderParameters.dim_block.y - dimBlock.y) *
                                               encoderParameters.dim_block.x,
                                           (encoderParameters.dim_block.u - dimBlock.u) *
                                               encoderParameters.dim_block.x * encoderParameters.dim_block.y);

                    std::cout << "Pos: " << it_pos << "\tBlock Size: " << dimBlock << std::endl;
                    for (int i = 0; i < encoderParameters.dim_block.getNSamples(); ++i)
                    {
                        orig4D[i] = tf4D[i] = qf4D[i] = 0;
                    }

                    for (int it_channel = 0; it_channel < 3; ++it_channel)
                    {

                        lf.getBlock(orig4D, it_pos, dimBlock, stride_block, encoderParameters.dim_block, stride_lf,
                                    it_channel);

#if TRANSF_QUANT

                        transform.dct_4d(orig4D, tf4D, dimBlock, encoderParameters.dim_block);


#if QUANTIZATION

                        quantization.foward(tf4D, qf4D);

#if DPCM_DC
                        qf4D[0] -= (float)dpcmDc[it_channel].get_reference(it_pos.x, it_pos.y);
#endif

                        for (int i = 0; i < encoderParameters.dim_block.getNSamples(); ++i)
                        {
                            temp_lre[i] = (int)std::trunc(qf4D[i]);
                        }

                        auto lre_result = lre.encodeCZI(temp_lre, 0, encoderParameters.dim_block.getNSamples());

                        bits_per_4D_Block = encoder.write4DBlock(temp_lre, encoderParameters.dim_block.getNSamples(), lre_result);

                        // TODO: entropy.encode

#if DPCM_DC
                        qf4D[0] += (float)dpcmDc[it_channel].get_reference(it_pos.x, it_pos.y);
#endif
                        quantization.inverse(qf4D, qi4D);

#else /* NO_QUANTIZATION */
                        std::copy(tf4D, tf4D + encoderParameters.dim_block.getNSamples(), qi4D);
#endif

                        transform.idct_4d(qi4D, ti4D, dimBlock, encoderParameters.dim_block);

#else /* NO_TRANSF_QUANT */
                        std::copy(orig4D, orig4D + dimBlock.getNSamples(), ti4D);
#endif

                        lf.rebuild(ti4D, it_pos, dimBlock, stride_block, encoderParameters.dim_block, stride_lf,
                                   it_channel);

#if DPCM_DC
                        dpcmDc[it_channel].update((int)qf4D[0], true);
#endif
                        encoder.write_completedBytes();
                    }
                }
            }
        }
    }

    lf.write(encoderParameters.getPathOutput());
    encoder.finish_and_write();
    encoder.~EncBitstreamWriter();

    cout << "\n#########################################################" << endl;
    cout << "Total bytes:\t" << encoder.getTotalBytes() << endl;
    cout << "#########################################################" << endl;

    return 0;
}