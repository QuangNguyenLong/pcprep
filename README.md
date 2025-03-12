# Point cloud preparation tool

## Requirements
The following packages are required to build the Point cloud preparation tool:
* cmake >= 3.0
* GNU Make
* gcc
* g++
* libpng-dev 

For Ubuntu, you can run the following:
  ```shell
  sudo apt-get install cmake make gcc g++ libpng-dev
  
  # (OPTIONAL) to build the tool with desktop mode (GPU support)
  # install the following additional dependencies 
  sudo apt-get install libglfw3-dev libglew-dev libgl-dev

  # (OPTIONAL) dependencies for example binaries 
  sudo apt-get install libglm-dev
```

## Building

Bash scripts can be used to build the pcprep project:

  ```shell
  chmod +x scripts/build.sh
  ./scripts/build.sh
  ```

### Linux (Tested on Ubuntu Desktop & Server)

* Build only the **pcp** app:
  ```shell
  ./scripts/build.sh
  ```

* Build with all optional libraries enabled:
  ```shell
  ./scripts/build.sh --all
  ```

* Build the **pcp** app with desktop mode (GL, glfw, GLEW enabled):
  ```shell
  ./scripts/build.sh --desktop
  ```

* Build the additional example binaries:
  ```shell
  ./scripts/build.sh --examples
  ```

All programs are placed in the `bin` directory.

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

## Compressed Point Cloud Tiles (`fil-CPCTv1`) Dataset  

The [`pcprep.sh`](pcprep.sh) script was used to generate a dataset of V-PCC-compressed point cloud tiles, derived from the [8i-VFBv2](http://plenodb.jpeg.org/pc/8ilabs) point cloud sequences. This dataset is publicly available [here](https://drive.google.com/drive/folders/1qQp4go6xP--utvJNhyrMi8RexRt6J5Dh).

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
