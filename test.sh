#! /bin/bash
set -xe

cd runner
make 
cd ..

rm -rf ./output
mkdir output
echo "running simulation"
time ./bin/run --graph-def-dir ./data/ --output-dir ./output/ --iterations 100

echo "combining output"
./bin/process_output --data-dir ./output

