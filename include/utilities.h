#include <stdio.h>
#include <bmpfile.h>
#include <stdlib.h>
#include <math.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/shm.h> 
#include <sys/stat.h>
#include <sys/mman.h>
#include <bmpfile.h>
#include <semaphore.h>

/*Parameters*/
 int SM_WIDTH = 1600;
 int SM_HEIGHT = 600;

int n_curses_width = 80;
int n_curses_height = 30;

int SM_FACTOR = 20;

#define shm_name "/AV"

int SHM_SIZE = SM_WIDTH*SM_HEIGHT*3;

#define sem_path_1 "/sem_AV_1" //Sem_procuder
#define sem_path_2 "/sem_AV_2" //Sem_consumer



/*Functions*/
