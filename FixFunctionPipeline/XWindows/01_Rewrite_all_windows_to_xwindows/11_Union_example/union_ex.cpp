#include<stdio.h>
#include<stdlib.h>
#include<string.h>

union Data {
    int intData;
    float floatData;
    char str[20];
};

int main (){
    union Data data;
    printf("Total Size of union is %lu\n", sizeof(data));


    printf("******** Ovewritting the data with string 'dummy data' **********\n");
    data.intData = 100;
    data.floatData = 120.5;
    strcpy(data.str, "dummy data");

    printf("data.intData = %d\n",data.intData);
    printf("data.floatData = %f\n",data.floatData);
    printf("data.str = %s\n", data.str);

    printf("******** Ovewritting the data with int 100 **********\n");
    data.floatData = 120.5;
    strcpy(data.str, "dummy data");
    data.intData = 100;

    printf("data.intData = %d\n",data.intData);
    printf("data.floatData = %f\n",data.floatData);
    printf("data.str = %s\n", data.str);

    return 0;
}