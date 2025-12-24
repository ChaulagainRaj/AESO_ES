
// stare attento con i puntatori 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // per fork(), exec()
#include <sys/wait.h> // wait 
#include <threads.h>  // per thread


// in this exercise we will see the use of lock mutex

// prototypes
// signature of the functions
int funt1(void *n );
int funt2(void *n); // declaration of these two functions which will take shared data as parameters & modify it 
mtx_t lucchetto; // let's declare lock which will be used by all threads
int main (){
    // let's use lock 
    // configuration of locks with mtx_plain , simple lock which is can only be used for close and open lock
    if(mtx_init(&lucchetto,mtx_plain)!=thrd_success){
        perror("Errore in configuration of locks");
        return -1;
    }
    // shared data to accesso by two different thread
    int count=0;
    int par=2;

    // let's create a thread :
    thrd_t th1; // questo ci contiene thread ID;
    thrd_t th2; //we have two threads
    // 1: create a thread
    int ret1 =  thrd_create (&th1,funt1,&count); // il ret1 returns status code 
    // verifichiamo se tutto bene 
    if(ret1!=thrd_success){
        perror("Errore nella creazione del thread");
        return -1;
    }
    int ret2 = thrd_create(&th2,funt2,&count);
    // verifichiamo 
    if(ret2!=thrd_success){
        perror("Errore nella creazione del thread");
        return -1;
    }

    // allora dobbiamo avere qualcosa per tenere il risultato dei threads
    int res1,res2;
    thrd_join(th1,&res1);
  printf("stampiamo il valore di res1:%d\n",res1);

    thrd_join(th2,&res2); // bisogna fare due thrd_join // si puo fare con un ciclo questo 
  printf("stampiamo il valore di res2:%d\n",res2);

  printf("stampiamo il valore di count:%d\n",count);
  mtx_destroy(&lucchetto);
    return 0;
}



// definition of the functions 
int funt1( void* n ){
    mtx_lock(&lucchetto); // lock critical section
    // n e' un puntatore quindi dobbiamo accedere il valore dell'indirizzo che ha n quindi differenziare
    int *pointerToInt = (int*)n; // casting esplicito
    (*pointerToInt)++; // add 1 and return : con operatore* defferenzzazione
    printf("ho modificato il valore di n:\n");
    // printf("ora lascio il lucchetto\n");

    mtx_unlock(&lucchetto); // after finishing release lock 
    return *pointerToInt; // perche si accede tramite ind : by reference not by value

}
int funt2( void* n ){
    mtx_lock(&lucchetto); // loc
    int *pointerToInt = (int*)n;
    (*pointerToInt)++; // add 1 and return 
    printf("ho modificato il valore di n\n");
    // printf("ora lascio il lucchetto\n");
    mtx_unlock(&lucchetto);
    return *pointerToInt;
}