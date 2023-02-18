        #include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "Sequencer.h"
#include "SequencerUtils.h"
#include "FrameBuffer.h"
#include "IOUtils.h"

#pragma once
class SequencerUI : public juce::Component {
    public:
        SequencerUI(Sequencer* sequencer);
        ~SequencerUI();
        void paint(juce::Graphics &) override;
        void resized() override;
        bool keyPressed(const juce::KeyPress &key) override;
        /** receives a key from a KeyReader raw device*/
        void rawKeyPressed(char c);
  /** draws sequencer to offscreen image. */
        void drawSequencer(juce::Graphics& painterG, juce::Image& offscreenImg);
private:
        Sequencer* sequencer; 
        SequencerEditor editor; 
        FrameBuffer frameBuffer; 
        // juce::Image offscreenImg;
        // juce::Graphics painterG;  
   
        void drawSequence(juce::Graphics& g, Sequence* seq, bool editorIsCurrent, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
        void drawStep(juce::Graphics& g, Step* step, bool isCurrent, bool editorIsCurrent, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
};
