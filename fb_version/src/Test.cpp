#include "IOUtils.h"
#include <iostream> 

int main(){
//KeyReader keyr{"/dev/input/by-id/usb-CHERRY_CHERRY_Keyboard-event-kbd"};
KeyReader keyr{};
while(true){
  char c = keyr.getChar();
  printf("Main:: key is %i\n", c);

}
return 0;
}
