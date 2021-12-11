#include "kernel/types.h"
#include "user.h"


void
helper(int* parent)
{
    close(parent[1]);
    int p, n;
    int child[2];
    pipe(child);
    int r = read(parent[0], &p, 4);
    if(r ==0 )
        exit(0);
    if(fork()>0){
        close(child[0]);
        printf("prime %d\n", p);
        while(read(parent[0], &n, 4)>0){
            if(n%p!=0)
                write(child[1], &n, 4);
        }
        close(child[1]);
        wait((int*)0);
        exit(0);

    }else{
        helper(child);

    }
    
}
int 
main(int argc, char const *argv[])
{
    if(argc>1){
        fprintf(2, "primes: to many args!");
        exit(1);
    }
    
    int p[2];
    pipe(p);

    if(fork()>0){
        close(p[0]);
        for(int i=2; i<=35; i++){
            write(p[1], &i, 4);
        }
        close(p[1]);
        wait((int*)0);

    }else{
        helper(p);
    }
    exit(0);
}

