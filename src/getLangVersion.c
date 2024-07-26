    
void getLangVersion() {

    printf("-= C Language version is ");
    
    if (__STDC_VERSION__ >= 201710L)
        printf("C17");
    else if (__STDC_VERSION__ >= 201112L)
        printf("C11");
    else if (__STDC_VERSION__ >= 199901L)
        printf("C99");
    else
        printf("C89/C90");

    printf(" =-\n\n");

}
