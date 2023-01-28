#include "calcdouble.h"

#define USE_DOUBLE_TYPE
#define STB_FFT_IMPLEMENTAION
#define STB_STATIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Walloc-size-larger-than="
#include "stb_fft.h"
#pragma GCC diagnostic pop

llft_plan_double_complex* lfft_create_plan_double_complex(int N)
{
    return (llft_plan_double_complex*)stb_fft_plan_dft_1d(N);
}

llft_plan_double_real* lfft_create_plan_double_real(int N)
{
    return (llft_plan_double_real*)stb_fft_real_plan_dft_1d(N);
}





void lfft_exec_double_fft_c2c(const llft_plan_double_complex* plan, const double* input, double* output)
{
    stb_fft_exec((const stb_fft_plan*)plan, (const cmplx*)input, (cmplx*)output);
}

void lfft_exec_double_ifft_c2c(const llft_plan_double_complex* plan, const double* input, double* output)
{
    stb_ifft_exec((const stb_fft_plan*)plan, (const cmplx*)input, (cmplx*)output);
}

void lfft_exec_double_fft_r2c(const llft_plan_double_real* plan, const double* input, double* output)
{
    stb_fft_r2c_exec((const stb_fft_real_plan*)plan, input, (cmplx*)output);
}

void lfft_exec_double_ifft_c2r(const llft_plan_double_real* plan, const double* input, double* output)
{
    stb_fft_c2r_exec((const stb_fft_real_plan*)plan, (const cmplx*)input, output);
}
