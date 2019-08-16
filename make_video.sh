./glauber_dynamics | ffmpeg -i pipe: -vf fps=100 -y -f nut  pipe: -y -f nut out.nut| mpv -
