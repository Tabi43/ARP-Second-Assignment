# Second Assignment ARP

The second assignment consist of a simulation of an industrial situation. There is two process (A and B) with their own window. 
On the window A We have a UI to control the position of a semplified circle and a simple print button which is used to take a snapshot of the current shared memory. The user can control the position of the circle by the arrow keys on the keyboard. At any change of the circle's position the process copy a bitmap image to the shared memory.
In the window B we have the position of circle's center in real time analizing the shared memory.

All snapshot are saved inside the directory ```/out```. Each snapshot is numbered from 0.

## Installing and Run
----------------------
To compile the whole simulation is provided a shell script runnable by:
```
    $ ./compile.sh
```

To work properly you need to have installed the *libbitmap* library. *libbitmap* is a library to create and interact with simple bitmap object.

IF YOU HAVEN'T INSTALED THIS LIBRARY FOLLOW THESE STEPS:
1. Download the source code from [this GitHub repo](https://github.com/draekko/libbitmap.git) in your file system.
2. Navigate to the root directory of the downloaded repo and run the configuration through command ```./configure```. Configuration might take a while.  While running, it prints some messages telling which features it is checking for.
3. Type ```make``` to compile the package.
4. Run ```make install``` to install the programs and any data files and documentation.
5. Upon completing the installation, check that the files have been properly installed by navigating to ```/usr/local/lib```, where you should find the ```libbmp.so``` shared library ready for use.
6. In order to properly compile programs which use the *libbitmap* library, you first need to notify the **linker** about the location of the shared library. To do that, you can simply add the following line at the end of your ```.bashrc``` file:      
```export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"```

Then you can run the simulation buy running directly the master process or running this code:
```
    $ ./run.sh
```
```
    $ ./bin/master
```
## User Guide
----------------------
The user, after having followed the instructions to compile the code, will see the simulation of the project. 
1. The simulation will open two windows: one for the moving image simulation and one for the position trace of the center of the image.
    1. In the first window, you will see a spot that you can move around using the arrow keys. This simulates the movement of a video camera.
    2. In the second window, you will see a point that represents the center of the image in the first window. The position of this point will change as you move the spot in the first window.
	3. As you move the spot around, the center of the image will be updated and the point in the second window will change its position accordingly.

2. Pressing an array key you can move the object that simulates the movement of a video camera, instead by clicking on the relative button (P) using your mouse you can save a snapshot of the image memory in a .bmp file in the "out" directory.
    1. This function is useful for saving a specific frame of the simulation.
	2. Each time a snapshot is taken, the file will be saved with a different name, starting from 0.

3. To stop the simulation, press "Ctrl + C" in the terminal, this will stop the execution of the program and close all open windows.



## How it works
----------------------
*	The master process forks two child processes (processA and processB) using the "spawn" function. These processes are responsible for simulating the moving image and extracting the feature from the acquired moving image, respectively. The master process also instantiates shared memory and semaphores for synchronization between the two processes.
*	Process A is responsible for simulating the moving image. It uses the ncurses library to create a window where the user can move a spot using arrow keys.
	* The position of the spot is used to create an image in the shared memory. This image is a colored circle in the center of the window.
    * The shared memory is used to store the image created by process A and accessed by process B.
	    * The shm_open function is used to create the shared memory object and the ftruncate function is used to configure the size of the shared memory object.
	    * The mmap function is used to memory map the shared memory object, allowing process B to access it.
	    * The ptr variable is used to point to the memory mapped area, allowing access to the shared memory.
	* Has a function for taking a snapshot of the image memory, which saves the current image in a .bmp file in the "out" directory.
    * Uses the bitmap object to handle the image and create the colored circle.
        * The bitmap library is used to create, edit and save the image in a .bmp file.
	    * The bmp_create function is used to instantiate the bitmap object, passing three parameters: width, height and depth of the image.
	    * The bmp_set_pixel function is used to color the pixel at the specified (x,y) position with the given pixel values.
    * Uses semaphores for synchronization with process B to ensure that the image in the shared memory is not accessed while it is being updated.
        * Semaphores are used to avoid problems when both process A and process B try to access the shared memory at the same time.
	    * The sem_wait function is used to wait for the semaphore before accessing the shared memory, and the sem_post function is used to signal that the semaphore is available again after it has been accessed.
	    * The sem_open function is used to open the semaphore and the sem_init function is used to initialize the semaphore with a value of 1, indicating that it is available for use.
*	Process B is responsible for extracting the feature from the acquired moving image. It uses the shared memory created by process A to access the image and extract the center of the image.
	* The center of the image was calculated by adding the coordinates of the pixels corresponding to the circle.
    * The position of the center is then displayed in a second ncurses window as a point represented by the number 0.
	* Process B uses semaphores for synchronization with process A to ensure that the image in the shared memory is not accessed while it is being updated by process A.
    * The ncurses library is used to create the window and display the position of the center of the image.
        * The initscr function is used to initialize the ncurses library and create the window.
	    * The mvaddch function is used to update the position of a circle on the screen in real-time as the user moves the camera.
        * The refresh function is used to update the window and display the changes made.
	    * The endwin function is used to end the ncurses session and restore the terminal to its original state.





