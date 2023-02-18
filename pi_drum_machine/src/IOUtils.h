#pragma once

#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <termios.h>
#include <fstream>
#include <map>
#include <vector>

class Display{
    public:

    /** attempts to get the serial device filename 
     * if it fails, returns ""
    */
    static std::string getSerialDevice(std::string devPrefix = "/dev/ttyACM", bool debug = true)
    {
        bool open = false;
        for (auto i=0;i<10;i++)
        {
        
        std::ofstream serial_bus;
        std::string port = devPrefix + std::to_string(i);
        if (debug)
            std::cout << "main.cpp::getSerialDevice trying to open " << port << std::endl;
        try{
            serial_bus.open (port);
            if (serial_bus.is_open()){
            serial_bus.close();
            // got one
            if (debug)
                std::cout << "main.cpp::getSerialDevice opened " << port << std::endl;
            return port; 
            break;
            }
            else {
            if (debug)
                std::cout << "main.cpp::getSerialDevice could not open " << port << std::endl;
            }
        }
        catch(int exception){
            if (debug)
            std::cout << "main.cpp::getSerialDevice open " << port << " failed " << std::endl;
        }
        }
        // fail condition
        return "";
    }

    static void redrawToConsole(const std::string& output)
    { 
        std::cout << "\x1B[2J\x1B[H";
        std::cout << output << std::endl;
    }

    static void redrawToWio(const std::string& device, const std::string& output)
    {    
        std::ofstream serial_bus;
        serial_bus.open (device);
        serial_bus << output << "\t"; // last character triggers the redraw
        serial_bus.close();
    }

};

/** class that provides keyboard input helpers 
 * including low level keyboard input */
class KeyReader 
{
    public:
        //KeyReader(std::string device = "/dev/input/by-id/usb-CHERRY_CHERRY_Keyboard-event-kbd")
        /** create a raw linux key event reader. If device is specified, 
         * use that, otherwise, trys a list of devices until one works
         * 
        */
        KeyReader(const std::string& device = "") : 
        rawEventDeviceReady{false}
        {
            std::cout << "ioutils::keyreader cons setting up dev"  << std::endl;
            std::vector<std::string> devices = {
            "/dev/input/by-id/usb-CHERRY_CHERRY_Keyboard-event-kbd",
            "/dev/input/event3"
            };
           setupRawInputDevice(devices[1]);
        }
        void setupRawInputDevice(const std::string& forceDevice){
            rawEventDeviceReady = false; // try to amake it true! 
  
            if (forceDevice != ""){
                std::cout << "ioutils::setupRawInputDevice opening specificed dev " << forceDevice << std::endl;
                // only try the sent device
                file_ref = open(forceDevice.c_str(), O_RDONLY);
                if (file_ref < 0) {rawEventDeviceReady = false; return;}
                else{rawEventDeviceReady = true;}
               // if (getChar() != 0) {rawEventDeviceReady = true; return;}
            }
            // else {
            //     // iterate over the defaults
            //     for (const std::string& dev : devices){
            //         std::cout << "ioutils::setupRawInputDevice Trying to open and read" << dev << std::endl;
            //         file_ref = open(dev.c_str(), O_RDONLY);
            //         if (file_ref < 0) {
            //             std::cout << "ioutils::setupRawInputDevice Bad file" << dev << std::endl;
            //             continue;
            //             }
            //         if (getChar() != 0) {
            //             std::cout << "ioutils::setupRawInputDevice read a char! " << dev << std::endl;
            //             rawEventDeviceReady = true; 
            //             return;
            //             }
            //     }
            // }

        }
        /** 
         * read a char from keyboard with no echo and instant response
         * https://stackoverflow.com/a/912796/1240660
         */
        static char getCharNoEcho() {
            char buf = 0;
            struct termios old = {0};
            if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
            old.c_lflag &= ~ICANON;
            old.c_lflag &= ~ECHO;
            old.c_cc[VMIN] = 1;
            old.c_cc[VTIME] = 0;
            if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
            if (read(0, &buf, 1) < 0)
                perror ("read()");
            old.c_lflag |= ICANON;
            old.c_lflag |= ECHO;
            if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
            return (buf);
        }
        /** read a character from the raw keyboard device */
        char getChar()
        {
          
            int rd, code_id, ev_count;
            char key_code{0};
            //std::cout << "KeyUtils::getChar called " << std::endl;
            ev_count = 0;
            key_code = 0;
            // keep reading until 
            // we get a key down or key held event
            while(1)
            {
                rd = read(file_ref, ev, sizeof(struct input_event) * 64);
                //printf("read %i\n", rd);
                if (rd > 72) 
                {
                  printf("read size is %i Ignoring\n", rd);
                  continue; // holding lots of keys, ignore.
                }
                ev_count = rd / sizeof(struct input_event);
                if(ev_count == 2)
                {
                    code_id = 0;
                }
                else if (ev_count == 3)
                {
                    code_id = 1;
                }

                // .code tells you the key they pressed
                key_code = ev[code_id].code;
                // they switched keys so we just return the original
                // .value tells you key up, down, hold 
                //1 and 2 are key down or held events
               // if (ev[code_id].value == 1 || ev[code_id].value == 2)
                //printf("KeyUtisl:: %i %i \n", key_code, ev[code_id].value);   
               
                if (ev[code_id].value == 1) // only break on key down (0 is key up)...
                {
                   //std::cout << "KeyUtils:: breaking " << std::endl;
                    break;
                }
                else {
                    //std::cout << "KeyUtils:: waiting " << std::endl;
                }
            }
            //std::cout << "KeyUtils:: returning " << key_code << std::endl;
            return key_code;  
        }
    /** returns a map from the chars obtained from getChar cast to ints to 
     * normak keyboard chars, e.g. 42 -> z
     */
    static std::map<int,char> getCharNameMap()
    {
        std::map<int, char> intToKey = 
      {
        {2, '1'}, 
        {3, '2'},
        {4, '3'}, 
        {5, '4'},
        {6, '5'}, 
        {7, '6'},
        {8, '7'}, 
        {9, '8'},
        {10, '9'},
        {11, '0'},
        {15, '\t'},
        {16, 'q'},
        {17, 'w'}, 
        {18, 'e'}, 
        {19, 'r'}, 
        {20, 't'}, 
        {21, 'y'}, 
        {22, 'u'}, 
        {23, 'i'}, 
        {24, 'o'}, 
        {25, 'p'}, 
        {30, 'a'}, 
        {31, 's'}, 
        {32, 'd'}, 
        {33, 'f'}, 
        {34, 'g'}, 
        {35, 'h'}, 
        {36, 'j'}, 
        {37, 'k'}, 
        {38, 'l'}, 
        {39, ';'}, 
        {28, '\n'}, 
        {44, 'z'}, 
        {45, 'x'}, 
        {46, 'c'}, 
        {47, 'v'}, 
        {48, 'b'}, 
        {49, 'n'}, 
        {50, 'm'}, 
        {51, ','}, 
        {52, '.'}, 
        {57, ' '}, 
        {105, 'L'}, 
        {106, 'R'},
        {103, 'U'},
        {108, 'D'},
      };

      return intToKey; 

    }
    bool isRawDeviceReady(){
        return rawEventDeviceReady;
    }

    private:
        bool rawEventDeviceReady;
        struct input_event ev[64];
        int file_ref;
};

