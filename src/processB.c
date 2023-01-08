#include "./../include/processB_utilities.h"
#include "./../include/utilities.h"

/*Bitmap object*/
bmpfile_t * bmp;
/*Data type for define a BGRA pixel*/
rgb_pixel_t pixel = {255, 0, 0, 0};

void load_bmp_sm(int * ptr, bmpfile_t * bmp){

    /*Open semaphore*/ 
    sem_t * sem_id1 = sem_open(sem_path_1, 0);
    sem_t * sem_id2 = sem_open(sem_path_2, 0);

    /*sem wait*/
    sem_wait(sem_id1);

    /*Starting critical section*/
    
    for(int i = 0; i < SM_HEIGHT; i++){
        for(int j = 0; j < SM_WIDTH; j++){
            int offset_blue = i*SM_WIDTH+j+1;
            int offset_green = offset_blue + COLOR_SEG;
            int offset_red = offset_green + COLOR_SEG;

            pixel.blue = ptr[offset_blue];
            pixel.green = ptr[offset_green];
            pixel.red = ptr[offset_red];

            bmp_set_pixel(bmp, i, j, pixel);
        }
    } 

    /*sem signal*/
    sem_post(sem_id2); 
}

int find_circle(bmpfile_t * bmp){
    const int SCLAE_SEARCH_FACTOR = 4;

    for(int i = 0; i < SM_HEIGHT; i+= SCLAE_SEARCH_FACTOR){
        for(int j = 0; j < SM_WIDTH; j+= SCLAE_SEARCH_FACTOR){
            if(bmp_get_pixel(bmp,i,j)->blue != 0) return i;
            if(bmp_get_pixel(bmp,i,j)->green != 0) return i;
            if(bmp_get_pixel(bmp,i,j)->red != 0) return i;
        }
    }
    return 0;
}

CIRCLE get_center(bmpfile_t * bmp, int row) {
    int mark_l, mark_r;
    int state = 0;
    int center[2];
    CIRCLE c;
    for(int i = row; i < SM_HEIGHT; i++) {
        for(int j = 0; j < SM_WIDTH; j++) {
            int blue = bmp_get_pixel(bmp,i,j)->blue;
            int green = bmp_get_pixel(bmp,i,j)->green;
            int red = bmp_get_pixel(bmp,i,j)->red;
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

    // Infinite loop
    while (TRUE) {       

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
            mvaddch(LINES/2, COLS/2, '0');
            refresh();
        }
    }

    endwin();
    return 0;
}
