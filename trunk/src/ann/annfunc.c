
#include <math.h>
#include "annfunc.h"

#define KNaturalNumber	2.71828182845904523536028747135266249775724709369995

unsigned char annfunc_step(double aInput, double aThreshold)
{
	return aInput>=aThreshold;
}

unsigned char annfunc_sign(double aInput)
{
	return aInput>=0;
}

double annfunc_identity(double aInput)
{
	return aInput;
}

double annfunc_sigmoid_unipolar(double aInput, double(*aFunction)(double))
{
	return 1.0 / (1.0+exp(-aFunction(aInput)));
}

double annfunc_sigmoid_bipolar(double aInput, double(*aFunction)(double))
{
	return (1.0-exp(-aFunction(aInput))) / (1.0+exp(-aFunction(aInput)));
}

double annfunc_sigmoid_unipolar_use_identity(double aInput)
{
	return 1.0 / (1.0+exp(-aInput));
}

double annfunc_sigmoid_bipolar_use_identity(double aInput)
{
	return (1.0-exp(-aInput)) / (1.0+exp(aInput));
}

// f'(x)=f(x)(1-f(x))
// after forwarded using sigmoid function at forward process:
// f'(y_in)=f(y_in)(1-f(y_in))
// f'(y_in)=y_out(1-y_out)
double annfunc_derivated_sigmoid_unipolar(double aInput)
{
	return aInput*(1-aInput);
}

