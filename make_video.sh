#! /bin/sh

find . -name "outputs/*.png" -print0 | xargs -0 rm
rm ./simulation.cconcat
make
./glauber_dynamics
ffmpeg -i simulation.cconcat -vf fps=25 out.mkv
