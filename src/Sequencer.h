/**
 * Sequencer stores a set of sequences each of which stores a step
 * By Matthew Yee-King 2020
* Porbably should separate into header and cpp at some point
 */

#pragma once 

#include <vector>
#include <iostream>
#include <string>
#include <functional>


class Step{
  public:
    Step()
    {
      // for (auto i=0;i<16;i++)
      // {
      //   data.push_back(0.0);
      // }
      data.push_back(0.0);
    }
    std::vector<double> getData() const
    {
      return data; 
    }
    void setData(std::vector<double> data)
    {
      this->data = data; 
    }
    void setCallback(std::function<void(std::vector<double>)> callback)
    {
      this->stepCallback = callback;
    }
    void trigger() const
    {
      stepCallback(data);
    }
  private: 
    std::vector<double> data;
    std::function<void(std::vector<double>)> stepCallback;
};


class Sequence{
  public:
    Sequence(unsigned int seqLength = 16) : currentStep{0}, currentLength{seqLength}
    {
      for (auto i=0;i<seqLength;i++)
      {
        Step s;
        s.setCallback([i](std::vector<double> data){
          std::cout << "step " << i << " triggered " << std::endl;
        });
        steps.push_back(s);
      }
      
    }
    /** go to the next step */
    void tick()
    {
      currentStep = (++currentStep) % currentLength;
      steps[currentStep].trigger();
    }
    unsigned int getCurrentStep() const
    {
      return currentStep; 
    }

    bool assertStep(unsigned int step) const
    {
      if (step >= steps.size() || step < 0) return false;
      return true; 
    }

    std::vector<double> getStepData(int step) const
    {
      return steps[step].getData();
    }
    std::vector<double> getCurrentStepData() const
    {
      return steps[currentStep].getData();
    }

    void setLength(int length)
    {
      if (length > steps.size()) // bad need more steps
      {
        int toAdd = length - steps.size();

        for (int i=0; i < toAdd; ++i)
        {
          steps.push_back(Step{});
        }
      }
      currentLength = length;
    }

    void setStepData(unsigned int step, std::vector<double> data)
    {
      steps[step].setData(data);
    }
    void setStepCallback(unsigned int step, 
                      std::function<void (std::vector<double>)> callback)
    {
      steps[step].setCallback(callback);
    }
    std::string stepToString(int step) const
    {
      std::vector<double> data = getStepData(step);
      return std::to_string(data[0]);
    }

    unsigned int howManySteps() const 
    {
      return steps.size();
    }

  private:
    unsigned int currentLength;
    unsigned int currentStep;
    std::vector<Step> steps;

};

/** represents a sequencer which is used to store a grid of data and to step through it */
class Sequencer  {
    public:
      Sequencer(unsigned int seqCount = 4, unsigned int seqLength = 16) 
      {
        for (auto i=0;i<seqCount;++i)
        {
          sequences.push_back(Sequence{seqLength});
        }
      }

      unsigned int howManySequences() const 
      {
        return sequences.size();
      }
      unsigned int howManySteps(unsigned int sequence) const 
      {
        if (assertSequence(sequence)) return sequences[sequence].howManySteps();
        else return 0;
      }
      unsigned int getCurrentStep(unsigned int sequence) const
      {
        return sequences[sequence].getCurrentStep();
      }

      /** move the sequencer along by one tick */
      void tick()
      {
        //std::cout << "Sequencer.h: tick" << std::endl;
        for (Sequence& seq : sequences)
        {
            seq.tick();
        }
      }

      void setSequenceLength(unsigned int sequence, unsigned int length)
      {
        sequences[sequence].setLength(length);
      }
      /** set a callback for all steps in a sequence*/
      void setSequenceCallback(unsigned int sequence, std::function<void (std::vector<double>)> callback)
      {
        for (int step = 0; step<sequences[sequence].howManySteps(); ++step)
        {
          sequences[sequence].setStepCallback(step, callback);
        }
      }

      /** set a lambda to call when a particular step in a particular sequence happens */
      void setStepCallback(unsigned int sequence, unsigned int step, std::function<void (std::vector<double>)> callback)
      {
         sequences[sequence].setStepCallback(step, callback); 
      }

      /** update the data stored at a step in the sequencer */
      void setStepData(unsigned int sequence, unsigned int step, std::vector<double> data)
      {
        if (!assertSeqAndStep(sequence, step)) return;
        sequences[sequence].setStepData(step, data);
      }
      /** retrieve the data for the current step */
      std::vector<double> getCurrentStepData(int sequence) const
      {
        if (sequence >= sequences.size() || sequence < 0) return std::vector<double>{};
        return sequences[sequence].getCurrentStepData();
      }
      /** retrieve the data for a specific step */
      std::vector<double> getStepData(int sequence, int step) const
      {
        if (!assertSeqAndStep(sequence, step)) return std::vector<double>{};
        return sequences[sequence].getStepData(step);
      }
      
      void addStepListener();

  /** print out a tracker style view of the sequence */
    std::string toString()
    {
      std::string s{""};
      for (int step = 0; step < 32; ++step)
      {
        s += std::to_string(step) + "\t: ";
        for (Sequence& seq : sequences)
        {
          s += seq.stepToString(step) + "\t";
        }
        s += "\n";
      }
      return s;
    }

    private:

    bool assertSeqAndStep(unsigned int sequence, unsigned int step) const
      {
        if (!assertSequence(sequence)) return false;
        if (!sequences[sequence].assertStep(step)) return false; 
        return true;
      }

    bool assertSequence(unsigned int sequence) const
    {
      if (sequence >= sequences.size() || sequence < 0)
        {
          //std::cout << "Sequence " << sequence << " out of range. use 0 - " << (sequences.size()-1) << std::endl;
          return false;
        } 
      return true;
    }

    /// class data members  

    std::vector<Sequence> sequences;

};

/** Represents an editor for a sequencer, which allows stateful edit operations to be applied 
 * to sequences. For example, select sequemce, select step, enter data
 * Used to build editing interfaces for a sequencer 
*/

enum class SequencerEditorMode {selectingStep, editingStep};

class SequencerEditor {
  public:
    SequencerEditor(Sequencer* sequencer) : sequencer{sequencer}, currentSequence{0}, currentStep{0}, currentStepIndex{0}, editMode{SequencerEditorMode::selectingStep}, stepIncrement{0.5f}
    {

    }

    void setEditMode(SequencerEditorMode mode)
    {
      this->editMode = mode;
    }

  /** moves the editor cursor up. 
   * If in selectingStep mode, steps through the sequenbces, wrapping at the top
   * if in editingStep mode, edits the 
   * */

  void moveCursorUp()
  {
    if (editMode == SequencerEditorMode::selectingStep)
    {
      currentSequence -= 1;
      if (currentSequence < 0) currentSequence = 0;
    }
  }

  void moveCursorDown()
  {
    if (editMode == SequencerEditorMode::selectingStep)
    {
      currentSequence += 1;
      if (currentSequence >= sequencer->howManySequences()) currentSequence = sequencer->howManySequences() - 1;
    }
  }

  void moveCursorLeft()
  {
    if (editMode == SequencerEditorMode::selectingStep)
      {
        currentStep -= 1;
        if (currentStep < 0) currentStep = 0;
      }
  }

  void moveCursorRight()
  {
    if (editMode == SequencerEditorMode::selectingStep)
      {
        currentStep += 1;
        if (currentStep >= sequencer->howManySteps(currentSequence)) currentStep = sequencer->howManySteps(currentSequence) - 1;
      }
  }

  int getCurrentSequence() const 
  {
    return currentSequence;
  }
  /**  */
  int getCurrentStep() const 
  {
    return currentStep;
  }
  /** which data point in a step are we editing */
  int getCurrentStepIndex() const
  {
    return currentStepIndex;
  }
  /** move the cursor to a specific sequence*/
  void setCurrentSequence(int seq)
  {
    currentSequence = seq;
  }
  /** move the cursor to a specific step*/
  void setCurrentStep(int step)
  {
    currentStep = step;
  }
  /** write the sent data to the current step and sequence */
  void writeStepData(std::vector<double> data)
  {
    sequencer->setStepData(currentSequence, currentSequence, data);
  }
  /** write the sent data to the sequence at 'currentSequence' - 1D data version for simple one value per step -style sequences*/
  void writeSequenceData(std::vector<double> data)
  {
    std::vector<double> stepData = {0};
    for (int i=0; i<sequencer->howManySteps(currentSequence); ++i)
    {
      stepData[0] = data[i % data.size()]; // wrap it around :) 
      sequencer->setStepData(currentSequence, currentSequence, stepData);
    }
  }
  /** write the sent data to a sequence - 1D data version */
  void writeSequenceData(std::vector<std::vector<double>> data)
  {
    for (int i=0; i<sequencer->howManySteps(currentSequence); ++i)
    {
      sequencer->setStepData(currentSequence, currentSequence, data[i % data.size()]); // wrap around
    }
  }

  private:
    Sequencer* sequencer; 
    /** which sequence*/
    int currentSequence;
    /** which step */
    int currentStep;
    /** which data point inside a step*/
    int currentStepIndex;
    
    SequencerEditorMode editMode;
    double stepIncrement;    
};

/** Provides functions to 
 * 
*/
class SequencerViewer{
  public:
    SequencerViewer()
    {}
    /** generate a 'rows' line string representation of the state of the editor
     * and sequencer. Examples:
     * Starting state - I is where the 
     * 1-Iooooooo
     * 2-Oooooooo
     */
    static std::string toTextDisplay(const int rows, const int cols, const Sequencer* sequencer, const SequencerEditor* editor)
    {
      // the editor cursor dictates which bit we show
      std::string disp{""};
      // we display the bit of the sequences
      // that the editor is looking at
      int seqOffset = editor->getCurrentSequence();
      int stepOffset = 0;//editor->getCurrentStep();

      if (editor->getCurrentStep() > cols - 4)
      {
        stepOffset = editor->getCurrentStep();
      }
      int displaySeq, displayStep;
      for (int seq=0;seq<rows;++seq)
      {
        displaySeq = seq + seqOffset;
        // the first thing is the channel number
        disp += std::to_string(displaySeq + 1);
        // space pad it
        if (displaySeq < 10) disp += " ";
        // the second thing is a '-' if this is the start of the 
        // sequence or a ' ' if it is not, based on the 
        // position of the cursor
        if (editor->getCurrentStep() == 0) disp += "-";
        else disp += " ";
        for (int step=0;step<cols - 3;++step) // -3 as we we used 3 chars already
        {
          displayStep = step + stepOffset;
          
          // three choices, in order of priority as two can be true:
          // I : the editor is at this step
          // - : the sequencer is at this step 
          // o : neither the editor or sequencer are at step
          //   : gone past the end of the sequence
          char state{'o'};// default
          if (sequencer->getCurrentStep(displaySeq) == displayStep) state = '-';
          if (editor->getCurrentSequence() == displaySeq &&
              editor->getCurrentStep() == displayStep)  state = 'I';          
          if (sequencer->howManySteps(displaySeq) <= displayStep) state = ' ';
          disp += state;
        }
        if (seq < rows - 1)
          disp += "\n";
      } 
      
      return disp;
    }
}; 




