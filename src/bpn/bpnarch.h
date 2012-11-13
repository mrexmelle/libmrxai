
#ifndef __S_BPN_ARCH_H__
#define	__S_BPN_ARCH_H__

#include "bpnnode.h"

#pragma pack(push, 1)

typedef struct 
{
	SBpnNode * iNodes;
	unsigned short iNodeCount;
} SBpnArch;

#pragma pack(pop)

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

extern void bpnarch_create(SBpnArch * aArch,
	unsigned short aNodeCount);

extern void bpnarch_connect(SBpnArch * aArch,
	unsigned short * aConnectionList, unsigned short aPairCount);

extern void bpnarch_clear_visited_flags(SBpnArch * aArch);

extern void bpnarch_query_probability(SBpnArch * aArch,
	unsigned short * aIndexList, unsigned short aIndexCount, double * aProbability);

extern void bpnarch_query_full_joint_distribution(SBpnArch * aArch,
	double * aProbability);

extern void bpnarch_query_using_variable_elimination(SBpnArch * aArch, unsigned short aQueriedId,
	unsigned short * aEvidenceList, unsigned char * aEvidenceValue, unsigned short aEvidenceCount,
	double * aFalseResult, double * aTrueResult);

extern void bpnarch_free(SBpnArch * aArch);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif	// __S_BPN_ARCH_H__

