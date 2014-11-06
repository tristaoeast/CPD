using namespace std;
#include<unistd.h>
#include<omp.h>
#include<iostream>
#include<fstream>
#include<string>
#include<string.h>
#include<stdlib.h>
#include<cmath>

short cost(int x);

void readFile(char *filename,char **a,char **b,int *aSize,int *bSize);
void createMatrix(unsigned short ***mtx,int aSize,int bSize);
void deleteMatrix(unsigned short ***mtx,int aSize,int bSize);
void printMatrix(unsigned short ***mtx,string a,string b,int aSize,int bSize);
void printTime(double start,double end);
void printAllTime(double start,double mid,double end);

int LCS(unsigned short ***mtx,string a,string b,int aSize,int bSize);
int LCS2(unsigned short ***mtx,string a,string b,int aSize,int bSize);
int LCS3(unsigned short ***mtx,string a,string b,int aSize,int bSize);
void backtrack(unsigned short ***mtx,string a,string b,int aSize,int bSize,int lcs);

int main(int argc,char *argv[]){
	double start,mid,end,time;
	start=omp_get_wtime();
	char *a,*b;
	unsigned short **mtx;
	int aSize,bSize;

	if(argc<2||argc>3||(argc==3&&strcmp(argv[2],"-t")!=0)){
		cout << "Usage : "<<argv[0]<<" [filename] [-t]\n";
		exit(-1);
	}

	// Le ficheiro
	readFile(argv[1],&a,&b,&aSize,&bSize);	// Le ficheiro dado por argv1 e guarda valores nas variaveis a,b,aSize,bSize

	// Cria tabela
	createMatrix(&mtx,aSize,bSize);	// Aloca memoria dinamicamente dados os comprimentos das strings

	// Preenche a tabela
	int lcs=LCS4(&mtx,a,b,aSize,bSize);	// Preenche a tabela usando o LCS e imprime o comprimento
	cout<<lcs<<endl;
	//printMatrix(&mtx,a,b,aSize,bSize);

	// Backtrack da tabela
	mid=omp_get_wtime();
	backtrack(&mtx,a,b,aSize,bSize,lcs);	// Faz backtrack da tabela e imprime a LCS
	end=omp_get_wtime();

	// Libertacao de memoria
	deleteMatrix(&mtx,aSize,bSize);
	delete[] a;
	delete[] b;
	
	// Imprime tempo
	if(argc==3) printAllTime(start,mid,end);

	return 0;
}

int LCS3(unsigned short ***mtx,string a,string b,int aSize,int bSize){
int n_threads=omp_get_max_threads();
int row[n_threads];
int col[n_threads];

for(int k=0;k<n_threads;k++){
	row[k]=k+1;
	col[k]=0;
}
//	row[n_threads-1]=0;
	omp_set_num_threads(4);//schedule (static,1)
	#pragma omp parallel
	{
		int tid=omp_get_thread_num();
		#pragma omp for schedule (static,1)
		for(int i=1;i<aSize+1;i++){
			//int idx=(i-1)%n_threads+1;
			row[tid]=i;
			col[tid]=0;
//if(tid==0) printf("T[%d] iniciou | linha: %d row:%d col:%d\n",tid,i,row[tid],col[tid]);fflush(stdout);
			for(int j=1;j<bSize+1;j++){
			//	printf("Visitou %d,%d\n",i,j);
				//printf("row[%d]=%d\n",idx,row[idx]);
				while(i!=1&&row[(tid+3)%4]<row[tid] && col[tid]>=col[(tid+3)%4]) ; // Espera pelas dependencias
		//		if(j==1) printf("T[%d] a executar linha %d de %d\n",tid,i,aSize);fflush(stdout);
				//printf("T[%d] Visitou %d,%d\n",tid,i,j);
				if(a[i-1]==b[j-1])
					(*mtx)[i][j]=(*mtx)[i-1][j-1]+1;//cost(i);
				else
					(*mtx)[i][j]=max((*mtx)[i][j-1],(*mtx)[i-1][j]);
				col[tid]++;
				//if(i>17990) cout<<(*mtx)[i][j]<<endl;
			}
			if(tid==0) printf("T[%d] Linha %d de %d terminada\n",tid,i,aSize);
		}
	}
	cout<<"Num threads: "<<n_threads <<endl;
	cout<<omp_get_thread_num()<<endl;
	
	return (*mtx)[aSize][bSize];
}

int LCS(unsigned short ***mtx,string a,string b,int aSize,int bSize){

	for(int i=1;i<aSize+1;i++)
		for(int j=1;j<bSize+1;j++)
			if(a[i-1]==b[j-1])
				(*mtx)[i][j]=(*mtx)[i-1][j-1]+1;//cost(i);
			else
				(*mtx)[i][j]=max((*mtx)[i][j-1],(*mtx)[i-1][j]);

	return (*mtx)[aSize][bSize];
}

void backtrack(unsigned short ***mtx,string a,string b,int aSize,int bSize,int lcs){
	char *result;
	int i=aSize;
	int j=bSize;
	
	//result=(char*)malloc(sizeof(char)*(lcs+1));
	result=new char [lcs+1];
	result[lcs--]='\0';

	//while(lcs>=0){
	while(i>=0&&j>=0){ // TODO
	//cout<<"Visited point: "<<i<<","<<j<<endl;
		if(a[i-1]==b[j-1]){
			result[lcs--]=a[i-1];
			//cout<<"MATCH: "<<a[i-1]<<endl;
			i--;
			j--;
		}
		else{
			if((*mtx)[i][j-1]==(*mtx)[i][j]) j--;	// Go left
			else i--;	// Go right
		}
	}

	cout<<result<<endl;
	delete[] result;
}


void createMatrix(unsigned short ***mtx,int aSize,int bSize){
	(*mtx)=new unsigned short* [aSize+1];
	
	for(int i=0;i<aSize+1;i++){
		(*mtx)[i]=new unsigned short[bSize+1];
		(*mtx)[i][0]=0;
	}
		
	for(int j=1;j<bSize+1;j++){
		(*mtx)[0][j]=0;		
	}
}

void deleteMatrix(unsigned short ***mtx,int aSize,int bSize){
	
	for(int i=0;i<aSize+1;i++){
		delete[] (*mtx)[i];
	}
		
	delete[] (*mtx);
}

void readFile(char *filename,char **a,char **b,int *aSize,int *bSize){
	ifstream input(filename);
	
	if(!input.is_open()){
		cout<<"Error opening file..."<<endl;
		exit(-1);
	}
	
	input>>*aSize;
	input>>*bSize;
	(*a)=new char [(*aSize)+1];
	(*b)=new char [(*bSize)+1];
	input>>*a;
	input>>*b;

	input.close();
}

void printMatrix(unsigned short ***mtx,string a,string b,int aSize,int bSize){
	cout<<"    "<<"    ";
	for(int j=0;j<bSize;j++) printf("%3c ",b[j]);
		cout <<endl;
		for(int i=0;i<aSize+1;i++){
			if(i>0) printf("%3c ",a[i-1]);
			else cout<<"    ";
			for(int j=0;j<bSize+1;j++)
				printf("%3d ",(*mtx)[i][j]);
			cout << endl;
		}
}

void printTime(double start,double end){
	double time=end-start;
	if((time/60)>=1){
	cout<<(int)time/60<<" minutes and "<<fmod(time,60)<<" seconds ("<<time<<" seconds)"<<endl;
	}else
	cout<<time<<" seconds"<<endl;
}

void printAllTime(double start,double mid,double end){
//	cout<<start<<"|"<<mid<<"|"<<end<<endl;
	cout<<"Time elapsed on LCS: ";
	printTime(start,mid);
	cout<<"Time elapsed on Backtrack: ";
	printTime(mid,end);
	cout<<"Total Time elapsed: ";
	printTime(start,end);
}

short cost(int x){
	int i,n_iter=20;
	double dcost=0;
	for(i=0;i<n_iter;i++) dcost+=pow(sin((double)x),2)+pow(cos((double)x),2);
	return (short) (dcost/n_iter+0.1);
}


int LCS2(int ***mtx,string a,string b,int aSize,int bSize){
	omp_set_num_threads(4);
	int n_threads;
	n_threads=omp_get_max_threads();
	
	int index[n_threads+1];
	int idx;
	int tasks=0;
	index[0]=bSize+2;
	cout<<"NTHREADS: "<<n_threads<<endl;
	
	for(int i=1;i<aSize+1;i++){
//		cout<<"IDX: "<<idx<<endl;
		if(tasks<n_threads){
			idx=(i-1)%4+1;
			index[idx]=0;
			tasks++;
			#pragma omp task firstprivate(a,b,idx,i) shared(index)
			{
//				cout<<omp_get_num_threads()<<endl;
	//cout<<tasks<<endl;
				for(int j=1;j<bSize+1;j++){
//				for(int j=1;j<3;j++){
				//	cout<<"index["<<idx-1<<"]="<<index[idx-1]<<endl;// thread trying to start"<<endl;
					//cout<<"index["<<idx<<"]="<<index[idx]<<endl;// thread trying to start"<<endl;
					while(index[idx-1]<=index[idx]);
				//cout<<idx<<" Thread Visited: "<<i<<","<<j<<endl;
						if(a[i-1]==b[j-1])
							(*mtx)[i][j]=(*mtx)[i-1][j-1]+1;//cost(i);
						else
							(*mtx)[i][j]=max((*mtx)[i][j-1],(*mtx)[i-1][j]);
						index[idx]++;
					
				}
			}
		}
		else {
			#pragma omp taskwait
			tasks=0;
			i--;
		}
	}

	return (*mtx)[aSize][bSize];
}

int LCS4(unsigned short ***mtx,string stra,string strb,int straSize,int strbSize){

	unsigned short tpos[omp_get_num_threads][strbSize];

	for(int i=1;i<straSize+1;i++)
		for(int j=1;j<strbSize+1;j++)
			if(stra[i-1]==strb[j-1])
				(*mtx)[i][j]=(*mtx)[i-1][j-1]+1;//cost(i);
			else
				(*mtx)[i][j]=max((*mtx)[i][j-1],(*mtx)[i-1][j]);
	return (*mtx)[straSize][strbSize];
}