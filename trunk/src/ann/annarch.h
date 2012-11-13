
#ifndef __S_ANN_ARCH_H__
#define __S_ANN_ARCH_H__

#include "annlayer.h"

#pragma pack(push, 1)

typedef struct
{
	SAnnLayer iInputLayer;
	SAnnLayer iHiddenLayer;
	SAnnLayer iOutputLayer;

	double * iTargetSet;

	double ** iWeightList[2];
	double ** iWeightDeltaList[2];
	double * iBiasList[2];
	double * iBiasDeltaList[2];
	double iAlpha;
	double iError;

} SAnnArch;

#pragma pack(pop)

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

extern void annarch_create(SAnnArch * aArch,
	unsigned short aInputCount, unsigned short aHiddenCount, unsigned short aOutputCount, double aAlpha, double aError);

extern void annarch_free(SAnnArch * aArch);

extern void annarch_query(SAnnArch * aArch, double * aInput, double * aOutput);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __S_ANN_ARCH_H__

