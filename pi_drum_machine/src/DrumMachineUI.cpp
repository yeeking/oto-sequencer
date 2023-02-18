/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "DrumMachineAudio.h"
#include "DrumMachineUI.h"


//==============================================================================
DrumMachineUI::DrumMachineUI (DrumMachineAudio& p, Sequencer* sequencer)
    : AudioProcessorEditor {&p}, audioProcessor {p}, 
      offscreenImg{juce::Image::RGB, 320, 240, true}, 
     painterG{offscreenImg},
    kbd{kbdState, juce::MidiKeyboardComponent::horizontalKeyboard}, 
    sequencerUI{sequencer}, 
      Thread{"keyReaderThread"}

{
    DBG("CON HosterPluginAudioProcessorEditor");
    //https://docs.juce.com/master/classTopLevelWindow.html#details    

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    setSize (320, 240);
    //getWindowHandle()
    loadBtn.addListener(this);
    showUIBtn.addListener(this);
    kbdState.addListener(this);
    // in case we got recreated, get the plugin gui back
    releasePluginGUI();
    // addAndMakeVisible(loadBtn);
    // addAndMakeVisible(showUIBtn);
    // addAndMakeVisible(kbd);
    // don't need this as components draw to offscreen 
    // image 'on command' :)
    //addAndMakeVisible(sequencerUI);
    
    // redraw UI timer
    startTimer(100);
  // grab the keyboard
    setWantsKeyboardFocus(true);
    // this starts the raw key reader which
    // should override the 
    // key focus but only if its working
    startThread(); // enable this if using low level keyboard driver e.g. pi
}

DrumMachineUI::~DrumMachineUI()
{
  DBG("DEST ~HosterPluginAudioProcessorEditor");
  //releasePluginGUI();
  //delete pluginWindow;
  stopThread(100);
}


void DrumMachineUI::buttonClicked(juce::Button* btn)
{
  if (btn == &loadBtn)
  {
      DBG("HosterPluginAudioProcessorEditor::buttonClicked trigger load");
      // plugins are dirs on linux and files on the others
      # ifdef JUCE_LINUX
      auto fileChooserFlags = juce::FileBrowserComponent::openMode | 
                              juce::FileBrowserComponent::canSelectDirectories;
                              
      # endif
      # ifdef JUCE_MAC
      auto fileChooserFlags = juce::FileBrowserComponent::openMode | 
                              juce::FileBrowserComponent::canSelectFiles;           
      # endif
      # ifdef JUCE_WINDOWS
      auto fileChooserFlags = juce::FileBrowserComponent::openMode | 
                              juce::FileBrowserComponent::canSelectFiles;           
      # endif
      
      //FileBrowserComponent::canSelectFiles;
      
      fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser){
        DBG("HosterPluginAudioProcessorEditor::buttonClicked " << chooser.getResult().getFileName() );
        DBG("HosterPluginAudioProcessorEditor::buttonClicked releaseGUI");
        this->releasePluginGUI();
        DBG("HosterPluginAudioProcessorEditor::buttonClicked loadPlugin");
        this->audioProcessor.loadPlugin(chooser.getResult());
        // DBG("HosterPluginAudioProcessorEditor::buttonClicked plugin load complete. prep gui ");
        // this->preparePluginGUI();    
      });
  }
  if (btn == &showUIBtn){
    showPluginGUI();
  }
}

void DrumMachineUI::showPluginGUI()
{
  // do we have a plugin instance? 
  // no? return
  if (!audioProcessor.pluginNode) {
    DBG("HosterPluginAudioProcessorEditor::showPluginGUI no plugin");
    return; 
  }
  // are we already showing it? 
  // yes? just return
  if (pluginWindow){
    DBG("HosterPluginAudioProcessorEditor::showPluginGUI window already open");
    return;
  } 
  
  // ok so we need a plugin window
  DBG("HosterPluginAudioProcessorEditor::showPluginGUI creating new gui");
  pluginWindow = std::make_unique<PluginWindow>(audioProcessor.pluginNode->getProcessor());
  //pluginWindow = new PluginWindow(audioProcessor.pluginNode->getProcessor());
  pluginWindow->addPluginWindowListener(this);

}

void DrumMachineUI::preparePluginGUI()
{


    // if (audioProcessor.pluginInstance) {// we have loaded a plugin 
    //     if (audioProcessor.pluginInstance->hasEditor()){
    //       // just so I can easuly call
    //       DBG("HosterPluginAudioProcessorEditor::preparePluginGUI: createEditorIfNeeded");
    //       // cannot rely on an editor being ready 
    //       audioProcessor.pluginInstance->createEditorIfNeeded();
    //       // insert it behind the others. 
    //       DBG("HosterPluginAudioProcessorEditor::preparePluginGUI: getActiveEditor");
    //       audioProcessor.pluginInstance->getActiveEditor()->setBounds(0, getHeight()/4, getWidth(), getHeight() * 0.75); 
    //       addAndMakeVisibl e(audioProcessor.pluginInstance->getActiveEditor(), 0);
    //       DBG("HosterPluginAudioProcessorEditor::preparePluginGUI gui prepped. repaint");
    //       repaint();// once loaded
    //       DBG("HosterPluginAudioProcessorEditor::preparePluginGUI repaint complete");
    //   }
    // }
    // DBG("HosterPluginAudioProcessorEditor::preparePluginGUI: end");

}


void DrumMachineUI::releasePluginGUI()
{
  if (pluginWindow) {
    pluginWindow.reset(); // trigger delete of unique_ptr-owned object
  }
}

void DrumMachineUI::pluginCloseButtonClicked()
{
  releasePluginGUI();
}

void DrumMachineUI::handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity)
{
    juce::MidiMessage msg1 = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    audioProcessor.addMidi(msg1);
    
}

void DrumMachineUI::handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity)
{
    juce::MidiMessage msg2 = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber, velocity);
    audioProcessor.addMidi(msg2); 
}


//==============================================================================
void DrumMachineUI::paint (juce::Graphics& g)
{
//    if (!getPeer()->isFullScreen()){
//      getPeer()->setFullScreen(true); 
//    }
  // paint it
  g.drawImageAt(offscreenImg, 0, 0);

}

void DrumMachineUI::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // loadBtn.setBounds(0, 0, getWidth()/4, getHeight()/4);
    // showUIBtn.setBounds(getWidth()/4, 0, getWidth()/4, getHeight()/4);
    // kbd.setBounds(0, getHeight() / 4 * 3, getWidth(), getHeight() / 4);
    sequencerUI.setBounds(0, 0, getWidth(), getHeight());
}

void DrumMachineUI::timerCallback()
{
    sequencerUI.drawSequencer(painterG, offscreenImg);
    if ( ! frameBuffer.ready()){// no framebuffer - normal painting mode
        repaint();
    }
}


bool DrumMachineUI::keyPressed(const juce::KeyPress &key)
{
    std::cout << "DrumMachineUI::keyPressed: " << key.getKeyCode() << std::endl;
    sequencerUI.keyPressed(key);
    return true; 
}


void DrumMachineUI::run()
{   
    while(true){    
        char c1 = keyReader.getChar();
        //setWantsKeyboardFocus(false);
        if (this->hasKeyboardFocus(true)){
          // we will manually trigger events
          this->giveAwayKeyboardFocus();
        }
        char c = KeyReader::getCharNameMap()[c1];
        DBG("DrumMachineUI::raw key '%c' to '%c'"<< (int)c1 <<  (int)c);
        // now trigger key pressed
        juce::KeyPress key{c};
        sequencerUI.rawKeyPressed(c);
        //this->keyPressed(key);
    }// end while read key
}
