# © Copyright IBM Corporation 2020

# Copyright © 2001-2020 Python Software Foundation; All Rights Reserved

from distutils.core import setup, Extension

import os
import sysconfig


def main():
    dir_path = os.path.dirname(os.path.realpath(__file__))
    zos_util_path = os.path.join(dir_path, "zos_util.c")
    zos_util_ext = Extension("zos_util", sources=[zos_util_path])
    setup(name="zos_util",
          version="1.0.1",
          license="License :: OSI Approved :: Apache Software License",
          description="This module provides a portable way of using z/OS utilities",
          author="IBM",
          ext_modules=[zos_util_ext]
          )


if __name__ == "__main__":
    main()
