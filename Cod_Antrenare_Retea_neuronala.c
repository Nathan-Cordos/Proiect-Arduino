#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <math.h>
#include <fcntl.h>

FILE *fis;

#define NUMPAT 6
#define NUMIN  2
#define NUMHID 4
#define NUMOUT 3

#define rando() ((double)rand()/((double)RAND_MAX+1))

main() {
	int    i, j, k, p, np, op, ranpat[NUMPAT + 1], epoch;
	int    NumPattern = NUMPAT, NumInput = NUMIN, NumHidden = NUMHID, NumOutput = NUMOUT;
	double Input[NUMPAT + 1][NUMIN + 1] = { {0, 0, 0},  {0, 0, 0},  {0, 1, 0},  {0, 0, 1},  {0, 1, 1},{0,0.5,0.4},{0,0.4,0.5} };
	double Target[NUMPAT + 1][NUMOUT + 1] = { {0,0, 0,0},  {0,0, 0,0},  {0,1,0,0},  {0,0, 1,0},  {0,0, 0, 1},{0,1,0,0} ,{0,0,1,0} };
	double SumH[NUMPAT + 1][NUMHID + 1], WeightIH[NUMIN + 1][NUMHID + 1], Hidden[NUMPAT + 1][NUMHID + 1];
	double SumO[NUMPAT + 1][NUMOUT + 1], WeightHO[NUMHID + 1][NUMOUT + 1], Output[NUMPAT + 1][NUMOUT + 1];
	double DeltaO[NUMOUT + 1], SumDOW[NUMHID + 1], DeltaH[NUMHID + 1];
	double DeltaWeightIH[NUMIN + 1][NUMHID + 1], DeltaWeightHO[NUMHID + 1][NUMOUT + 1];
	double Error, eta = 0.1, alpha = 0.2, smallwt = 0.5;

	for (j = 1; j <= NumHidden; j++) {    /* initialize WeightIH and DeltaWeightIH */
		for (i = 0; i <= NumInput; i++) {
			DeltaWeightIH[i][j] = 0.0;
			WeightIH[i][j] = 2.0 * (rando() - 0.5) * smallwt;
		}
	}
	for (k = 1; k <= NumOutput; k++) {    /* initialize WeightHO and DeltaWeightHO */
		for (j = 0; j <= NumHidden; j++) {
			DeltaWeightHO[j][k] = 0.0;
			WeightHO[j][k] = 2.0 * (rando() - 0.5) * smallwt;
		}
	}

	for (epoch = 0; epoch < 100000000; epoch++) {    /* iterate weight updates */
		for (p = 1; p <= NumPattern; p++) {    /* randomize order of training patterns */
			ranpat[p] = p;
		}
		for (p = 1; p <= NumPattern; p++) {
			np = p + rando() * (NumPattern + 1 - p);
			op = ranpat[p]; ranpat[p] = ranpat[np]; ranpat[np] = op;
		}
		Error = 0.0;
		for (np = 1; np <= NumPattern; np++) {    /* repeat for all the training patterns */
			p = ranpat[np];
			for (j = 1; j <= NumHidden; j++) {    /* compute hidden unit activations */
				SumH[p][j] = WeightIH[0][j];
				for (i = 1; i <= NumInput; i++) {
					SumH[p][j] += Input[p][i] * WeightIH[i][j];
				}
				Hidden[p][j] = 1.0 / (1.0 + exp(-SumH[p][j]));
			}
			for (k = 1; k <= NumOutput; k++) {    /* compute output unit activations and errors */
				SumO[p][k] = WeightHO[0][k];
				for (j = 1; j <= NumHidden; j++) {
					SumO[p][k] += Hidden[p][j] * WeightHO[j][k];
				}
				Output[p][k] = 1.0 / (1.0 + exp(-SumO[p][k]));   
             
				Error += 0.5 * (Target[p][k] - Output[p][k]) * (Target[p][k] - Output[p][k]);   
             
				DeltaO[k] = (Target[p][k] - Output[p][k]) * Output[p][k] * (1.0 - Output[p][k]);  
              
			}
			for (j = 1; j <= NumHidden; j++) {   
				SumDOW[j] = 0.0;
				for (k = 1; k <= NumOutput; k++) {
					SumDOW[j] += WeightHO[j][k] * DeltaO[k];
				}
				DeltaH[j] = SumDOW[j] * Hidden[p][j] * (1.0 - Hidden[p][j]);
			}
			for (j = 1; j <= NumHidden; j++) {     /* update weights WeightIH */
				DeltaWeightIH[0][j] = eta * DeltaH[j] + alpha * DeltaWeightIH[0][j];
				WeightIH[0][j] += DeltaWeightIH[0][j];
				for (i = 1; i <= NumInput; i++) {
					DeltaWeightIH[i][j] = eta * Input[p][i] * DeltaH[j] + alpha * DeltaWeightIH[i][j];
					WeightIH[i][j] += DeltaWeightIH[i][j];
					
				}
			}
			for (k = 1; k <= NumOutput; k++) {    /* update weights WeightHO */
				DeltaWeightHO[0][k] = eta * DeltaO[k] + alpha * DeltaWeightHO[0][k];
				WeightHO[0][k] += DeltaWeightHO[0][k];
				for (j = 1; j <= NumHidden; j++) {
					DeltaWeightHO[j][k] = eta * Hidden[p][j] * DeltaO[k] + alpha * DeltaWeightHO[j][k];
					WeightHO[j][k] += DeltaWeightHO[j][k];
				}
			}
		}
		if (epoch % 100 == 0) fprintf(stdout, "\nEpoch %-5d :   Error = %f", epoch, Error);
		if (Error < 0.0004) break;  /* stop learning when 'near enough' */
	}

	fprintf(stdout, "\n\nNETWORK DATA - EPOCH %d\n\nPat\t", epoch);   /* print network outputs */
	for (i = 1; i <= NumInput; i++) {
		fprintf(stdout, "Input%-4d\t", i);
	}
	for (k = 1; k <= NumOutput; k++) {
		fprintf(stdout, "Target%-4d\tOutput%-4d\t", k, k);
	}
	for (p = 1; p <= NumPattern; p++) {
		fprintf(stdout, "\n%d\t", p);
		for (i = 1; i <= NumInput; i++) {
			fprintf(stdout, "%f\t", Input[p][i]);
		}
		for (k = 1; k <= NumOutput; k++) {
			fprintf(stdout, "%f\t%f\t", Target[p][k], Output[p][k]);
		}
	}

	fis = fopen("Weights.txt", "w");

	fprintf(fis,"\n---------------------------------------------------------------\n");
	fprintf(fis,"\n INPUT -> HID \n");
	for (i = 0; i < NUMIN + 1; i++)
	{
		fprintf(fis,"\nInput node %d: ", i);
		for (j = 0; j < NUMHID + 1; j++)
		{
			fprintf(fis,"W[%d] = %f ", j, WeightIH[i][j]);
		}
	}
	fprintf(fis,"\n HID -> OUT \n");

	for (i = 0; i < NUMHID + 1; i++)
	{
		fprintf(fis,"\nHidden node %d: ", i);
		for (j = 0; j < NUMOUT + 1; j++)
		{
			fprintf(fis,"W[%d] = %f ", j, WeightHO[i][j]);
		}
	}

	
	_getch();
	
	return 1;
}

/*******************************************************************************/