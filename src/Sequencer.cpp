
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
void Step::setCallback(std::function<void(std::vector<double>)> callback)
{
  this->stepCallback = callback;
}
/** trigger this step, causing it to pass its data to its callback*/
void Step::trigger() const
{
  if (active && data[Step::note1Ind] != 0) stepCallback(data);
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

StepDataPreProcessor::StepDataPreProcessor() : active{false} {}
void StepDataPreProcessor::activate()
{
  active = true;
}
void StepDataPreProcessor::deactivate()
{
  active = false; 
}
bool StepDataPreProcessor::isActive() const
{
  return active;
}

StepDataTranspose::StepDataTranspose(double transposeAmount) : transposeAmount{transposeAmount}
{
}
void StepDataTranspose::processData(std::vector<double>& data) 
{
  data[Step::note1Ind] = fmod(data[Step::note1Ind] + transposeAmount, 127);
}

Sequence::Sequence(Sequencer* sequencer, 
                  unsigned int seqLength, 
                  unsigned short midiChannel) 
: sequencer{sequencer}, currentStep{0}, currentLength{seqLength}, midiChannel{midiChannel}, type{SequenceType::midiNote}, sdpTranspose{0}
{
  for (auto i=0;i<seqLength;i++)
  {
    Step s;
    s.setCallback([i](std::vector<double> data){
      if (data.size() > 0){
        std::cout << "Sequence::Sequence default step callback " << i << " triggered " << std::endl;
      }
    });
    steps.push_back(s);
  }
}

/** go to the next step */
void Sequence::tick()
{
  currentStep = (++currentStep) % currentLength;
  switch (type){
    case SequenceType::midiNote:
      triggerMidiNoteType();
      break;
    case SequenceType::transposer:
      triggerTransposeType();
      break;
  }
  
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
      s.setCallback([i](std::vector<double> data){
        //std::cout << "step " << i << " triggered " << std::endl;
      });
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
                  std::function<void (std::vector<double>)> callback)
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
  return currentLength;
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
void Sequence::setStepProcessorTranspose(StepDataTranspose transpose)
{
  transpose.activate();
  sdpTranspose = transpose; 
}

void Sequence::triggerMidiNoteType()
{
  // make a local copy
  Step s = steps[currentStep];
  // apply changes to local copy if needed      
  if(sdpTranspose.isActive()) 
  {
    std::vector<double> data = s.getData();
    sdpTranspose.processData(data);
    s.setData(data);
  }
  //steps[currentStep].trigger();
  s.trigger();
}

/** 
 * Called when the sequence ticks and it is a transpose type
*/
void Sequence::triggerTransposeType()
{
  std::vector<double> data = steps[currentStep].getData();
  sequencer->getSequence(data[Step::channelInd])->setStepProcessorTranspose(
    StepDataTranspose{data[Step::note1Ind]}
  );
} 
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

/** move the sequencer along by one tick */
void Sequencer::tick()
{
  //std::cout << "Sequencer.h: tick" << std::endl;
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


void Sequencer::setAllCallbacks(std::function<void (std::vector<double>)> callback)
{
    for (int seq = 0; seq < sequences.size(); ++seq)
    {
      setSequenceCallback(seq, callback);
    }
}

/** set a callback for all steps in a sequence*/
void Sequencer::setSequenceCallback(unsigned int sequence, std::function<void (std::vector<double>)> callback)
{
  for (int step = 0; step<sequences[sequence].howManySteps(); ++step)
  {
    sequences[sequence].setStepCallback(step, callback);
  }
}

/** set a lambda to call when a particular step in a particular sequence happens */
void Sequencer::setStepCallback(unsigned int sequence, unsigned int step, std::function<void (std::vector<double>)> callback)
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

