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


/** default spec for a Step's data, 
 * so data[0] specifies length, 
 * data[1] specifies velocity 
 * and data[2] is the first note
 * 
*/
class Step{
  
  public:
    const static int channelInd{0};
    const static int lengthInd{1};
    const static int velInd{2};
    const static int note1Ind{3};
  
    Step();
    /** returns a copy of the data stored in this step*/
    std::vector<double> getData() const;
    /** sets the data stored in this step */
    void setData(std::vector<double> data);
    /** update one value in the data vector for this step*/
    void updateData(unsigned int dataInd, double value);
    /** set the callback function called when this step is triggered*/
    void setCallback(std::function<void(std::vector<double>)> callback);
    /** trigger this step, causing it to pass its data to its callback*/
    void trigger() const;
    /** toggle the activity status of this step*/
    void toggleActive();
    /** returns the activity status of this step */
    bool isActive() const;
  private: 
    std::vector<double> data;
    bool active;
    std::function<void(std::vector<double>)> stepCallback;
};

/**
 * Represents a pre-processor for a step's data, for example, allowing a step to be 
 * transposed. Normally these are stored per sequence and are activated when 
 * another sequence is a pre-preocessor sequence
 */
class StepDataPreProcessor{
  public:
  StepDataPreProcessor();
  void activate();
  void deactivate();
  bool isActive() const;
private:
  bool active; 
};

/** */
class StepDataTranspose : public StepDataPreProcessor
{
  public:
  StepDataTranspose(double transposeAmount);
  void processData(std::vector<double>& data);
private:
  double transposeAmount;
};

/** need this so can have a Sequencer data member in Sequence*/
class Sequencer;

/** use to define the type of a sequence. 
 * midiNote sends midi notes out
 * samplePlayer triggers internal samples
 * transposer transposes another sequence 
 **/
enum class SequenceType {midiNote, samplePlayer, transposer};

class Sequence{
  public:
    Sequence(Sequencer* sequencer, unsigned int seqLength = 16, unsigned short midiChannel = 1);
   
    /** go to the next step */
    void tick();
    /** which step are you on? */
    unsigned int getCurrentStep() const;
    /** is this step number valid? */
    bool assertStep(unsigned int step) const;
    /** retrieve a copy of the step data for the sent step */
    std::vector<double> getStepData(int step) const;
    /** set the data for the sent step */
    void setStepData(unsigned int step, std::vector<double> data);
    /** retrieve a copy of the step data for the current step */
    std::vector<double> getCurrentStepData() const;
    /** what is the length of the sequence? Length is a temporary property used
     * to define the playback length. There might be more steps than this
    */
    unsigned int getLength() const;
    /** set the length of the sequence 
     * If it is higher than the current max length, new steps will be created
    */
    void setLength(int length);
    /** how many steps does this sequence have it total. This is independent of the length. Length can be lower than how many steps*/
    unsigned int howManySteps() const ;
    
    /** update a single data value in a given step*/
    void updateStepData(unsigned int step, unsigned int dataInd, double value);
    /** set the callback for the sent step */
    void setStepCallback(unsigned int step, 
                  std::function<void (std::vector<double>)> callback);
    std::string stepToString(int step) const;
    /** activate/ deactive the sent step */
    void toggleActive(unsigned int step);
    /** check if the sent step is active */
    bool isStepActive(unsigned int step) const;
    /** set the sequence type */
    void setType(SequenceType type);
    SequenceType getType() const;
  /** add a transpose processor to this sequence. 
     * Normally, a transposer type sequence will call this on a midiNote type seqience
     * to apply a transpose to it 
    */
    void setStepProcessorTranspose(StepDataTranspose transpose);
    /** deactivate all data processors, e.g. transposers. */
    void deactivateProcessors();
  private:
    /** function called when the sequence ticks and it is a normal midi 
     * note type sequence
    */
    void triggerMidiNoteType();
 
    /** 
     * Called when the sequence ticks and it is a transpose type
    */
    void triggerTransposeType();
 
    Sequencer* sequencer;
    unsigned int currentLength;
    unsigned int currentStep;
    unsigned short midiChannel;
    std::vector<Step> steps;
    SequenceType type;
    /** Step pre-processors are stored directly onto
     * sequences so the sequence can apply them. This turned out to be
     * neater than a polymorphic data structure approach as 
     * it allows for a wider range of pre-processor functionality.
    */
    StepDataTranspose sdpTranspose;
    
};

/** represents a sequencer which is used to store a grid of data and to step through it */
class Sequencer  {
    public:
      Sequencer(unsigned int seqCount = 4, unsigned int seqLength = 16);
      unsigned int howManySequences() const ;
      unsigned int howManySteps(unsigned int sequence) const ;
      unsigned int getCurrentStep(unsigned int sequence) const;
      SequenceType getSequenceType(unsigned int sequence) const;
      /** move the sequencer along by one tick */
      void tick();
      Sequence* getSequence(unsigned int sequence);
      void setSequenceType(unsigned int sequence, SequenceType type);
      void setSequenceLength(unsigned int sequence, unsigned int length);
      void shrinkSequence(unsigned int sequence);
      void extendSequence(unsigned int sequence);
      void setAllCallbacks(std::function<void (std::vector<double>)> callback);
      /** set a callback for all steps in a sequence*/
      void setSequenceCallback(unsigned int sequence, std::function<void (std::vector<double>)> callback);
      /** set a lambda to call when a particular step in a particular sequence happens */
      void setStepCallback(unsigned int sequence, unsigned int step, std::function<void (std::vector<double>)> callback);
      /** update the data stored at a step in the sequencer */
      void setStepData(unsigned int sequence, unsigned int step, std::vector<double> data);
      /** update a single value in the  data 
       * stored at a step in the sequencer */
      void updateStepData(unsigned int sequence, unsigned int step, unsigned int dataInd, double value);
      /** retrieve the data for the current step */
      std::vector<double> getCurrentStepData(int sequence) const;
  
      /** retrieve the data for a specific step */
      std::vector<double> getStepData(int sequence, int step) const;
      void toggleActive(int sequence, int step);
      bool isStepActive(int sequence, int step) const;
      void addStepListener();

  /** print out a tracker style view of the sequence */
    std::string toString();

    private:
    bool assertSeqAndStep(unsigned int sequence, unsigned int step) const;
      
    bool assertSequence(unsigned int sequence) const;
    
    /// class data members  
    std::vector<Sequence> sequences;;
};



