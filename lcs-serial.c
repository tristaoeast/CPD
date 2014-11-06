#include<unistd.h>
#include<omp.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

int max(unsigned short a, unsigned short b){
	if(a>b) return a;
	else return b;
}

short cost(int x);

void readFile(char *filename,char **a,char **b,int *aSize,int *bSize);
void createMatrix(unsigned short ***mtx,int aSize,int bSize);
void deleteMatrix(unsigned short **mtx,int aSize,int bSize);
void printMatrix(unsigned short **mtx,char *a,char *b,int aSize,int bSize);
void printTime(double start,double end);
void printAllTime(double start,double mid,double end);

int LCS(unsigned short **mtx,char *a,char *b,int aSize,int bSize);
void backtrack(unsigned short **mtx,char *a,char *b,int aSize,int bSize,int lcs);

int main(int argc,char *argv[]){
	double start,mid,end,time;
	start=omp_get_wtime();
	char *a,*b;
	unsigned short **mtx;
	int aSize,bSize;

	if(argc<2){
		printf("Usage : %s [filename] [-t]\n",argv[0]);
		exit(-1);
	}

	// Le ficheiro
	readFile(argv[1],&a,&b,&aSize,&bSize);	// Le ficheiro dado por argv1 e guarda valores nas variaveis a,b,aSize,bSize

	// Cria tabela
	createMatrix(&mtx,aSize,bSize);	// Aloca memoria dinamicamente dados os comprimentos das strings

	// Preenche a tabela
	int lcs=LCS(mtx,a,b,aSize,bSize);
	printf("%d\n",lcs);
//	printMatrix(&mtx,a,b,aSize,bSize);

	// Backtrack da tabela
	mid=omp_get_wtime();
	backtrack(mtx,a,b,aSize,bSize,lcs);	// Faz backtrack da tabela e imprime a LCS
	end=omp_get_wtime();

	// Libertacao de memoria
	deleteMatrix(mtx,aSize,bSize);
	free(a);
	free(b);
	
	// Imprime tempo
	if(argc>2) printAllTime(start,mid,end);

	return 0;
}

int LCS(unsigned short **mtx,char *a,char *b,int aSize,int bSize){
	int i,j;
	for(i=1;i<aSize+1;i++)
		for(j=1;j<bSize+1;j++)
			if(a[i-1]==b[j-1])
				mtx[i][j]=mtx[i-1][j-1]+cost(i);
			else
				mtx[i][j]=max(mtx[i][j-1],mtx[i-1][j]);

	return mtx[aSize][bSize];
}

void backtrack(unsigned short **mtx,char *a,char *b,int aSize,int bSize,int lcs){
	char *result;
	int i=aSize;
	int j=bSize;
	
	result=(char*)malloc(sizeof(char)*(lcs+1));
	result[lcs--]='\0';

	while(lcs>=0){
		if(a[i-1]==b[j-1]){
			result[lcs--]=a[i-1];
			i--;
			j--;
		}
		else{
			if(mtx[i][j-1]==mtx[i][j]) j--;	// Go left
			else i--;	// Go right
		}
	}

	printf("%s\n",result);
	free(result);
}


void createMatrix(unsigned short ***mtx,int aSize,int bSize){
	int i,j;

	(*mtx)=(unsigned short**)malloc(sizeof(unsigned short*)*(aSize+1));
	
	for(i=0;i<aSize+1;i++){
		(*mtx)[i]=(unsigned short*)malloc(sizeof(unsigned short)*(bSize+1));
		(*mtx)[i][0]=0;
	}
		
	for(j=1;j<bSize+1;j++){
		(*mtx)[0][j]=0;		
	}
}

void deleteMatrix(unsigned short **mtx,int aSize,int bSize){
	int i;
	for(i=0;i<aSize+1;i++){
		free(mtx[i]);
	}
		
	free(mtx);
}

void readFile(char *filename,char **a,char **b,int *aSize,int *bSize){
	FILE *input;
	
	input=fopen(filename,"r");	
	if(input==NULL){
		printf("Error opening file...\n");
		exit(-1);
	}
	
	fscanf(input,"%d %d\n",aSize,bSize);
	(*a)=(char*)malloc(sizeof(char)*((*aSize)+1));
	(*b)=(char*)malloc(sizeof(char)*((*bSize)+1));
	fscanf(input,"%s\n",(*a));
	fscanf(input,"%s\n",(*b));

	fclose(input);
}

void printMatrix(unsigned short **mtx,char *a,char *b,int aSize,int bSize){
	int i,j;
	printf("        ");
	for(j=0;j<bSize;j++) printf("%3c ",b[j]);
	printf("\n");
	for(i=0;i<aSize+1;i++){
		if(i>0) printf("%3c ",a[i-1]);
		else printf("    ");
		for(j=0;j<bSize+1;j++)
			printf("%3d ",mtx[i][j]);
		printf("\n");
	}
}

void printTime(double start,double end){
	double time=end-start;
	if((time/60)>=1){
	printf("%d minutes and %f seconds (%f seconds)\n",(int)time/60,fmod(time,60),time);
	}else
	printf("%f seconds\n",time);
}

void printAllTime(double start,double mid,double end){
	printf("Number of threads: %d\n",omp_get_max_threads());
	printf("Time elapsed on LCS: ");
	printTime(start,mid);
	printf("Time elapsed on Backtrack: ");
	printTime(mid,end);
	printf("Total Time elapsed: ");
	printTime(start,end);
}

short cost(int x){
	int i,n_iter=20;
	double dcost=0;
	for(i=0;i<n_iter;i++) dcost+=pow(sin((double)x),2)+pow(cos((double)x),2);
	return (short) (dcost/n_iter+0.1);
}


