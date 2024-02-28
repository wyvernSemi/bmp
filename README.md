
# The bmp bitmap utility


## Basic commands</h3>

The <tt>bmp</tt> utility
was originally written to understand and explore the bitmap format,
and as such is an experimental program with only minor thought
given to a user interface. The interface will be more familiar
to Linux/UN*X users&mdash;i.e. it is command line driven. For those 
more used to a Windows world, now's the time to open a console command
window and do some real computing.  However, it does have the
advantage of being able to do batch processing of multiple
files, wrapping up calls to <tt>bmp</tt> in a scripting language. Indeed
it has been used to do just this by a student in a University
in Poland.

If you've <a href="https://github.com/wyvernSemi/bmp">downloaded</a> <tt>bmp</tt>, then place it somewhere in your <tt>PATH</tt>. If that phrase means nothing to you, then simply place <tt>bmp</tt> in
the folder with your images, and where you see <tt>bmp</tt> in the description
below, type <tt>./bmp</tt> (or <tt>.\bmp.exe</tt> on windows) instead.

If you type <tt>bmp -h</tt> in a command window, the following usage message
appears.

<pre>
Usage: bmp [-dhrgVH] [-b <val>] [-c <val>] [-m <colour>]
           [-C <rect quad>] [-i <file>] [-o <file>]
&nbsp;
    -h Display this message
    -d Increase debug output level (default no debug output)
    -b Change image brightness by specified percent (100% = normal)
    -c Change image contrast by specified percent (50% = normal)
    -g Change image to grey scale
    -r Reverse image colours
    -V Flip image about vertical axis
    -H Flip image about horizontal axis
    -m Extract monochromatic colour image:
         R[ed]
         G[reen]
         B[lue]
         Y[ellow]
         C[yan]
         M[agenta]
    -C Clip image to rectangle
    -i Input filename (default test.bmp)
    -o Output filename (default no output)
</pre>
</p>


### File options

All the command line options are listed. These can be used in isolation
or in combination. There are three types of command option: 
file specification, information display and image manipulation. 
By default <tt>bmp</tt> reads a file <tt>test.bmp</tt>, and outputs no data. To change
the input file to be read, the <tt>-i</tt> option is used. To enable output and specify the
output file, the <tt>-o</tt> option is used (required if using manipulation commands).

### Information options

The <tt>-h</tt> option mentioned above, used to 
display the usage message, is of the information display type. 
The other important 
option of this type is <tt>-d</tt>. This increases the 'debug' level by 1. Currently
there is only one debug level above 0 (off), and this displays the 
input file's bitmap header. So typing, for example:

<pre>
  bmp -d -i test.bmp
</pre>


yields an output to the console like the following.


<pre>
  Type               = BM
  File Size          = 0x00038ae8
  Offset             = 0x00000036

  Size               = 0x00000028
  Width              = 0x000000d9
  Height             = 0x00000164
  Planes             = 0x0001
  Bits per Pixel     = 0x0018
  Compression        = 0x00000000
  Image Size         = 0x00000000
  X Pixels per Meter = 0x00000b12
  Y Pixles per Meter = 0x00000b12
  Colour Used        = 0x00000000
  Colour Important   = 0x00000000
</pre>

The numbers are in hexadecimal, which if you aren't familiar with
is a bit of a pain. You might want to check you have a calculator which can do
the conversion to decimal for you. For those familiar with hexadecimal, the good
news is that <tt>bmp</tt> will accept hexadecimal numbers (with a leading <tt>0x</tt>). The rest of the
displayed parameters may be of interest to those wanting to more of the
bitmap file format. More information about the format can be found 
<a target="_parent" href="http://netghost.narod.ru/gff/graphics/book/ch03_01.htm">here</a>.


### Image manipulation options

The manipulation commands are pretty much self-explanatory, and I will
just briefly discuss them here. The <tt>-b</tt> option allows the brightness to be increased
by a specified percentage, with numbers less than 100 dimming the image. The <tt>-c</tt>
option specifies a percent change in contrast, where 50% is unchanged. Values
less than 50 decrease contrast and numbers greater than 50 increase it. The
<tt>-g</tt> option simply converts to a grey scale image, whilst the <tt>-r</tt> produces a negative.
The image may be flipped about its vertical and horizontal axes using
<tt>-H</tt> and <tt>-V</tt> respectively. Finally, <tt>-m</tt> is used to extract just a primary
(or its negative) component of the input bitmap. The argument can be
a single letter, or the colour may be specified in full.

Of the image manipulating command, <tt>-C</tt> is the most complex to use.
This extracts a rectangular region
from a bitmap image. With this command you must also specify an output file with 
<tt>-o</tt> (but do <em>not</em> use the same name as the input file, as it will be destroyed!).
An input file name is also likely to be needed, but not strictly necessary if
your input data is in a file called <tt>test.bmp</tt>. The command is used like the following example:
</p>

<pre>
  bmp -i calc.bmp -o display.bmp -C "0 217 226 271"
</pre>

Note that the quotes around the four specified numbers are necessary so
that they are passed as a single argument to bmp. The numbers could easily
have been in hexadecimal. I.e. <tt>-C "0x0 0xd9 0xe2 0x10f"</tt>. Unlike the bitmap
format itself, the rectangle specified here is referenced from the bottom
left hand corner of the image (bitmaps&mdash;or DIBs to be precise&mdash;are stored
top row first, or upside down). The numbers specify left, right, bottom
and top limits, in that order. The right and top numbers actually specify
the pixel <em>after</em> the last column/row extracted. This has two advantages.
Firstly, the height (say) of the extracted file is simply the second number minus the first, with
no +1 adjustment. Similarly for width. Secondly, when used in splitting images, the
end number of the last image extracted is the beginning of the current
image to be extracted, if you want them to be adjoining. I got this wrong so
many times when splitting up bitmaps without this feature, that I changed
the program. Trust me&mdash;it's better this way. 

## Download

The above manipulation commands can be used in combination to produce different
results, and it is surprising just how much can be done with just these few
basic tools. Much more can be done with this, so download the source and
have a go at adding features.

The <tt>bmp</tt> program is by no means a complete bitmap manipulation tool. Buy a
commercial package for that or use and open source tool such as <a href="https://www.gimp.org/">gimp</a>.
But it does illustrate the format and some
basic methods of graphic manipulation (or my naive understanding of it at least).
I think this aspect of the program is only useful with the source code, so
you can get the executables, along with the source code from 
<a href="https://github.com/wyvernSemi/bmp">github</a>. It has a makefile for
linux/un*x environments, and files for MSVC Express 2010 on
windows. NB. there are 'endian' sensitivities to bitmap data, so if
you compile the code in your own environment be aware of this, and
check the header files (in particular <tt>general.h<tt>).


<hr>
<address>
Copyright &copy; 2010-2024 Simon Southwell<br>
<a href="mailto:simon&#64anita-simulators.org.uk">simon&#64anita-simulators.org.uk</A>
</address>

