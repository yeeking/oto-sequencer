#include "SamplePlayer.h"

SamplePlayer::SamplePlayer(juce::String audioFile, int _note)  
: 
// this jazz lets this AudioProcessor tell audiograph how to deal with node creation 
// in terms of I/O. If you don't do this you get a default channel config on the node
// with no channels
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
playbackPosition{0}, 
  triggerNote{_note}, 
  playing{false}
{
    audioFormatManager.registerBasicFormats();
    loadFile(audioFile);
}

void SamplePlayer::trigger()
{
    playbackPosition = 0;
    playing = true; 
}



void SamplePlayer::loadFile(juce::String audioFile)
{
    DBG("SamplePlayer::loadFile loading " << audioFile);
    juce::File file{audioFile};
    if (file == juce::File{})
        return;

    std::unique_ptr<juce::AudioFormatReader> reader (audioFormatManager.createReaderFor (file)); 
    if (reader.get() != nullptr){
        auto duration = (float) reader->lengthInSamples / reader->sampleRate;
        if (duration < 60)// max 60 seconds
        {
            audioData.setSize ((int) reader->numChannels, (int) reader->lengthInSamples); 
            reader->read (&audioData,                                                     
                            0,                                                           
                            (int) reader->lengthInSamples,                                
                            0,                                                            
                            true,                                                          
                            true);                                                            
            playbackPosition = 0;
            DBG("SamplePlayer::loadFile loaded " << audioFile << "samples " << audioData.getNumSamples() << " chans " << audioData.getNumChannels());
            //setAudioChannels (0, (int) reader->numChannels);                               
        }
        else
        {
            DBG("Not loading file as too long");
            // handle the error that the file is 2 seconds or longer..
        }
    }
}



//==============================================================================
void SamplePlayer::prepareToPlay (double sampleRate, int samplesPerBlock)
{

}
void SamplePlayer::releaseResources()
{

}

void SamplePlayer::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiBuff)
{

    for (auto ch = 0; ch < 1; ++ch){
        auto* writeBuffer = buffer.getWritePointer (ch);
        for (auto sInd = 0; sInd < buffer.getNumSamples(); ++sInd){
            // get events matching this sample offset
            if (midiBuff.getNumEvents() >0){
                for (const auto meta : midiBuff){
                    //if (meta.samplePosition > sInd) break;
                    if (meta.samplePosition != sInd) continue;
                    if (meta.samplePosition == sInd) {
                        auto msg = meta.getMessage();
                        if (msg.isNoteOn() && msg.getNoteNumber() == this->triggerNote){
                            //DBG("SamplePlayer::processBlock got midi on note " << msg.getNoteNumber() << " trig note is " << this->triggerNote);
                            this->trigger();
                            midiBuff.clear(0, meta.samplePosition);
                        }
                    }
                }
            }
            float sample = getNextSample(ch);
            writeBuffer[sInd] = sample;
        }
    }
    
}

float SamplePlayer::getNextSample(int channel)
{   

    //std::cout << playbackPosition << std::endl;
    if (playbackPosition >= audioData.getNumSamples()) {
        //DBG("SamplePlayer::getNextSample END " << playbackPosition);
        playbackPosition = 0;
        playing = false; 
    }
    if (playing){
        auto* bufRd = audioData.getReadPointer(channel,playbackPosition);
        ++playbackPosition;
        return bufRd[0];
    }
    else {
        return 0.0f;
    }

}


#ifndef JucePlugin_PreferredChannelConfigurations
bool SamplePlayer::isBusesLayoutSupported (const BusesLayout& layouts) const
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