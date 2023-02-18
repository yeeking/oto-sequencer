/*
  ==============================================================================

    PluginWindow.cpp
    Created: 25 Sep 2022 3:30:54pm
    Author:  matthewyk

  ==============================================================================
*/

#include "PluginWindow.h"



PluginWindow::PluginWindow (juce::AudioProcessor* _audioProcessor)
//, OwnedArray<PluginWindow>& windowList)
    : DocumentWindow (_audioProcessor->getName(),
                      juce::LookAndFeel::getDefaultLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId),
                      juce::DocumentWindow::minimiseButton | juce::DocumentWindow::closeButton),
    //  activeWindowList (windowList),
      audioProcessor (_audioProcessor)
{
    setSize (400, 300);

    if (auto* ui = createProcessorEditor (audioProcessor))
    {
        setContentOwned (ui, true);
        setResizable (ui->isResizable(), false);
    }

    // TODO - do something clever here for window placement
    setTopLeftPosition(100, 100);
    // setTopLeftPosition (node->properties.getWithDefault (getLastXProp (type), Random::getSystemRandom().nextInt (500)),
    //                     node->properties.getWithDefault (getLastYProp (type), Random::getSystemRandom().nextInt (500)));
 
    setVisible (true);
}

PluginWindow::~PluginWindow() 
{
    clearContentComponent();
}

void PluginWindow::closeButtonPressed()
{
  // brutal but it should work
  //delete this;
  listener->pluginCloseButtonClicked();
}

void PluginWindow::addPluginWindowListener(PluginWindowListener* _listener)
{
  this->listener = _listener; 
}

juce::BorderSize<int> PluginWindow::getBorderThickness() 
{ 
  return DocumentWindow::getBorderThickness();
}

juce::AudioProcessorEditor* PluginWindow::createProcessorEditor (juce::AudioProcessor* _processor)
{
    if (_processor->hasEditor())
        if (auto* ui = _processor->createEditorIfNeeded())
            return ui;

    jassertfalse;
    return {};
}



 
