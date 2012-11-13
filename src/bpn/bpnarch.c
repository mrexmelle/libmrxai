
#include <math.h>
#include <stdlib.h>
#include "bpnarch.h"
#include "bpnveformula.h"

void bpnarch_create(SBpnArch * aArch,
	unsigned short aNodeCount)
{
	// vars
	unsigned short i;

	// set count of nodes
	aArch->iNodeCount=aNodeCount;

	// alloc nodes as much as count
	aArch->iNodes=(SBpnNode*)malloc(aNodeCount*sizeof(SBpnNode));

	for(i=0; i<aNodeCount; i++)
	{
		bpnnode_create(&aArch->iNodes[i], i);
	}
}

void bpnarch_connect(SBpnArch * aArch,
	unsigned short * aConnectionList, unsigned short aPairCount)
{
	// vars
	unsigned short i;

	// reset trigger and impact count of each node
	for(i=0; i<aArch->iNodeCount; i++)
	{
		aArch->iNodes[i].iTriggerCount=0;
		aArch->iNodes[i].iImpactCount=0;
	}

	// set trigger and impact based on connection list input
	for(i=0; i<2*aPairCount; i++)
	{
		if(i%2==0)
		{
			aArch->iNodes[aConnectionList[i]].iImpactCount++;
		}
		else
		{
			aArch->iNodes[aConnectionList[i]].iTriggerCount++;
		}
	}

	// provide proper size of array of pointer for each node
	for(i=0; i<aArch->iNodeCount; i++)
	{
		if(aArch->iNodes[i].iTriggerCount>0)
		{
			aArch->iNodes[i].iTriggers=(SBpnNode**)malloc(aArch->iNodes[i].iTriggerCount*sizeof(SBpnNode*));
		}

		if(aArch->iNodes[i].iImpactCount>0)
		{
			aArch->iNodes[i].iImpacts=(SBpnNode**)malloc(aArch->iNodes[i].iImpactCount*sizeof(SBpnNode*));
		}
	}

	// reset counters so we can use it as incrementing index
	for(i=0; i<aArch->iNodeCount; i++)
	{
		aArch->iNodes[i].iTriggerCount=0;
		aArch->iNodes[i].iImpactCount=0;
	}

	// connect trigger and impact nodes
	for(i=0; i<2*aPairCount; i++)
	{
		if(i%2==0)
		{
			aArch->iNodes[aConnectionList[i]].iImpacts[aArch->iNodes[aConnectionList[i]].iImpactCount++]=
				&aArch->iNodes[aConnectionList[i+1]];
		}
		else
		{
			aArch->iNodes[aConnectionList[i]].iTriggers[aArch->iNodes[aConnectionList[i]].iTriggerCount++]=
				&aArch->iNodes[aConnectionList[i-1]];
		}
	}
}

void bpnarch_clear_visited_flags(SBpnArch * aArch)
{
	// vars
	unsigned short i;

	for(i=0; i<aArch->iNodeCount; i++)
	{
		aArch->iNodes[i].iVisited=0;		
	}
}

void bpnarch_query_probability(SBpnArch * aArch,
	unsigned short * aIndexList, unsigned short aIndexCount, double * aProbability)
{
	// vars
	unsigned short i;
	double inProb=1.0;

	if(aIndexCount==0) { *aProbability=1.0; return; }

	*aProbability=1.0;

	bpnarch_clear_visited_flags(aArch);
	for(i=0; i<aIndexCount; i++)
	{
		bpnnode_count_probability(&(aArch->iNodes[aIndexList[i]]), &inProb);
//		printf("bpnarch_count_probability - inProb: %lf\n", inProb);
		*aProbability*=inProb;
	}
	bpnarch_clear_visited_flags(aArch);
}

void bpnarch_query_full_joint_distribution(SBpnArch * aArch,
	double * aProbability)
{
	// vars
	unsigned short i;
	double inProb=1.0;

	*aProbability=1.0;

	bpnarch_clear_visited_flags(aArch);
	for(i=0; i<aArch->iNodeCount; i++)
	{
		bpnnode_count_probability(&(aArch->iNodes[i]), &inProb);
//		printf("bpnarch_count_probability - inProb: %lf\n", inProb);
		*aProbability*=inProb;
	}
	bpnarch_clear_visited_flags(aArch);	
}

void bpnarch_query_using_variable_elimination(SBpnArch * aArch, unsigned short aQueriedId,
	unsigned short * aEvidenceList, unsigned char * aEvidenceValue, unsigned short aEvidenceCount,
	double * aFalseResult, double * aTrueResult)
{
	// vars
	int i, j, k;
	SBpnVeFormula vef;
	SBpnCpt cpt;

	// create formula
	bpnveformula_create(&vef, aArch->iNodeCount);

	// create temp cpt
	bpncpt_create(&cpt, NULL, 0);
	
	// set cpt list of vef
	for(i=0; i<vef.iCptCount; i++)
	{
		bpnveformula_assign_cpt(&vef, i, aArch->iNodes[i].iCpt);
	}

	// send all id into the array, bpnveformula_query will later filter unnecessary inclusion
	unsigned short * nodeList=(unsigned short *)malloc(aArch->iNodeCount*sizeof(unsigned short));
	for(i=0; i<aArch->iNodeCount; i++)
	{
		nodeList[i]=aArch->iNodes[i].iId;
	}

	// run the inference engine
	bpnveformula_query(&vef, aQueriedId,
		aEvidenceList, aEvidenceValue, aEvidenceCount,
		nodeList, aArch->iNodeCount, &cpt);

	// store the result
	if(aFalseResult) { *aFalseResult=cpt.iTable[0]; }
	if(aTrueResult) { *aTrueResult=cpt.iTable[1]; }

	// clean-up
	free(nodeList);
	bpncpt_free(&cpt);
	bpnveformula_free(&vef);
}

void bpnarch_free(SBpnArch * aArch)
{
	// vars
	unsigned short i;

	for(i=0; i<aArch->iNodeCount; i++)
	{
		bpnnode_free(&aArch->iNodes[i]);
	}

	// free nodes
	if(aArch->iNodes)
	{
		free(aArch->iNodes);
		aArch->iNodes=NULL;
	}

	// reset counter to 0
	aArch->iNodeCount=0;
}

