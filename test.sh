printf "test I/O\n"
./bin/pcp -i longdress0000.ply -o test/IO_test.ply --pre-process=NONE
printf "\n"

printf "test tiling\n"
./bin/pcp -i longdress0000.ply -o test/tile%04d.ply -t 2,2,2
printf "\n"

printf "test sample\n"
./bin/pcp -i longdress0000.ply -o test/half.ply --pre-process=NONE -p sample 0.5 0
printf "\n"

printf "test voxel\n"
./bin/pcp -i longdress0000.ply -o test/voxel_step_3.ply --pre-process=NONE -p voxel 3
printf "\n"

printf "test aabb\n"
./bin/pcp -i longdress0000.ply -o test/dummy.ply --pre-process=NONE -s aabb 1 0 test/bbox%04d.ply 
printf "\n"

printf "test pixel-per-tile\n"
./bin/pcp -i longdress0000.ply -o test/dummy.ply --pre-process=NONE -s pixel-per-tile examples/cam2mat/cam-matrix.json 2,2,2 test/visi.json
printf "\n"

printf "test save-viewport\n"
./bin/pcp -i longdress0000.ply -o test/dummy.ply --pre-process=NONE -s save-viewport examples/cam2mat/cam-matrix.json 255,255,255 test/tile%04d.view%04d.png
printf "\n"

printf "test screen-area-estimation\n"
./bin/pcp -i longdress0000.ply -o test/dummy.ply --pre-process=NONE -s screen-area-estimation examples/cam2mat/cam-matrix.json blabla
printf "\n"
