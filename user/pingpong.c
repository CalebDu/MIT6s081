#include "user.h"
#include"kernel/types.h"

int 
main(int argc, char const *argv[])
{
  int p[2];
  char buf[1024];
  pipe(p);
  int pid =fork(); 
  if(pid>0){
    write(p[1],buf, 1);
    wait((int*)0);
    if(read(p[0], buf, 1)){
      printf("%d: received pong\n", pid);
    }else{
      fprintf(2, "error: parent read");
      exit(1);
    }
  }else{
    if(read(p[0], buf, 1))
      printf("%d: received ping\n", pid);
    else{
      fprintf(2, "error:child read");
      exit(1);
    }
    write(p[1], buf, 1);
  }
  exit(0);
}