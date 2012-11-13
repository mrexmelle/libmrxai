
#ifndef __S_BPN_NODE_H__
#define __S_BPN_NODE_H__

#include "bpncpt.h"

#pragma pack(push, 1)

struct SBpnNode_
{
	unsigned short iId;

	// for tracing purpose
	char iActive;
	char iVisited;

	unsigned char iTriggerCount;
	struct SBpnNode_ ** iTriggers;

	unsigned char iImpactCount;	
	struct SBpnNode_ ** iImpacts;

	SBpnCpt * iCpt;
};

#pragma pack(pop)

typedef struct SBpnNode_ SBpnNode;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

extern void bpnnode_create(SBpnNode * aNode, unsigned short aId);
extern void bpnnode_free(SBpnNode * aNode);
extern void bpnnode_attach_cpt(SBpnNode * aNode, SBpnCpt * aCpt);
extern int bpnnode_count_probability(SBpnNode * aNode, double * aProbability);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __S_BPN_NODE_H__

