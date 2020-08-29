#include <iostream>
#include "../lib/ml/rapidLib.h"
#include "Sequencer.h"
#include "SequencerUtils.h"
#include "MidiUtils.h"
#include "RapidLibUtils.h"
#include "EventQueue.h"

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
  
  // now ts 0 should have one event
  int sizeBefore =  q.getEventsAtTimestamp(0).size();
  q.triggerAndClearEventsAtTimestamp(0);
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
    [&test](std::vector<double> data){
      test = std::to_string((int)data[Step::note1Ind]);
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
    [&test](std::vector<double> data){
      test = std::to_string((int)data[Step::note1Ind]);
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
    [&test](std::vector<double> data){
      test = std::to_string((int)data[Step::note1Ind]);
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
  std::string want {"c:0 t:midi"};
  std::string got = SequencerViewer::getSequenceConfigView(seq.getStepData(0)[Step::channelInd], 
                                                           seq.getType());
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
  SequencerEditor::incrementStepData(data, SequenceType::midiNote);
  if (data[Step::note1Ind] == 72) res = true;
  return res;
}
bool testIncStepMidiWrap()
{
  bool res = false; 
  std::vector<double> data {0, 0, 0, 0};
  data[Step::note1Ind] = 120;
  SequencerEditor::incrementStepData(data, SequenceType::midiNote);
  if (data[Step::note1Ind] == 120) res = true;
  return res;
}

bool testIncStepTrans()
{
  bool res = false; 
  std::vector<double> data {0, 0, 0, 0};
  data[Step::note1Ind] = 5;
  SequencerEditor::incrementStepData(data, SequenceType::transposer);
  if (data[Step::note1Ind] == 6) res = true;
  return res;
}

bool testIncStepTransWrap()
{
  bool res = false; 
  std::vector<double> data {0, 0, 0, 0};
  data[Step::note1Ind] = 60;
  SequencerEditor::incrementStepData(data, SequenceType::transposer);
  //std::cout << "data  is "
  if (data[Step::note1Ind] == 61) res = true;
  return res;
}


bool testIncStepTransWrapDown()
{
  bool res = false; 
  std::vector<double> data {0, 0, 0, 0};
  data[Step::note1Ind] = 0;
  SequencerEditor::decrementStepData(data, SequenceType::transposer);
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
  //  log("testIsStepActiveStep", testIsStepActiveStep());
  //  log("testIsStepActiveSeq", testIsStepActiveSeq());
  //  log("testIsStepActiveSeqr", testIsStepActiveSeqr());
  // log("testToggleStepActiveStep", testIsStepActiveStep());
  //  log("testToggleStepActiveSeq", testIsStepActiveSeq());
  //  log("testToggleStepActiveSeqr", testIsStepActiveSeqr());
  // log("testCycleAtCursorTogglesActive", testCycleAtCursorTogglesActive());
   //log("testCycleAtCursorTogglesActiveTwice", testCycleAtCursorTogglesActiveTwice());
   //log("testShowInactiveSteps", testShowInactiveSteps());
  //log("testStepEditModeLen", testStepEditModeLen());
//  log("testStepEditModeVel", testStepEditModeVel());
//log("testStepEditModeVel2", testStepEditModeVel2());

//log("testEnterNoteData", testEnterNoteData());
  // log("testEQAdd", testEQAdd());

  // log("testEQAddAtTs", testEQAddAtTs());
  // log("testEQAddAtTs2", testEQAddAtTs2());
  // log("testEQAddAtTs10", testEQAddAtTs10());
//log("testEQTriggerRemove", testEQTriggerRemove());
//log("testEQTriggerRemove10", testEQTriggerRemove10());
//  log("testSequenceConfigMode", testSequenceConfigMode());
//log("testSequenceConfigModeDirect", testSequenceConfigMode());
  //log("testSetChannelDirect", testSetChannelDirect());
//  log("testSetChannelEditor", testSetChannelEditor());

//log("testDisplaySeqInfoPage", testDisplaySeqInfoPage());
//log("testDisplaySeqInfoPageUpdates", testDisplaySeqInfoPageUpdates());
//log("testChannelUp", testChannelUp());
//log("testAddPrePro", testAddPrePro());
//log("testTranspose", testTranspose());
//log("testTransposeReturns", testTransposeReturns());
//log("testTransposeViaSeq", testTransposeViaSeq());
// log("testSeqTakesTransposeMode", testSeqTakesTransposeMode());
//log("viewSeqTypeMidi", viewSeqTypeMidi());
//log("testIncStepMidi", testIncStepMidi());
//log("testIncStepMidiWrap", testIncStepMidiWrap());
log("testIncStepTrans", testIncStepTrans());
log("testIncStepTransWrap", testIncStepTransWrap());
log("testIncStepTransWrapDown", testIncStepTransWrapDown());
log("testAdjLen", testAdjLen());
  std::cout << "passed: " << global_pass_count << " \nfailed: " << global_fail_count << std::endl;
}
