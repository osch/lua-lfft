#ifndef LFFT_CALCDOUBLE_H
#define LFFT_CALCDOUBLE_H

typedef struct llft_plan_double_complex  llft_plan_double_complex;
typedef struct llft_plan_double_real     llft_plan_double_real;

llft_plan_double_complex* lfft_create_plan_double_complex(int N);

llft_plan_double_real*    lfft_create_plan_double_real(int N);

void lfft_exec_double_fft_c2c(const llft_plan_double_complex* plan, const double* input, double* output);

void lfft_exec_double_ifft_c2c(const llft_plan_double_complex* plan, const double* input, double* output);

void lfft_exec_double_fft_r2c(const llft_plan_double_real* plan, const double* input, double* output);

void lfft_exec_double_ifft_c2r(const llft_plan_double_real* plan, const double* input, double* output);

#endif /* LFFT_CALCDOUBLE_H */
