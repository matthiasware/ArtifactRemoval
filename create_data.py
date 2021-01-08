import os

try:
    from bin.svg_convert import svg2png
except ImportError:
    os.system("cythonize -i ./bin/svg_convert.pyx")
    from bin.svg_convert import svg2png

#### customize for your needs ####
#  svg_dir = "svgs"
svg_dir = "svgs"
out_dir = "training_data"
dim = 128
is_gray = True
is_background = True
##################################

svg2png(svg_dir, out_dir, dim, is_gray, is_background)
