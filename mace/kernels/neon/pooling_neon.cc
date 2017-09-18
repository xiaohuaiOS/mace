//
// Copyright (c) 2017 XiaoMi All rights reserved.
//

#include "mace/kernels/pooling.h"
#include <arm_neon.h>
#include "mace/kernels/conv_pool_2d_util.h"

namespace mace {
namespace kernels {

extern void PoolingMaxNeonK2x2S2x2(const float *input,
                                   const index_t *in_shape,
                                   float *output,
                                   const index_t *out_shape,
                                   const int *paddings);

extern void PoolingAvgNeonK2x2S2x2(const float *input,
                                   const index_t *in_shape,
                                   float *output,
                                   const index_t *out_shape,
                                   const int *paddings);

extern void PoolingMaxNeonK3x3S2x2(const float *input,
                                   const index_t *in_shape,
                                   float *output,
                                   const index_t *out_shape,
                                   const int *paddings);

extern void PoolingAvgNeonK3x3S2x2(const float *input,
                                   const index_t *in_shape,
                                   float *output,
                                   const index_t *out_shape,
                                   const int *paddings);

#ifdef __COPY_MAKE_PADDING
extern void PoolingMaxNeonK2x2S2x2Padded(const float *input,
                                         const index_t *in_shape,
                                         float *output,
                                         const index_t *out_shape);

extern void PoolingAvgNeonK2x2S2x2Padded(const float *input,
                                         const index_t *in_shape,
                                         float *output,
                                         const index_t *out_shape);

extern void PoolingMaxNeonK3x3S2x2Padded(const float *input,
                                         const index_t *in_shape,
                                         float *output,
                                         const index_t *out_shape);

extern void PoolingAvgNeonK3x3S2x2Padded(const float *input,
                                         const index_t *in_shape,
                                         float *output,
                                         const index_t *out_shape);
#endif

template<>
void PoolingFunctor<DeviceType::NEON, float>::operator()(
    const float *input,
    const index_t *input_shape,
    float *output,
    const index_t *output_shape) {
#ifdef __COPY_MAKE_PADDING
  Tensor padded_input;
  ConstructInputWithPadding(input, input_shape, paddings_, &padded_input);
  input = padded_input.data<float>();
  input_shape = padded_input.shape().data();
#endif

  if (kernels_[0] == 2 && kernels_[1] == 2 && strides_[0] == 2 &&
      strides_[1] == 2) {
    // kernel_size: 2x2, strides: 2x2
    if (pooling_type_ == MAX) { // MAX_POOL_2x2s2x2
#ifdef __COPY_MAKE_PADDING
      PoolingMaxNeonK2x2S2x2Padded(input, input_shape, output, output_shape);
#else
      PoolingMaxNeonK2x2S2x2(input, input_shape, output, output_shape,
                             paddings_);
#endif
    } else { // AVG_POOL_2x2s2x2
#ifdef __COPY_MAKE_PADDING
      PoolingAvgNeonK2x2S2x2Padded(input, input_shape, output, output_shape);
#else
      PoolingAvgNeonK2x2S2x2(input, input_shape, output, output_shape,
                             paddings_);
#endif
    }
  } else if (kernels_[0] == 3 && kernels_[1] == 3 && strides_[0] == 2 &&
      strides_[1] == 2) {
    // kernel_size: 3x3, strides: 2x2
    if (pooling_type_ == MAX) { // MAX_POOL_3x3s2x2
#ifdef __COPY_MAKE_PADDING
      PoolingMaxNeonK3x3S2x2Padded(input, input_shape, output, output_shape);
#else
      PoolingMaxNeonK3x3S2x2(input, input_shape, output, output_shape,
                             paddings_);
#endif
    } else { // AVG_POOL_3x3s2x2
#ifdef __COPY_MAKE_PADDING
      PoolingAvgNeonK3x3S2x2Padded(input, input_shape, output, output_shape);
#else
      PoolingAvgNeonK3x3S2x2(input, input_shape, output, output_shape,
                             paddings_);
#endif
    }
  } else {  // not implement yet
    PoolingFunctor<DeviceType::CPU, float>(pooling_type_, kernels_, strides_,
                                           paddings_, dilations_)(
        input, input_shape, output, output_shape);
  }
}

}  //  namespace kernels
}  //  namespace mace