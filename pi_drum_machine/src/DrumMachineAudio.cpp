/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "DrumMachineAudio.h"
#include "DrumMachineUI.h"

//==============================================================================
DrumMachineAudio::DrumMachineAudio()
:
#ifndef JucePlugin_PreferredChannelConfigurations
  AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif 
// initialiser list 
   audioProcGraph{new juce::AudioProcessorGraph()}, 
   sequencer{3,16}, 
   // this gets set properly once
   // the audio fires up
   tickInterval{44100}, 
   bpm{120}, 
   samplesSinceLastTick{0}, 
   currentBufferPosn{0} 
// constructor body 
{
    DBG("CONS HosterPluginAudioProcessor");
   
   // configure the basic audio graph
  setupInitialGraph();
  // patch in some sample players to the graph
  //juce::String sampleDir = "/home/pi/src/JUCE/drum-machine/src/audio/";
  juce::String sampleDir = "/home/matthewyk/src/ai-enhanced-audio/versioned-docs/ai-audio-book-code/src/TestTools/drum-machine/src/audio/";

  std::vector<juce::String> files = {sampleDir + "kick.wav", sampleDir + "snare.wav", sampleDir + "hat.wav"};
  for (const juce::String& f : files){
    createAndPatchSamplePlayer(f);
  }
  // configure the sequencer 
  setupSequencerCallbacks();

}

DrumMachineAudio::~DrumMachineAudio()
{
    DBG("DEST ~HosterPluginAudioProcessor");
}

//==============================================================================
const juce::String DrumMachineAudio::getName() const
{
    return JucePlugin_Name;
}

bool DrumMachineAudio::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DrumMachineAudio::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DrumMachineAudio::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DrumMachineAudio::getTailLengthSeconds() const
{
    return 0.0;
}

int DrumMachineAudio::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DrumMachineAudio::getCurrentProgram()
{
    return 0;
}

void DrumMachineAudio::setCurrentProgram (int index)
{
  DBG("DrumMachineAudio::setCurrentProgram set to " << index);
}

const juce::String DrumMachineAudio::getProgramName (int index)
{
    DBG("DrumMachineAudio::getProgramName at " << index);

    return {""};
}

void DrumMachineAudio::changeProgramName (int index, const juce::String& newName)
{
  DBG("DrumMachineAudio::changeProgramName" << index << newName);
}

//==============================================================================
void DrumMachineAudio::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    DBG("HosterPluginAudioProcessor::prepareToPlay");
    tickInterval = DrumMachineAudio::getTickInterval(bpm, sampleRate);
    audioProcGraph->prepareToPlay(sampleRate, samplesPerBlock);
}

void DrumMachineAudio::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    audioProcGraph->releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DrumMachineAudio::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DrumMachineAudio::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{   
    //DBG("DrumMachineAudio::processBlock block size " << buffer.getNumSamples());
    // transfer any pending notes into the midi messages and 
    // clear pending
    if (midiToProcess.getNumEvents() > 0){
      midiMessages.addEvents(midiToProcess, midiToProcess.getFirstEventTime(), midiToProcess.getLastEventTime()+1, 0);
      midiToProcess.clear();
    }
   
    // this line tells it to ignore very 
    // low float values I think
    juce::ScopedNoDenormals noDenormals;
    // auto totalNumInputChannels  = getTotalNumInputChannels();
    // auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto ch = 0; ch < 1; ++ch){
      currentBufferPosn = 0;
      auto* writeBuffer = buffer.getWritePointer (ch);
      for (auto sInd = 0; sInd < buffer.getNumSamples(); ++sInd){
        // do the sequencer stuff 
        if (ch == 0){

          if (samplesSinceLastTick == tickInterval){
            sequencer.tick();
            samplesSinceLastTick = 0;
          }
          ++samplesSinceLastTick;
          ++currentBufferPosn;
          
        }// end sequencer management stuff
      }// end loop through samples in a channel
    }// end loop through channels


   // now apply plugin post-processing to 
   // the audio 
   //if (pluginNode) { // graph does the work 
  //samplerNode->getProcessor()->processBlock(buffer, midiMessages);
    audioProcGraph->processBlock(buffer, midiMessages);
   //}
}


//==============================================================================
bool DrumMachineAudio::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DrumMachineAudio::createEditor()
{
    return new DrumMachineUI (*this, &this->sequencer);
}

//==============================================================================
void DrumMachineAudio::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DrumMachineAudio::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    DBG("DrumMachineAudio::setStateInformation " << ":"<< sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DrumMachineAudio();
}


void DrumMachineAudio::loadPlugin(const juce::File& pluginFile)
{
  DBG("loadPlugin: suspend");
  suspendProcessing(true);

  // kill the old one
  if (pluginNode) {
    audioProcGraph->removeNode(pluginNode->nodeID);
  }

  // Add VST3 Plugins
  juce::OwnedArray<juce::PluginDescription> pluginDescriptions;
  juce::KnownPluginList plist;
  juce::AudioPluginFormatManager pluginFormatManager;
  pluginFormatManager.addDefaultFormats();
  // find the index of the vst format to ensure we use that to scan 
  // the vst plugin. really we should select the right format 
  // based on the file extension
  int currInd{0}, vstInd{0};
  for (const juce::AudioPluginFormat* f : pluginFormatManager.getFormats()){
    if (f->getName() == "VST3"){
      vstInd = currInd;
      break;
    }
    currInd++; 
  }
  bool added = plist.scanAndAddFile(pluginFile.getFullPathName(), true, pluginDescriptions, *pluginFormatManager.getFormat(vstInd));
  
  if (added) DBG("HosterPluginAudioProcessor::loadPlugin scanned and added " << pluginFile.getFileName() << " using format " << pluginFormatManager.getFormat(vstInd)->getName());
  else {
    DBG("HosterPluginAudioProcessor::loadPlugin failed to scan and add " << pluginFile.getFileName() << " using format " << pluginFormatManager.getFormat(vstInd)->getName());
    return; // no need to do anything else 
  }

  juce::String msg ("Error Loading Plugin: ");
 
  // TODO: verify what happens when I assign
  // a new thing to a previously assigned unique ptr-  is it 
  // automatically deleted? 
  pluginInstance = pluginFormatManager.createPluginInstance(*pluginDescriptions[0], getSampleRate(), 512, msg);
  if (pluginInstance){
    // do not do the gui stuff now... do it later
    // pluginInstance->createEditorIfNeeded();
    // DBG("HosterPluginAudioProcessor::loadPlugin created plugin instance " << pluginInstance->getName());
    // if (pluginInstance->getActiveEditor()) {DBG("HosterPluginAudioProcessorEditor::loadPlugin plugin has editor");}
    // else {DBG("HosterPluginAudioProcessor::loadPlugin plugin has no editor");}
    // // need this otherwise no sound
    pluginInstance->enableAllBuses();
    // need this otherwise processblock never actually gets called on the plug
    //pluginInstance->prepareToPlay(getBlockSize(), getSampleRate());
    // convert the plug to a node 
    pluginNode = audioProcGraph->addNode (std::move (pluginInstance) );
    
    // connect the node to the output??
    audioProcGraph->addConnection({
      {pluginNode->nodeID, 0}, 
      {outputNode->nodeID, 0}});
    // this will crash if its mono ... check if you want! 
    audioProcGraph->addConnection({
      {pluginNode->nodeID, 1}, 
      {outputNode->nodeID, 1}});
    // hook up midi in
    audioProcGraph->addConnection({
      {midiInNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex}, 
      {pluginNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex}});

    //audioProcGraph->prepareToPlay(getSampleRate(), getBlockSize());
    pluginNode->getProcessor()->prepareToPlay(getSampleRate(), getBlockSize());
  }
  else {
    DBG("HosterPluginAudioProcessor::loadPlugin could not create plugin instance");
  }
  DBG("loadPlugin: unsuspend");

  suspendProcessing(false);
}

void DrumMachineAudio::addMidi(juce::MidiMessage msg)
{
  //DBG("DrumMachineAudio::addMidi( adding midi at " << currentBufferPosn);
  int sampleOffset = currentBufferPosn;
  midiToProcess.addEvent(msg, sampleOffset);
}

void DrumMachineAudio::setupSequencerCallbacks()
{
  sequencer.setStepData(0, 0, {1, 1, 1, 1});
  for (int i=0; i<sequencer.howManySequences(); ++i){
    sequencer.setSequenceCallback(i, [this, i](std::vector<double>* data){
        juce::MidiMessage msg1 = juce::MidiMessage::noteOn(1, i, 1.0f);
        this->addMidi(msg1);
        //this->player1.trigger();
    });
  }
} 


void DrumMachineAudio::setupInitialGraph()
{
    // make the i/o processors
    std::unique_ptr<juce::AudioPluginInstance> inputProc = std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode);
    std::unique_ptr<juce::AudioPluginInstance> outputProc = std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);
    std::unique_ptr<juce::AudioPluginInstance> midiInputProc = std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(juce::AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode);
    std::unique_ptr<juce::AudioPluginInstance>  midiOutputProc = std::make_unique<juce::AudioProcessorGraph::AudioGraphIOProcessor>(juce::AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode);

    audioProcGraph->enableAllBuses();
    inputProc->enableAllBuses();
    outputProc->enableAllBuses();
   
    // convert the io processors into nodes
    inputNode = audioProcGraph->addNode(std::move(inputProc));
    outputNode = audioProcGraph->addNode(std::move(outputProc));
    midiInNode = audioProcGraph->addNode(std::move(midiInputProc));
    midiOutNode = audioProcGraph->addNode(std::move(midiOutputProc));
}

void DrumMachineAudio::createAndPatchSamplePlayer(juce::String audiFilename)
{
  // select midi note for this sampler 
  int midiNote = this->samplerNodes.size();
  std::unique_ptr<juce::AudioProcessor> player = std::make_unique<SamplePlayer>(audiFilename, midiNote);
  juce::AudioProcessorGraph::Node::Ptr samplerNode = audioProcGraph->addNode(std::move(player)); 
  // connect the node to the output??
  audioProcGraph->addConnection({
    {samplerNode->nodeID, 0}, 
    {outputNode->nodeID, 0}});
  // this will crash if its mono ... check if you want! 
  audioProcGraph->addConnection({
    {samplerNode->nodeID, 1}, 
    {outputNode->nodeID, 1}});
  // hook up midi in
  audioProcGraph->addConnection({
    {midiInNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex}, 
    {samplerNode->nodeID, juce::AudioProcessorGraph::midiChannelIndex}});
  // store it for later 
  samplerNodes.push_back(samplerNode);
}
