#include <stdio.h>
#include <stdlib.h> // per malloc 
#include <string.h> 
#include <unistd.h> // serve per fork exec
#include <sys/wait.h> // serve per wait

 
#define N 100  // macro 


// for now i only know this let's do the first part while

int main (){
    // when i call function main i should show mi enter script
    // we need variabile to allocate the comanda
    char cmd[N];
    while(1){ // infinite loop asking the command 
        printf("please enter the cmd\n");
        if (fgets(cmd,N,stdin)==NULL){
            perror("Errore in apertura e lettura dalla tastiera");
            break;
        } // con questo sto dicendo che leggi dal fp il puntatore alla tastiera
        /// EVERYTHING IS A FILE 
        cmd[strcspn(cmd, "\n")] = 0;  // togliere \n e mettere \0

     char *argtok[N];
        //  dobbiamo fare la token
        char *token = strtok(cmd," ");
        int i = 0; 
        while (token !=NULL){
            argtok[i]=token; // in questo metto il token
            i++;
            token = strtok(NULL," "); // vai alla next token
        }
        argtok[i]=NULL; // questo e' fondamentale: argtok deve finire con NULL; ora non prendi arg da 
        // cmd ma da argtok 
// Controllo anti-crash: Se non hai scritto nulla, ricomincia
        if (argtok[0] == NULL) {
            continue; // "Salta tutto il resto e torna all'inizio del while" se non hai il ind di programma
        }

        // tutto bene!! fino a qui abbiamo letto dal file tastierea , ora arriva arogmento nuovo fork
        int pid = fork(); // questo mi crea un clone del processo padre principale, pid = process ID
        // verifichiamo se abbiamo creato il clone 
        if (pid <0){
            perror("errore nella creazione del figlio");
        }
        if (pid ==0){
            printf("sono il processo figlio e il mio pidf:%d",getpid());
            // ora devo eseguire il commandi
            execvp(argtok[0],argtok);// vado a eseguire il commando dal fp stdin tastiera
            // se arriviamo qui significa che fallito il figlio 
            printf("fallito il figlio\n");
            exit(1); // se fallisce deve uscire 
          }
       else  if (pid>0){
            // allora aspetto che il mio finisce il suo lavoro 
            wait(NULL);
          }

    }

    return 0;
}