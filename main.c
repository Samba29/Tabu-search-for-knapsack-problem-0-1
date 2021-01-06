// Code write by Mbono Samba Dieudonne
// University of Yaounde 1, 14 November 2020
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define M 2 // Represent a number of contraint
#define ITERMAX 29861
#define TABUTENURE N*N
#define MAXITERWITHOUTMOVE 100
int N = 20; //represent a number of item of our knapsack problem
int spanInterval = 7;
int Evaluate(int Solution[N], int itemValues[N]);
int BestItemToAdd(int solution[N], int constraint[N][M], int valueConstraint[N], int valuesItem[N], int phase); // A implementer
int VerifyConstraint(int constraint[N][M], int valueConstraint[M], int solution[N]);
int TabuContain(int tabuList[N][N], int solution[N]);
int KnapsackProblemSolveByTabuSearch();
int EvaluateNumberVariable();

int main()
{
    N = EvaluateNumberVariable();
    KnapsackProblemSolveByTabuSearch();
}
int KnapsackProblemSolveByTabuSearch()
    {
        // We assume that the knapsack problem is on the form
        // Max(Z)
        // AX <= B
        // A represent the weight of each item, depending of contraint B
        // ValueConstraint represent contraint
        // Contraint B is a set of two inequation:
        //      0X1 + 0X2 + ... +0XN = 0
        //      X1 + X2 + ... + XN <= MAXWEIGHT
        int constraint[N][M];
        int valueConstraint[M];
        int valuesItem[N];
        int tabuList[N][N];
        int cIter = -1; //Count a number of iteration, for sure that we will not fall in infinite loop
        int solution[N];
        int bestSolution[N]; //Represent a best solution of our problem
        int span; // Represent the reandom number of movement which we will realize durant one iteration
        int tabuTenure = 0; // The length of the list
        int bestSolutionNeighborhood[N];
        int copySolution[N];
        int cIterWithoutMove = 1;
        FILE *fichier = NULL;
        for(int i = 0; i < N; i++)
            {
                solution[i] = 0;
                bestSolution[i] = 0;
                valuesItem[i] = 0;
                for(int j = 0; j < N; j++)
                {
                    tabuList[i][j] = 0;
                }
            }
        for(int j = 0; j < M; j++)
        {
            valueConstraint[j] = 0;
            for(int i = 0; i < N; i++)
            {
                constraint[i][j] = 0;
            }
        }
        //Add read instruction
        fichier = fopen("TEST.csv","r");
        if(fichier != NULL)
        {
            int pos = 1;
            int i = 0;
            char chaine[100];
            while(fgets(chaine, 1000,fichier) != NULL)
            {
                int temp[N];
                char* pch;
                pch = strtok(chaine, ";");
                int i = 0;
                if(pos == 1) // We take a weight of contraint
                {
                    while(pch != NULL)
                    {
                        constraint[i][1] = atoi(pch);
                        i+=1;
                        pch = strtok(NULL, " ;");
                    }
                }
                if(pos ==2 )//value of each item
                {
                    while(pch != NULL)
                    {
                        valuesItem[i] = atoi(pch);
                        i+=1;
                        pch = strtok(NULL, " ;");
                    }
                }
                if(pos==3)
                {
                    valueConstraint[1] =  atoi(pch);
                }
                pos+=1;
            }
        }
        while((cIter != ITERMAX) && (cIterWithoutMove != MAXITERWITHOUTMOVE))
            {
                cIter +=1;
                span = rand()%spanInterval;
                for(int i = 0; i < N; i++)
                {
                    copySolution[i] = solution[i];
                }
                for(int k = 0; k <= span; k++)
                {
                    // Start of constructive phase
                    printf("*********************************Start of constructive phase, iteration number: %d ********************************* \n", cIter);
                    int itemToAdd;
                    itemToAdd = BestItemToAdd(copySolution, constraint, valueConstraint, valuesItem, 1);
                    copySolution[itemToAdd] = 1;
                    printf("Variable à changer pour trouver une nouvelle solution: %d \n", itemToAdd);
                    if (VerifyConstraint(constraint, valueConstraint, copySolution) == 1)
                    {
                        printf("\n les contraintes sont verifier pour la solution: ");
                        // If the new solution verify all contraint, we retain the new solution
                        for(int m = 0; m < N; m++)
                        {
                            printf("%d", copySolution[m]);
                            solution[m] = copySolution[m];
                            if(Evaluate(bestSolution, valuesItem) == 0)
                            {
                                bestSolutionNeighborhood[m] = solution[m];
                            }
                        }
                        if(Evaluate(bestSolutionNeighborhood, valuesItem) >= Evaluate(solution, valuesItem))
                        {
                            for(int m = 0; m < N; m++)
                            {
                                bestSolutionNeighborhood[m] = solution[m];
                            }
                        }
                        printf("valeur de la solution actuelle: %d. valeur de la meilleure solution est: %d \n",Evaluate(solution, valuesItem), Evaluate(bestSolutionNeighborhood, valuesItem));

                    }
                    else
                    {
                        // if the new solution don't verify all contraints, we are in infeasible solution area
                        //  We search one variable to flip to 1 to 0 who maximize the objective function
                        printf("Contraintes non verifier pour la solution actuelle");
                        int temp[N];
                        int j = 0;
                        while((j != N) && (VerifyConstraint(constraint, valueConstraint,copySolution)!= 1))
                        {
                            // Critical event
                            // We initialize temp for be sure that we modify only one variable
                            for(int l = 0; l < N; l++)
                            {
                                temp[l] = solution[l];
                            }
                            if (temp[j] == 1)
                                temp[j] = 0;
                            if (VerifyConstraint(constraint, valueConstraint,temp) == 1 && TabuContain(tabuList, temp) == 0)
                                // If the new solution veryfy all constraint, and is not present in tabu list
                                {
                                    if (Evaluate(solution, valuesItem) > Evaluate(temp, valuesItem))
                                    {
                                        // if f(s') < f(s), we retain this solution and remove the oldest solution present in tabu list
                                        for(int m = 0; m < N; m++)
                                        {
                                            solution[m] = temp[m];
                                        }
                                        tabuTenure += 1;
                                        if (tabuTenure >= TABUTENURE)
                                        {
                                            for(int a = 1; a < N; a++)
                                            {
                                                for(int b = 0; b < N; b++)
                                                {
                                                    tabuList[a - 1][b] = tabuList[a][b];
                                                }
                                            }
                                            tabuTenure -= 1;
                                            for(int b = 0; b < M; b++)
                                            {
                                                // Remove one solution in the tabu list
                                                tabuList[tabuTenure-1][b] = solution[b];
                                            }
                                        }


                                    }
                                    if(Evaluate(bestSolution, valuesItem) < Evaluate(solution, valuesItem))
                                    {
                                        // if f(s*) < f(s), then we copy the new solution
                                        for(int m = 0; m < N; m++)
                                        {
                                            bestSolution[m] = solution[m] = 0;
                                        }
                                    }
                                }

                        j+=1;
                        }
                    }
                }// end of constructive phase
                // If we have the best solution, we save it
                if(Evaluate(bestSolutionNeighborhood, valuesItem) >= Evaluate(bestSolution, valuesItem))
                {
                    for(int i = 0; i < N; i++)
                    {
                        bestSolution[i] = bestSolutionNeighborhood[i];
                        cIterWithoutMove -= 1;
                    }
                }
                // Start of destructive phase
                for(int k = 0; k < span; k++)
                {
                    printf("\n************************Destructive phase at iteration number %d************************ \n", cIter);
                    int itemToAdd;
                    itemToAdd = BestItemToAdd(solution, constraint, valueConstraint, valuesItem, 0);
                    copySolution[itemToAdd] = 0;
                    if (VerifyConstraint(constraint, valueConstraint, copySolution) == 1)
                    {
                        printf("contraintes valide par la solution \n");
                        // If the new solution verify all contraint, we retain the new solution
                        for(int m = 0; m < N; m++)
                        {
                            solution[m] = copySolution[m];
                        }
                        if(Evaluate(bestSolutionNeighborhood, valuesItem) >= Evaluate(solution, valuesItem))
                        {
                            printf("Meilleure solution dans le voisinnage: ");
                            for(int m = 0; m < N; m++)
                            {
                                printf("%d", bestSolutionNeighborhood[m]);
                                bestSolutionNeighborhood[m] = solution[m];
                            }
                        }
                    }
                    else
                    {
                        // if the new solution don't verify all contraints, we are in infeasible solution area
                        //  We search one variable to flip to 1 to 0 who maximize the objective function
                        printf("contraintes non valide");
                        int temp[N];
                        int j = 0;
                        while((j != N) && (VerifyConstraint(constraint, valueConstraint,copySolution)!= 1))
                        {
                            // Critical event
                            // We initialize temp for be sure that we modify only one variable
                            printf("\n solution de base de l'evenement critiqu0e ");
                            for(int l = 0; l < N; l++)
                            {
                                printf("%d", copySolution[l]);
                                temp[l] = copySolution[l];
                            }
                            if (temp[j] == 1)
                            {
                                temp[j] = 0;
                                printf(" \n solution a tester pour retirer une variable:");
                                for(int l = 0; l < N; l++)
                                    printf("%d",temp[l]);
                                if (VerifyConstraint(constraint, valueConstraint,temp) == 1 && TabuContain(tabuList, temp) == 0)
                                    // If the new solution veryfy all constraint, and is not present in tabu list
                                    {
                                        if (Evaluate(solution, valuesItem) > Evaluate(temp, valuesItem))
                                        {
                                            // if f(s') < f(s), we retain this solution and remove the oldest solution present in tabu list
                                            for(int m = 0; m < N; m++)
                                            {
                                                solution[m] = temp[m];
                                            }
                                            tabuTenure += 1;
                                            if (tabuTenure >= TABUTENURE)
                                            {
                                                for(int a = 1; a < N; a++)
                                                {
                                                    for(int b = 0; b < N; b++)
                                                    {
                                                        tabuList[a - 1][b] = tabuList[a][b];
                                                    }
                                                }
                                                tabuTenure -= 1;
                                            }
                                            for(int b = 0; b < M; b++)
                                            {
                                                // Add the new solution in the tabu list
                                                tabuList[tabuTenure-1][b] = solution[b];
                                            }

                                        }
                                        if(Evaluate(bestSolution, valuesItem) < Evaluate(solution, valuesItem))
                                        {
                                            // if f(s*) < f(s), then we copy the new solution
                                            for(int m = 0; m < N; m++)
                                            {
                                                bestSolution[m] = solution[m] = 0;
                                                cIterWithoutMove -= 1;
                                            }
                                        }
                                    }
                                }
                        j+=1;
                        }
                    }
                }//end of destructive phase
                if(Evaluate(bestSolutionNeighborhood, valuesItem) >= Evaluate(bestSolution, valuesItem))
                {
                    for(int i = 0; i < N; i++)
                        bestSolution[i] = bestSolutionNeighborhood[i];
                }
                cIterWithoutMove += 1;
            }
        printf(" \n\n\n\n\n\n\n********************************Best solution for your knapsack problem**********************\n");
        for(int i = 0; i < N; i++)
            printf("X%d = %d; ",i+1, bestSolution[i]);
        printf("The value is: %d", Evaluate(bestSolution, valuesItem));
        printf("\nrequiered number of iterations: %d", cIter);
        printf("\nAppuyez sur n'importe quelle bouton pour sortir");
        scanf("%d",NULL);
        return 1;
}

int Evaluate(int Solution[N], int itemValues[N])
{
    int sum = 0;
    for(int i = 0; i < N; i++)
    {
        if (Solution[i] == 1)
            sum += Solution[i]*itemValues[i];
    }
    return sum;
}

int BestItemToAdd(int solution[N], int constraint[N][M], int valueConstraint[N], int valuesItem[N], int phase)
{
    int B[N];
    int W[M];
    int S[N];
    int sum = 0;
    int S0 = 0;
    float comparaitor = 0.0;
    int index = -1;
    // This is only initialisation
    printf("\n solution a tester:");
    for(int i= 0; i < N; i++)
    {
        B[i] = 0;
        S[i] = 0;
        printf("%d", solution[i]);
    }
    printf("\n");
    for(int i = 0; i < M; i++)
    {
        W[i] = 0;
    }
    for(int i = 0; i < M; i++)
    {
        for(int j = 0; j < N; j++)
        {
            sum += constraint[i][j]*solution[j];
        }
        B[i] -= sum;
        if (B[i] > 0)
            W[i] = 1/B[i];
        else
            W[i] = 2 + B[i]*(-1);
    }
    for(int i = 0; i < M; i++)
    {
        S0 = S0 + W[i]*B[i];
    }
    for(int j = 0; j < N; j++)
    {
        for(int i = 0; i < M; i++)
        {
            S[j] = W[i]*constraint[j][i];
        }
        if(phase == 1) //constructive phase
        {
            printf("index %d, cj: %f, sj: %f , cj/sj: %d \n", j, (float)valuesItem[j], (float)S[j], (float)valuesItem[j]/(float)S[j]);
            if((comparaitor < ((float)valuesItem[j]/(float)S[j])) && (solution[j] == 0))
            {
                index = j;
                comparaitor = (float)valuesItem[j]/(float)S[j];
            }
        }
        else
        {
           if(S[j] != 0 && (comparaitor > ((float)valuesItem[j]/(float)S[j])) && solution[j] == 1)
            {
                index = j;
                comparaitor = (float)valuesItem[j]/(float)S[j];
            }
        }
    }
    return index;
}
int VerifyConstraint(int constraint[N][M], int valueConstraint[M], int solution[N])
{
    for(int j = 0; j < M; j++)
    {
        int valueCol = 0;
        for(int i = 0; i < N; i++)
        {
            valueCol+= constraint[i][j]*solution[i];
        }
        if(valueCol > valueConstraint[j])
        {
            // on suppose que les contraintes sont toutes mises sous la forme AX <= B
            return 0;
        }

    }
    return 1;
}

int TabuContain(int tabuList[N][N], int solution[N])
{
    int found;
    for(int i = 0; i < N; i++)
    {
        found = 1;
        for(int j = 0; j < N && found == 1; j++)
        {
            //printf("%d =? %d \n",solution[j], tabuList[i][j]);
            if(solution[j] != tabuList[i][j])
                found = 0;
        }
        if(found == 1)
            return 1;
    }
    return 0;
}

int EvaluateNumberVariable()
{
    FILE* file = NULL;
    int row = 1;
    char separator = 'a';
    file = fopen("TEST.csv", "r");
    if(file != NULL)
    {
        while(separator != '\n')
        {
            separator = fgetc(file);
            if(separator == ';')
                row+=1;
        }
    }
    return row;
}
