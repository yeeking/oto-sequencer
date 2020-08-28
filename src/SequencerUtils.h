#include "Sequencer.h"

/**
 * Top level modes that dictate the main UI output 
 */
enum class SequencerEditorMode {settingSeqLength, configuringSequence, selectingSeqAndStep, editingStep};

/**
 * Sub modes for moving between items in a sub menu (e.g. configuring midi channel)
 */
enum class SequencerEditorSubMode {settingSeqMidiChannel, settingSeqSampleId};

/** Represents an editor for a sequencer, which allows stateful edit operations to be applied 
 * to sequences. For example, select sequemce, select step, enter data
 * Used to build editing interfaces for a sequencer 
*/
class SequencerEditor {
  public:
    SequencerEditor(Sequencer* sequencer) : sequencer{sequencer}, currentSequence{0}, currentStep{0}, currentStepIndex{0}, editMode{SequencerEditorMode::selectingSeqAndStep}, stepIncrement{0.5f}
    {

    }
    SequencerEditorMode getEditMode() const
    {
      return this->editMode;
    }
    void setEditMode(SequencerEditorMode mode)
    {
      this->editMode = mode;
    }
    /** cycle through the edit modes in the sequence:
     * settingSeqLength (start mode)
     * selectingSeqAndStep
     * editingStep
    */
    void cycleEditMode()
    {
      switch(editMode)
      {
        case SequencerEditorMode::settingSeqLength:
          editMode = SequencerEditorMode::selectingSeqAndStep;
          return;
        case SequencerEditorMode::selectingSeqAndStep:
          editMode = SequencerEditorMode::settingSeqLength;
          currentStep = 0;
          return;
        case SequencerEditorMode::editingStep:
          editMode = SequencerEditorMode::selectingSeqAndStep;
          return;  
        case SequencerEditorMode::configuringSequence: 
          editMode = SequencerEditorMode::settingSeqLength;
          currentStep = 0; 
          return;
      }
    }
    /** 
     * depending on the mode, whoops bad coupling again! 
     * cycles the condition of the thing under the cursor
     * 
     */
    void cycleAtCursor()
    {
    switch(editMode)
      {
        case SequencerEditorMode::settingSeqLength:
          // change the type of sequence somehow??
          return;
        case SequencerEditorMode::selectingSeqAndStep:
          // toggle the step on or off
          sequencer->toggleActive(currentSequence, currentStep);
          return;
        case SequencerEditorMode::editingStep:
          //std::vector<double> data = {0, 0, 0};
          //writeStepData(data);
          sequencer->toggleActive(currentSequence, currentStep);
          return;  
      }
    }
    
    /**
     *  Go into edit mode for either the sequence or step
     */
  void enterAtCursor()
  {
    switch(editMode)
    {
      case SequencerEditorMode::settingSeqLength:
      // go into configuring sequence mode
        editMode = SequencerEditorMode::configuringSequence;
        return;
      case SequencerEditorMode::selectingSeqAndStep:
        // go into edit step mode
        editMode = SequencerEditorMode::editingStep;
        return;
      case SequencerEditorMode::editingStep:
      // go back out to the sequence view
        editMode = SequencerEditorMode::selectingSeqAndStep;
        return;  
    }
  }

/**
 * Tell the editor the user entered note data
 */
  void enterNoteData(double note)
  {
      if (editMode == SequencerEditorMode::editingStep ||
          editMode == SequencerEditorMode::selectingSeqAndStep)
      {     
        std::vector<double> data = sequencer->getStepData(currentSequence, currentStep);
        // set a default vel and len if needed.
        if (data[Step::velInd] == 0) data[Step::velInd] = 64;
        if (data[Step::lengthInd] == 0) data[Step::lengthInd] = 1; // two ticks
        data[Step::note1Ind] = note;
        writeStepData(data);
        return;
      }
      if (editMode == SequencerEditorMode::configuringSequence)
      {
        // set channel on all notes for this sequence
        int channelI = (unsigned int) note;
        channelI = channelI % 16; // 16 channels
        for (int step=0; step < sequencer->howManySteps(currentSequence); ++step)
        {
          sequencer->updateStepData(currentSequence, step, Step::channelInd, channelI);
        }
      }
  }

  /** moves the editor cursor up. 
   * If in selectingSeqAndStep mode, steps through the sequenbces, wrapping at the top
   * if in editingStep mode, edits the 
   */

  void moveCursorUp()
  {
      switch(editMode)
      {
        case SequencerEditorMode::settingSeqLength:
          currentSequence -= 1;
          if (currentSequence < 0) currentSequence = 0;
          if (currentStep >= sequencer->howManySteps(currentSequence)) currentStep = sequencer->howManySteps(currentSequence) - 1;
          break;
        case SequencerEditorMode::selectingSeqAndStep:
          currentSequence -= 1;
          if (currentSequence < 0) currentSequence = 0;
          if (currentStep >= sequencer->howManySteps(currentSequence)) currentStep = sequencer->howManySteps(currentSequence) - 1;
          break;
    
        case SequencerEditorMode::editingStep:
        {
          std::vector<double> data = sequencer->getStepData(currentSequence, currentStep);
          //if (data[Step::note1Ind] > 0) // note is set
          //{
            SequencerEditor::incrementStepData(data, sequencer->getSequenceType(currentSequence));
            writeStepData(data);
          //}
          break;  
        }
  
        case SequencerEditorMode::configuringSequence:
        {
          // set the channel based on step 0
          std::vector<double> data2 = sequencer->getStepData(currentSequence, 0);
          int channel = data2[Step::channelInd];
          channel = (channel + 1) % 16;
          for (int step=0; step < sequencer->howManySteps(currentSequence); ++step)
          {
            sequencer->updateStepData(currentSequence, step, Step::channelInd, channel);
          }
          break;
        }
      }
  }

  void moveCursorDown()
  {
      switch(editMode)
      {
        case SequencerEditorMode::settingSeqLength:
        {
          currentSequence += 1;
          if (currentSequence >= sequencer->howManySequences()) currentSequence = sequencer->howManySequences() - 1;
          if (currentStep >= sequencer->howManySteps(currentSequence)) currentStep = sequencer->howManySteps(currentSequence) - 1;
         break;
        }
        case SequencerEditorMode::selectingSeqAndStep:
        {
          currentSequence += 1;
          if (currentSequence >= sequencer->howManySequences()) currentSequence = sequencer->howManySequences() - 1;
          if (currentStep >= sequencer->howManySteps(currentSequence)) currentStep = sequencer->howManySteps(currentSequence) - 1;
          break;
        }
        case SequencerEditorMode::editingStep:
        {
          std::vector<double> data = sequencer->getStepData(currentSequence, currentStep);
          //if (data[Step::note1Ind] > 0) // note is set
          //{
            SequencerEditor::decrementStepData(data, sequencer->getSequenceType(currentSequence));
            writeStepData(data);
          //}
          break;  
        }
        case SequencerEditorMode::configuringSequence:
        {
          // set the channel based on step 0
          std::vector<double> data2 = sequencer->getStepData(currentSequence, 0);
          int channel = data2[Step::channelInd];
          channel = (channel - 1) % 16;
          for (int step=0; step < sequencer->howManySteps(currentSequence); ++step)
          {
            sequencer->updateStepData(currentSequence, step, Step::channelInd, channel);
          }
          break;
        }
      }
   }
/** does an in place increment of the step data, as appropriate for the 
 * type of sequence
*/
static void decrementStepData(std::vector<double>& data, SequenceType seqType)
{
  switch(seqType)
  {
    case SequenceType::midiNote: // octave adjust
    {
        data[Step::note1Ind] -= 12;
        if (data[Step::note1Ind] < 0) data[Step::note1Ind] += 12;      
        break;
    }
    case SequenceType::transposer: // single step wrapped on 24
    {
        data[Step::note1Ind] -= 1;

        //data[Step::note1Ind] = fmod(data[Step::note1Ind], 24);
        //if (data[Step::note1Ind] < 0) data[Step::note1Ind] += 1;      
        break;
    }
  }
}
static void incrementStepData(std::vector<double>& data, SequenceType seqType)
{
    switch(seqType)
  {
    case SequenceType::midiNote: // octave adjust
    {
        data[Step::note1Ind] += 12;
        if (data[Step::note1Ind] > 127) data[Step::note1Ind] -= 12;      
        break;
    }
    case SequenceType::transposer: // single step wrapped on 24
    {
        data[Step::note1Ind] += 1;
        //data[Step::note1Ind] = fmod(data[Step::note1Ind], 24);
        //if (data[Step::note1Ind] < 0) data[Step::note1Ind] = 0;      
        break;
    }
  }
}

  void moveCursorLeft()
  {
      switch(editMode)
      {
        case SequencerEditorMode::settingSeqLength:
        {
          sequencer->shrinkSequence(currentSequence);
          break;
        }
        case SequencerEditorMode::selectingSeqAndStep:
        {
          currentStep -= 1;
          if (currentStep < 0) currentStep = 0;
          break;
        }
        case SequencerEditorMode::editingStep:
        {
        // left shortens the note
          std::vector<double> data = sequencer->getStepData(currentSequence, currentStep);
          data[Step::lengthInd] --;
          if (data[Step::lengthInd] < 1) data[Step::lengthInd] = 1;
          writeStepData(data);
          break;  
        }
        case SequencerEditorMode::configuringSequence:
        {
          SequencerEditor::nextSequenceType(sequencer, currentSequence);
          break;
        }
      }
  }

  void moveCursorRight()
  {
      switch(editMode)
      {
        case SequencerEditorMode::settingSeqLength:
        {
          sequencer->extendSequence(currentSequence);
          break;
        }
        case SequencerEditorMode::selectingSeqAndStep:
         {
            currentStep += 1;
          if (currentStep >= sequencer->howManySteps(currentSequence)) currentStep = sequencer->howManySteps(currentSequence) - 1;
          break;
         }
        case SequencerEditorMode::editingStep:
        {
        // right lengthens the note
          std::vector<double> data = sequencer->getStepData(currentSequence, currentStep);
          data[Step::lengthInd] ++;
          if (data[Step::lengthInd] > 10) data[Step::lengthInd] = 10;
          writeStepData(data);
          break;  
        }
        case SequencerEditorMode::configuringSequence:
        {
        // right changes the type
          SequencerEditor::nextSequenceType(sequencer, currentSequence);
          break;
        }
      }
  }

 static void nextSequenceType(Sequencer* seqr, unsigned int sequence)
 {
   SequenceType type = seqr->getSequenceType(sequence);
   switch (type){
     case SequenceType::midiNote:
      seqr->setSequenceType(sequence, SequenceType::transposer);
      break;
     //case SequenceType::samplePlayer:
     // break;
     case SequenceType::transposer:
      seqr->setSequenceType(sequence, SequenceType::midiNote);
      break;
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
    sequencer->setStepData(currentSequence, currentStep, data);
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

    static std::string toTextDisplay(const int rows, const int cols, const Sequencer* sequencer, const SequencerEditor* editor)
    {
      switch(editor->getEditMode())
      {
        case SequencerEditorMode::settingSeqLength:
         return getSequencerView(rows, cols, sequencer, editor);
        case SequencerEditorMode::selectingSeqAndStep:
         return getSequencerView(rows, cols, sequencer, editor);
        case SequencerEditorMode::configuringSequence:
          return getSequenceConfigView(sequencer->getStepData(editor->getCurrentSequence(), 0)[Step::channelInd], sequencer->getSequenceType(editor->getCurrentSequence()));
        case SequencerEditorMode::editingStep:
          return getStepView(sequencer->getStepData(editor->getCurrentSequence(), editor->getCurrentStep()), sequencer->isStepActive(editor->getCurrentSequence(), editor->getCurrentStep()));
      }
      return "Nothing to draw...";
    }

    /**
     * Returns a view of an individual step based on the sent step data
     */
    static std::string getStepView(const std::vector<double>& stepData, bool active)
    {
      std::string disp{""};
      if (active) disp += "O";
      else disp += " ";
      disp += "n:" + std::to_string((int)stepData[Step::note1Ind]);
      disp += " l:" + std::to_string((int)stepData[Step::lengthInd]);
      disp += " v:" + std::to_string((int)stepData[Step::velInd]);
      return disp;
    }
    /**
     * Returns a view that shows the config for the sent sequence, i.e.
     * the channel, where the channel is based on the first step's channel data
     */
    static std::string getSequenceConfigView(const unsigned int channel, SequenceType type)
    {
      std::string disp{""};
      disp += "c:" + std::to_string(channel);
      disp += " t:"; 
      
      switch (type){
        case SequenceType::midiNote:
          disp += "midi";
          break;
        case SequenceType::transposer:
          disp += "transpose";
          break;
        case SequenceType::samplePlayer:
          disp += "sample";
          break;
      }
      return disp;
    }
  
    /** generate a 'rows' line string representation of the state of the editor
     * and sequencer. Examples:
     * Starting state - I is where the 
     * 1-Iooooooo
     * 2-Oooooooo
     * For the student of module coupling, this is control coupling because the state of the incoming 
     * edutor dictates the behaviour of the function. It would probably be better to remove that 
     * and make two separate functions even if they are really similar
     */
   
    static std::string getSequencerView(const int rows, int cols, const Sequencer* sequencer, const SequencerEditor* editor)
    {

    // fix to display key info at the end of the row 
    //cols = cols - 3;

    // the editor cursor dictates which bit we show
      std::string disp{""};
      // we display the bit of the sequences
      // that the editor is looking at
      int seqOffset = 0;//-editor->getCurrentSequence();
      if (editor->getCurrentSequence() >= rows)
      {
        seqOffset = editor->getCurrentSequence() - 1;
      }
      int stepOffset = 0;//editor->getCurrentStep();

      if (editor->getCurrentStep() > cols - 4)
      {
        stepOffset = editor->getCurrentStep();
      }
      int displaySeq, displayStep;
      std::string preview {""};
      for (int seq=0;seq<rows;++seq)
      {
        displaySeq = seq + seqOffset;
        // the first thing is the channel number
        disp += std::to_string(displaySeq + 1);
        // space pad it
        if (displaySeq < 9) disp += " ";
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
          char cursor{'I'};
          
          // in step edit mode, printing a particular step
          // that does not have data
          if (editor->getEditMode() == SequencerEditorMode::selectingSeqAndStep && 
              sequencer->howManySteps(displaySeq) > displayStep && 
              sequencer->getStepData(displaySeq, displayStep)[Step::note1Ind] == 0)
          {
            state = '.';
          } 
       
          // inactive/ shortened/ non-existent sequence   
          if ((sequencer->howManySteps(displaySeq) <= displayStep || 
              sequencer->isStepActive(displaySeq, displayStep) == false)) state = ' ';

          // sequence length mode
          if (editor->getEditMode() == SequencerEditorMode::settingSeqLength && 
              sequencer->howManySteps(displaySeq) > displayStep) state = '>';

          // override inactive ' ' for 
          // sequencer playback is at this position
          if (sequencer->getCurrentStep(displaySeq) == displayStep) state = '-';
          
          
          // cursor is at this position
          if (editor->getCurrentSequence() == displaySeq &&
              editor->getCurrentStep() == displayStep &&
              sequencer->isStepActive(displaySeq, displayStep) 
              )
              {
                preview = std::to_string((int)sequencer->getStepData(displaySeq, displayStep)[Step::note1Ind]);
                state = cursor;          
              }
       
          disp += state;
        }
        if (seq < rows - 1)
          disp += preview + "\n";
          preview = "";
      }  
      return disp;
    }   
}; 

