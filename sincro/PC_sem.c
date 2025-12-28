#include <stdio.h>
#include <threads.h>
#include <unistd.h>
#include <semaphore.h>
#define N 5 // per il size del buffer
int buffer[N]; // nella memoria statica
    sem_t vuoti; // Conta quanti SLOT LIBERI sono rimasti 
    sem_t pieni; // per indicare che il buffer e' pieno
    sem_t lock;
 int in,out =0;

// i due prototipi pro e con
int pro (void *arg); // sempre questa firma 
int con (void *arg); 

int main (){

    
    sem_init(&vuoti,0,N); // per dire abbiamo N spazi vuoti 
    sem_init(&pieni,0,0);// Inizializzo a 0 perché all'inizio ci sono ZERO elementi da consumare
    sem_init(&lock,0,1); // 1 indica lo stato libero del lock 
     thrd_t TA,TB; // due variabili per thread del tipo thrd_t
    // creazione dei 
    if (thrd_create(&TA,pro,NULL)!=thrd_success){
        perror("Errore nella creazione del threadA");
        return -1;
    }
    if(thrd_create(&TB,con,NULL)!=thrd_success){
        perror("Errore nella creazione del threadB");
        return -1;
    }
    // una volta hai creato due thread per eseguire i funzioni pro e con 
    thrd_join(TA,NULL); // aspettimao che finisicono e null perche non restituiscono nulla
    thrd_join(TB,NULL);
    //  i thrd_join restituisce il status code
    sem_destroy(&lock);
    sem_destroy(&vuoti);
    sem_destroy(&pieni);
    // distruggiamo quando abbiamo finito di usarli 
    return 0;
}


// definizione dei funzioni 
int pro (void*arg){
    // produciamo 
    for (int i =0; i< 20;i++){
        int data= i+10;
        sem_wait(&vuoti);// Decremento i posti liberi. Se 0, mi blocco (Buffer Pieno)
        sem_wait(&lock);
        buffer[in]=data;
        in = (in+1)%N;
        printf("[PROD] Prodotto: %d (in: %d)\n", data, in);
        sem_post(&lock);// rilascio il lock
        sem_post(&pieni); // Incremento il conteggio degli elementi. Se era 0, sveglio il consumatore.
        // sleep(1); // dormo per un secondo 
    }
    return 0;
}
 

// consumatore 
int con (void*arg){
    for (int i = 0; i<20;i++){
        sem_wait(&pieni);// Decremento gli elementi disponibili. Se 0, mi blocco (Buffer Vuoto)
        sem_wait(&lock);// ce la data e per entrare nella SC prendo prima la chiave 
        int data = buffer[out]; // leggo la data dal buffer 
        out = (out+1)%N; // il buffer circolare 
        printf("ho letto la data:%d\n", data);
        sem_post(&lock);// lascio la chiave: ovvero aumento il valore dal 0 a 1;
        sem_post(&vuoti); // Segnalo che c'è UNO spazio libero in più (incremento contatore)
        sleep(1); // dormi per 1 secondo
    }
    return 0;
}