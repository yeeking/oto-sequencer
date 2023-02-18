#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <iostream> 
#include <mutex>

#include <JuceHeader.h>

#pragma once 

class FrameBuffer {

    public:
         struct fb_var_screeninfo vinfo;
        struct fb_fix_screeninfo finfo;
        char *fbp;
        int fbfd = 0;
        long int screensize = 0;
        std::mutex mtx;

      FrameBuffer(){

        // Open the file for reading and writing
        fbfd = open("/dev/fb0", O_RDWR);
        if (fbfd == -1) {
            perror("Error: cannot open framebuffer device");
            initSuccessful = false; 
            return; 
            //exit(1);
        }
        printf("The framebuffer device was opened successfully.\n");

        // Get fixed screen information
        if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
            perror("Error reading fixed information");
            initSuccessful = false; 
            return; 
            //exit(2);
        }

        // Get variable screen information
        if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
            perror("Error reading variable information");
            //exit(3);
            initSuccessful = false; 
            return; 
        }

        printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

        // Figure out the size of the screen in bytes
        screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

        // Map the device to memory
        fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
                        fbfd, 0);
        if ((long int)fbp == -1) {
            perror("Error: failed to map framebuffer device to memory");
            //exit(4);
            initSuccessful = false; 
            return; 
        }
        printf("The framebuffer device was mapped to memory successfully.\n");
        initSuccessful = true; 

        }


        /** clear up framebuffer resources */
        ~FrameBuffer(){
            munmap(fbp, screensize);
            close(fbfd);
        }
        bool ready(){
            return initSuccessful; 
        }
        /** send the RGB values in the sent graphics buffer
         * to the framebuffer 
        */
        void update(juce::Image& img){// TODO: receive an image cos can extract pixel values
       //void update(){
            
            mtx.lock();
            updateDisplay(img);
            mtx.unlock();
        }
   

    private:


        
        bool initSuccessful;
        void updateDisplay(juce::Image& img){
            //std::cout << "FB::updateDisplay" << std::endl;
            //printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

            long int location = 0;
            int x = 0, y = 0;

            // Figure out where in memory to put the pixel
            for (y = 0; y < vinfo.height; y++){
            for (x = 0; x < vinfo.width; x++) {

            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                    (y+vinfo.yoffset) * finfo.line_length;
            if (vinfo.bits_per_pixel == 16){
                
                juce::Colour c = img.getPixelAt(x, y);
                int r = c.getRed();
                int g = c.getGreen();     // A little green
                int b = c.getBlue();    // A lot of red
                unsigned short int t = r<<11 | g << 5 | b;
               *((unsigned short int*)(fbp + location)) = t;
            }


            }
            }
        }


       
};



