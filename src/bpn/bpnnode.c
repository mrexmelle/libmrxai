
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "bpnnode.h"

void bpnnode_create(SBpnNode * aNode, unsigned short aId)
{
	aNode->iId=aId;
	aNode->iActive=0;
	aNode->iVisited=0;
	aNode->iTriggers=NULL;
	aNode->iTriggerCount=0;
	aNode->iImpacts=NULL;
	aNode->iImpactCount=0;
	aNode->iCpt=NULL;
}

void bpnnode_free(SBpnNode * aNode)
{
	// free triggers
	aNode->iTriggerCount=0;
	if(aNode->iTriggers)
	{
		free(aNode->iTriggers);
		aNode->iTriggers=NULL;
	}

	// free impacts
	aNode->iImpactCount=0;
	if(aNode->iImpacts)
	{
		free(aNode->iImpacts);
		aNode->iImpacts=NULL;
	}

	// dettach cpt
	aNode->iCpt=NULL;
}

void bpnnode_attach_cpt(SBpnNode * aNode, SBpnCpt * aCpt)
{
	aNode->iCpt=aCpt;
}

int bpnnode_count_probability(SBpnNode * aNode, double * aProbability)
{
	// vars
	double prob=1.0;
	double inProb;
	unsigned int i, mapAddress;
	unsigned int mask;
	int containsOwnId;

	SBpnNode * traveler=aNode;
	if(traveler==NULL) { *aProbability=1.0; return 0; }

	if(traveler->iVisited)
	{
		*aProbability=1.0; return 0;
	}
	else
	{
		traveler->iVisited=1;
	}
//	printf("bpnnode_count_probability - node id: %lu\n", traveler->iId);	
//	printf("bpnnode_count_probability - trigger count: %hu\n", traveler->iTriggerCount);

	mapAddress=0x00000000;

	if(traveler->iActive)
	{
		containsOwnId=bpncpt_id_mask_in_list(traveler->iCpt, traveler->iId, &mask);
		if(containsOwnId<0) { return -1; }
		mapAddress|=mask;
	}

	for(i=0; i<traveler->iTriggerCount; i++)
	{
		if(traveler->iTriggers[i]->iActive)
		{
			bpncpt_id_mask_in_list(traveler->iCpt, traveler->iTriggers[i]->iId, &mask);
			mapAddress|=mask;
		}
	}
//	printf("bpnnode_count_probability - mapAddress: %hu\n", mapAddress);	
	
	prob*=traveler->iCpt->iTable[mapAddress];
//	printf("bpnnode_count_probability - prob: %lf\n", prob);

	for(i=0; i<traveler->iTriggerCount; i++)
	{
		traveler=traveler->iTriggers[i];
		bpnnode_count_probability(traveler, &inProb);
		traveler=aNode;
		prob*=inProb;
	}

	memcpy(aProbability, &prob, sizeof(double));

	return 0;
}

