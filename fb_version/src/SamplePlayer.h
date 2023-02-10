
//#include <JuceHeader.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>

#pragma once 
/**
 * SamplePlayer: Sample playing AudioProcessor
 * @brief 
 * 
 */
class SamplePlayer : public juce::AudioProcessor {
    public:
        SamplePlayer() {}
        /**
         * @brief Construct a new Sample Player object
         * 
         * @param audioFile : load this file into the buffer
         * @param note : trigger playback when receive this note
         */
        SamplePlayer(juce::String audioFile, int note);
        void setMidiNote(int note);
        void trigger();

        /** directly access the next sample (as opposed to rendering it to a renderbuffer as renderNextSample does)*/
        float getNextSample(int channel);
        

        // AudioProcesser stuff
        //==============================================================================
        void prepareToPlay (double sampleRate, int samplesPerBlock) override;
        void releaseResources() override;

        #ifndef JucePlugin_PreferredChannelConfigurations
        bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
        #endif

        // this 'using' removes the 'hidden overloads' issue 
        // which you might see 
        // if you have all compiler warnings enabled
        // https://stackoverflow.com/questions/6727087/c--function-being-hidden#6727196
        using juce::AudioProcessor::processBlock;
        void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

         const juce::String getName() const override{return "sampler";}
         double getTailLengthSeconds() const override{return 0.1;}
         bool acceptsMidi() const override{return true; }
         bool producesMidi() const override{return false;}
         juce::AudioProcessorEditor* createEditor() override{return nullptr;}
         bool hasEditor() const override{return false; }
         int getNumPrograms() override{return 1;}
         int getCurrentProgram() override{return 0;}
         void setCurrentProgram (int index) override{}
         const juce::String getProgramName (int index) override{return "sampler";}
         void changeProgramName (int index, const juce::String& newName) override{}
         void getStateInformation (juce::MemoryBlock& destData) override{}
         void setStateInformation (const void* data, int sizeInBytes) override{}

         // end of AudioProcesser stuff



    private:
    /** current position in the complete sample data buffer*/
        int playbackPosition;
        /** current posn in the render buffer. resets each time 
         * processBlock is called
        */
        int renderBufferPosn;
        bool playing; 
        juce::AudioFormatManager audioFormatManager; 
        void loadFile(juce::String audioFile);
        juce::AudioBuffer<float> audioData;
        int triggerNote; 
        //juce::AudioBuffer<float> buffer; 

};