#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct PTEntries
{
	char act;
	char page[6];
	int valid; //1/0	// 0 not valid so we can write on it
	int pid;
};

struct Process
{
	int pid;
	int counter;
	int alive; //1/0
	int wr_disc;
	int PFs;
	int dirtyPages;
}process1, process2;

int pfs1=0, pfs2=0;

void FLUSK(struct PTEntries *IPT, int pid, int k){
	printf("%d\n",pid);
	int i;
	for(i=0;i<2*k;i++){
		if(IPT[i].pid==pid && IPT[i].valid ){
			if(IPT[i].act=='W'){
				if(pid==process1.pid)
					process1.dirtyPages++;
				else
					process2.dirtyPages++;
			}
			IPT[i].valid = 0;
			printf("DOBE\n" );
		}
	}
	if(pid==process1.pid)
		process1.wr_disc = process1.wr_disc + k;
	else
		process2.wr_disc = process2.wr_disc + k;
}



int FWF(struct PTEntries *IPT, int k, char pnum[6], char act, int process){
	int i, found;
	printf("FWF STARTS add: %s\n", pnum);
	printf("====================$$========================\n");
	for(i=0;i<2*k;i++){		
		printf("|%d|%c|%s|%d|\n",IPT[i].pid,IPT[i].act,IPT[i].page,IPT[i].valid );
	}
	printf("============================================\n");
	found =0;
	for(i=0;i<2*k;i++){
		if(strcmp(IPT[i].page,pnum)==0 && IPT[i].valid && IPT[i].pid == process){
			found=1;
			break;
		}
	}
	if(found){
		printf("Page excists in memory.\n");
		return 0;
	}
	else{
		printf("~~PAGE FAULT~~\n");
		if(process1.pid==process){
			process1.PFs++;
			pfs1++;
		}
		else{
			process2.PFs++;
			pfs2++;
		}
		if(pfs1==k+1){
			printf("MEMORY IS FULL ---FLUSK---%d\n",process);
			FLUSK(IPT , process, k);
			pfs1=0;
			FWF(IPT, k, pnum, act, process);
			process1.PFs--; // it has been summed one extra
			return 0;
		}
		if(pfs2==k+1){
			printf("MEMORY IS FULL ---FLUSK---%d\n",process);
			FLUSK(IPT , process, k);		
			pfs2=0;
			FWF(IPT, k, pnum, act, process);
			process2.PFs--; // it has been summed one extra
			return 0;
		}
		else{
			for(i=0;i<2*k;i++){
				if(IPT[i].valid==0){
					strcpy(IPT[i].page, pnum);
					IPT[i].act = act;
					IPT[i].valid=1;
					IPT[i].pid=process;
					printf("Page added in memory\n");
					return 0;
				}
			}
		}
	}
}

int main(int argc, char *argv[]){
	int  q, k, max, i, j, ipt_sz;
	char pnum[6];
	int PFcount=0;
	process1.pid = 1001;
	process1.counter = 0;
	process1.alive = 1;
	process1.wr_disc=0;
	process1.PFs =0;
	process1.dirtyPages=0;
	process2.pid = 1002;
	process2.counter = 0;
	process2.alive = 1;
	process2.wr_disc=0;
	process2.PFs = 0;
	process2.dirtyPages=0;
//***** GET THE ARGUMENTS ********\\

	printf ("This program was called with \"%s\".\n",argv[0]);
	k=0;
  	if (argc > 1){
    	for (i = 1; i < argc; i++){
    		printf("argv[%d] = %s\n",i, argv[i] );
    		if(strcmp(argv[i], "-k")==0)
    			k =atoi(argv[i+1]);
	  		else if(strcmp(argv[i], "-q")==0)
	  			q=atoi(argv[i+1]);
	  		else if(strcmp(argv[i], "-m")==0)
	  			max =atoi(argv[i+1]);
	  		else if(strcmp(argv[i], "-i")==0)
	  			ipt_sz =atoi(argv[i+1]);
    	}
    }
	else
    	printf("The command had no other arguments.\n");

    printf("k=%d-q=%d-max%d\n",k,q,max );
    if(ipt_sz < 2*k){
    	printf("%s\n","ERROR: IPT size is smaller than 2*k." );
    	return;
    }
	struct PTEntries *IPT = malloc(ipt_sz*sizeof(struct PTEntries));
	for(i=0;i<2*k;i++){		//init
		IPT[i].valid = 0; 
		strcpy(IPT[i].page,"-");
		IPT[i].act='-';
		IPT[i].pid = 0;
	}
	printf("IPT AT FIRST!:\n");
	for(i=0;i<2*k;i++){
		printf("|%c|%s|%d|\n",IPT[i].act,IPT[i].page,IPT[i].valid );
	}
	FILE *fp1, *fp2;
	fp1 = fopen("bzip.trace", "r");
	fp2 = fopen("gcc.trace", "r");
	char s[2]=" ", *token;
	int count1=0, count2=0;
	while(1){
		char line[12], address[8], act;
		if(!process1.alive && !process2.alive)
			break;
		printf("-Process #1 acts:");
		if(process1.alive){
			for(i=0;i<q;i++){
				printf("%s%d=========\n","~~~>Q : ",i );
				fgets(line, sizeof(line), fp1);
				printf("%s",line);
				token = strtok(line, s);
				strcpy(address, token);
				while( token != NULL ){
    				token = strtok(NULL, s);
    				if(token==NULL) break;
    				act=token[0];
   				}
   				for(j=0;j<5;j++)
   					pnum[j]=address[j];
   				pnum[5]='\0';	
   				printf("ADDRES: %s \nACT: %c\nP#: %s \n", address,act,pnum);
   				FWF(IPT, k, pnum, act, process1.pid);
   				count1++;
				if(max != 0 ){
   					if(count1 == max){
   						process1.alive=0;
   						printf("process 1 died\n");
   						break;
   					}
   				}
   				printf("%s\n","back1" );
			}
		}
		printf("-Process #2 acts:");
		if(process2.alive){
			for(i=0;i<q;i++){
				printf("%s%d=========\n","~~~>Q : ",i );
				fgets(line, sizeof(line), fp2);
				printf("%s",line);
				token = strtok(line, s);
				strcpy(address, token);
				while( token != NULL ){
    				token = strtok(NULL, s);
    				if(token==NULL) break;
    				act=token[0];
   				}
   				for(j=0;j<5;j++)
   					pnum[j]=address[j];
   				pnum[5]='\0';
   				printf("ADDRES: %s \nACT: %c\nP#: %s \n", address,act,pnum);
   				FWF(IPT, k, pnum, act, process2.pid);
   				count2++;
				if(max != 0 ){   				
   					if(count2 == max){
   						process2.alive=0;
   						printf("process 2 died\n");
   						break;
   					}
   				}
   				printf("%s\n","back2" );
			}
		}
	}
	for(i=0;i<2*k;i++)
		printf("|%d|%c|%s|%d|\n",IPT[i].pid, IPT[i].act,IPT[i].page,IPT[i].valid );
	printf("Process1 PFs: %d\nProcess2 PFs: %d\n",process1.PFs, process2.PFs );
	printf("Process1 W/R_in_Disc: %d\nProcess2 W/R_in_Disc: %d\n",process1.wr_disc, process2.wr_disc );
	printf("Process1 Writes in Disc: %d\nProcess2 Writes in Disc: %d\n",process1.dirtyPages, process2.dirtyPages );
	fclose(fp1);
	fclose(fp2);
}