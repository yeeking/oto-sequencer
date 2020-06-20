#include <functional>
#include <iostream>
#include <stdio.h>  


//#include "../lib/grovepi.h"
//#include "../lib/grove_rgb_lcd.h"
#include "../lib/ml/rapidLib.h"

#include "SimpleClock.h"
#include "Sequencer.h"
//#include "GroveUtils.h"
#include "RapidLibUtils.h"


#include <unistd.h>
#include <termios.h>

// getch with no echo and instant response
// https://stackoverflow.com/a/912796/1240660
char getch() {
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

void redraw(Sequencer& seqr, SequencerEditor& seqEditor)
{
  std::cout << "\x1B[2J\x1B[H";
  std::string disp = SequencerViewer::toTextDisplay(8, 32, &seqr, &seqEditor);
  std::cout << disp << std::endl;
}

int main()
{
    Sequencer seqr;
    SequencerEditor seqEditor{&seqr};
    SimpleClock clock{};
    // this will map joystick x,y to 16 sequences
    rapidLib::regression network = NeuralNetwork::getMelodyStepsRegressor();

    clock.setCallback([&seqr, &seqEditor](){
            seqr.tick();
            // only redraw if in step selecting mode
            if (seqEditor.getEditMode() == SequencerEditorMode::selectingStep)
            {
              redraw(seqr, seqEditor);    
            }
            // otherwise, it will redraw as they move around
        });

   clock.start(1000);

 
    char x {1};
    bool escaped = false;
    while (x != 'x')
    {
      x = getch();





      if (x == '\033') {
        escaped = true;
        continue;
      }
      if (escaped){
        switch(x){
          case '[': 
            continue;
          case 'A':
            // up
            seqEditor.moveCursorUp();
            redraw(seqr, seqEditor);
            continue;
          case 'D':
            // left
            seqEditor.moveCursorLeft();
            redraw(seqr, seqEditor);
            continue;
          case 'C':
            // right
            seqEditor.moveCursorRight();
            redraw(seqr, seqEditor);
            continue;
          case 'B':
            // down
            seqEditor.moveCursorDown();
            redraw(seqr, seqEditor);
            continue;     
        }
      }
    }

  
  return 0;
}
