#include "./../include/processB_utilities.h"
#include "./../include/utilities.h"

/*Shared memory*/
int shm_fd; 
rgb_pixel_t * ptr;

/*Semaphore*/
sem_t * sem_id1;
sem_t * sem_id2;

int find_circle(rgb_pixel_t * ptr){    
    const int SCLAE_SEARCH_FACTOR = 4;
    for(int i = 0; i < SM_HEIGHT; i+= SCLAE_SEARCH_FACTOR){
        for(int j = 0; j < SM_WIDTH; j+= SCLAE_SEARCH_FACTOR){

            int offset = i*SM_WIDTH+j;

            if((ptr+offset)->blue != 0) return i;
            if((ptr+offset)->green != 0) return i;
            if((ptr+offset)->red != 0) return i;
        }
    }
    return 0;
}

CIRCLE get_center(rgb_pixel_t * ptr, int row) {
    int mark_l, mark_r;
    int state = 0;
    int center[2];
    CIRCLE c;
    for(int i = row; i < SM_HEIGHT; i++) {
        for(int j = 0; j < SM_WIDTH; j++) {

            int offset = i*SM_WIDTH+j;

            int blue = (ptr+offset)->blue;
            int green = (ptr+offset)->green;
            int red = (ptr+offset)->red;

            switch (state) {
                case 0: {
                    //Search the first left pixel                    
                    if(red) {
                        mark_l = j;
                        state = 1;
                    }
                    if(blue) {
                        mark_l = j;
                        state = 1;
                    }
                    if(green) {
                        mark_l = j;
                        state = 1;
                    }
                };break;
                case 1: {
                    //Search the last right pixel
                    if(!(red && blue && green)){
                        mark_r = j-1;
                        state = 2;
                    }
                };break;
                case 2: {
                    //Lenght mesuration
                    if((mark_r-mark_l) == 60){
                        c.x = i;
                        c.y = mark_l+30;
                        return c;
                    }else{
                        state = 1;
                        mark_l = 0;
                        mark_r = 0;
                    }
                };break;
                default:
                    break;
                }
        }
    }
    c.x = 0;
    c.y = 0;
    return c;
}

int main(int argc, char const *argv[])
{
    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initialize UI
    init_console_ui();

    //Open semaphore
    sem_id1 = sem_open(sem_path_1, 0);
    sem_id2 = sem_open(sem_path_2, 0);

    // create the shared memory object
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);

    /* configure the size of the shared memory object */
    ftruncate(shm_fd, SHM_SIZE);

    /* memory map the shared memory object */
    ptr = mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Infinite loop
    while (TRUE) {       

        /*Wait to enter in the critic section*/
        sem_wait(sem_id1);

        /*Find circle in SM*/
        int line = find_circle(ptr);
        if(line){
            mvaddch(LINES/2, COLS/2, '1');
            refresh();
        }else{
            mvaddch(LINES/2, COLS/2, '0');
            refresh();
        }

        /*Signal exiting from the critic section*/
        sem_post(sem_id2);  

        // Get input in non-blocking mode
        int cmd = getch();

        // If user resizes screen, re-draw UI...
        if(cmd == KEY_RESIZE) {
            if(first_resize) {
                first_resize = FALSE;
            }
            else {
                reset_console_ui();
            }
        }

        else {
            //mvaddch(LINES/2, COLS/2, '0');
            refresh();
        }
    }

    endwin();
    return 0;
}
