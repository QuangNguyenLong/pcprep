# Point Cloud Prepare (PCP)

The **Point Cloud Prepare (PCP)** is a command-line tool designed for preparing, processing, and managing point cloud data. This document provides a guide on how to use the tool effectively.

---

## Usage

```bash
./bin/pcp [OPTION...] -i <point-cloud-source-file> -o <output-source-file(s)>
```

### Description

The `pcp` program processes point cloud data from a source file and generates one or more output files based on specified options. It supports customizable processing steps, status calculations, and tiling configurations.

---

## Options

### General Options
#### `-b, --binary=0|1`  
  Specifies the output format:  
  - `0`: Output in ASCII format.  
  - `1` (default): Output in binary format.

#### `-i, --input=FILE`  
  Specifies the input point cloud source file.  
  - Supported formats: Polygon File Format (`.ply`).

#### `-o, --output=FILE`  
  Specifies the output file(s). 
  Example: `tiles%04d.ply` is the output path for multiple output files. 

#### `-?, --help`  
  Displays the help message.

#### `--usage`  
  Displays a short usage message.

#### `-V, --version`  
  Prints the program version.

---

### Tiling Option
#### `-t, --tile=nx,ny,nz`  
  Divides the point cloud into a grid of tiles along each axis, the **tile** option is performed first, before **process** and **status** option.  
  - `nx, ny, nz`: Number of divisions along the x, y, and z axes.  
  - Example: `2,2,2`.

---

### Process Option
#### `-p, --process=PROCESS [<ARG>...]`  
  Defines a specific process to be applied to the point cloud.  
  - `PROCESS`: An string identifier of the process.  
  - `<ARG>,...`: Arguments for the process.  
  Example: `--process=sample 0.5 0`, `-p sample 0.5 0`

#### Sample process
##### `sample <ratio> <binary>`
 - `ratio=FLOAT`
  Specifies the sample ratio compare to the input point cloud.
- `binary=0|1`
  Strategy for sampling.
  | Value | Description                 |
  |:-----:| ----------------------------|
  | 0     | Uniform (default)           |
  | 1     | Still working ...           |

#### Voxel process
##### `voxel <voxel-size>`
- `voxel-size=FLOAT`
  Specifies the step size to voxel the input point cloud.

#### Remove duplicates process
##### `remove-dupplicates`
--- 

### Status Option
#### `-s, --status=STATUS [<ARG>...]`  
  Calculates the status of the point cloud based on the given factors.  
  - `STATUS`: An string identifier of the status calculation method.  
  - `<ARG>,...`: Arguments for the calculation.  
  Example: `--status=aabb 1 0 bbox%04d.ply`,`-s aabb 1 0 bbox%04d.ply`

#### AABB status
##### `aabb <output> <binary> <output-path>`
- `output=0|1|2`
  Specifies how to get the bounding box:
  - `0` (default): Print on terminal.  
  - `1`: Save to file(s) (Supported formats: `.ply`).
  - `2`: Both of the above.
- `binary=0|1`
  Specifies the output format:  
  - `0`: Output in ASCII format.  
  - `1` (default): Output in binary format.
- `output-path=FILE`
  Specifies the output file(s). 
  Example: `bbox%04d.ply` is the output path for multiple output files. 

---



## Examples

1. **Basic Usage**  
   Import a point cloud and save it to an output file:  
   ```bash
   ./bin/pcp -i input.ply -o output.ply
   ```

2. **Output in Non-Binary Format**  
   Disable binary output for the result:  
   ```bash
   ./bin/pcp -b 0 -i input.ply -o output.ply
   ```

3. **Apply a Processing Step**  
   Sample a point cloud with halves of the points using uniform rule:  
   ```bash
   ./bin/pcp -p 0,0.5,0 -i input.ply -o output.ply
   ```

4. **Tile the Point Cloud**  
   Divide the point cloud into 2x2x2 tiles:  
   ```bash
   ./bin/pcp -i input.ply -o tiles%04d.ply -t 2,2,2
   ```

5. **Combine**
   Tile the point cloud and sample the tiles.
   ```bash
   ./bin/pcp \
       --process=sample 0.5 0 \
       --tile=2,2,2 \
       --binary=0 \
       --input=longdress0000.ply \
       --output=tile%04d.ply
   ```
5. **Sequencing**
   Tile the point cloud and voxel then sample the tiles.
   ```bash
   ./bin/pcp \
       --process=voxel 3 \
       --process=sample 0.5 0 \
       --tile=2,2,2 \
       --binary=0 \
       --input=longdress0000.ply \
       --output=tile%04d.ply
   ```