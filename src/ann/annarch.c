
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "annarch.h"
#include "anntraining.h"

#define KMaxSignedShort	0x7FFF

void annarch_create(SAnnArch * aArch,
	unsigned short aInputCount, unsigned short aHiddenCount, unsigned short aOutputCount, double aAlpha, double aError)
{
	// vars
	unsigned short i;
	unsigned short j;

	// create layers [1]
	annlayer_create(&aArch->iInputLayer, aInputCount);
	annlayer_create(&aArch->iHiddenLayer, aHiddenCount);
	annlayer_create(&aArch->iOutputLayer, aOutputCount);

	// create target buffer and fill [2]
	aArch->iTargetSet=(double*)malloc(aOutputCount*sizeof(double));

	// initiate randomizer
	srand((unsigned int)time(NULL));

	// allocate weight and delta list from input->hidden and fill weights with random value from -1.0 - 1.0 [3]
	aArch->iWeightList[0]=(double**)malloc(aInputCount*sizeof(double*));
	aArch->iWeightDeltaList[0]=(double**)malloc(aInputCount*sizeof(double*));
	for(i=0; i<aInputCount; i++)
	{
		aArch->iWeightList[0][i]=(double*)malloc(aHiddenCount*sizeof(double));
		aArch->iWeightDeltaList[0][i]=(double*)malloc(aHiddenCount*sizeof(double));
		for(j=0; j<aHiddenCount; j++)
		{
			aArch->iWeightList[0][i][j]=(( (double)(rand()%KMaxSignedShort)/KMaxSignedShort )-0.5 ) * 2.0;
			aArch->iWeightDeltaList[0][i][j]=0.0;
//			printf("%lf\n", aArch->iWeightList[0][i][j]);
		}
	}

	// allocate weight and delta list from hidden->output and fill weights with random value from -1.0 - 1.0 [4]
	aArch->iWeightList[1]=(double**)malloc(aHiddenCount*sizeof(double*));
	aArch->iWeightDeltaList[1]=(double**)malloc(aHiddenCount*sizeof(double*));
	for(i=0; i<aHiddenCount; i++)
	{
		aArch->iWeightList[1][i]=(double*)malloc(aOutputCount*sizeof(double));
		aArch->iWeightDeltaList[1][i]=(double*)malloc(aOutputCount*sizeof(double));
		for(j=0; j<aOutputCount; j++)
		{
			aArch->iWeightList[1][i][j]=(( (double)(rand()%KMaxSignedShort)/KMaxSignedShort )-0.5 ) * 2.0;
			aArch->iWeightDeltaList[1][i][j]=0.0;
//			printf("%lf\n", aArch->iWeightList[1][i][j]);
		}
	}

	// allocate bias and delta list for aHiddenCount [5]
	aArch->iBiasList[0]=(double*)malloc(aHiddenCount*sizeof(double));
	aArch->iBiasDeltaList[0]=(double*)malloc(aHiddenCount*sizeof(double));

	// allocate bias and delta list for aOutputCount [6]
	aArch->iBiasList[1]=(double*)malloc(aOutputCount*sizeof(double));
	aArch->iBiasDeltaList[1]=(double*)malloc(aOutputCount*sizeof(double));

	// fill bias for hidden nodes with random value from -1.0 - 1.0
	for(i=0; i<aHiddenCount; i++)
	{
		aArch->iBiasList[0][i]=(( (double)(rand()%KMaxSignedShort)/KMaxSignedShort )-0.5 ) * 2.0;
		aArch->iBiasDeltaList[0][i]=0.0;
//		printf("%lf\n", aArch->iBiasList[0][i]);
	}

	// fill bias for output nodes with random value from -1.0 - 1.0
	for(i=0; i<aOutputCount; i++)
	{
		aArch->iBiasList[1][i]=(( (double)(rand()%KMaxSignedShort)/KMaxSignedShort )-0.5 ) * 2.0;
		aArch->iBiasDeltaList[1][i]=0.0;
//		printf("%lf\n", aArch->iBiasList[1][i]);
	}

	// fill alpha
	aArch->iAlpha=aAlpha;

	// fill error
	aArch->iError=aError;
}

void annarch_free(SAnnArch * aArch)
{
	// vars
	unsigned short i;
	unsigned short j;

	// free biases for hidden nodes [5]
	if(aArch->iBiasList[0])
	{
		free(aArch->iBiasList[0]);
		aArch->iBiasList[0]=NULL;
	}

	if(aArch->iBiasDeltaList[0])
	{
		free(aArch->iBiasDeltaList[0]);
		aArch->iBiasDeltaList[0]=NULL;
	}

	// free biases for output nodes [6]
	if(aArch->iBiasList[1])
	{
		free(aArch->iBiasList[1]);
		aArch->iBiasList[1]=NULL;
	}

	if(aArch->iBiasDeltaList[1])
	{
		free(aArch->iBiasDeltaList[1]);
		aArch->iBiasDeltaList[1]=NULL;
	}

	// free weight list from input->hidden [3]
	for(i=0; i<aArch->iInputLayer.iNodeCount; i++)
	{
		if(aArch->iWeightList[0][i])
		{
			free(aArch->iWeightList[0][i]);
			aArch->iWeightList[0][i]=NULL;
		}

		if(aArch->iWeightDeltaList[0][i])
		{
			free(aArch->iWeightDeltaList[0][i]);
			aArch->iWeightDeltaList[0][i]=NULL;
		}
	}

	if(aArch->iWeightList[0])
	{
		free(aArch->iWeightList[0]);
		aArch->iWeightList[0]=NULL;
	}

	if(aArch->iWeightDeltaList[0])
	{
		free(aArch->iWeightDeltaList[0]);
		aArch->iWeightDeltaList[0]=NULL;
	}

	// free weight list from hidden->output [4]
	for(i=0; i<aArch->iHiddenLayer.iNodeCount; i++)
	{
		free(aArch->iWeightList[1][i]);
		aArch->iWeightList[1][i]=NULL;

		free(aArch->iWeightDeltaList[1][i]);
		aArch->iWeightDeltaList[1][i]=NULL;
	}

	if(aArch->iWeightList[1])
	{
		free(aArch->iWeightList[1]);
		aArch->iWeightList[1]=NULL;
	}

	if(aArch->iWeightDeltaList[1])
	{
		free(aArch->iWeightDeltaList[1]);
		aArch->iWeightDeltaList[1]=NULL;
	}

	// free target [2]
	if(aArch->iTargetSet)
	{
		free(aArch->iTargetSet);
		aArch->iTargetSet=NULL;
	}

	// free layers [1]
	annlayer_free(&aArch->iInputLayer);
	annlayer_free(&aArch->iHiddenLayer);
	annlayer_free(&aArch->iOutputLayer);
}

void annarch_query(SAnnArch * aArch, double * aInput, double * aOutput)
{
	// vars
	unsigned short i;

	for(i=0; i<aArch->iInputLayer.iNodeCount; i++)
	{
		aArch->iInputLayer.iNodes[i].iInputSignal=aInput[i];
		aArch->iInputLayer.iNodes[i].iOutputSignal=aInput[i];
	}

	anntraining_forward_propagate(aArch);

	for(i=0; i<aArch->iOutputLayer.iNodeCount; i++)
	{
		aOutput[i]=aArch->iOutputLayer.iNodes[i].iOutputSignal;
	}
}

