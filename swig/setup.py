#!/usr/bin/env python
'''
@Author: xin.xing.lzj
@Date: 2019-08-12 11:10:46
@LastEditors: xinxing.lzj
@LastEditTime: 2019-08-21 19:39:23
@Description: 
'''

"""
swig -python -c++ example.i
python setup.py build_ext --inplace
"""

    
import os, sys
from distutils.core import setup, Extension
from distutils import sysconfig

# Remove strict-prototypes compiler flag
cfg_vars = sysconfig.get_config_vars()
for key, value in cfg_vars.items():
    if type(value) == str:
        cfg_vars[key] = value.replace('-Wstrict-prototypes', '')

cpp_args = ['-std=c++17']

treePredict_module = Extension('_treePredict',
                           include_dirs=['../include'],
                           libraries = ['morseot'],
                           extra_objects=['../build/lib/libHFB.a', '/usr/local/lib/libseal.a'],
                           extra_compile_args = cpp_args,
                           sources=['treePredict_wrap.cxx', 'treeofflineOT.cxx',
                           'treeuser.cxx','treemodel.cxx'],
                           )

setup (name = 'treePredict',
       version = '0.1',
       author      = "LZJ",
       description = """treePredict""",
       ext_modules = [treePredict_module],
       py_modules = ["treePredict"],
       )