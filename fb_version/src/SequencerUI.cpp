#include "SequencerUI.h"

SequencerUI::SequencerUI(Sequencer* _sequencer) 
: sequencer{_sequencer}, 
  editor{_sequencer}, 
  offscreenImg{juce::Image::RGB, 320, 240, true}, 
  painterG{offscreenImg},
  Thread{"keyReaderThread"}

{
    //setWantsKeyboardFocus(true);
    startThread();

}
SequencerUI::~SequencerUI()
{
    stopThread(100);
}
//==============================================================================
void SequencerUI::paint (juce::Graphics& g)
{
}

void SequencerUI::drawSequencer()
{
     int64 start = juce::Time::currentTimeMillis(); 
    int xPos = 0;
    int yPos = 0;
    int rowHeight = offscreenImg.getHeight() / sequencer->howManySequences();
    for (int seq=0;seq<sequencer->howManySequences(); ++seq){
        drawSequence(painterG, sequencer->getSequence(seq), 
        editor.getCurrentSequence() == seq, 
        xPos, 
        yPos, offscreenImg.getWidth(), rowHeight);
        yPos += rowHeight; 
    }
    if (frameBuffer.ready()){
        frameBuffer.update(offscreenImg);
    }
    int64 end = juce::Time::currentTimeMillis(); 
    //DBG("Paint took ms " << (end - start));
}

void SequencerUI::drawSequence(juce::Graphics& g, Sequence* seq, bool editorIsCurrent, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
    unsigned int xPos = x;
    unsigned int stepWidth = width / seq->howManySteps();
    for (unsigned int step=0;step<seq->howManySteps();++step){
        drawStep(g, seq->getStep(step), 
        seq->getCurrentStep() == step, 
        editorIsCurrent && editor.getCurrentStep() == step, 
        xPos, y, stepWidth, height);
        xPos += stepWidth; 
    }
}

void SequencerUI::drawStep(juce::Graphics& g, Step* step, bool isCurrent, bool editorIsCurrent, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
    //std::cout << "Step x, y, w, h" << x << "," << y << "," << "," << width << ","<< height << std::endl;
    if (step->getDataDirect()->at(Step::note1Ind) > 0 )
        g.setColour (juce::Colours::blue);
    else 
        g.setColour (juce::Colours::black);
    // current step overrides other things
    if (isCurrent)
        g.setColour (juce::Colours::red);
    if (editorIsCurrent)
        g.setColour (juce::Colours::yellow);
    
    //g.drawRect(x, y, width-4, height-4);
    g.fillRect((int)x, (int)y, (int)width-4, (int)height-4);
}

void SequencerUI::resized()
{

}

bool SequencerUI::keyPressed(const juce::KeyPress &key)
{
    std::cout << key.getKeyCode() << ":left:" << key.leftKey << std::endl;
    if (key.getKeyCode() == key.leftKey){
        editor.moveCursorLeft();
    }
    if (key.getKeyCode() == key.rightKey){
        editor.moveCursorRight();
    }
    if (key.getKeyCode() == key.upKey){
        editor.moveCursorUp();
    }
    if (key.getKeyCode() == key.downKey){
        editor.moveCursorDown();
    }
    if(key.getKeyCode() == key.spaceKey){
        std::vector<double>* stepData = sequencer->getStepDataDirect(
            editor.getCurrentSequence(), editor.getCurrentStep()
        );
        // toggle it
        stepData->at(Step::note1Ind) = 1 - stepData->at(Step::note1Ind);
    }
    repaint();
    return true; 
}

void SequencerUI::run()
{   

    while(true){    
        char c = KeyReader::getCharNameMap()[keyReader.getChar()];
        if (c == ' '){
                std::vector<double>* stepData = sequencer->getStepDataDirect(
                     editor.getCurrentSequence(), editor.getCurrentStep()
                 );
                 // toggle it
                stepData->at(Step::note1Ind) = 1 - stepData->at(Step::note1Ind);
        }
        switch(c){
            case ' ':
               break;
            case 'U':
                editor.moveCursorUp();
                break;
            case 'D':
                editor.moveCursorDown();
                break;
            case 'L':
                editor.moveCursorLeft();
                break;
            case 'R':
                editor.moveCursorRight();
                break;
                
        }
   //     const MessageManagerLock mmLock;
   //     repaint();


    }
}
