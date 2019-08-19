/** \file */
/** \mainpage Glauber Dynamics simulator in C
 * 
 * \section Introduction
 *
 * This is a library which serves to simulate evolutions of graph
 * configurations using Poisson Point process clocks which are put on every
 * vertex. 
 *
 * \subsection installation Prerequisites and Installation
 *
 * To use this library you will need `make`, `pkg-config`, `glib-2.0`, `graphviz`
 * (such that the C `pkg-config --libs pkg glib-2.0 libgvc`). The commands in
 * this section further use `ffmpeg` to convert frames to video and save the
 * video/stream it to a video player (here `mpv`). This library has been used
 * and tested on GNU/Linux based systems so no assurance is given that
 * everything works on other systems.
 *
 * Change directories into the download location of this library. Download the
 * [the C library of the PCG Pseudo Random Number Generator
 * family](http://www.pcg-random.org/), unpack it into the lib subdirectory
 * (resulting in a `lib/pcg-c` directory) and run `make` in it. Alternatively,
 * change `PCG_DIR` in `Makefile.am` if you have already compiled this random
 * number generator somewhere else. In this case you have to run `autoreconf
 * -i` again which might install some extra files but ensures that your new
 *  your change to `Makefile.am` is respected in the further process.
 *
 * Finally run 
 *
 *      ./configure && make
 *
 * to compile this library. You can also run `sudo make install` to install
 * the library and be able to execute the file from anywhere without having
 * to specify the path to this library.
 * 
 * \subsection usage Usage
 *
 * If you wish to run the simulation and save the final state of the
 * configuration in a png (`final.png`), then run
 * 
 *      ./glauber_dynamics -q.
 *
 * To have a comparison between the state after 10 iterations and the end of
 * the simulation add the `-i` flag
 *
 *      ./glauber_dynamics -qi.
 *
 * To save the video file of the configuration evolution using `ffmpeg` for
 * example use that `./glauber_dynamics` streams `png` files to `stdout` and
 * hence you can pipe the output to ffmpeg directly and save it for example in
 * the nut format as `out.nut` by writing
 *
 *      ./glauber_dynamics | ffmpeg -i pipe: -vf fps=100 -y -f nut out.nut
 *
 * If you additionally want to concurrently stream the evolution while
 * simulating specify an additional output to `ffmpeg` which pipes its rendered
 * video to `stdout` and can thus be piped into any regular video player, here
 * `mpv`, as
 *      
 *      ./glauber_dynamics | ffmpeg -i pipe: -vf fps=100 -y -f nut  pipe: -y -f nut out.nut| mpv -
 *
 * Adapt the `fps` (frames per second) to your need. If you increase the frames
 * per second you will have a shorter video but streaming it will be laggy as
 * more frames have to be calculated for a second of output.
 *
 * For detailed info on what options are available and the inline command-line
 * options available to change the output run
 *
 *      ./glauber_dynamics --help
 *
 * \subsection conribute Adapt code to personal use case and contributing
 *
 * This code was written with the possibilty for future extension in mind. To
 * use different update rules or write your own consult \ref updaterules about
 * details. Extending the available graphs is currently not as straightforward
 * but not impossible. All graph related stuff has been isolated into the
 * `weightegraph` lib (cf. \ref weightedgraph).
 *
 * Contributions are always welcome but it is important that tests are added
 * using the glib testing harness (for examples see \ref testing) and the
 * entries in `Makefile.am` should be changed accordingly so that `make check`
 * includes the new tests after the usual `autoreconf`. All code here should be
 * in `C11`. If all these conditions are met a pull request is more than welcome.
 **/
