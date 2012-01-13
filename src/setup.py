import os

from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

ext_modules = [
    Extension(
        "cprovenance",
        sources = ["cprovenance.pyx"],
        include_dirs = [os.getcwd()],
        library_dirs = [os.getcwd()],
        libraries = ['prov'])
]

setup(
    name = 'cprovenance',
    cmdclass = {'build_ext': build_ext},
    ext_modules = ext_modules
)
