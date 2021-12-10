// #include "kernel/stat.h"
#include "kernel/types.h"
#include "user/user.h"

int
main (int argc, char const *argv[])
{
  
  if(argc !=2){
    fprintf(2, "Usage: sleep [second]\n");
    exit(1);
  }
  for(int i=0; i<strlen(argv[1]); i++){
    if(argv[1][i]<'0'||argv[1][i]>'9'){
      fprintf(2, "wrong input");
      exit(1);
    }

  }
  int second = atoi(argv[1]);
  sleep(second);
  exit(0);
}
