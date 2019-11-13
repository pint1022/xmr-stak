#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main(int argc, char* arcgv[]) {
  void* FunctionLib;
  const char *dlError;
  void *fn;

  printf("Testing dll shared routine\n");
//  FunctionLib = dlopen("./bin/libxmrstak_ppu_backend.so", RTLD_LAZY);
  FunctionLib = dlopen("./bin/libpintapi.so", RTLD_LAZY);
  dlError = dlerror();
  printf("%s\n",dlError);

  fn = dlsym(FunctionLib,"dlclose");
  dlError = dlerror();
  printf("%s %p\n",dlError,fn);

  dlclose(FunctionLib);

  return 0;
}
