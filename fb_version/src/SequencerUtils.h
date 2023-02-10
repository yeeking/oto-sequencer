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
    /** returns current major edit mode which is a SequencerEditorMode*/
    SequencerEditorMode getEditMode() const; 
    /** returns current minor edit mode which is a SequencerEditorSubMode*/
    SequencerEditorSubMode getEditSubMode() const;
    /** directly set major edit mode */
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

struct MidiUtils{

    static std::map<int,char> getIntToNoteMap()
    {
    std::map<int, char> intToNote = 
      {
        {0, 'c'}, 
        {1, 'C'},
        {2, 'd'},
        {3, 'D'},
        {4, 'e'},
        {5, 'f'},
        {6, 'F'},
        {7, 'g'},
        {8, 'G'},
        {9, 'a'},
        {10, 'A'},
        {11, 'b'}    
      };
      return intToNote;
    }
    
    /** maps from integer values, i.e. midi notes modded on 12 or 24 to drum 
     * names, e.g. 0->B for bassdrum. Can be used to display one character drum 
     * names 
    */
    static std::map<int,char> getIntToDrumMap()
    {
    std::map<int, char> intToDrum = 
      {
        {0, 'B'}, 
        {1, 's'},
        {2, 'S'},
        {3, 'r'},
        {4, 'H'},
        {5, 'h'},
        {6, 't'},
        {7, 'T'},
        {8, 'c'},
        {9, 'R'},
        {10, 'C'},
        {11, 'p'}    
      };
      return intToDrum;
    }
    /**
     * returns a mapping from a scale starting at note 48 ... 59
     * mapped to midi notes for drums remapped to get the most useful drums 
     */
    static std::map<int,int> getScaleMidiToDrumMidi()
    {
      std::map<int,int> scaleToDrum = 
      {
        {48, 36}, 
        {49, 38},
        {50, 40},
        {51, 37},
        {52, 42},
        {53, 46},
        {54, 50},
        {55, 45},
        {56, 39},
        {57, 51},
        {58, 57},
        {59, 75}    
      };
      return scaleToDrum;
    }

    /** maps from drum names (e.g. B, s) to general midi notes 
     * e.g. B(bass drum) -> 36. 
    */
    static std::map<char,int> getDrumToMidiNoteMap()
    {
     std::map<char,int> drumToInt = 
      {
        {'B', 36}, 
        {'s', 38},
        {'S', 40},
        {'r', 37},
        {'H', 42},
        {'h', 46},
        {'t', 50},
        {'T', 45},
        {'c', 39},
        {'R', 51},
        {'C', 57},
        {'p', 75}    
      };
      return drumToInt;
    }

    static std::map<char, double> getKeyboardToMidiNotes(int transpose = 0)
    {
      std::map<char, double> key_to_note =
      {
        { 'z', 48+transpose},
        { 's', 49+transpose},
        { 'x', 50+transpose},
        { 'd', 51+transpose},
        { 'c', 52+transpose},
        { 'v', 53+transpose},
        { 'g', 54+transpose},
        { 'b', 55+transpose},
        { 'h', 56+transpose},
        { 'n', 57+transpose},
        { 'j', 58+transpose},
        { 'm', 59+transpose}
      };
      return key_to_note;
    }
    // /** from midi note as stored in the seq to a drum name
    //  * used to display drum mode sequences
    // */
    // static std::map<int,char> getNoteToDrum()
    // {

    // }

};

