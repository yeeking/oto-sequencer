#pragma once

#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <termios.h>
#include <fstream>


class Display{
    public:
    //static 

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
        KeyReader(std::string device = "/dev/input/event0")
        {
            if ((file_ref = open("/dev/input/event0", O_RDONLY)) < 0) {
                perror("KeyReader::construct cannot read keyboard device ");
            }
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
          
            int rd, code_id, ev_count, key_code;
            ev_count = 0;
            key_code = 0;
            // keep reading until 
            // we get a key down or key held event
            while(1)
            {
                rd = read(file_ref, ev, sizeof(struct input_event) * 64);
                if (rd > 48) continue; // holding lots of keys, ignore.
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
                // .value tells you key up, down, hold 
                //1 and 2 are key down or held events
               // if (ev[code_id].value == 1 || ev[code_id].value == 2)
                //printf("KeyUtisl:: %i %i \n", key_code, ev[code_id].value);   
               
                if (ev[code_id].value == 0) // only break on key up...
                {
          //          std::cout << "KeyUtils:: breaking " << std::endl;
                    break;
                }
                // else {
                //     std::cout << "KeyUtils:: waiting " << std::endl;
                // }
            }
            //std::cout << "KeyUtils:: returning " << key_code << std::endl;

            return key_code;  
        }

    private:
        struct input_event ev[64];
        int file_ref;
};

