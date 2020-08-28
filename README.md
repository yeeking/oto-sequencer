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

## TODO

* probability per step
* ticks per beat per sequence
* pre-processor for ticks per beat
* pre-processor for sequence length
* pre-processor for repeats




