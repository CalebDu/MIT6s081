#include"kernel/param.h"
#include"kernel/types.h"
#include"user.h"
#define MAX_LEN 100

int 
main(int argc, char *argv[])
{
    char* cmd = argv[1];
    char param[MAXARG][MAX_LEN];
    char *m[MAXARG];
    char bf;
    while(1){
        int count = argc - 1;
        memset(param, 0, MAXARG*MAX_LEN);

        for(int i=1; i<argc; i++)
            strcpy(param[i-1], argv[i]);

        int cursor = 0, flag = 0, read_result;
        while((read_result=read(0, &bf, 1)>0)&&(bf!='\n')){
            if(bf==' '&& flag==1){
                count++;
                cursor = 0;
                flag = 0;
            }else if(bf!=' '){
                param[count][cursor++] = bf;
                flag = 1;
            }
        }
        if(read_result <= 0)
            break;
        for(int i=0; i<MAXARG-1; i++)
            m[i] = param[i];
        m[MAXARG - 1] = 0;
        if(fork()==0){
            exec(cmd, m);
            exit(0);
        }else{
            wait((int*)0);

        }
    }    
    exit(0);
}
