//==============================================================================
/**
    A desktop window containing a plugin's GUI.
*/

#pragma once

//#include <JuceHeader.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>


class PluginWindowListener{
  public:
    PluginWindowListener(){};
   ~PluginWindowListener(){};
    virtual void pluginCloseButtonClicked() = 0;
};


class PluginWindow  : public juce::DocumentWindow
{
public:
    PluginWindow (juce::AudioProcessor* audioProcessor);
    ~PluginWindow() override;
    //OwnedArray<PluginWindow>& activeWindowList;
   // const AudioProcessorGraph::Node::Ptr node;
    juce::AudioProcessor* audioProcessor; 
    juce::BorderSize<int> getBorderThickness() override;
    /** tell someone you closed! */
    void closeButtonPressed() override;
    /** I will send close button and other major events to this listener */
    void addPluginWindowListener(PluginWindowListener*  listener);
private:
    float getDesktopScaleFactor() const override
         { return 1.0f; }

    PluginWindowListener*  listener;    
    

    static juce::AudioProcessorEditor* createProcessorEditor (juce::AudioProcessor* processor);
   

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow)
};
 
