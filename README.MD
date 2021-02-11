![gridgame](gridgame.png)

# GridGame

A game for the [Commander X-16](https://www.commanderx16.com).

A 'C' implementation of a Flash game I used to play a lot of back in the
days of Flash games.

Unfortunately, I don't know the original author's name, the closest I
can find is a URL pointing to a now dead page:

http://www.columbia.edu/~chc18/gridgame.swf

If *chc18* was the original author, and if anyone knows *chc18*, or
whomever the original author of the SWF version was, and can get us in
contact with each other, I'll gladly give further credit to them for
their inspiration/original work.

I'd say this game is ~95% faithful to that original game.
Some of the limitations are due to my use of the so-called
[PETSCII](https://en.wikipedia.org/wiki/PETSCII) character set for the
graphics instead of custom graphics, which means there is no smooth
animation of the pieces as they are activated.  I've also not included
any sounds, but that might come in a future version.

This is original code that implements the game play, I did not port the
embedded ActionScript of the original SWF game.

## Why?

Why write this game?  

Well, two basic reasons.  

First, I decided to take another run at picking up the C programming
language, and like learning any new language, it helps if you have a
project in mind that you want to implement.

Why write for this platform?  Being a fan of the Commodore
8-bit computers and following the development of the
[Commander-X16](https://www.commanderx16.com/), I thought it would be a
fun platform to target.

## Compiling the code

This was developed with the [cc65](https://cc65.github.io/) compiler,
so, you'll need it installed.  I tend to work with a semi-recent version
out of git rather than one of the binary packages, so, YMMV with getting
it compile if you use a potentially older version of the compiler suite.

Edit the `Makefile` and fix the paths to the various binaries and
destinations at the top.

A simple `make` should give you a `GRIDGAME` that you can now load up on
an X-16.

With a little prep-work, the `Makefile` will 'install' to your sdcard
for you (location is easily configurable in the `Makefile`).  There is
also a `run` target that will do the full build/install/run cycle.

To get this to work you will need a suitable sdcard image
file.  Either use the one provided with the emulator archive,
or make a new one.  If you're on a Linux box, check out the
[script](https://github.com/mparson/mkcard) I wrote for making new
images.  It also has instructions for setting up a `~/.mtoolsrc` file so
the `make install` will work.  You might also need to edit the `runner`
script if you've changed where on the sdcard you install the game.

## Notes on the kwalitee of my code

Yeah, very intentional bad spelling there.  I make no claims to my
skills as a C programmer.  This is probably not "good" C code and most
definitely not optimized in the suggested ways you should when writing
for `cc65` compiler and the 6502 CPU.

## Changelog:

v0.6 - High Scores are saved across runs of the game.  
v0.5 - Initial release of playable game.

## Potential features for future versions:

* Versions for the C-64 & C-128
* Sound - I'll need to read up on how to make cc65 generate sounds.
  It's documented, but I've not tried that yet.  Step 1 was to get the
  basic game working.
* Board Edit Mode - Add a mode where you can click around and rotate
  individual pieces w/o triggering the game play, let you set up custom
  boards to see how the patterns play out.  This will also give you the
  ability to save/load the custom boards.
* See if it will compile with [KickC](https://gitlab.com/camelot/kickc),
  then all dev work could even be done on the system it is played on!
