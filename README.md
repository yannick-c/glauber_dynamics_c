This library is fully documented using doxygen which can be generated using

        doxygen Doxyfile

and then observed by opening `documentation/html/index.html`. This README
contains the necessary information to get the program running (only tested on
Linux).

1. Make sure you have `glib-2.0`, `graphviz` and `pkg-config` installed and
   their respective C libraries. A quick check for this is that the command
   ```
        pkg-config --libs libgvc glib-2.0
   ```
   should not fail. Furthermore I expect you to have a standard POSIX system
   with all the standard libraries.
2. Download the PCG Random number generators
   [C implementation](http://www.pcg-random.org/download.html)
   and extract it into the lib folder such that `pcg-c` is a subdirectory of
   `lib` (Note: Do not download the minimal version).
3. Run `./configure && make` in the `pcg-c` folder.
4. Run `./configure && make` in the main directory of this repository. If this
   fails make sure you have `autotools` installed.

There should be a runnable `glauber_dynamics` in the main directory of this
library now. To see how to run it run

```
     ./glauber_dynamics --help
```

As a quick reference the three main commands are
If you wish to run the simulation and save the final state of the
configuration in a png (`final.png`), then run

```
     ./glauber_dynamics -q
```

To have a comparison between the state after 10 iterations and the end of
the simulation add the `-i` flag

```
     ./glauber_dynamics -qi
```

To save the video file of the configuration evolution using `ffmpeg` for
example use that `./glauber_dynamics` streams `png` files to `stdout` and
hence you can pipe the output to ffmpeg directly and save it for example in
the nut format as `out.nut` by writing

```
     ./glauber_dynamics | ffmpeg -i pipe: -vf fps=100 -y -f nut out.nut
```

If you additionally want to concurrently stream the evolution while
simulating specify an additional output to `ffmpeg` which pipes its rendered
video to `stdout` and can thus be piped into any regular video player, here
`mpv`, as
     
```
     ./glauber_dynamics | ffmpeg -i pipe: -vf fps=100 -y -f nut  pipe: -y -f nut out.nut| mpv -
```

Adapt the `fps` (frames per second) to your need. If you increase the frames
per second you will have a shorter video but streaming it will be laggy as
more frames have to be calculated for a second of output.
