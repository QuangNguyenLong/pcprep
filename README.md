# Point cloud preparation tool

## Dependency
The tool is built using Cmake >= 3.0 and GNU make. 

### Linux user
* OpenGL Mathematics (GLM):
<br/>`sudo apt-get install -y libglm-dev`<br/>

## Compilation

In project's root directory:
```shell
mkdir build
cd build
cmake ../
make
```

All programs are placed in `bin`.   

### Examples
## Tiling

This program takes input a point cloud, currently supporting Polygon File Format (PLY), the numbers of division per axis, and output tiles by deviding the point cloud bounding box.

<br/>`tiling <input_file_path> <n_x> <n_y> <n_z> <is_binary> <output_folder>`<br/>

For example:
```shell
tiling longdress0000.ply 2 2 2 1 test
```
## Subsampling

This program takes input a point cloud, currently supporting Polygon File Format (PLY), subsampling percentage (from 0 to 1) and output subsampled point cloud.

<br/>`subsampling <input_file_path> <subsample_percentage> <output_file_path>`<br/>

For example:
```shell
subsampling longdress0000.ply 0.5 ouput.ply
```

## Screen area

This program is currently still in developing. This function print the screen proportion (estimated by the bounding box) of the 8 tiles given by upper "tilling" example command.

<br/>`screen_area`<br/>

For example:
```shell
screen_area
```