#!/bin/bash

set -ex

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "$SCRIPT_DIR"

mkdir -p build

if [ -z "$DEBUG" ] ; then
  CPP_ARGS=(-O2 -std=c++17 -Isrc -lglut -lGLU -lGL -lglfw -lGLEW)
else
  echo "Running in debug mode!"
  CPP_ARGS=(-g -std=c++17 -Isrc -lglut -lGLU -lGL -lglfw -lGLEW)
fi

rm build/*.o

g++ -c -o build/ObjModel.o src/ObjModel.cpp "${CPP_ARGS[@]}" &
g++ -c -o build/BillBoard.o src/BillBoard.cpp "${CPP_ARGS[@]}" &
g++ -c -o build/TrafficLight.o src/TrafficLight.cpp "${CPP_ARGS[@]}" &
g++ -c -o build/utility.o src/utility.cpp "${CPP_ARGS[@]}" &
g++ -c -o build/PPMImage.o src/PPMImage.cpp "${CPP_ARGS[@]}" &

wait

g++ -o build/main src/main.cpp build/ObjModel.o build/BillBoard.o build/TrafficLight.o build/utility.o build/PPMImage.o "${CPP_ARGS[@]}"

if [ -z "$DEBUG" ] ; then
  ./build/main $1 $2 $3 $4 $5 $6
else
  gdb -batch -ex "run" -ex "bt" --args ./build/main $1 $2 $3 $4 $5 $6
fi

# f3d src/Models/TrafficLight.obj

