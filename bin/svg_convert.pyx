# linux version (compiler directives for cython compiler)
# distutils: language = c++
# distutils: sources = convert_svgs.cpp
# distutils: extra_compile_args = -fopenmp -ffast-math -march=native
# distutils: extra_link_args = -fopenmp
# cython: language_level = 3


# we tell cython which function from the header convert_svgs.h
cdef extern from "convert_svgs.h":
    void convert_svgs(const char* _svg_dir, const char* _out_dir, const char* dim, int _is_gray, int _is_background)

def svg2png(svg_dir, out_dir, dim, is_gray, is_background): # call this from python
    if not svg_dir.endswith("/"):
        svg_dir += "/"
    if not out_dir.endswith("/"):
        out_dir += "/"
    ptr_svg_dir = svg_dir.encode('UTF-8')
    cdef char* _svg_dir = ptr_svg_dir

    ptr_out_dir = out_dir.encode('UTF-8')
    cdef char* _out_dir = ptr_out_dir

    dim_str = str(dim)
    ptr_dim = dim_str.encode('UTF-8')
    cdef char* _dim = ptr_dim

    convert_svgs( _svg_dir, _out_dir, _dim, is_gray, is_background)



