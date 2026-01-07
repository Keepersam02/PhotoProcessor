# PhotoProcessor Roadmap

## Initial Operational Capacity

### Image IO
- image classification
  - determine if image is tiff, png, or raw
  - reject non raw images add non raw support later
- process images 
  - use libraw to debayer
  - change exposure
  - export to TIFF 

### TUI input
- create task queue
  - tasks can be any process we will eventually add
  - initial task will be image processing
- task
  - a set of defined processes
    - should be able to run on its own thread
  - we will start with a set of image processes 
    - image input
      - raw images
  - exposure adjustment
  - image writing
    - tiff out 



# Implementation Details
## Image IO 
Classifying images 
- only want raw images initially
  - if the image is not a TIFF format feed it to libraw and see if it can read it 
  - if it is check for CFA TIFF tags having anything meaningful
    - this should suffice for an initial simple Implementation

Process images
- only simple exposure adjustment to start 
  - ensures as a check the pipeline works

Output images
- start with simple TIFF output

## TUI
Goal is to be simple yet strong
  - program will start from CLI and as much as possible will be done from there
    - this will hopefully keep the program from being misused
      - this is not meant to edit individual photos but serve as a ingest tool 
GUI goals
- GUI will temporarily pop up to aid in more annoying tasks to do with a CLI and for non power users 
- create image processing pipelines
  - begin with a layer based pipeline for simplicity
  - preview on a stock image
- open file explorer should a user prompt for it for folder and file pathing
Process 
- need a queue of tasks 
  - users can add multiple tasks to the queue 
  - queue needs to continue if one task fails and cannot be finished
    - no jamming because one task is slow
    - report skipped task to user and why
    - write failures to log and task report file 
- app will handle task schedule ordering
  - can be user changed
  - default to mission critical data safety tasks first
