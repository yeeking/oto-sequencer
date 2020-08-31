#include <vector>
#include <iostream>
#include <string>
#include <functional>
#include <cmath> // fmod
#include "Sequencer.h"

Step::Step() : active{true}
{
  data.push_back(0.0);
  data.push_back(0.0);
  data.push_back(0.0);
  data.push_back(0.0);
}
/** returns a copy of the data stored in this step*/
std::vector<double> Step::getData() const
{
  return data; 
}
std::vector<double>* Step::getDataDirect()
{
  return &data; 
}


/** sets the data stored in this step */
void Step::setData(std::vector<double> data)
{
  this->data = data; 
}

/** update one value in the data vector for this step*/
void Step::updateData(unsigned int dataInd, double value)
{
  if(dataInd < data.size()) data[dataInd] = value;
}
/** set the callback function called when this step is triggered*/
void Step::setCallback(std::function<void(std::vector<double>*)> callback)
{
  this->stepCallback = callback;
}
std::function<void(std::vector<double>*)> Step::getCallback()
{
  return this->stepCallback;
}

/** trigger this step, causing it to pass its data to its callback*/
void Step::trigger() 
{
  //std::vector<double>* dataP = &data;// convert to a real pointer
  if (active && data[Step::note1Ind] != 0) stepCallback(&data);
}
/** toggle the activity status of this step*/
void Step::toggleActive()
{
  active = !active; 
}
/** returns the activity status of this step */
bool Step::isActive() const
{
  return active; 
}

Sequence::Sequence(Sequencer* sequencer, 
                  unsigned int seqLength, 
                  unsigned short midiChannel) 
: sequencer{sequencer}, currentStep{0}, currentLength{seqLength}, 
  midiChannel{midiChannel}, type{SequenceType::midiNote}, 
  transpose{0}, lengthAdjustment{0}, ticksPerStep{1}, ticksElapsed{0}
{
  for (auto i=0;i<seqLength;i++)
  {
    Step s;
    s.setCallback([i](std::vector<double>* data){
      if (data->size() > 0){
        std::cout << "Sequence::Sequence default step callback " << i << " triggered " << std::endl;
      }
    });
    steps.push_back(s);
  }
}

/** go to the next step */
void Sequence::tick()
{
  ++ticksElapsed;
  if (ticksElapsed == ticksPerStep)
    {
      ticksElapsed = 0;
      switch (type){
        case SequenceType::midiNote:
          triggerMidiNoteType();
          break;
        case SequenceType::transposer:
          triggerTransposeType();
          break;
        case SequenceType::lengthChanger:
          triggerLengthType();
          break;
        case SequenceType::tickChanger:
          triggerTickType();
          break;
          
    }
    currentStep = (++currentStep) % (currentLength + lengthAdjustment);
    if (currentStep == 0)
    {
      // reset transpose at the start of each sequence
      deactivateProcessors();
    }
  }
}

void Sequence::deactivateProcessors()
{
    transpose = 0;
    lengthAdjustment = 0;
}

void Sequence::triggerMidiNoteType()
{
  // make a local copy
  Step s = steps[currentStep];
  // apply changes to local copy if needed      
  if(transpose > 0) 
  {
    std::vector<double> data = s.getData();
    if (data[Step::note1Ind] > 0 ) // only transpose non-zero steps
    {
      data[Step::note1Ind] = fmod(data[Step::note1Ind] + transpose, 127);
      s.setData(data);
    }
  }
  // trigger the local, adjusted copy of the step
  s.trigger();
}

/** 
 * Called when the sequence ticks and it is a transpose type
 * Causes this sequence to apply a transpose to another sequence
*/
void Sequence::triggerTransposeType()
{
  if (steps[currentStep].isActive() )
  {
    std::vector<double> data = steps[currentStep].getData();
    if (data[Step::note1Ind] != 0) // only do anything if they set a non-zero value
    {
      sequencer->getSequence(data[Step::channelInd])->setTranspose(data[Step::note1Ind]);
    }
  }
} 

void Sequence::triggerLengthType()
{
  if (steps[currentStep].isActive())
  {
    std::vector<double> data = steps[currentStep].getData();  
    if (data[Step::note1Ind] != 0) // only do anything if they set a non-zero value
    { 
      sequencer->getSequence(data[Step::channelInd])->setLengthAdjustment(data[Step::note1Ind]);
    }
  } 
}

void Sequence::triggerTickType()
{
  if (steps[currentStep].isActive() )
  {
    std::vector<double>* data = steps[currentStep].getDataDirect();
    if (data->at(Step::note1Ind) != 0) // only do anything if they set a non-zero value
    {
      sequencer->getSequence(data->at(Step::channelInd))->setTicksPerStep(data->at(Step::note1Ind));
    }
  }
} 


void Sequence::setLengthAdjustment(int lenAdjust)
{
  // a little assert
  if (currentLength + lenAdjust < 1) return;
  lengthAdjustment = lenAdjust;
  
  // this code makes sure we can accommodate the 
  // length adjust, but without changing the real length now
  // noting that the tick function decides
  // when to go back to step 0
  // this allows the length adjustment (which is caused by another sequence)
  // to be separate from the current length
  // and therefore easily removed (which prevents repeatedly extending 
  // )
  int original_length = currentLength;
  setLength(original_length + lenAdjust);
  setLength(original_length);
}

void Sequence::setTicksPerStep(int tps)
{
  this->ticksPerStep = tps;
}

int Sequence::getTicksPerStep() const
{
  return this->ticksPerStep;
}

unsigned int Sequence::getCurrentStep() const
{
  return currentStep; 
} 
bool Sequence::assertStep(unsigned int step) const
{
  if (step >= steps.size() || step < 0) return false;
  return true; 
}
std::vector<double> Sequence::getStepData(int step) const
{
  return steps[step].getData();
}
std::vector<double>* Sequence::getStepDataDirect(int step)
{
  return steps[step].getDataDirect();
}

std::vector<double> Sequence::getCurrentStepData() const
{
  return steps[currentStep].getData();
}
unsigned int Sequence::getLength() const
{
  return currentLength; 
}

void Sequence::setLength(int length)
{
  if (length < 1) return;
  if (length > steps.size()) // bad need more steps
  {
    int toAdd = length - steps.size();
    for (int i=0; i < toAdd; ++i)
    {
      Step s;
      s.setCallback(
        steps[0].getCallback()
      );
      steps.push_back(s);
    }
  }
  currentLength = length;
}

void Sequence::setStepData(unsigned int step, std::vector<double> data)
{
  steps[step].setData(data);
}
/** update a single data value in a given step*/
void Sequence::updateStepData(unsigned int step, unsigned int dataInd, double value)
{
  steps[step].updateData(dataInd, value);
}

void Sequence::setStepCallback(unsigned int step, 
                  std::function<void (std::vector<double>*)> callback)
{
  steps[step].setCallback(callback);
}
std::string Sequence::stepToString(int step) const
{
  std::vector<double> data = getStepData(step);
  if (data.size() > 0)
    return std::to_string(data[0]);
  else
    return "---";
}

unsigned int Sequence::howManySteps() const 
{
  //return steps.size();
  return currentLength + lengthAdjustment;
}

void Sequence::toggleActive(unsigned int step)
{
  steps[step].toggleActive();
}
bool Sequence::isStepActive(unsigned int step) const
{
  return steps[step].isActive();
}
void Sequence::setType(SequenceType type)
{
  this->type = type;
}
SequenceType Sequence::getType() const
{
  return this->type;
}

void Sequence::setTranspose(double transpose)
{
  this->transpose = transpose;
}


/////////////////////// Sequencer 

Sequencer::Sequencer(unsigned int seqCount, unsigned int seqLength) 
{
  for (auto i=0;i<seqCount;++i)
  {
    sequences.push_back(Sequence{this, seqLength});
  }
}

unsigned int Sequencer::howManySequences() const 
{
  return sequences.size();
}
unsigned int Sequencer::howManySteps(unsigned int sequence) const 
{
  if (assertSequence(sequence)) return sequences[sequence].howManySteps();
  else return 0;
}
unsigned int Sequencer::getCurrentStep(unsigned int sequence) const
{
  return sequences[sequence].getCurrentStep();
}

SequenceType Sequencer::getSequenceType(unsigned int sequence) const 
{
  return sequences[sequence].getType();
}

/** move the sequencer along by one tick */
void Sequencer::tick()
{
  for (Sequence& seq : sequences)
  {
      seq.tick();
  }
}

Sequence* Sequencer::getSequence(unsigned int sequence)
{
  return &(sequences[sequence]);
}

void Sequencer::setSequenceType(unsigned int sequence, SequenceType type)
{
  sequences[sequence].setType(type);
}

void Sequencer::setSequenceLength(unsigned int sequence, unsigned int length)
{
  sequences[sequence].setLength(length);
}

void Sequencer::shrinkSequence(unsigned int sequence)
{
  sequences[sequence].setLength(sequences[sequence].getLength()-1);
}
void Sequencer::extendSequence(unsigned int sequence)
{
  sequences[sequence].setLength(sequences[sequence].getLength()+1);
}


void Sequencer::setAllCallbacks(std::function<void (std::vector<double>*)> callback)
{
    for (int seq = 0; seq < sequences.size(); ++seq)
    {
      setSequenceCallback(seq, callback);
    }
}

/** set a callback for all steps in a sequence*/
void Sequencer::setSequenceCallback(unsigned int sequence, std::function<void (std::vector<double>*)> callback)
{
  for (int step = 0; step<sequences[sequence].howManySteps(); ++step)
  {
    sequences[sequence].setStepCallback(step, callback);
  }
}

/** set a lambda to call when a particular step in a particular sequence happens */
void Sequencer::setStepCallback(unsigned int sequence, unsigned int step, std::function<void (std::vector<double>*)> callback)
{
    sequences[sequence].setStepCallback(step, callback); 
}

/** update the data stored at a step in the sequencer */
void Sequencer::setStepData(unsigned int sequence, unsigned int step, std::vector<double> data)
{
  if (!assertSeqAndStep(sequence, step)) return;
  sequences[sequence].setStepData(step, data);
}
/** update a single value in the  data 
 * stored at a step in the sequencer */
void Sequencer::updateStepData(unsigned int sequence, unsigned int step, unsigned int dataInd, double value)
{
  if (!assertSeqAndStep(sequence, step)) return;
  sequences[sequence].updateStepData(step, dataInd, value);
}

/** retrieve the data for the current step */
std::vector<double> Sequencer::getCurrentStepData(int sequence) const
{
  if (sequence >= sequences.size() || sequence < 0) return std::vector<double>{};
  return sequences[sequence].getCurrentStepData();
}

/** retrieve the data for a specific step */
std::vector<double> Sequencer::getStepData(int sequence, int step) const
{
  if (!assertSeqAndStep(sequence, step)) return std::vector<double>{};
  return sequences[sequence].getStepData(step);
}
/** retrieve the data for a specific step */
std::vector<double>* Sequencer::getStepDataDirect(int sequence, int step)
{
  // TODO should throw an exception if they ask for an invalid step or sequence
  //if (!assertSeqAndStep(sequence, step)) return std::vector<double>{};
  return sequences[sequence].getStepDataDirect(step);
}

void Sequencer::toggleActive(int sequence, int step)
{
  if (!assertSeqAndStep(sequence, step)) return;
  sequences[sequence].toggleActive(step);
}
bool Sequencer::isStepActive(int sequence, int step) const
{
  if (!assertSeqAndStep(sequence, step))  return false; 
  return sequences[sequence].isStepActive(step);
}
void Sequencer::addStepListener()
{

}

/** print out a tracker style view of the sequence */
std::string Sequencer::toString()
{
std::string s{""};
for (int step = 0; step < 32; ++step)
{
  s += std::to_string(step) + "\t: ";
  for (Sequence& seq : sequences)
  {
    // s += seq.stepToString(step) + "\t";
  }
  s += "\n";
}
return s;
}


bool Sequencer::assertSeqAndStep(unsigned int sequence, unsigned int step) const
{
  if (!assertSequence(sequence)) return false;
  if (!sequences[sequence].assertStep(step)) return false; 
  return true;
}

bool Sequencer::assertSequence(unsigned int sequence) const
{
if (sequence >= sequences.size() || sequence < 0)
  {
    //std::cout << "Sequence " << sequence << " out of range. use 0 - " << (sequences.size()-1) << std::endl;
    return false;
  } 
return true;
}

