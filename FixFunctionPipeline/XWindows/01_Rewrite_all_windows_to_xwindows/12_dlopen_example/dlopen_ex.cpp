#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <gnu/lib-names.h>

typedef double (*SqrtFn)(double);

SqrtFn pfn = NULL;
void *handle;

int main() {
    
    //loading the library
    handle = dlopen(LIBM_SO, RTLD_LAZY);
    
    if(!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return(EXIT_FAILURE);
    }
    dlerror(); // clearing existing error
    // getting fuction pointer using the handle
    pfn = (SqrtFn) dlsym(handle, "sqrt");
   

    if(pfn == NULL) {
        fprintf(stderr, "%s\n", dlerror());
        return(EXIT_FAILURE);
    }
    
    //Using the function
    printf("Squar root of %lf is %lf \n",16.0, pfn(16.0));
    
    dlclose(handle);

    return(EXIT_SUCCESS);

}
