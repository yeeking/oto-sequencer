/** TODO: split this to h and cpp */
#include "Sequencer.h"

/**
 * Top level modes that dictate the main UI output 
 */
enum class SequencerEditorMode {settingSeqLength, configuringSequence, selectingSeqAndStep, editingStep};

/**
 * Dictates which item in a sub page we are editing
 **/
enum class SequencerEditorSubMode {editCol1, editCol2, editCol3};

/** Represents an editor for a sequencer, which allows stateful edit operations to be applied 
 * to sequences. For example, select sequemce, select step, enter data
 * Used to build editing interfaces for a sequencer 
*/
class SequencerEditor {
  public:
    SequencerEditor(Sequencer* sequencer);
    void setSequencer(Sequencer* sequencer);
    Sequencer* getSequencer();
    /** resets editor, e.g. when changing sequence*/
    void resetCursor();
    SequencerEditorMode getEditMode() const;
    SequencerEditorSubMode getEditSubMode() const;
    void setEditMode(SequencerEditorMode mode);
    /** cycle through the edit modes in the sequence:
     * settingSeqLength (start mode)
     * selectingSeqAndStep
     * editingStep
    */
    void cycleEditMode();
    /** 
     * depending on the mode, whoops bad coupling again! 
     * cycles the condition of the thing under the cursor
     * 
     */
    void cycleAtCursor();
    /** mode dependent reset function. Might reset */
    void resetAtCursor();
    /**
     *  Go into edit mode for either the sequence or step
     */
   void enterAtCursor();
/**
 * Tell the editor the user entered note data. The incoming note 
 * value is assumed to be in the range 0-127
 * 
 */
  void enterNoteData(double note);

  /** moves the editor cursor up. 
   * If in selectingSeqAndStep mode, steps through the sequenbces, wrapping at the top
   * if in editingStep mode, edits the 
   */
  void moveCursorUp();
  void moveCursorDown();
  void moveCursorLeft();
  void moveCursorRight();
  static SequencerEditorSubMode cycleSubModeLeft(SequencerEditorSubMode subMode);
  static SequencerEditorSubMode cycleSubModeRight(SequencerEditorSubMode subMode);

  /** decreas the sent step's data
   * based on current edit mode and edit sub mode
  */
  void decrementStepData(std::vector<double>& data, SequenceType seqType);

  /** increase the sent step's data
   * based on current edit mode and edit sub mode
  */
  void incrementStepData(std::vector<double>& data, SequenceType seqType);
  /** increase the value of the seq param relating to the 
   * current subMode
  */
  void incrementSeqConfigParam();

  /** decrease the value of the seq param relating to the 
   * current subMode
  */
  void decrementSeqConfigParam();

  void incrementChannel();
  void decrementChannel();
  void incrementTicksPerStep();
  void decrementTicksPerStep();
  static void nextSequenceType(Sequencer* seqr, unsigned int sequence);
  int getCurrentSequence() const;
  /**  */
  int getCurrentStep() const;
  /** which data point in a step are we editing */
  int getCurrentStepIndex() const;
  /** move the cursor to a specific sequence*/
  void setCurrentSequence(int seq);
  /** move the cursor to a specific step*/
  void setCurrentStep(int step);
  /** write the sent data to the current step and sequence */
  void writeStepData(std::vector<double> data);
  /** write the sent data to the sequence at 'currentSequence' - 1D data version for simple one value per step -style sequences*/
  void writeSequenceData(std::vector<double> data);
  /** write the sent data to a sequence - 1D data version */
  void writeSequenceData(std::vector<std::vector<double>> data);

  private:
    Sequencer* sequencer; 
    /** which sequence*/
    int currentSequence;
    /** which step */
    int currentStep;
    /** which data point inside a step*/
    int currentStepIndex;
    
    SequencerEditorMode editMode;
    SequencerEditorSubMode editSubMode;
    double stepIncrement;    
};

/** Provides functions to generate string-based views of the sequencer
 * 
*/
class SequencerViewer{
  public:
    SequencerViewer();

    static std::string toTextDisplay(const int rows, const int cols, Sequencer* sequencer, const SequencerEditor* editor);

    /**
     * Returns a view of an individual step based on the sent step data
     * stepData is the data for te step
     * active is if the step is active or note
     * editField is the field they are currently using
     * stepInd is the index of this step
     * clockInd is the index of the clock (so should we highlight it somehow)
     */
    static std::string getStepView(const std::vector<double>& stepData, bool active, SequencerEditorSubMode editField, int stepInd, int clockInd);

    /**
     * Returns a view that shows the config for the sent sequence, i.e.
     * the channel, where the channel is based on the first step's channel data
     */
    static std::string getSequenceConfigView(const unsigned int channel, const SequenceType type, const unsigned int ticksPerStep,  const SequencerEditorSubMode editField);
    /** generate a 'rows' line string representation of the state of the editor
     * and sequencer. Examples:
     * Starting state - I is where the 
     * 1-Iooooooo
     * 2-Oooooooo
     * For the student of module coupling, this is control coupling because the state of the incoming 
     * edutor dictates the behaviour of the function. It would probably be better to remove that 
     * and make two separate functions even if they are really similar
     */
   
    static std::string getSequencerView(const int max_rows, const int cols,  Sequencer* sequencer, const SequencerEditor* editor);
   
}; 

