# [Point cloud preparation tool](https://github.com/QuangNguyenLong/pcprep)

The original project has now been relocated to [filrg/pcprep](https://github.com/filrg/pcprep).

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

* Build **pcp**, **pcprep** library:

```shell
mkdir build
cd build
cmake ..
make -j$(nproc)
make install
```

* Build with desktop mode:

```shell
mkdir build
cd build
cmake .. -DDESKTOP_MODE=ON 
make -j$(nproc)
make install
```

* Build with additional examples:

```shell
mkdir build
cd build
cmake .. -DBUILD_EXAMPLES=ON 
make -j$(nproc)
make install
```

* Build with VPCC (for [pcprep.sh](pcprep.sh)):

```shell
mkdir build
cd build
cmake .. -DBUILD_VPCC=ON 
make -j$(nproc)
make install
```

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

## Synthetic Trajectory Generation

To edit and create synthetic user's camera trajectory, a tool has been designed to be used in Unity Engine as a script. More information about how to use the scripts can be found [here](https://github.com/nghiantran03/Camera-Path-Tool).

The scripts can be found in the folder [`trajectory-tool`](trajectory-tool) if the repository was cloned with submodule:

```shell
git clone --recursive https://github.com/QuangNguyenLong/pcprep.git
```

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
