# Point cloud preparation tool

## Requirements
The following packages are required to build the Point cloud preparation tool:
* cmake >= 3.0
* GNU Make
* gcc >= 9.4.0
* g++ >= 9.4.0

To build the tool with all features enabled, the following packages are required:
* libglfw3-dev 
* libglew-dev 
* libpng-dev 
* libgl-dev 

Additional requirements for example binaries:
* libglm-dev >= 0.9.9.7

## Building
Bash scripts can be use to build pcprep project:

* scripts/build.sh: build solutions.

### Linux
* Build all solutions:
```shell
./scripts/build.sh --all
```

* Build only the **pcp** app with basic features:
```shell
./scripts/build.sh
```

* Build the additional example binaries:
```shell
./scripts/build.sh --with-examples
```

All programs are placed in `bin`.   


## Point Cloud Prepare (`pcp`)

```shell
./bin/pcp \
    --process=sample 0.5 0 \
    --tile=2,2,2 \
    --binary=0 \
    --input=longdress0000.ply \
    --output=tile%04d.ply
```

More infomations could be found in the [user manual](doc/README.usage.md).

## Examples
### Tiling

This program takes input a point cloud, currently supporting Polygon File Format (PLY), the numbers of division per axis, and output tiles by deviding the point cloud bounding box.

<br/>`tiling <input_file_path> <n_x> <n_y> <n_z> <is_binary> <output_folder>`<br/>

For example:
```shell
tiling longdress0000.ply 2 2 2 1 test
```
### Subsampling

This program takes input a point cloud, currently supporting Polygon File Format (PLY), subsampling percentage (from 0 to 1) and output subsampled point cloud.

<br/>`subsampling <input_file_path> <subsample_percentage> <output_file_path>`<br/>

For example:
```shell
subsampling longdress0000.ply 0.5 ouput.ply
```

### Camera to matrix

This program takes input a JSON file which specified camera parameters, and output the MVP matrixes in a JSON file.
<br/>`cam2mat <input-cam-param.json> <output-cam-matrix.json>`<br/>

For example:
```shell
cam2mat cam-param.json cam-matrix.json
```

### Screen area

This program is currently in development. This program print the screen proportion (estimated by the bounding box) of the 8 tiles given by upper "tilling" example command.

<br/>`screen_area`<br/>

For example:
```shell
screen_area
```