/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include "DrumMachineAudio.h"
#include "PluginWindow.h"
#include "Sequencer.h"
#include "SequencerUI.h"
//==============================================================================
/**
 */

class SequencerUIListener {
  public:
  SequencerUIListener(){}
  ~SequencerUIListener(){}
  /** override this with functo*/
  virtual void changeBPM() = 0;

};

class DrumMachineUI : public juce::AudioProcessorEditor,
                                         public juce::Button::Listener,
                                         public PluginWindowListener,
                                         private juce::MidiKeyboardState::Listener, 
                                         public juce::Timer
// public juce::MidiKeyboardComponent::Listener
{
public:
  DrumMachineUI(DrumMachineAudio &, Sequencer* sequencer);
  ~DrumMachineUI() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

  void buttonClicked(juce::Button *) override;

  void handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;
  void handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;

  void timerCallback() override;


private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  DrumMachineAudio &audioProcessor;
  juce::TextButton loadBtn{"Load Plugin"};
  juce::TextButton showUIBtn{"Show Plugin UI"};
  juce::MidiKeyboardState kbdState;
  juce::MidiKeyboardComponent kbd;
  SequencerUI sequencerUI;
  

  juce::FileChooser fChooser{"Select a file..."};
  /** tells the plugin to instantiate its gui then adds it and makes visible*/
  void preparePluginGUI();
  /** call this when you are going to load a new plugin instance
   * to clean up the old one's resources
   */
  void releasePluginGUI();
  /** call this when you want to see the plugin gui*/
  void showPluginGUI();
  /** stores all opened plugin windows*/
  // OwnedArray<PluginWindow> activePluginWindows;
  /** unique_ptr to store the plugin's UI wrapper window*/
  std::unique_ptr<PluginWindow> pluginWindow;

  /** PluginWindowListener interface */
  void pluginCloseButtonClicked() override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumMachineUI)
};
