/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

//#include <JuceHeader.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "Sequencer.h"
#include "SamplePlayer.h"
//using namespace juce;

//==============================================================================
/**
*/
class DrumMachineAudio  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    DrumMachineAudio();
    ~DrumMachineAudio() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    // this 'using' removes the 'hidden overloads' issue 
    // which you might see 
    // if you have all compiler warnings enabled
    // https://stackoverflow.com/questions/6727087/c-virtual-function-being-hidden#6727196
    using juce::AudioProcessor::processBlock;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    
    /** plugin gets converted into an AudioProcessorGraph node and this variable stores a ref to that node*/
    juce::AudioProcessorGraph::Node::Ptr  pluginNode;
  

    /** load the sent plugin and assign it to this->pluginInstance*/
    void loadPlugin(const juce::File& pluginFile);

    /** add some midi to be played at the sent sample offset*/
    void addMidi(juce::MidiMessage msg);

private:
    // /** add a sampler to the graph. results in the creation of a SamplePlayer
    //  * then its conversion to
    // */
    // void addSampler(juce::String audioFile);    
    /// @brief stores a ref to the plugin object. unique ptr so no delete needed
    std::unique_ptr<juce::AudioPluginInstance>  pluginInstance;

 /** the audio processor graph that wires the plugins together */
    std::unique_ptr<juce::AudioProcessorGraph> audioProcGraph;
   
    juce::AudioProcessorGraph::Node::Ptr  inputNode;
    juce::AudioProcessorGraph::Node::Ptr  outputNode;
    juce::AudioProcessorGraph::Node::Ptr  midiInNode;
    juce::AudioProcessorGraph::Node::Ptr  midiOutNode;
    std::vector<juce::AudioProcessorGraph::Node::Ptr> samplerNodes;
    // juce::AudioProcessorGraph::Node::Ptr  sampler1Node;
    // juce::AudioProcessorGraph::Node::Ptr  sampler2Node;
    void setupInitialGraph();
    /** creates a sample player and patches it to the 
     * audiograph
    */
    void createAndPatchSamplePlayer(juce::String audioFilename);

    /** stores messages added from the addMidi function*/
    juce::MidiBuffer midiToProcess;
  
    Sequencer sequencer; 
    int tickInterval;
    double bpm;
    int samplesSinceLastTick;
    int currentBufferPosn; 

//    SamplePlayer player1;

    void setupSequencerCallbacks();
    /** */
    static int getTickInterval(double bpm, double sampleRate, double ticksPerBeat = 8)
    {
      return (int) (60.0 / (bpm * ticksPerBeat) * sampleRate);
    } 

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrumMachineAudio)
};
