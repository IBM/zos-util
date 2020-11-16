# zos-util
This module provides a Python interface into various z/OS utilities

# API
See [here](https://www.ibm.com/support/knowledgecenter/SSCH7P_3.8.0/zos_util.html) for the API

# Example
    import zos_util
    import tempfile
    f = tempfile.NamedTemporaryFile()
    # To specify a file with IBM-1047 code set
    fpath = f.name
    zos_util.chtag(fpath, 1047)
     
    # To specify a file with ISO8859-1 code set
    zos_util.chtag(fpath)
    tag_info = zos_util.get_tag_info(fpath)
    print(f"CCSID:{tag_info[0]}, TXT_FLAG:{tag_info[1]}")
     
    # set to tag_mixed mode
    zos_util.tag_mixed(fpath)
    tag_info = zos_util.get_tag_info(fpath)
    print(f"CCSID:{tag_info[0]}, TXT_FLAG:{tag_info[1]}")
     
    # remove the tag from the file
    zos_util.untag(fpath)
    tag_info = zos_util.get_tag_info(fpath)
    print(f"CCSID:{tag_info[0]}, TXT_FLAG:{tag_info[1]}")

# Build Instruction  
  `python3 ./setup install`

# Test Instruction  
  `cd test`  
  `python3 ./tag_test.py`
