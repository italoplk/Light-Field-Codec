# LF_Codec (ICIP 2020)
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
