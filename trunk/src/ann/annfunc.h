
#ifndef __ANN_FUNC_H__
#define __ANN_FUNC_H__

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

extern unsigned char annfunc_step(double aInput, double aThreshold);
extern unsigned char annfunc_sign(double aInput);
extern double annfunc_identity(double aInput);
extern double annfunc_sigmoid_unipolar(double aInput, double(*aFunction)(double));
extern double annfunc_sigmoid_bipolar(double aInput, double(*aFunction)(double));

extern double annfunc_sigmoid_unipolar_use_identity(double aInput);
extern double annfunc_sigmoid_bipolar_use_identity(double aInput);

extern double annfunc_derivated_sigmoid_unipolar(double aInput); 

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __ANN_FUNC_H__

