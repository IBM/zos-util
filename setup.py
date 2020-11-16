# © Copyright IBM Corporation 2020

# Copyright © 2001-2020 Python Software Foundation; All Rights Reserved

from setuptools import setup, Extension

import os
import sysconfig

def main():
    dir_path = os.path.dirname(os.path.realpath(__file__))
    zos_util_ext = Extension("zos_util", sources=["zos_util.c"])
    with open(os.path.join(dir_path, "README.md")) as f:
        project_description = f.read()

    setup(name="zos_util",
          version="1.0.1",
          license="License :: OSI Approved :: Apache Software License",
          description="This module provides a Python interface into various z/OS utilities",
          long_description=project_description,
          long_description_content_type='text/markdown',
          project_urls={
            "Documentation": "https://www.ibm.com/support/knowledgecenter/SSCH7P_3.8.0/zos_util.html",
            "Source Code": "https://github.com/IBM/zos-util",
          },
          author="IBM",
          ext_modules=[zos_util_ext]
          )


if __name__ == "__main__":
    main()
