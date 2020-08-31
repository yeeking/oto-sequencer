# Oto-sequencer

A sound sequencer. Runs on raspberry pi. 

To build, first set up the cmake stuff:
```
   mkdir build
   cd build
   cmake ..
```

On raspberry pi (using grove sensors):
```
   make oto-sequencer-pi
   ./oto-sequencer-pi
```

On normal linux or raspi linux without grove sensors:

```
  make oto-sequencer
  ./oto-sequencer
```
## Keys

In all modes:

* q: quit 
* -: go slower
* =: go faster

In step overview mode:

* Cursor keys:  move the cursor to the desired step
* Computer keyboard piano keys: set note for step under cursor 
* Enter: go into single step edit mode
* Tab: go to sequence overview mode

In step edit mode:
* Cursor left and right: select field to edit
* Computer keyboard piano keys: set note for step under cursor 
* Cursor up and down: set fieid's value
* Space: toggle active/ inactive step
* Enter: exit to step overview mode
 
In sequence overview mode:
* Cursor left and right: change sequence length
* Cursor up and down: select sequence
* Enter: go into sequence config mode

In sequencer config mode:
* Cursor left and right: set sequence type
* Cursor up and down: set sequence channel


## Using the sequencer

Here follows a description of how to use the sequencer with screenshots of the various modes.

### Select midi input

When it starts you will see a list of available midi outs:

```
    Output port #0: Midi Through:Midi Through Port-0 14:0
    Output port #1: Pure Data:Pure Data Midi-In 1 128:0

  Choose a port number: 
```
If you wanted Pure Data in that list, you would press '1'. 

### Note off on all channels

Next it will send note offs for all notes on all channels, in case you have any stuck notes. 

### Step editor view

Now you will see the step editor view:

```
    1 -I...............             0
    2 --...............             
    3 --...............             
    4 --...............  
```

Put a note in using the computer keyboard as a mini piano, mapped like this:

```
{ 'z', 60},
{ 's', 61},
{ 'x', 62},
{ 'd', 63},
{ 'c', 64},
{ 'f', 65},
{ 'v', 66},
{ 'b', 67},
{ 'h', 68},
{ 'n', 69},
{ 'j', 70},
{ 'm', 71}
```

Press return and you can edit the note, length and velocity. 

```
O n:0 [l:0] v:0 
```

n: note 0-127, l: length in steps, v: velocity 0-127

The 'O' at the start indicates that this step is active. Press space to toggle that. 

Press return to exit the note edit mode and back out to steps overview. 

### Sequencer edit mode

Press TAB when in steps overview to go into sequencer view. Here you can change the length and type of sequences. 

```
1 -I>>>>>>>>>>>>>>>             0
2 -->>>>>>>>>>>>>>>             
3 -->>>>>>>>>>>>>>>             
4 -->>>>>>>>>>>>>>>   
```
Use up and down to move between sequences. Use left and right to change sequence length. 

Press return to enter sequencer config screen. 

```
c:0 t:midi
```
c: channel, t: type

c:0 t:midi means send notes on MIDI channel 1. 

Change the channel with the up and down keys. Change the type with left and right. 
Other types:
* transpose: transpose the sequence on channel 'c' by this number of steps
* length: change the length of the sequence on channel 'c' by the step's amount

transpose and length changes are reset by the target sequence each time it hits its first step.


## TODO

* probability per step
* chord type sequences
* ticks per beat per sequence
* pre-processor for ticks per beat
* pre-processor for repeats
* change Sequence::trigger functions to use data pointers


