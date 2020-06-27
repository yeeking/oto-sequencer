#include "Sequencer.h"
#include <iostream>
#include "RtMidiUtils.h"
#include "RapidLibUtils.h"
#include "../lib/ml/rapidLib.h"

bool testTick()
{
  Sequencer seq{};
  seq.tick();
  seq.tick();
  return true;
}

bool testUpdate()
{
  Sequence seq{};
  seq.setStepData(0, std::vector<double>{0.1f});
  std::vector<double> data = seq.getStepData(0);
  if (data[0] == 0.1f) return true;
  return false;
}

bool testUpdate2()
{
  Sequence seq{};
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
  Sequence seq{};
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
  Sequence seq{};
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
  
  std::cout << editor.getCurrentSequence() << ":" << editor.getCurrentStep() << " : " << SequencerViewer::getStepView(data);
  if (data[1] == 2) return true;
  return false;
}


void log(std::string test, bool res)
{
  std::string msg;
  if (res) msg = " passed.";
  if (!res) msg = " failed.";
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
log("testStepEditModeVel2", testStepEditModeVel2());



}
