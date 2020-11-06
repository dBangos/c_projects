#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
 int nG=0;//number of guides in the room
 int maxG=3;//number of all guides
 int nV=0;
 int maxV=30;
 int nI=0;
 int maxI=2;
 int R=10;//analogia episkeptwn/3enagwn
 int AlreadyEntered [3];//flag wste na ekteleitai mia fora to guide_enter apo ka8e numa guide
 //ews edw einai oi sun8hkes tou senariou
 pthread_mutex_t GuideLock;
 pthread_mutex_t VisitorLock;
 
 
//Guide Functions=======================================================================
void *Guide(void *threadid){
 while (1){
  long tid;
  tid=(long)threadid;
  int intID;
  intID=(int)tid;
  if(AlreadyEntered[intID]==0){
   guide_enter(tid);
  }//endif
  guide(tid);
  guide_exit(tid,AlreadyEntered);
 }//endwhile
}//endGuide
 
void guide_enter(long tid){
 pthread_mutex_lock(&GuideLock);
 nG=nG+1;
 pthread_mutex_unlock(&GuideLock);
 printf("Guide %ld has entered the room, %d guides remaining.\n",tid,nG);
}//endguide_enter
void guide(long tid){
 printf("Guide %ld is working.\n",tid);
 sleep(420);
}//endguide
void guide_exit(long tid,int *AlreadyEntered){
 if((nG-1)*R>=nV){
  if(pthread_mutex_trylock(&GuideLock)==0){
   nG=nG-1;
   printf("Guide %ld has exited the room, %d guides remaining.\n",tid,nG);
   pthread_mutex_unlock(&GuideLock);
   pthread_exit(NULL);
  }//endif
 }//endif
 else{
  printf("Guide %ld attempted to leave but was unable to and is returning to work\n",tid);
  int intID;
  intID=(int)tid;
  AlreadyEntered[intID]=1;
 }//endelse
}
//Visitor Functions ======================================================================
void *Visitor(void *threadid){
 while(1){
  long tid;
  tid=(long)threadid;
  visitor_enter(tid);
  learn(tid);
  visitor_exit(tid);
 }//endwhile
}//endVisitor
 
void visitor_enter(long tid){
 float nVfloat=(float)nV;
 float temp=((nVfloat+1)/R);
 START:
 if(nG>=temp){
  pthread_mutex_lock(&VisitorLock);
  nV=nV+1;
  pthread_mutex_unlock(&VisitorLock);
  printf("Visitor %ld has entered the room, %d visitors remaining.\n",tid,nV);
 }//endif
 else goto START;
}
void learn(long tid){
 printf("Visitor %ld is learning.\n",tid);
 sleep(300);
}//endlearn
void visitor_exit(long tid){
  pthread_mutex_lock(&VisitorLock);
  nV=nV-1;
  pthread_mutex_unlock(&VisitorLock);
  printf("Visitor %ld has left the room, %d visitors remaining.\n",tid,nV);
  pthread_exit(NULL);
}//endvisitor_exit
 
//Inspector Functions=======================================================================
void *Inspector(void *threadid){
 while(1){
 long tid;
 tid=(long)threadid;
 inspector_enter(tid);
 verify_compliance(tid);
 inspector_exit(tid);
 }
}
void inspector_enter(long tid){
 printf("Inspector %ld has entered the room.\n",tid);
}
void verify_compliance(long tid){
 if(nG>=(nV/R)){
  printf("Inspector %ld has verified that the protocol is in place.\n",tid);
 }
 else{
  printf("Inspector %ld has found staff to be inadequate.\n",tid);
 }
}
void inspector_exit(long tid){
 printf("Inspector %ld has completed his task and is exiting the room.\n",tid);
 pthread_exit(NULL);
}
 
 
 
 
 
int main(){
 pthread_mutex_init(&GuideLock,NULL);
 pthread_mutex_init(&VisitorLock,NULL);
 int rcg;
 int rcv;
 int rci;
 long g=0;
 long v=0;
 long i=0;
 pthread_t Gthreads[maxG];
 pthread_t Vthreads[maxV];
 pthread_t Ithreads[maxI];
 int temp1=0;
 int temp2=0;//auth h metavlhth xrhsimopoieitai gia ton xronismo dhmiourgias twn threads
 //Creating First Guide Thread ========================================
 rcg=pthread_create(&Gthreads[g],NULL,Guide,(void*)g);
 //Creating Visitor and Guide Threads============================================
 for(v=0;v<maxV;v++){
  rcv=pthread_create(&Vthreads[v],NULL,Visitor,(void*)v);
  sleep(60);
  temp1++;
  temp2++;
  if((temp1==5)&&(g<maxG-1)){
   temp1=0;
   g++;
   rcg=pthread_create(&Gthreads[g],NULL,Guide,(void*)g);
   if(rcg){
    printf("Error in pthread create %ld\n",g);
    exit(-1);
   }//endif
  }//endif
  if(temp2==14){
   rci=pthread_create(&Ithreads[i],NULL,Inspector,(void*)i);
   i++;
   temp2=0;
   if(rci){
   printf("Error in pthread create %ld\n",i);
   exit(-1);
   }//endif
  }//endif
  if(rcv){
   printf("Error in pthread create %ld\n",v);
   exit(-1);
  }//endif
 }//endfor
 //kanw join ola ta numata wste h main na perimenei thn ektelesh tous prin termatistei
 for(v=0;v<maxV;v++){
  rcv=pthread_join(Vthreads[v],NULL);
  if(rcv){
   printf("Error in pthread join %ld\n",v);
   exit(-1);
  }//endif
 }//endfor
 for(g=0;g<maxG;g++){
  rcg=pthread_join(Gthreads[g],NULL);
  if(rcg){
   printf("Error in pthread join %ld\n",g);
   exit(-1);
  }//endif
 }//endfor
 for(i=0;i<maxI;i++){
  rci=pthread_join(Ithreads[i],NULL);
  if(rci){
   printf("Error in pthread join %ld\n",i);
   exit(-1);
  }//endif
 }//endfor
 printf("Main Completed.\n");
}