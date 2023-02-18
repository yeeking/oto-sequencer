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
    kbd{kbdState, juce::MidiKeyboardComponent::horizontalKeyboard}, 
    sequencerUI{sequencer}
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
    addAndMakeVisible(loadBtn);
    addAndMakeVisible(showUIBtn);
    addAndMakeVisible(kbd);
    addAndMakeVisible(sequencerUI);

    // go native if we are in standalone mode
    // uncomment if you want
    // if (juce::PluginHostType::getPluginLoadedAs() == juce::AudioProcessor::wrapperType_Standalone){
    //   juce::TopLevelWindow* win = juce::TopLevelWindow::getTopLevelWindow(0);
    //   win->setUsingNativeTitleBar(true);
    // }    
  //startTimer(100);
}

DrumMachineUI::~DrumMachineUI()
{
  DBG("DEST ~HosterPluginAudioProcessorEditor");
  //releasePluginGUI();
  //delete pluginWindow;
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

//  // DBG("HosterPluginAudioProcessorEditor::paint");
//     // (Our component is opaque, so we must completely fill the background with a solid colour)
//     g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
//     //juce::String msg {"" + std::to_string(juce::Time::currentTimeMillis())};
//     //if (audioProcessor.pluginInstance) msg = audioProcessor.pluginInstance->getName() + msg; 
//     //else msg = "no plugin " + msg; 
//     g.setColour (juce::Colours::white);
//     g.setFont (15.0f);
//     juce::String msg{"cmake vst host v0.4"};
//     g.drawFittedText (msg, getLocalBounds(), juce::Justification::centred, 1);
//  // DBG("HosterPluginAudioProcessorEditor::paint ends");
  
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

}
