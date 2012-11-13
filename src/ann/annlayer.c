
#include <stdlib.h>
#include "annfunc.h"
#include "annlayer.h"

void annlayer_create(SAnnLayer * aAnnLayer, unsigned short aNodeCount)
{
	// vars
	unsigned short i;

	// set count of nodes
	aAnnLayer->iNodeCount=aNodeCount;

	// alloc nodes as much as count
	aAnnLayer->iNodes=(SAnnNode*)malloc(aNodeCount*sizeof(SAnnNode));

	// add function to activate and inv activate
	for(i=0; i<aNodeCount; i++)
	{
		aAnnLayer->iNodes[i].iActivationFunction=annfunc_sigmoid_unipolar_use_identity;
		aAnnLayer->iNodes[i].iDerivatedActivationFunction=annfunc_derivated_sigmoid_unipolar;
	}
}

void annlayer_free(SAnnLayer * aAnnLayer)
{
	free(aAnnLayer->iNodes);
	aAnnLayer->iNodes=NULL;

	aAnnLayer->iNodeCount=0;
}

