
#ifndef __S_BPN_VE_FORMULA_H__
#define __S_BPN_VE_FORMULA_H__

#include "bpncpt.h"

#pragma pack(push, 1)

typedef struct 
{
	unsigned short iCptCount;
	SBpnCpt ** iCptList;
} SBpnVeFormula;

#pragma pack(pop)

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// public functions
extern int bpnveformula_create(SBpnVeFormula * aVeFormula, unsigned short aCptCount);
extern void bpnveformula_free(SBpnVeFormula * aVeFormula);
extern void bpnveformula_assign_cpt(SBpnVeFormula * aVeFormula, unsigned short aCptIndex, SBpnCpt * aCpt);
extern int bpnveformula_query(SBpnVeFormula * aVeFormula, unsigned short aQueriedId,
	unsigned short * aEvidenceList, unsigned char * aEvidenceValue, unsigned short aEvidenceCount,
	unsigned short * aSumOverSequence, unsigned short aSumOverCount, SBpnCpt * aResult);
// private functions
extern int bpnveformula__eliminate_exact_evidence(SBpnVeFormula * aVeFormula, unsigned short aId, unsigned char aValue);
extern int bpnveformula__sum_over(SBpnVeFormula * aVeFormula, unsigned short aId);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __S_BPN_VE_FORMULA_H__

