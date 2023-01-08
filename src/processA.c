#include "./../include/processA_utilities.h"
#include "./../include/utilities.h"

/*Bitmap object*/
bmpfile_t * bmp;
/*Data type for define a BGRA pixel*/
rgb_pixel_t pixel = {255, 0, 0, 0};
rgb_pixel_t empty_pixel = {0, 0, 0, 0};
int old_x, old_y; //Cordinate of the circle on the interface

/*Shared memory*/
int shm_fd; 
void * ptr;

/*Print counter*/
int print_counter = 0;

bool take_snapshot(){
    char path[20];


    /*snprintf(path, 20, "%s%d.bmp", out_bmp, print_counter);
    print_counter++;
*/
    for(int i = SM_HEIGHT/2; i < SM_HEIGHT; i++){
        for(int j = SM_WIDTH/2; j < SM_WIDTH; j++){
            bmp_set_pixel(bmp, i, j, pixel);
        }
    }

    bmp_save(bmp, "test.bmp");

   
}

void draw__colored_circle_bmp(bmpfile_t * bmp, int xc, int yc){
    const int radius = 30;    
    /*for (int i = xc-radius; i <xc+radius; i++){
        for(int j = yc-radius; j<yc+radius; j++){
            if(sqrt(i*i + j*j) < radius) {
                bmp_set_pixel(bmp, i, j, pixel);
            }
        }
    }*/   
}

void draw__empty_circle_bmp(bmpfile_t * bmp, int xc, int yc){
    const int radius = 30;    
    for (int i = xc-radius; i <xc+radius; i++){
        for(int j = yc-radius; j<yc+radius; j++){
            if(sqrt(i*i + j*j) < radius) {
                bmp_set_pixel(bmp, i, j, empty_pixel);
            }
        }
    }
}

void load_bmp_to_shm(bmpfile_t * bmp, int * ptr){
    int pos = 0;

    /*Open semaphore*/ 
    sem_t * sem_id1 = sem_open(sem_path_1, 0);
    sem_t * sem_id2 = sem_open(sem_path_2, 0);

    /*Sem wait*/
    sem_wait(sem_id2);

    /*Loading pixel*/
    for(int i = 0; i < SM_HEIGHT; i++){
        for(int j = 0; j < SM_WIDTH; j++){
            pos = (i*SM_WIDTH)+j+1;
            ptr[pos] = bmp_get_pixel(bmp,i,j)->blue; 
            ptr[pos+COLOR_SEG] = bmp_get_pixel(bmp,i,j)->green; 
            ptr[pos+COLOR_SEG+COLOR_SEG] = bmp_get_pixel(bmp,i,j)->red; 
        }
    }

    /*sem signal*/
    sem_post(sem_id1); 
    sem_close(sem_id1);
    sem_close(sem_id2);
}

/*Producer-Server*/
int main(int argc, char *argv[])
{
    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initialize UI
    init_console_ui();

    /*Create the bmp object*/
    bmp = bmp_create(SM_WIDTH, SM_HEIGHT, 4);     

    /*Open shared memory*/
    shm_fd = shm_open(shm_name, O_WRONLY, 0666);
    if (shm_fd == 1) {
        printf("Shared memory segment failed\n");
        exit(1);
    }
    /*resizes memory region to the correct size*/
    ftruncate(shm_fd, SHM_SIZE);

    ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    for(int i = SM_HEIGHT/2; i < SM_HEIGHT; i++){
        for(int j = SM_WIDTH/2; j < SM_WIDTH; j++){
            bmp_set_pixel(bmp, i, j, pixel);
        }
    }
    

    bmp_save(bmp, "test.bmp");

    // Infinite loop
    while (TRUE)
    {
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

        // Else, if user presses print button...
        else if(cmd == KEY_MOUSE) {
            if(getmouse(&event) == OK) {
                if(check_button_pressed(print_btn, &event)) {
                    mvprintw(LINES - 1, 1, "Print button pressed");

                    /*Call print to file function*/
                    take_snapshot();

                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS - BTN_SIZE_X - 2; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }
            }
        }

        // If input is an arrow key, move circle accordingly...
        else if(cmd == KEY_LEFT || cmd == KEY_RIGHT || cmd == KEY_UP || cmd == KEY_DOWN) {
            //draw__empty_circle_bmp(bmp, circle.x, circle.y);
            move_circle(cmd);
            draw__colored_circle_bmp(bmp, circle.x, circle.y);
            draw_circle();     
            /*Sync with Shared memory image*/
            //load_bmp_to_shm(bmp, ptr);
        }
    }
    
    endwin();
    return 0;
}
