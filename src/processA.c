#include "./../include/processA_utilities.h"
#include "./../include/utilities.h"

/*Bitmap object*/
bmpfile_t * bmp;

/*Data type for define a BGRA pixel*/
rgb_pixel_t pixel = {255, 0, 0, 0};
rgb_pixel_t empty_pixel = {255, 255, 255, 0};

int old_x, old_y; //Cordinate of the circle on the interface

/*Shared memory*/
int shm_fd; 
rgb_pixel_t * ptr;

/*Print counter*/
int print_counter = 0;

/*Semaphores*/ 
sem_t * sem_id1;
sem_t * sem_id2;

bool take_snapshot(){
    char path[20];

    snprintf(path, 20, "out/%d.bmp", print_counter);
    print_counter += 1;

    bmp_save(bmp, path);   
}

void draw__colored_circle_bmp(bmpfile_t * bmp, int xc, int yc){   
  for(int x = -RADIUS; x <= RADIUS; x++) {
        for(int y = -RADIUS; y <= RADIUS; y++) {
        // If distance is smaller, point is within the circle
            if(sqrt(x*x + y*y) < RADIUS) {
                /*
                * Color the pixel at the specified (x,y) position
                * with the given pixel values
                */
                bmp_set_pixel(bmp, xc + x, yc + y, pixel);
            }
        }
    }
}

void draw__empty_circle_bmp(bmpfile_t * bmp, int xc, int yc){
    for(int x = -RADIUS; x <= RADIUS; x++) {
        for(int y = -RADIUS; y <= RADIUS; y++) {
        // If distance is smaller, point is within the circle
            if(sqrt(x*x + y*y) < RADIUS) {
                /*
                * Color the pixel at the specified (x,y) position
                * with the given pixel values
                */
                bmp_set_pixel(bmp, xc + x, yc + y, empty_pixel);
            }
        }
    }
}

void load_bmp_to_shm(bmpfile_t * bmp, rgb_pixel_t * ptr){
    int pos = 0;   

    /*Sem wait*/
    sem_wait(sem_id1);

    /*Loading pixel*/
    for(int i = 0; i < SM_WIDTH; i++){
        for(int j = 0; j < SM_HEIGHT; j++){
            pos = (i*SM_WIDTH)+j+1; 
            ptr++;             
            /*BGRA*/
            rgb_pixel_t * tmp_p = bmp_get_pixel(bmp,i,j);
            int b = tmp_p->blue;
            int g = tmp_p->green;
            int r = tmp_p->red;
            int a = tmp_p->alpha;
            rgb_pixel_t alfio = {b,g,r,a};          
            *ptr = alfio;                
        }
    }

    /*sem signal*/
    sem_post(sem_id2);    
}

/*Producer-Server*/
int main(int argc, char *argv[])
{
    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    //Open semaphores
    sem_id1 = sem_open(sem_path_1, 0);
    sem_id2 = sem_open(sem_path_2, 0);

    // Initialize UI
    init_console_ui();

    // Instantiate bitmap
    bmp = bmp_create(SM_WIDTH, SM_HEIGHT, DEPTH);

    // create the shared memory object
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);

    /* configure the size of the shared memory object */
    ftruncate(shm_fd, SHM_SIZE);

    /* memory map the shared memory object */
    ptr = mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

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
            float scale_x = SM_WIDTH/(COLS-BTN_SIZE_X);
            float scale_y = SM_HEIGHT/LINES;
            draw__empty_circle_bmp(bmp, floor(circle.x*scale_x), floor(circle.y*scale_y));
            move_circle(cmd);
            draw__colored_circle_bmp(bmp, floor(circle.x*scale_x), floor(circle.y*scale_y));
            draw_circle();     
            /*Sync with Shared memory image*/
            load_bmp_to_shm(bmp, ptr);
        }
    }
    
    endwin();
    return 0;
}
