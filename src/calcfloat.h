#ifndef LFFT_CALCFLOAT_H
#define LFFT_CALCFLOAT_H

typedef struct llft_plan_float_complex  llft_plan_float_complex;
typedef struct llft_plan_float_real     llft_plan_float_real;

llft_plan_float_complex* lfft_create_plan_float_complex(int N);

llft_plan_float_real*    lfft_create_plan_float_real(int N);

void lfft_exec_float_fft_c2c(const llft_plan_float_complex* plan, const float* input, float* output);

void lfft_exec_float_ifft_c2c(const llft_plan_float_complex* plan, const float* input, float* output);

void lfft_exec_float_fft_r2c(const llft_plan_float_real* plan, const float* input, float* output);

void lfft_exec_float_ifft_c2r(const llft_plan_float_real* plan, const float* input, float* output);

#endif /* LFFT_CALCFLOAT_H */
