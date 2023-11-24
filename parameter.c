#include "parameter.h"



bool ProcessCommandLineArgs(int argc, char *argv[], bool *randomMap, bool *saveMap, int *rW, int *rH, int *mapPathIndex,
                            int *artyDataFileIndex) {
    for (int i = 0; i < argc; ++i) {
        printf("%s\n",argv[i]);
    }

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i],"--generate") == 0)
        {
            *randomMap = true;
            if (argc < i+2)
            {
                printf("Generating default size (50km x 50km)\n");
            }
            else{
                sscanf(argv[++i],"%d", rW);
                sscanf(argv[++i],"%d", rH);
            }


        }
        else if(strcmp(argv[i], "--file") == 0)
        {
            if(randomMap)
                *saveMap = true;
            *mapPathIndex = ++i;
        }
        else if(strcmp(argv[i],"--arty")==0)
        {
            *artyDataFileIndex = ++i;
        }
    }

    if(*mapPathIndex == -1 && !*randomMap){
        *randomMap = true;
    }

    if (*artyDataFileIndex == -1){
        printf("Artillery data file required (--arty [filename])\n");
        return false;
    }
    return true;
}
