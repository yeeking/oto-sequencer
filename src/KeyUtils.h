#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>

class KeyReader 
{
    public:
        KeyReader(std::string device = "/dev/input/event0")
        {
            if ((file_ref = open("/dev/input/event0", O_RDONLY)) < 0) {
                perror("KeyReader::construct cannot read keyboard device ");
            }
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

