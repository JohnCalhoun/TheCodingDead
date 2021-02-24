#! /bin/bash
set -x

cd runner
make 
cd ..

./bin/run --graph-def-dir ./data/ --output-dir ./output/ --iterations 10

./bin/process_output --data-dir ./runner/output/

