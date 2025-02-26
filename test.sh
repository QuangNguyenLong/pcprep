mkdir test

printf "test I/O\n"
./bin/pcp -i longdress0000.ply -o test/IO_test.ply
printf "\n"

printf "test tiling\n"
./bin/pcp -i longdress0000.ply -o test/tile%04d.ply --pre-process=TILE -t 2,2,2
printf "\n"

printf "test sample\n"
./bin/pcp -i longdress0000.ply -o test/half.ply -p sample 0.5 0
printf "\n"

printf "test voxel and remove dupplicates\n"
./bin/pcp -i longdress0000.ply -o test/voxel_clean.ply -p voxel 3 -p remove-duplicates
printf "\n"

printf "test aabb\n"
./bin/pcp -i longdress0000.ply -o test/dummy.ply -s aabb 1 0 test/bbox%04d.ply
printf "\n"

printf "test pixel-per-tile\n"
./bin/pcp -i longdress0000.ply -o test/dummy.ply -s pixel-per-tile examples/cam2mat/cam-matrix.json 2,2,2 test/visi.json
printf "\n"

printf "test save-viewport\n"
./bin/pcp -i longdress0000.ply -o test/dummy.ply -s save-viewport examples/cam2mat/cam-matrix.json 255,255,255 test/view%04d.tile%04d.png
printf "\n"

printf "test screen-area-estimation\n"
./bin/pcp -i test/tile%04d.ply -o test/dummy.ply --tiled-input=8 -s screen-area-estimation examples/cam2mat/cam-matrix.json test/screen-area-tile%04d.json
printf "\n"
