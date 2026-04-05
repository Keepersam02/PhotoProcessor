# General Program Scope

PhotoProcessor is to be a batch focused image editor designed to efficiently ingest and process hundreds or thousands of images at once.  As such, PhotoProcessor will be a simple to use program with a strong focus on performance.

## Motivations
Main stream photo editors treat the user as if they know nothing about image pipelines and do not allow them to get the control back.  Video coloring softwares do allow full control, but do not support most raw images.  

## Functionality Stages
### Version 1
General image IO 
* need to verify images can be handled before trying to import 
* take verified images and bring them into memory
-   single threaded intially 
-     need to 
