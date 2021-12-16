# SplineImages

This program is trying to reproduce the original image, but based on only a few tens of points. To achieve this program is using third degrees spline functions.

## How to compile this

You need to have 'gcc' and 'make'.

## How to use it

You need to prepare two files with xpoints and ypoints (you need to prepare at least few of them). Then you just need to type:

```bash
./nspline NUMBER STRING1 STRING2
```

NUMBER <-- number of measurements on spline functions
STRING1 <-- path to xpoints file
STRING2 <-- path to ypoints file

then your image will be generated and saved to "image.bmp".

## Settings

You can change the resolution of the image at the 153'rd line of main.c

## Example

I have attached example x and y data.
