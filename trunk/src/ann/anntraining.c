
#include <math.h>
#include <stdio.h>
#include "annfunc.h"
#include "anntraining.h"

void anntraining_forward_propagate(SAnnArch * aArch)
{
	// vars
	unsigned short i;
	unsigned short j;

	// forward from input.outputsignal to hidden.inputsignal
	for(i=0; i<aArch->iHiddenLayer.iNodeCount; i++)
	{
		// add bias of this node
		aArch->iHiddenLayer.iNodes[i].iInputSignal=aArch->iBiasList[0][i];

		// add accumulation of all input layer*weight
		for(j=0; j<aArch->iInputLayer.iNodeCount; j++)
		{
			aArch->iHiddenLayer.iNodes[i].iInputSignal+=
				(aArch->iInputLayer.iNodes[j].iOutputSignal*aArch->iWeightList[0][j][i]);
		}

		// activate node from hidden.inputsignal to hidden.outputsignal
		annnode_activate(&aArch->iHiddenLayer.iNodes[i]);
//		printf("ann_forward_propagate - hidden[%d].in=%lf; hidden[%d].out=%lf\n",
//			i, aArch->iHiddenLayer.iNodes[i].iInputSignal,
//			i, aArch->iHiddenLayer.iNodes[i].iOutputSignal);
	}

	// forward from hidden.outputsignal to output.inputsignal
	for(i=0; i<aArch->iOutputLayer.iNodeCount; i++)
	{
		// add bias of this node
		aArch->iOutputLayer.iNodes[i].iInputSignal=aArch->iBiasList[1][i];

		// add accumulation of all hidden layer*weight
		for(j=0; j<aArch->iHiddenLayer.iNodeCount; j++)
		{
			aArch->iOutputLayer.iNodes[i].iInputSignal+=
				(aArch->iHiddenLayer.iNodes[j].iOutputSignal*aArch->iWeightList[1][j][i]);
		}

		// activate node from output.inputsignal to output.outputsignal
		annnode_activate(&aArch->iOutputLayer.iNodes[i]);
//		printf("ann_forward_propagate - output[%d].in=%lf; output[%d].out=%lf\n",
//			i, aArch->iOutputLayer.iNodes[i].iInputSignal,
//			i, aArch->iOutputLayer.iNodes[i].iOutputSignal);
	}
}

void anntraining_backward_propagate(SAnnArch * aArch)
{
	// vars
	unsigned short i;
	unsigned short j;

	// count error delta of each weight hidden -> output
	for(i=0; i<aArch->iHiddenLayer.iNodeCount; i++)
	{
		for(j=0; j<aArch->iOutputLayer.iNodeCount; j++)
		{
			aArch->iWeightDeltaList[1][i][j]+=
				aArch->iAlpha*
				aArch->iOutputLayer.iNodes[j].iOutputError*
				aArch->iHiddenLayer.iNodes[i].iOutputSignal;
		}
	}

	// count error delta of each bias hidden -> output
	for(i=0; i<aArch->iOutputLayer.iNodeCount; i++)
	{
		aArch->iBiasDeltaList[1][i]+=aArch->iAlpha*aArch->iOutputLayer.iNodes[i].iOutputError;
	}

	// backward from output.outputerror to hidden.inputerror
	for(i=0; i<aArch->iHiddenLayer.iNodeCount; i++)
	{
		// set input error to 0
		aArch->iHiddenLayer.iNodes[i].iInputError=0.0;

		// add accumulation of all output layer*weight
		for(j=0; j<aArch->iOutputLayer.iNodeCount; j++)
		{
			aArch->iHiddenLayer.iNodes[i].iInputError+=
				(aArch->iOutputLayer.iNodes[j].iOutputError*aArch->iWeightList[1][i][j]);
		}

		// derivated activate node from hidden.inputerror to hidden.outputerror
		aArch->iHiddenLayer.iNodes[i].iOutputError=
			aArch->iHiddenLayer.iNodes[i].iInputError*
			aArch->iHiddenLayer.iNodes[i].iDerivatedActivationFunction(aArch->iHiddenLayer.iNodes[i].iOutputSignal);
	}

	// count error delta of each weight input -> hidden
	for(i=0; i<aArch->iInputLayer.iNodeCount; i++)
	{
		for(j=0; j<aArch->iHiddenLayer.iNodeCount; j++)
		{
			aArch->iWeightDeltaList[0][i][j]+=
				aArch->iAlpha*
				aArch->iHiddenLayer.iNodes[j].iOutputError*
				aArch->iInputLayer.iNodes[i].iOutputSignal;
		}
	}

	// count error delta of each bias input -> hidden
	for(i=0; i<aArch->iHiddenLayer.iNodeCount; i++)
	{
		aArch->iBiasDeltaList[0][i]+=aArch->iAlpha*aArch->iHiddenLayer.iNodes[i].iOutputError;
	}
}

double anntraining_count_total_error(SAnnArch * aArch)
{
	// vars
	unsigned short i;
	double totalError=0.0;

	// forward from hidden.outputsignal to output.inputsignal
	for(i=0; i<aArch->iOutputLayer.iNodeCount; i++)
	{
		// count error of each node
		aArch->iOutputLayer.iNodes[i].iInputError=
			(aArch->iTargetSet[i]-aArch->iOutputLayer.iNodes[i].iOutputSignal)*
			aArch->iOutputLayer.iNodes[i].iDerivatedActivationFunction(aArch->iOutputLayer.iNodes[i].iOutputSignal);
		aArch->iOutputLayer.iNodes[i].iOutputError=aArch->iOutputLayer.iNodes[i].iInputError;		

		// sum up error
		totalError+=pow(fabs(aArch->iTargetSet[i]-aArch->iOutputLayer.iNodes[i].iOutputSignal), 2);

//		printf("ann_forward_propagate - output[%d].error.out=%lf\n",
//			i, aArch->iOutputLayer.iNodes[i].iOutputError);
	}

//	return totalError/(double)(aArch->iOutputLayer.iCount);
	return 0.5*totalError;
}

void anntraining_update_weights_and_biases(SAnnArch * aArch)
{
	// vars
	unsigned short i;
	unsigned short j;

	// update weight from input -> hidden
	for(i=0; i<aArch->iInputLayer.iNodeCount; i++)
	{
		for(j=0; j<aArch->iHiddenLayer.iNodeCount; j++)
		{
			aArch->iWeightList[0][i][j]+=aArch->iWeightDeltaList[0][i][j];
		}
	}

	// update bias from input -> hidden
	for(i=0; i<aArch->iHiddenLayer.iNodeCount; i++)
	{
		aArch->iBiasList[0][i]+=aArch->iBiasDeltaList[0][i];
	}

	// update weight from hidden -> output
	for(i=0; i<aArch->iHiddenLayer.iNodeCount; i++)
	{
		for(j=0; j<aArch->iOutputLayer.iNodeCount; j++)
		{
			aArch->iWeightList[1][i][j]+=aArch->iWeightDeltaList[1][i][j];
		}
	}

	// update bias from hidden -> output
	for(i=0; i<aArch->iOutputLayer.iNodeCount; i++)
	{
		aArch->iBiasList[1][i]+=aArch->iBiasDeltaList[1][i];
	}
}

void anntraining_reset_weights_and_biases_delta(SAnnArch * aArch)
{
	// vars
	unsigned short i;
	unsigned short j;

	// reset weight delta hidden->output
	for(i=0; i<aArch->iHiddenLayer.iNodeCount; i++)
	{
		for(j=0; j<aArch->iOutputLayer.iNodeCount; j++)
		{
			aArch->iWeightDeltaList[1][i][j]=0;
		}
	}

	// reset bias delta hidden->output
	for(i=0; i<aArch->iOutputLayer.iNodeCount; i++)
	{
		aArch->iBiasDeltaList[1][i]=0;
	}

	// reset weight delta input->hidden
	for(i=0; i<aArch->iInputLayer.iNodeCount; i++)
	{
		for(j=0; j<aArch->iHiddenLayer.iNodeCount; j++)
		{
			aArch->iWeightDeltaList[0][i][j]=0;
		}
	}

	// reset bias delta input->hidden
	for(i=0; i<aArch->iHiddenLayer.iNodeCount; i++)
	{
		aArch->iBiasDeltaList[0][i]=0;
	}
}

unsigned long long anntraining_train_single_pattern(SAnnArch * aArch, double * aInputSet, double * aOutputSet)
{
	// vars
	unsigned short i;
	double avgError=0.0;
	unsigned long long iteration=0;

	// set input
	for(i=0; i<aArch->iInputLayer.iNodeCount; i++)
	{
		aArch->iInputLayer.iNodes[i].iInputSignal=aInputSet[i];
		aArch->iInputLayer.iNodes[i].iOutputSignal=aInputSet[i];
	}

	// set output
	for(i=0; i<aArch->iOutputLayer.iNodeCount; i++)
	{
		aArch->iTargetSet[i]=aOutputSet[i];
	}

	while(1)
	{
		iteration++;
		anntraining_forward_propagate(aArch);
		avgError=anntraining_count_total_error(aArch);
//		printf("ann_do_training - avgError: %lf\n", avgError);
		if(avgError<=aArch->iError) { break; }
		anntraining_backward_propagate(aArch);
		// update weights and biases
		anntraining_update_weights_and_biases(aArch);
		// reset weights and biases delta
		anntraining_reset_weights_and_biases_delta(aArch);
	}

	return iteration;
}

unsigned long long anntraining_train_multiple_pattern(SAnnArch * aArch, double * aInputSet, double * aOutputSet, unsigned short aSetCount)
{
	// vars
	unsigned short i, j;
	double avgError=0.0;
	unsigned long long iteration=0;

	while(1)
	{
		avgError=0.0;
		for(i=0; i<aSetCount; i++)
		{
			// set input
			for(j=0; j<aArch->iInputLayer.iNodeCount; j++)
			{
				aArch->iInputLayer.iNodes[j].iInputSignal=aInputSet[(i*aArch->iInputLayer.iNodeCount)+j];
				aArch->iInputLayer.iNodes[j].iOutputSignal=aInputSet[(i*aArch->iInputLayer.iNodeCount)+j];
			}

			// set output
			for(j=0; j<aArch->iOutputLayer.iNodeCount; j++)
			{
				aArch->iTargetSet[j]=aOutputSet[(i*aArch->iOutputLayer.iNodeCount)+j];
			}

			iteration++;
			anntraining_forward_propagate(aArch);
			avgError+=anntraining_count_total_error(aArch);
			anntraining_backward_propagate(aArch);
		}

		avgError/=(double)aSetCount;
		printf("avgError: %lf\n", avgError);
		if(avgError<=aArch->iError) { break; }
		
		// get average of weight delta hidden->input of each axon
		for(i=0; i<aArch->iHiddenLayer.iNodeCount; i++)
		{
			for(j=0; j<aArch->iOutputLayer.iNodeCount; j++)
			{
				aArch->iWeightDeltaList[1][i][j]/=(double)aSetCount;
			}
		}

		// get average of bias delta hidden->output of each axon
		for(i=0; i<aArch->iOutputLayer.iNodeCount; i++)
		{
			aArch->iBiasDeltaList[1][i]/=(double)aSetCount;
		}

		// get average of weight delta input->hidden of each axon
		for(i=0; i<aArch->iInputLayer.iNodeCount; i++)
		{
			for(j=0; j<aArch->iHiddenLayer.iNodeCount; j++)
			{
				aArch->iWeightDeltaList[0][i][j]/=(double)aSetCount;
			}
		}

		// get average of bias delta input->hidden of each axon
		for(i=0; i<aArch->iHiddenLayer.iNodeCount; i++)
		{
			aArch->iBiasDeltaList[0][i]/=(double)aSetCount;
		}
		
		// update weights and biases
		anntraining_update_weights_and_biases(aArch);

		// reset weights and biases delta
		anntraining_reset_weights_and_biases_delta(aArch);
	}

	return iteration;
}

