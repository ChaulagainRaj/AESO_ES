#include <stdio.h>
#include <threads.h>
#include <unistd.h> // Per la funzione sleep()

// in questo problema utiliziamo il condition variables e mutex quindi bisogna definire 
mtx_t lock;
cnd_t full; // per svelgiare produttore e si trova in consumer
cnd_t empty; // per svegliare il consumatore 

// i prototipi dei funzioni 
#define N 5
int produttore (void *arg);
int consumatore(void *arg);
int buffer[N];
int nelim,in,out,size=0;

int main (){

    mtx_init(&lock,mtx_plain);// configurazione dei lock e un lock semplice 
    cnd_init(&full); // importante 
    cnd_init(&empty);
    // creazione dei thread 
    thrd_t thrdA,thrdB; // due puntatori per scrivere il loro ID
    if(thrd_create(&thrdA,produttore,NULL)!=thrd_success){
        perror("Errore nella creazione dei thread A");
        return -1;
    }

     if(thrd_create(&thrdB,consumatore,NULL)!=thrd_success){
        perror("Errore nella creazione dei thread B");
        return -1;
    }

    // ora abbiamo i due thread , una volta hanno finito di lavorare aspettiamo 

    thrd_join(thrdA,NULL);// null perche non restituiscono nulla, solo modificano 
    thrd_join(thrdB,NULL); // lo stesso motivo 
    mtx_destroy(&lock);
    cnd_destroy(&full);
    cnd_destroy(&empty);
    printf("il programma terminato con successo\n");
    return 0;
}


// definiamo i due funzioni 


int produttore (void*arg){
    //lock prima di entrare in sezione critica 
    for (int i=0;i<5;i++){
        int data = i*10;
    
    mtx_lock(&lock);
    while (nelim==N){ // se non ho piu spazio allora, aspetto 
        cnd_wait(&full,&lock); // questo evita il busy waiting
    }
    // qui devo fare il casting esplicito 
    // int *puntatore = (int *)arg;
    // int data = *puntatore;// se non passo i parametri allora non devo fare il casting
    // produciamo i dati 
    buffer[in]=data; // indice in per dire metti la data 
    printf("la data prodotto:%d\n",buffer[in]);
    // poi vai avanti con il % modulo 
    in =(in+1)%N;
    printf("il next indice entrante:%d\n",in);
    nelim++; // per dire ho appena scritto per la condizione di while
    cnd_signal(&empty); // devo svegliare il consumatore 
    mtx_unlock(&lock);// devo lasciare la chiave 
    sleep(1);
  }; // per vedere la concorrenza: appena produttore finisce di produrre metto a dormire cosi 
  // os e' costretto a passare il controllo del CPU al consuamtore 
  return 0;

}

int consumatore (void *arg){
    for (int i=0; i<5;i++){
    mtx_lock(&lock);
    while (nelim==0){
        cnd_wait(&empty,&lock); // se e' vero allora aspetto il segnale dal produttore e lascio il lock
    }
    int data = buffer[out]; // per dire a prendere la data dal buffer 
    printf("la data consumato dal buffer:%d\n",data);
    out = (out+1)%N;
    printf("il next indice uscente:%d\n",out);

    nelim--; // diminiure per dire ho preso la data 
    cnd_signal(&full); // faccio il signal per svegliare il produttore 
    mtx_unlock(&lock); // lascio la chiave 
    sleep(1);
    } 
    return 0; // importante per ritornare 
}