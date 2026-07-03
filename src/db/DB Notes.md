## BPS Notes

### Priority TODO/Goals (V1)
- Create batch
    - create pipeline
- run batch

### Questions
- unique names for pipeline and batches?

# Photo Processor DB

Batch Table:
- Int Primary Key
- String Name
- Long Date Created
- Long Date Modified
- Int Status
- Output dir

Batch Pipeline Map:
- Int Primary key
- Int Foreign key (Batch)
- Foreign Key (pipeline)

Pipeline table
- Int Primary Key
- date created
- date modified
- name

Operation pipeline map: 
- Int Primary key
- int foreign key (Pipeline)
- foreign key (operation)
- position(for order)

Operation:
- Int Primary Key
- name unique
- ...?

(individual image)
File Table:
- Int Primary Key
- Int Foreign Key (source)
- Int Foreign Key (batch)
- string name 
- string path
- long DBCreated (added in DB)
- date DBModified in DB ()?
- standard file info (v2)

# (v2)

Source Dir Table:
- Int Primary Key
- name
- string path
- int numOfFiles
- long DB Created
- long DBModified
- standard file info


Source Batch Map:
- Int Primary Key 
- Int Foreign Key (Dir)
- Int Foreign Key (Batch)






# DB functions:

### Add
- error_type add_batch(name, vector<> pipelines)
- error_type add_pipeline(name, vector pipelines)
- error_type add_batch_pipe_map(bath_key, pipe key)


### Get
- vector\<pkey>\<name> get_all_batches()
- vector\<pkey>\<name> get_all_un-run_batches()
- vector\<pkey>\<name> get_all_pipelines()
- vector\<pkey>\<name> get_all_pipelines_from_batch(batch_key)
- vector\<pkey>\<name> get_all_operations()
- vector\<pkey>\<name> get_all_operations_from_pipeline(pipe_key)
- vector\<pkey>\<name> get_files(batch_key)



# Operations

## Boot
- get_all_batches
- populate app_state
- display home page 


## Batch
### Create Batch
- enter source
- enter name
- Add pipeline
    - choose existing
        - get_pipeline()
        - insert_pipeline_map()
    - create new (v2)
        - add_pipeline()
        - 
- save
    - add_batch()
    - add_batch_pipeline_map()
 


## Pipeline
