# Light_Field_Codec

## Clone and build
To build this repository, you should have cmake 3.15 installed in your system.

First, clone the repo and its modules dependencies.

```
$ mkdir repo
$ git clone https://github.com/IDSRosler/Light-Field-Codec.git repo
$ cd repo
$ git submodule update
```

Then, make a directory to build the project binaries

```
$ mkdir build
$ cd build
$ cmake ..
$ make 
```

From the `build` directory, run the binary `./lfcodec`.

### Running tests

To execute the tests, run the binary `build/tst/lfcodec-test`.

## Encoder Parameters (CLI)

**Use:**  -**flag** *value*

**Example:** ./bin.exe -input /temp/

### Directories
- **input** *string*: \*.ppm dir to be encoded
- **output** *string*:  output dir for the encoded \*.ppm 

### Base block dimensions
- **blx** *int*: block size in x dimension
- **bly** *int*: block size in y dimension
- **blu** *int*: block size in u dimension
- **blv** *int*: block size in v dimension

### Quantization Parameters
- **qp** *float*: linear quantization parameter for the quantization 4d-volum
- **qx** *int*:  quantization weight in x dimension
- **qy** *int*:  quantization weight in y dimension
- **qu** *int*:  quantization weight in u dimension
- **qv** *int*:  quantization weight in v dimension

### Light Field dimensions
- **lfx** *int*: light field size in x dimension
- **lfy** *int*: light field size in y dimension
- **lfu** *int*: light field size in u dimension
- **lfv** *int*: light field size in v dimension 


### Scripts 
- Scripts to run the Light Field Codec for different **QPs** and different dataset
##### Run simulation.py:
1. In the simulation.py, change the input_file variable and the output_file variable
    - **input_file:** Directory with .ppm files to be encoded
    - **output_file:** Output directory for the encoded *.ppm
2. Execute
    - taskset -c 0 python3 simulation.py


