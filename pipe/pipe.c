// versione con syscall 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // per il fork(), pipe (), fopene(),,,,
#include <sys/wait.h> // per wait() sincronizzazione tra padre e figlio 


int main (){
    // apriamo il file con il fopen // se non esiste il file va a creare 
    FILE *fp = fopen("info.txt","r");
    // questo mi crea il file.text in questo cartello 
    if(fp==NULL){
        perror("Errore nella apertura del file");
        return (1);
    }
    char buffer[100];
    char *token; // un puntatore 
    // leggiamo dal disco al RAM ovvero scriviamo al buffer
    while(fgets(buffer,100,fp)!=NULL){
        // Rimuoviamo il newline '\n' che fgets include
        buffer[strcspn(buffer, "\n")] = 0;
        //correction: we use return value for flow :NULL
        // leggi dal disco e ora tokenizzare quello letto:
        token = strtok(buffer," ");
        int i =0;
        while (token !=NULL){
            // prima di creare il processo child we have to create tunnel uni direzionale per la 
            // communicazione tra padre e figlio e usiamo la funzione pipe()
            int pipefd[2]; // usiamo per file descriptor del tubo
            // correction: fd is an array of integers 
            // creazione del pipe 
            if(pipe(pipefd)==-1){ //
                //correction: pipe create an buffer in kernel with two fd[0] read: input and fd[1] write
                perror("Errore nella creazione del tubo");
                // we created tube here to read and write from kernel's buffer
                return 1;
                // questo pipe(fd) ha creato fd[0] leggere e fd[1] per scrivere 
            }
            // ora facciamo il fork
            //correction: now we have to create a new processo: child process
            int pid = fork();
            // questo creato il processo figlio 
            //correction: create a child process with fork
            // verificaion 
            if(pid > 0){
                // father's world
                printf("Creation of child process successfully done\n");  
                // correction: we want to write in kernel's buffer so we have to close read array so 
                close(pipefd[0]);
                // tutta la teoria: convertire intero a FILE all'oggetto
                write(pipefd[1],token,strlen(token)+1);
                // explaination why we use strlen instead of sizeof : because when father process write in buffer 
                // with strlen write in this way ciao\n which indicate the terminatore also so when child process
                // goes to read from bufpadre he will know when to stop as it also finds \n as consequences no 
                // segmentation fault: 
                // if we use sizeof(token) : as we know token is pointers and in 64 bit system it will return 8 bytes
                // so sizeof(token) = 8 bytes and it will write ciao without terminator and child processo will not know
                // when to stop reading as it wont be able to find \n and try to read from other's memory 
//                 Parola,token punta a...,sizeof(token),strlen(token)
// """SI""","""SI\0""",8 (dimensione indirizzo),2 (lunghezza testo)
// """PRECIPITEVOLISSIMEVOLMENTE""","""PREC...""",8 (dimensione indirizzo),26 (lunghezza testo)
                // ok questo mi scrive il token in fd[1] scrittura
                //correction: let's write on K's buffer: written by father process
                printf(" primo token:%d,sono il padre e il mio pid:%d e ho scritto il token:%s\n",i,getppid(),token);
                close(pipefd[1]); // per dire che ho finito di scrivere al K's buffer
                //correction: after writing let's close il file descriptor for writing of father process
                // ora aspetto il figlio cosi non diventa orfano o zombie 
                wait(NULL);
                // correction: once we finish we have to wait for child process to finish it's execution
            }else{
                // son's world
            close(pipefd[1]);// chiudo la scrittura per il processo figlio
            // correction: close write fd of child as we only want to read from K's buffer 
            char bufpadre[10]; // il posto per mettere il dati ricevuti 
            //correction: why do we need a bufpadre ovvero this bufpadre in child process?
            // ans: because we have to bring the data written in k's buffer to the virtual memory of child process
            read(pipefd[0],bufpadre,sizeof(bufpadre));
            // in this way, we can read from buffer : work of coping in bufpadre is done by fread
            printf(" primo token:%d ,sono il figlio con pid:%d e  ho letto il token:%s\n",i,getpid(),bufpadre);
            //ora chiudiamo la lettura 
            close(pipefd[0]);
            exit(0);// quando ha finito deve uscire e resituire il controllo al padre 
            }
            i++;
       token = strtok(NULL," ");
       // riprendi da dove eri // e torna al mondo del padre e padre inizia di nuovo a tokenizzare 
      // correction : start making token from where you left
    }

    
}
fclose(fp);
return 0;
}