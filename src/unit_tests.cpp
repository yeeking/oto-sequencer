#include <iostream>
#include "../lib/ml/rapidLib.h"
#include "Sequencer.h"
#include "SequencerUtils.h"
#include "MidiUtils.h"
#include "RapidLibUtils.h"
#include "EventQueue.h"
#include <fstream>
#include "ChordUtils.h"

bool assertStrEqual(std::string want, std::string got)
{
  if (want == got) return true;
  else {
    std::cout << "assertStrEq want " << want << " got " << got << std::endl;
    return false; 
  }
}

bool assertNumEqual(double want, double got)
{
  if (want == got) return true;
  else {
    std::cout << "assertNumEqual want " << want << " got " << got << std::endl;
    return false; 
  }
}



bool testTick()
{
  Sequencer seq{};
  seq.tick();
  seq.tick();
  return true;
}

bool testUpdate()
{
  Sequencer* seqrP;
Sequence seq{seqrP};
  seq.setStepData(0, std::vector<double>{0.1f});
  std::vector<double> data = seq.getStepData(0);
  if (data[0] == 0.1f) return true;
  return false;
}

bool testUpdate2()
{
  Sequencer* seqrP;
Sequence seq{seqrP};
  seq.setStepData(1, std::vector<double>{0.5f});
  std::vector<double> data = seq.getStepData(1);
  if (data[0] == 0.5f) return true;
  return false;
}

bool testToString()
{
  Sequencer seq;
  std::string s = seq.toString();
  //std::cout << s << std::endl;
  return true;
}

bool testTick2()
{
  // set a value and check we get it back after a tick
  Sequencer seqr;
  seqr.setStepData(0, 0, std::vector<double>{0.1f});
  seqr.setStepData(0, 1, std::vector<double>{0.2f});
  // pull data for track 0, current step
  std::vector<double> data = seqr.getCurrentStepData(0);
  if (data[0] != 0.1f) return false;
  seqr.tick();
  data = seqr.getCurrentStepData(0);
  if (data[0] != 0.2f) return false;
  return true;
}

bool testCursorStart()
{
  Sequencer seqr{};
  SequencerEditor cursor{&seqr};
  // should be 0
  if (cursor.getCurrentSequence() == 0 && cursor.getCurrentStep() == 0)
  return true;
  else return false; 
}
bool testCursorRight()
{
  Sequencer seqr{};
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  cursor.moveCursorRight();
  // should be at step 1
  if (cursor.getCurrentSequence() == 0 && cursor.getCurrentStep() == 1)
  return true;
  else return false; 
}
bool testCursorRightLimit()
{
  Sequencer seqr{};
  SequencerEditor cursor{&seqr};
  for (int i=0;i<1000;++i) cursor.moveCursorRight();
  // should be at step seqr.numbersteps
  if (cursor.getCurrentSequence() == 0 && cursor.getCurrentStep() == seqr.howManySteps(0)-1)
  return true;
  else return false; 
}

bool testCursorLeftLimit()
{
  Sequencer seqr{};
  SequencerEditor cursor{&seqr};
  cursor.moveCursorLeft();
  cursor.moveCursorLeft();
  cursor.moveCursorLeft();
  
  // should be at step 0
  if (cursor.getCurrentSequence() == 0 && cursor.getCurrentStep() == 0)
  return true;
  else {
    return false; 
  }
}

bool testCursorLeft()
{
  Sequencer seqr{};
  SequencerEditor cursor{&seqr};
  // go into step selecting mode
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  // 10 to the right then 1 to the left
  // should be at 9
  for (int i=0;i<10;i++) cursor.moveCursorRight();
  cursor.moveCursorLeft();
  // should be at step 1
  if (cursor.getCurrentStep() == 9)
  return true;
  else return false; 
}


bool testTwoLinesInit()
{
  Sequencer seqr{};
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);

  std::string want = "1 -Ioooo\n2 --oooo";
  std::string got = SequencerViewer::toTextDisplay(2, 8, &seqr, &cursor);
  if (want != got) 
  {
    std::cout << "testTwoLinesInit:: Wanted \n" << want << " \n got \n" << got << std::endl;
    return false;
  }
  else return true;
}

bool testTwoLines16()
{
  Sequencer seqr{};
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  std::string want = "1 -Ioooooooooooo\n2 --oooooooooooo";
  std::string got = SequencerViewer::toTextDisplay(2, 16, &seqr, &cursor);
  if (want != got) 
  {
    std::cout << "testTwoLinesInit:: Wanted \n" << want << " \n got \n" << got << std::endl;
    return false;
  }
  else return true;
}

bool testTwoLines16Tick()
{
  Sequencer seqr{};
  seqr.tick();
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  std::string want = "1 -I-ooooooooooo\n2 -o-ooooooooooo";
  std::string got = SequencerViewer::toTextDisplay(2, 16, &seqr, &cursor);
  if (want != got) 
  {
    std::cout << "testTwoLinesInit:: Wanted \n" << want << " \n got \n" << got << std::endl;
    return false;
  }
  else return true;
}

bool testTwoLinesWrapSeq()
{
  Sequencer seqr{};
  // set seq lengths to 8
  // 
  // 9 ticks takes it to step 1 (index 2)
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  seqr.setSequenceLength(0, 8);
  seqr.setSequenceLength(1, 8);
  
  for (int i=0;i<10;++i) seqr.tick();

  std::string want = "1 -Io-oo\n2 -oo-oo";

  std::string got = SequencerViewer::toTextDisplay(2, 8, &seqr, &cursor);
  if (want != got) 
  {
    std::cout << "testTwoLinesWrapSeq:: Wanted \n" << want << " \n got \n" << got << std::endl;
    return false;
  }
  else return true;
}



bool testFollowEditCursorRight()
{
  Sequencer seqr{};
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  cursor.moveCursorRight();
  std::string want = "1 -Ioooo\n2 --oooo";

  std::string got = SequencerViewer::toTextDisplay(2, 8, &seqr, &cursor);
  if (want != got) 
  {
    std::cout << "testFollowEditCursorRight:: Wanted \n" << want << " \n got \n" << got << std::endl;
    return false;
  }
  else return true;
}

bool testFollowEditCursorDown()
{
  Sequencer seqr{};
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  cursor.moveCursorDown();
  std::string want = "1 -Ioooo\n2 --oooo";

  std::string got = SequencerViewer::toTextDisplay(2, 8, &seqr, &cursor);
  if (want != got) 
  {
    std::cout << "testFollowEditCursorDown:: Wanted \n" << want << " \n got \n" << got << std::endl;
    return false;
  }
  else return true;
}

bool testFollowEditCursorLeftLimit()
{
  Sequencer seqr{};
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  cursor.moveCursorLeft();
  std::string want = "1 -Ioooo\n2 --oooo";

  std::string got = SequencerViewer::toTextDisplay(2, 8, &seqr, &cursor);
  if (want != got) 
  {
    std::cout << "testFollowEditCursorLeftLimit:: Wanted \n" << want << " \n got \n" << got << std::endl;
    return false;
  }
  else return true;
}

bool testFollowEditCursorRightLimit()
{
  Sequencer seqr{};
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  for (int i=0; i < 20; ++i) cursor.moveCursorRight();
  std::string want = "1  I    \n2  o    ";

  std::string got = SequencerViewer::toTextDisplay(2, 8, &seqr, &cursor);
  if (want != got) 
  {
    std::cout << "testFollowEditCursorRightLimit:: Wanted \n" << want << " \n got \n" << got << std::endl;
    return false;
  }
  else return true;
}

bool testFollowEditCursorRightLimitNearly()
{
  Sequencer seqr{4, 4};
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  for (int i=0; i < 2; ++i) cursor.moveCursorRight();
  std::string want = "1  Io   \n2  oo   ";

  std::string got = SequencerViewer::toTextDisplay(2, 8, &seqr, &cursor);
  if (want != got) 
  {
    std::cout << "testTwoLinesInit:: Wanted \n" << want << " \n got \n" << got << std::endl;
    return false;
  }
  else return true;
}

bool testFollowEditCursorDownLimit()
{
  Sequencer seqr{4}; // 4 seqs
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  for (int i=0; i<10; ++i) cursor.moveCursorDown();

  std::string want = "2 -Ioooo\n3 --oooo";

  std::string got = SequencerViewer::toTextDisplay(2, 8, &seqr, &cursor);
  if (want != got) 
  {
    std::cout << "testFollowEditCursorDownLimit:: Wanted \n" << want << " \n got \n" << got << std::endl;
    return false;
  }
  else return true;
}
bool testNeuralNetwork()

{
  rapidLib::regression network = NeuralNetwork::getMelodyStepsRegressor();
  std::vector<double> output = network.run({0.1,   0.2});
  // verify it is not returning zeros
  if (output[0] == 0) return false;
  if (output[0] > 127) return false;
  std::cout << output[0];
  return true;
}


bool testStepMode()
{
  Sequencer seqr{4}; // 4 seqs
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  return true;  
}

bool testIsStepActiveStep()
{
  Step step{};
  return step.isActive();
}

bool testIsStepActiveSeq()
{
  Sequencer* seqrP;
Sequence seq{seqrP};
  return seq.isStepActive(0);;
}

bool testIsStepActiveSeqr()
{
  Sequencer seqr{};
  return seqr.isStepActive(0, 0);
}

bool testToggleStepActiveStep()
{
  Step step{};
  step.toggleActive();
  return !step.isActive();
}

bool testToggleStepActiveSeq()
{
  Sequencer* seqrP;
Sequence seq{seqrP};
  seq.toggleActive(0);
  return !seq.isStepActive(0);;
}

bool testToggleStepActiveSeqr()
{
  Sequencer seqr{};
  seqr.toggleActive(0, 0);
  return !seqr.isStepActive(0, 0);
}


bool testCycleAtCursorTogglesActive()
{
  Sequencer seqr{};
  SequencerEditor editor{&seqr};
  editor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  // should set the first step to inactive
  editor.cycleAtCursor();
  return !seqr.isStepActive(0, 0);
}


bool testCycleAtCursorTogglesActiveTwice()
{
  Sequencer seqr{};
  SequencerEditor editor{&seqr};
  editor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  // should set the first step to inactive
  editor.cycleAtCursor();
  // another toggle to go back to true
  editor.cycleAtCursor();
  
  return seqr.isStepActive(0, 0);
}

bool testShowInactiveSteps()
{
  Sequencer seqr{};
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  cursor.moveCursorLeft();
  seqr.toggleActive(0, 3); // deactive step 4 on seq 1
  bool test = seqr.isStepActive(0, 3);
  std::cout << "testShowInactiveSteps:: state of 0.3 " << test << std::endl;

  std::string want = "1 -Ioo o\n2 --oooo";

  std::string got = SequencerViewer::toTextDisplay(2, 8, &seqr, &cursor);
    std::cout << "testShowInactiveSteps:: Wanted \n" << want << " \n got \n" << got << std::endl;
  
  if (want != got) 
  {
    std::cout << "testShowInactiveSteps:: Wanted \n" << want << " \n got \n" << got << std::endl;
    return false;
  }
  
  else return true;
}


bool testStepEditModeLen()
{
  Sequencer seqr;
  SequencerEditor editor(&seqr);
  editor.setEditMode(SequencerEditorMode::editingStep);
  // should now be editing seq 0, step 0
  // make an edit then verify
  std::vector<double> data;
  editor.moveCursorRight();
  data = seqr.getStepData(0, 0);
  if (data[0] == 1) return true;
  return false;
}

bool testStepEditModeVel()
{
  Sequencer seqr;
  SequencerEditor editor(&seqr);
  editor.setEditMode(SequencerEditorMode::editingStep);
  // should now be editing seq 0, step 0
  // make an edit then verify
  std::vector<double> data;
  editor.moveCursorUp();
  data = seqr.getStepData(0, 0);
  if (data[1] == 1) return true;
  return false;
}
bool testStepEditModeVel2()
{
  Sequencer seqr;
  SequencerEditor editor(&seqr);
  editor.setEditMode(SequencerEditorMode::selectingSeqAndStep);

  editor.moveCursorRight();
  // should now be editing seq 0, step 0
  // make an edit then verify
  std::vector<double> data;
  editor.setEditMode(SequencerEditorMode::editingStep);
  editor.moveCursorUp();
  editor.moveCursorUp();
  data = seqr.getStepData(editor.getCurrentSequence(), editor.getCurrentStep());
  
  if (data[1] == 2) return true;
  return false;
}


bool testEnterNoteData()
{
  Sequencer seqr;
  SequencerEditor editor(&seqr);
  editor.setEditMode(SequencerEditorMode::editingStep);
  editor.enterNoteData(64);
  std::vector<double> data = seqr.getStepData(0, 0);
  if (data[Step::note1Ind] == 64) return true;
  return false;
}

// add something to the queue
bool testEQAdd()
{
  EventQueue q;
  q.addEvent(0, [](){});
  return true;
}

bool testEQAddAtTs()
{
  EventQueue q;
  q.addEvent(0, [](){});
  // verify that we get back this item when we query
  CallbackVector callbacks = q.getEventsAtTimestamp(0);
  if (callbacks.size() == 1) return true;
  return false;
}

bool testEQAddAtTs2()
{
  EventQueue q;
  q.addEvent(0, [](){});
  q.addEvent(0, [](){});
  
  q.addEvent(10, [](){});
  
  // verify that we get back this item when we query
  CallbackVector callbacks = q.getEventsAtTimestamp(0);
  if (callbacks.size() == 2) return true;
  return false;
}


bool testEQAddAtTs10()
{
  EventQueue q;
  q.addEvent(0, [](){});
  q.addEvent(0, [](){});
  q.addEvent(10, [](){});
  // verify that we get back this item when we query
  CallbackVector callbacks = q.getEventsAtTimestamp(10);
  if (callbacks.size() == 1) return true;
  return false;
}


bool testEQTriggerRemove()
{
  EventQueue q;
  q.addEvent(0, [](){std::cout << "0 1" << std::endl;});
  q.addEvent(0, [](){std::cout << "0 2" << std::endl;});
  q.addEvent(10, [](){std::cout << "10 1" << std::endl;});
;  
  // now ts 0 should have one event
  int sizeBefore =  q.getEventsAtTimestamp(0).size();
  std::cout << "testEQTriggerRemove getting events before triugger " << std::endl;
  q.triggerAndClearEventsAtTimestamp(0);
  std::cout << "testEQTriggerRemove getting events after triugger " << std::endl;
  int sizeAfter =  q.getEventsAtTimestamp(0).size();
  if (sizeBefore == 2 && sizeAfter == 0) return true;
  return false;
}


bool testEQTriggerRemove10()
{
  EventQueue q;
  q.addEvent(0, [](){std::cout << "0 1" << std::endl;});
  q.addEvent(0, [](){std::cout << "0 2" << std::endl;});
  q.addEvent(10, [](){std::cout << "10 1" << std::endl;});
  
  // now ts 0 should have one event
  int sizeBefore =  q.getEventsAtTimestamp(10).size();
  q.triggerAndClearEventsAtTimestamp(10);
  int sizeAfter =  q.getEventsAtTimestamp(10).size();
  if (sizeBefore == 1 && sizeAfter == 0) return true;
  return false;
}

// get it into configuring sequence mode
// via controls
bool testSequenceConfigMode()
{
   bool res = false;
   // check if we can get into sequence config mode
   Sequencer seqr{};
   SequencerEditor cursor{&seqr};
   // starting at sequence edit mode
   // this should put it into config sequence 
   // mode
   cursor.cycleEditMode();// it starts in selectingSequence and step. this takes it to seq edit
   cursor.enterAtCursor();
   if (cursor.getEditMode() == SequencerEditorMode::configuringSequence){
     res = true;
   } 
   return res;
}


// direct version instructs it directly
// to go into the configuring seq mode
bool testSequenceConfigModeDirect()
{
   bool res = false;
   // check if we can get into sequence config mode
   Sequencer seqr{};
   SequencerEditor cursor{&seqr};
   cursor.setEditMode(SequencerEditorMode::configuringSequence);
   if (cursor.getEditMode() == SequencerEditorMode::configuringSequence){
     res = true;
   } 
   return res;
}

bool testSetChannelDirect()
{
   bool res = false;
   // check if we can get into sequence config mode
   Sequencer seqr{};
   SequencerEditor cursor{&seqr};
   // now we should be able to 
   // move up and down to set the channel
   seqr.updateStepData(0, 0, Step::channelInd, 1);
   // verify we have that as our midi channel
   double channel = seqr.getStepData(0, 0)[Step::channelInd]; 
   if (channel == 1) res = true;
   return res;
}

bool testSetChannel()
{
   bool res = false;
   // check if we can get into sequence config mode
   Sequencer seqr{};
   SequencerEditor cursor{&seqr};
   // now we should be able to 
   // move up and down to set the channel
   seqr.updateStepData(0, 0, Step::channelInd, 1);
   // verify we have that as our midi channel
   double channel = seqr.getStepData(0, 0)[Step::channelInd]; 
   if (channel == 1) res = true;
   return res;
}
bool testSetChannelEditor()
{
   bool res = false;
   // check if we can get into sequence config mode
   Sequencer seqr{};
   SequencerEditor cursor{&seqr};
   cursor.cycleEditMode();// go to seq length edit
   cursor.enterAtCursor();// go into sequence config edit 
  
   cursor.enterNoteData(3); // change channel to 3 
   double channel = seqr.getStepData(0, 0)[Step::channelInd]; 
   if (channel == 3) res = true;
   return res;
}


bool testDisplaySeqInfoPage()
{
  bool res = false;
   Sequencer seqr{};
   SequencerEditor cursor{&seqr};
   cursor.setEditMode(SequencerEditorMode::settingSeqLength);
   cursor.enterAtCursor();// go into sequence config edit 
  std::string want = "c:0";
  std::string got = SequencerViewer::toTextDisplay(2, 8, &seqr, &cursor);
  if (want != got) 
  {
    std::cout << "testDisplaySeqInfoPage:: Wanted \n'" << want << "'\n got \n'" << got << "'" << std::endl;
  }
  else {
    res = true;
  }
  return res;     
}


bool testDisplaySeqInfoPageUpdates()
{
  bool res = false;
   Sequencer seqr{};
   SequencerEditor cursor{&seqr};
   cursor.setEditMode(SequencerEditorMode::settingSeqLength);
   cursor.enterAtCursor();// go into sequence config edit 
   cursor.enterNoteData(4);
  std::string want = "c:4";
  std::string got = SequencerViewer::toTextDisplay(2, 8, &seqr, &cursor);
  if (want != got) 
  {
    std::cout << "testDisplaySeqInfoPage:: Wanted \n'" << want << "'\n got \n'" << got << "'" << std::endl;
  }
  else {
    res = true;
  }
  return res;     
}


bool testChannelUp()
{
  bool res = false;
  Sequencer seqr{};
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::settingSeqLength);
  cursor.enterAtCursor();// go into sequence config edit 
  cursor.moveCursorUp();// increase channel to 1
  cursor.moveCursorUp();// increase channel to 2
  double channel = seqr.getStepData(0, 0)[Step::channelInd]; 
  if (channel == 2) res = true;
  return res;
}

bool testTranspose()
{
  bool res = false;
  std::string test{""};
  Sequencer seqr{};
  Sequence seq{&seqr};
  //seq.setStepProcessorTranspose(StepDataTranspose{2});
  seq.setTranspose(2);
  // now we have added a transposer, verify that 
  // the sequences are being transposed
  // by setting a callback that checks the transposed data
  seq.setStepData(1, {0,0,0,0});
  seq.setStepCallback(1, 
    [&test](std::vector<double>* data){
      test = std::to_string((int)data->at(Step::note1Ind));
    });
  seq.tick();  

  if (test == "2") // it went up from 0 to 2 as a result of the transpose
  {
    res = true;
  }
  else {
    std::cout << "Wanted 2 but got " << test << std::endl;
  }
  return res;
}



bool testTransposeReturns()
{
  bool res = false;
  std::string test{""};
  Sequencer seqr{};
  Sequence seq{&seqr};
  //seq.setStepProcessorTranspose(StepDataTranspose{2});
  seq.setTranspose(2);
  // now we have added a transposer, verify that 
  // the sequences are being transposed
  // by setting a callback that checks the transposed data
  seq.setStepData(1, {0,0,0,0});
  seq.setStepCallback(1, 
    [&test](std::vector<double>* data){
      test = std::to_string((int)data->at(Step::note1Ind));
    });
  //seq.setStepProcessorTranspose(StepDataTranspose{1});
  seq.setTranspose(1);
  seq.tick();  

  if (test == "1") // it went up from 0 to 2 as a result of the transpose
  {
    res = true;
  }
  else {
    std::cout << "Wanted 2 but got " << test << std::endl;
  }
  return res;
}

bool testTransposeViaSeq()
{
  bool res = false;
  std::string test{""};
  Sequencer seqr{};
  // setup seq 0 to transpose seq 1
  std::vector<double> data {0, 0, 0, 0};
  data[Step::channelInd] = 1;  // channel 2
  data[Step::note1Ind] = 3; // up by 3
  seqr.setSequenceType(0, SequenceType::transposer);
  seqr.setStepData(0, 1, data);// 
  // now seq 1
  std::vector<double> data2 {0, 0, 0, 0};
  data2[Step::note1Ind] = 10; // should go up to 13
  seqr.setStepData(1, 1, data2);
  seqr.setStepCallback(1, 1, // seq 1, step 1
    [&test](std::vector<double>* data){
      test = std::to_string((int)data->at(Step::note1Ind));
    });
  // calling tick should 
  // cause seq 0 to attach a transposer 
  // to seq 1
  seqr.tick();  

  if (test == "13") // it went up from 0 to 2 as a result of the transpose
  {
    res = true;
  }
  else {
    std::cout << "Wanted 13 but got " << test << std::endl;
  }
  return res;
}

bool testSeqTakesTransposeMode()
{
  Sequencer seqr{};
  Sequence seq{&seqr};
  seqr.setSequenceType(0, SequenceType::transposer);
  return true;
}

bool viewSeqTypeMidi()
{
  bool res = false;
  Sequencer seqr{};
  Sequence seq{&seqr};
  seqr.setSequenceType(0, SequenceType::midiNote);
  std::string want {"c:0 t:midi tps:4"};
  std::string got = SequencerViewer::getSequenceConfigView(seq.getStepData(0)[Step::channelInd], 
                                                           seq.getType(), 
                                                           seq.getTicksPerStep(), 
                                                           SequencerEditorSubMode::editCol1);
  if (got == want) res = true;
  else {
    std::cout << "viewSeqTypeMidi want " << want << " got " << got << std::endl;
  }
  return res;
}


bool testIncStepMidi()
{
  bool res = false; 
  std::vector<double> data {0, 0, 0, 0};
  data[Step::note1Ind] = 60;
  //SequencerEditor::incrementStepData(data, SequenceType::midiNote);
  if (data[Step::note1Ind] == 72) res = true;
  return res;
}
bool testIncStepMidiWrap()
{
  bool res = false; 
  std::vector<double> data {0, 0, 0, 0};
  data[Step::note1Ind] = 120;
  //SequencerEditor::incrementStepData(data, SequenceType::midiNote);
  if (data[Step::note1Ind] == 120) res = true;
  return res;
}

bool testIncStepTrans()
{
  bool res = false; 
  std::vector<double> data {0, 0, 0, 0};
  data[Step::note1Ind] = 5;
  //SequencerEditor::incrementStepData(data, SequenceType::transposer);
  if (data[Step::note1Ind] == 6) res = true;
  return res;
}

bool testIncStepTransWrap()
{
  bool res = false; 
  std::vector<double> data {0, 0, 0, 0};
  data[Step::note1Ind] = 60;
  //SequencerEditor::incrementStepData(data, SequenceType::transposer);
  //std::cout << "data  is "
  if (data[Step::note1Ind] == 61) res = true;
  return res;
}


bool testIncStepTransWrapDown()
{
  bool res = false; 
  std::vector<double> data {0, 0, 0, 0};
  data[Step::note1Ind] = 0;
  //SequencerEditor::decrementStepData(data, SequenceType::transposer);
  if (data[Step::note1Ind] == -1) res = true;
  return res;
}

bool testAdjLen()
{
  bool res = false; 
  Sequencer seqr{};
  //Sequence seq{&seqr};
  seqr.setSequenceType(0, SequenceType::lengthChanger);
  seqr.setSequenceType(1, SequenceType::midiNote);
  std::vector<double> data {0, 0, 0, 0};
  data[Step::channelInd] = 1; // affect seq 1
  data[Step::note1Ind] = 2; // add 2 to the length
  seqr.setStepData(0, 1, data); // put in the length adjsuter
  int before = seqr.howManySteps(1);// length of the midi chan
  seqr.tick();
  int after = seqr.howManySteps(1);
  if (after - before == 2) res =  true;
  std::cout << "went from "<< before <<" to  " << after << std::endl;
  return res; 
}

bool setTicksPerBeat()
{
  bool res = false; 
  Sequencer seqr{};
  Sequence seq{&seqr};
  seq.setTicksPerStep(4);
  std::string test{""};
  seq.setStepCallback(1, 
   [&test](std::vector<double>* data){
      test = "x";
    }
  );
  seq.tick();
  seq.tick();
  seq.tick();
  // test should still be '' not 'xxx'
  std::string want = "";
  if (test == want)
  {
    res = true;
  }
  return res;
}

bool setTicksPerBeatTick()
{
  bool res = false; 
  Sequencer seqr{};
  Sequence seq{&seqr};
  seq.setTicksPerStep(1);
  seq.setType(SequenceType::midiNote);
  std::string test{""};
  seq.setStepData(0, {2, 2, 2, 2});
  seq.setStepCallback(0, 
   [&test](std::vector<double>* data){
      test += "x";
    }
  );
  seq.setStepData(1, {2, 2, 2, 2});
  seq.setStepCallback(1, 
   [&test](std::vector<double>* data){
      test += "x";
    }
  );

  seq.tick();
  seq.tick();
  
  // test should still be '' not 'xxx'
  std::string want = "xx";
  if (test == want)
  {
    res = true;
  }
  return res;
}

bool setTicksPerBeatTwoTick()
{
  bool res = false; 
  Sequencer seqr{};
  Sequence seq{&seqr};
  seq.setType(SequenceType::midiNote);
  std::string test{""};
  seq.setStepData(0, {2, 2, 2, 2});
  seq.setStepCallback(0, 
   [&test](std::vector<double>* data){
      test += "x";
    }
  );
  seq.setStepData(1, {2, 2, 2, 2});
  seq.setStepCallback(1, 
   [&test](std::vector<double>* data){
      test += "x";
    }
  );

  seq.setTicksPerStep(2);
  seq.tick();
  seq.tick();
  
  // should only have triggered one step
  std::string want = "x";
  if (test == want)
  {
    res = true;
  }
  return res;
}

bool testExtraStepsRightCallback()
{
  bool res;
  Sequencer seqr{1, 1};
  std::string test {""};
  seqr.setAllCallbacks([&test](std::vector<double>* data){
    test += "x";
  }); 
// now extend
  seqr.setSequenceLength(0, 2);
  seqr.setStepData(0, 0, {1,1,1,1});// make sure steps will trigger by passing non-zero data
  seqr.setStepData(0, 1, {1,1,1,1});// make sure steps will trigger by passing non-zero data
// now tick it and see if the second step adds an x
  seqr.tick();
  seqr.tick();
  res = assertStrEqual("xx", test);
  return res;
}


bool testTPSSequence()
{
  Sequencer seqr{};
  Sequence seq{&seqr};
  seq.setType(SequenceType::tickChanger);
  return true;
}

bool testTPSSeqChangesOther()
{
  bool res;
  Sequencer seqr{};
  seqr.getSequence(0)->setType(SequenceType::tickChanger);
  std::vector<double>* data = seqr.getStepDataDirect(0, 0);
  // target seq
  data->at(Step::channelInd) =  1;// change seq 1
  data->at(Step::note1Ind) =  10; // set ticks per beat to 2
  // now tick it and verify that seq 1 has tbb == 2
  seqr.tick();
  int tps = seqr.getSequence(1)->getTicksPerStep();
  res = assertNumEqual(tps, 10);  
  return res;
}

bool testTPSTwoBreaks()
{

  bool res{false};
  Sequencer seqr{2, 2};
  seqr.getSequence(0)->setType(SequenceType::tickChanger);

  seqr.setStepData(0, 0, {1, 0, 0, 2}) ; // step 0 change ch 1 speed to 2
  seqr.setStepData(0, 1, {1, 0, 0, 4}) ; // step 1 change ch 1 speed to 4

  // now tick it and verify that seq 1 has tbb == 2
  
  int tps{0};
  for (int i=0;i<4;++i)
  {
    std::cout << "iter " << i << std::endl;
    seqr.tick();
    tps = seqr.getSequence(1)->getTicksPerStep();
    if (!assertNumEqual(tps, 2)) return false;
    seqr.tick();
    tps = seqr.getSequence(1)->getTicksPerStep();
    if (!assertNumEqual(tps, 4)) return false;  
  }
  return true;
}


bool testCursorRightOnNoteEntry()
{
  return false; 
}
/** not really a unit test but lets me check what comes out on WIO*/
bool testWioDisplay2Line()
{
  Sequencer seqr{};
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  std::string want = "1 -Ioooo\n2 --oooo";
  std::string got = SequencerViewer::toTextDisplay(2, 8, &seqr, &cursor);
  std::string port{"/dev/ttyACM0"};
  ofstream serial_bus;
  serial_bus.open (port);
  serial_bus << got << "\t"; // last character triggers the redraw
  serial_bus.close();
  return true;
}

bool testWioDisplay9Line()
{
  Sequencer seqr{10, 16};
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  std::string got = SequencerViewer::toTextDisplay(9, 14, &seqr, &cursor);
  std::string port{"/dev/ttyACM0"};
  ofstream serial_bus;
  serial_bus.open (port);
  serial_bus << got << "\t"; // last character triggers the redraw
  serial_bus.close();
  std::cout << got << std::endl;
  
  return true;
}
bool testWioDisplayLine()
{
  Sequencer seqr{10, 16};
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  std::string got = SequencerViewer::toTextDisplay(9, 14, &seqr, &cursor);
  std::string port{"/dev/ttyACM0"};
  ofstream serial_bus;
  serial_bus.open (port);
  serial_bus << got << "\t"; // last character triggers the redraw
  serial_bus.close();
  std::cout << got << std::endl;
  
  return true;
}

bool testSetDrumSequenceType()
{
  Sequencer seqr{10, 16};
  seqr.setSequenceType(0, SequenceType::drumMidi);
  return true; 
}

bool testSetDrumSequenceTypeEditor()
{
  Sequencer seqr{10, 16};
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::configuringSequence);
  // verify it starts as midiNote
  if (seqr.getSequenceType(0) != SequenceType::midiNote) return false;
  cursor.moveCursorRight();// should move from midi to drum midi
  if (seqr.getSequenceType(0) == SequenceType::drumMidi)
    return true;
  else
    return false;
}

bool testVelEdit()
{
  Sequencer seqr{10, 16};
  SequencerEditor cursor{&seqr};
  cursor.enterAtCursor();
  cursor.cycleEditMode(); // to len
  cursor.cycleEditMode(); // to vel 
  cursor.moveCursorUp(); // vel > 0
  double vel = seqr.getStepData(0, 0)[Step::velInd];
  if (vel > 0)  return true;
  else return false;
}


bool testDrumMap()
{
  std::map<int,char> intToDrum = MidiUtils::getIntToDrumMap();
  if (intToDrum[0] == 'B') return true;
  else return false; 
}

bool testIntToBD()
{
  std::map<int,char> intToDrum = MidiUtils::getIntToDrumMap();
  int ind = intToDrum.size() * 2;
  if (intToDrum[ind % intToDrum.size()] == 'B') return true;
  else return false; 
}

bool testDrumToMidiNote()
{
  std::map<char, int> drumToMidiNote = MidiUtils::getDrumToMidiNoteMap(); 
  if (drumToMidiNote['B'] == 36) return true;
  else return false; 
}

bool testKeyBoardToDrumNote()
{
  // maps qwerty keyboard to midi
  std::map<char, double> key_to_note = MidiUtils::getKeyboardToMidiNotes();
  // need qwerty to drum??
  std::map<char, int> drumToMidiNote = MidiUtils::getDrumToMidiNoteMap(); 
  std::map<int,char> intToDrum = MidiUtils::getIntToDrumMap();
  char keyIn = 'z';// should eventually end up as a kick drim
  // can we get to note 36 from here? 
  double note = key_to_note[keyIn];
  char drum = intToDrum[(int)note % intToDrum.size()];
  if (drumToMidiNote[drum] == 36) return true;
  else return false; 
}
bool testSetStepDataV2()
{
  bool res = false; 
  Sequencer seqr{};
  //Sequence seq{&seqr};
  std::vector<double> dataIn {0, 0, 0, 0, 0};
  dataIn[Step::note1Ind] = 48;
  seqr.setStepData(0, 1, dataIn);// note A/48 maps to bassdrum 36  
  std::vector<double>* data = seqr.getStepDataDirect(0, 1); 
  if (data->at(Step::note1Ind) == 48)
  {
    return true;
  }
  else {
    std::cout << "testSetStepDataV2 data should be 48 but is " << data->at(Step::note1Ind) << std::endl;
    return false; 
  }
}


bool testDrumChannelCorrectNotes()
{
  bool res = false; 
  Sequencer seqr{};
  Sequence* seq = seqr.getSequence(0);
  //Sequence seq{&seqr};
  std::vector<double> dataIn {0, 0, 0, 0, 0};
  dataIn[Step::note1Ind] = 48;
  seqr.setStepData(0, 1, dataIn);// note A/48 maps to bassdrum 36  
  //seq.setType(SequenceType::drumMidi);
  std::string test{""};
  seq->setType(SequenceType::drumMidi);

  std::cout << "setting callback" << std::endl;
  seq->setStepCallback(1, 
   [&test](std::vector<double>* data){
     std::cout << "callback::note:" << data->at(Step::note1Ind) << std::endl;
     if (data->at(Step::note1Ind) == 36)
     {
       test.append("yes");
     }
    }
  );
  seq->tick();
  seq->tick();
  
  // test should still be '' not 'xxx'
  std::string want = "yes";
  if (test == want)
  {
    res = true;
  }
  else
  {
    std::cout << "Want " << want << " got " << test << std::endl; 
  }
  return res;
}

bool testExtendSeq()
{
  bool res = false; 
  Sequencer seqr{1, 2};// seq length = 2
  std::string test{""};
  seqr.setAllCallbacks(
   [&test](std::vector<double>* data){
     std::cout << "seq callback '" << test << "'" << std::endl;
      test.append("yes");
    }
  );
  // now extend
  seqr.setSequenceLength(0, 3); // should create a new step and copy the callback
  // now add data to all steps
  seqr.setStepData(0, 0, std::vector<double> {1, 1, 1, 1});
  seqr.setStepData(0, 1, std::vector<double> {1, 1, 1, 1});
  seqr.setStepData(0, 2, std::vector<double> {1, 1, 1, 1});
  //seqr.getSequence(0)->setTicksPerStep(1);
  seqr.tick();
  seqr.tick();
  seqr.tick();
  
  // test should still be '' not 'xxx'
  std::string want = "yesyesyes";
  if (test == want)
  {
    res = true;
  }
  else
  {
    std::cout << "Want " << want << " got " << test << std::endl; 
  }
  return res;
}

bool testNoteDisplay()
{
  bool res = false;
  Sequencer seqr{1, 4};
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  cursor.moveCursorRight();
  std::vector<double> data {1, 1, 1, 1, 1};
  data[Step::note1Ind] = 48; // that's a c
  seqr.setStepData(0, 0, data);
  seqr.tick();
  std::string want = "0 cI.. ";
  std::string got = SequencerViewer::toTextDisplay(2, 8, &seqr, &cursor);
  if (got == want) res = true; 
  else std::cout << "Want '" << want << "' got '" << got << "'"<< std::endl; 

  return res;
}

bool testDrumDisplay()
{
  bool res = false;
  Sequencer seqr{1, 4};
  SequencerEditor cursor{&seqr};
  cursor.setEditMode(SequencerEditorMode::selectingSeqAndStep);
  cursor.moveCursorRight();
  seqr.setSequenceType(0, SequenceType::drumMidi);
  std::vector<double> data {1, 1, 1, 1, 1};
  data[Step::note1Ind] = 48; // that's a c
  seqr.setStepData(0, 0, data);
  seqr.tick();
  std::string want = "0 BI.. ";
  std::string got = SequencerViewer::toTextDisplay(2, 8, &seqr, &cursor);
  if (got == want) res = true; 
  else std::cout << "Want '" << want << "' got '" << got << "'"<< std::endl; 

  return res;
}


bool testExtendSeqCorrectStepChannel()
{
  bool res = false;
  Sequencer seqr{1, 1};
  SequencerEditor cursor{&seqr};
  seqr.getStepDataDirect(0, 0)->at(Step::channelInd) = 10;
  seqr.setSequenceLength(0, 2);
  // now verify the channel on the step
  int channel1 =  seqr.getStepDataDirect(0, 0)->at(Step::channelInd);
  int channel2 =  seqr.getStepDataDirect(0, 1)->at(Step::channelInd);
  if (channel1 == channel2) res = true;
  else {
    std::cout << "want " << channel1 << " got " << channel2 << std::endl;
  }
  return res;
}

bool testExtendSeqCorrectStepType()
{
  return false; 
}

bool testSongMode()
{
  // can we stop
  // Sequencer seqr{1, 1};
  // seqr.tick();
  // seqr.howManySequences();
  // SequencerEditor cursor{&seqr};
  // cursor.stop();
  // cursor.start();
  std::vector<Sequencer*> seqrs{};
  for (int i=0;i<4;i++)  seqrs.push_back(new Sequencer{2, 2});
  for (Sequencer* s: seqrs)
  {
    s->howManySequences();
  }
  for (Sequencer* s: seqrs)
  {
    s->tick();
    s->howManySequences();
  }

  return true;
}

bool testLengthChanger()
{
  //Sequencer seqr{2, 16};
  Sequencer seqr{2, 10};
  
  seqr.setSequenceType(0, SequenceType::midiNote);
  seqr.setSequenceType(1, SequenceType::lengthChanger);
  std::vector<double> data = {0, 0, 0, 0};
  data[Step::note1Ind] = -4;
  seqr.setStepData(1, 0, data);
  //seqr.setStepData(3, 0, data);
  for (int i=0;i<1000;i++)
  {
    std::cout << i << std::endl;
    seqr.tick();
  }
  return true;
}
bool testLengthChangerDisp()
{
  //Sequencer seqr{2, 16};
  Sequencer seqr{2, 4};
  SequencerEditor ed{&seqr};

  
  seqr.setSequenceType(0, SequenceType::midiNote);
  seqr.setSequenceType(1, SequenceType::lengthChanger);
  std::vector<double> data = {0, 0, 0, 0};
  data[Step::note1Ind] = -4;
  seqr.setStepData(0, 0, data);
  data[Step::note1Ind] = 4;
  seqr.setStepData(1, 0, data);
  
  //seqr.setStepData(3, 0, data);
  for (int i=0;i<1000;i++)
  {
    std::cout << i << std::endl;
    seqr.tick();
    int note =  ((int) seqr.getStepDataDirect(0, 0)->at(Step::note1Ind))
                % 12 ; 
            
    std::string output = SequencerViewer::toTextDisplay(9, 13, &seqr, &ed);

  }
  return true;
}

bool testSetAllChannels()
{
  std::vector<Sequencer*> seqrs{};
  for (int i=0;i<4;i++) seqrs.push_back(new Sequencer{2,8});
  Sequencer* currentSeqr = seqrs[0];
  // seq 0 is channel 4
  currentSeqr->getStepDataDirect(0, 0)->at(Step::channelInd) = 4;
  // seq 0 and 1 
  currentSeqr->setSequenceType(0, SequenceType::tickChanger);
  

  // if here, they pressed a key and they are in 
  for (Sequencer* seqr : seqrs) {
    if (seqr != currentSeqr)
      seqr->copyChannelAndTypeSettings(currentSeqr);
    if (seqr->getSequenceType(0) != SequenceType::tickChanger) return false;
  } 

  return true;
}

bool testChord()
{
  std::vector<double> chord = ChordUtils::getChord(48, 0);
  if (chord[0] == 0) return false;
  return true;
}

int global_pass_count = 0;
int global_fail_count = 0;

void log(std::string test, bool res)
{
  std::string msg;
  if (res)
  {
    msg = " passed.";
    global_pass_count ++;
  } 
  if (!res) 
  {
    msg = " failed.";
    global_fail_count ++;
  }
  std::cout << test << msg << std::endl;
}


int main()
{
//   log("testTick", testTick());
//   log("testTick2", testTick2());
//   log("testUpdate", testUpdate());
//   log("testUpdate2", testUpdate2());
//   log("testToString", testToString());

//   log("testCursorStart", testCursorStart());
//   log("testCursorLeft", testCursorLeft());
//   log("testCursorLeftLimit", testCursorLeftLimit());
//   log("testCursorRight", testCursorRight());
//   log("testCursorRightLimit", testCursorRightLimit());
//   log("testTwoLinesInit", testTwoLinesInit());
//   log("testTwoLines16", testTwoLines16());
//   log("testTwoLines16Tick", testTwoLines16());
//   log("testTwoLinesWrapSeq", testTwoLinesWrapSeq());
//  log("testFollowEditCursorRight", testFollowEditCursorRight());
//  log("testFollowEditCursorDown", testFollowEditCursorDown());
//   log("testFollowEditCursorLeftLimit", testFollowEditCursorLeftLimit());
// log("testFollowEditCursorRightLimit", testFollowEditCursorRightLimit());
// log("testFollowEditCursorRightLimitNearly", testFollowEditCursorRightLimitNearly());

//   log("testNeuralNetwork", testNeuralNetwork());
//    log("testIsStepActiveStep", testIsStepActiveStep());
//    log("testIsStepActiveSeq", testIsStepActiveSeq());
//    log("testIsStepActiveSeqr", testIsStepActiveSeqr());
//   log("testToggleStepActiveStep", testIsStepActiveStep());
//    log("testToggleStepActiveSeq", testIsStepActiveSeq());
//    log("testToggleStepActiveSeqr", testIsStepActiveSeqr());
//   log("testCycleAtCursorTogglesActive", testCycleAtCursorTogglesActive());
//    log("testCycleAtCursorTogglesActiveTwice", testCycleAtCursorTogglesActiveTwice());
//    log("testShowInactiveSteps", testShowInactiveSteps());
//   log("testStepEditModeLen", testStepEditModeLen());
//  log("testStepEditModeVel", testStepEditModeVel());
// log("testStepEditModeVel2", testStepEditModeVel2());

// log("testEnterNoteData", testEnterNoteData());
  // log("testEQAdd", testEQAdd());

  // log("testEQAddAtTs", testEQAddAtTs());
  // log("testEQAddAtTs2", testEQAddAtTs2());


//  log("testEQAddAtTs10", testEQAddAtTs10());
// log("testEQTriggerRemove", testEQTriggerRemove());
// log("testEQTriggerRemove10", testEQTriggerRemove10());
//  log("testSequenceConfigMode", testSequenceConfigMode());
// log("testSequenceConfigModeDirect", testSequenceConfigMode());
//   log("testSetChannelDirect", testSetChannelDirect());
//  log("testSetChannelEditor", testSetChannelEditor());

// log("testDisplaySeqInfoPage", testDisplaySeqInfoPage());
// log("testDisplaySeqInfoPageUpdates", testDisplaySeqInfoPageUpdates());
// log("testChannelUp", testChannelUp());
// //log("testAddPrePro", testAddPrePro());
// log("testTranspose", testTranspose());
// log("testTransposeReturns", testTransposeReturns());
// log("testTransposeViaSeq", testTransposeViaSeq());
// log("testSeqTakesTransposeMode", testSeqTakesTransposeMode());
//log("viewSeqTypeMidi", viewSeqTypeMidi());
// log("testIncStepMidi", testIncStepMidi());
// log("testIncStepMidiWrap", testIncStepMidiWrap());
// log("testIncStepTrans", testIncStepTrans());
// log("testIncStepTransWrap", testIncStepTransWrap());
// log("testIncStepTransWrapDown", testIncStepTransWrapDown());
// log("testAdjLen", testAdjLen());
// log("setTicksPerBeat", setTicksPerBeat());
// log("setTicksPerBeatTick", setTicksPerBeatTick());
// //log("setTicksPerBeatTwpTick", setTicksPerBeatTwpTick());
// log("testExtraStepsRightCallback", testExtraStepsRightCallback());
// log("testTPSSequence", testTPSSequence());
// log("testTPSSeqChangesOther", testTPSSeqChangesOther());
// log("testTPSTwoBreaks", testTPSTwoBreaks());
  
//  log("testWioDisplay1Line", testWioDisplay1Line());
 // log("testWioDispla81Line", testWioDisplay8Line());


  // log("testSetDrumSequenceType", testSetDrumSequenceType());
  // log("testSetDrumSequenceTypeEditor", testSetDrumSequenceTypeEditor());
  // log("testDrumMap", testDrumMap());
  // log("testIntToBD", testIntToBD());
  // log("testDrumToMidiNote", testDrumToMidiNote());

 // log("testKeyBoardToDrumNote", testKeyBoardToDrumNote());
 //log("testDrumChannelCorrectNotes", testDrumChannelCorrectNotes());
 // log("testSetStepDataV2", testSetStepDataV2());
// log("testExtendSeq", testExtendSeq());
//log("testNoteDisplay", testNoteDisplay());
//log("testDrumDisplay", testDrumDisplay());
//log("testExtendSeqCorrectStepChannel", testExtendSeqCorrectStepChannel());
//log("testSongMode", testSongMode());
log("testChord", testChord());
  std::cout << "passed: " << global_pass_count << " \nfailed: " << global_fail_count << std::endl;
}
