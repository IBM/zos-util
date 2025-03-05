# zos-util
This module provides a Python interface into various z/OS utilities

# API
zos_util.chtag(filepath, ccsid=819, set_txtflag=True)
    changes information in a file tag. A file tag is composed of a numeric coded character set identifier (ccsid) and a text flag (set_txtflag) codeset.
    set_txtflag = True indicates that the file has uniformly encoded text data.
    set_txtflag = False indicates that the file has non-uniformly encoded text data.

zos_util.untag(filepath)
    removes any tagging information that is associated with the file and sets the status of the file to untagged.

zos_util.get_tag_info(filepath)
    returns a tuple of file tagging information (ccsid, set_txtflag) associated with the file.

zos_util.tag_binary(filepath)
    changes the file tag to binary mode to indicate that the file contains only binary (non-uniformly encoded) data. 

zos_util.tag_text(filepath)
    changes the file tag to text mode, which indicates that the specified file contains pure text (uniformly encoded) data.
    The existing ccsid that is associated with the file is retained.

zos_util.tag_mixed(filepath)
    changes the file tag to mixed mode, which indicates that the file contains mixed text and binary data.
    The existing ccsid that is associated with the file is retained.

zos_util.enable_apf(filepath)
    sets APF-authorized attribute on an executable program file (load module). It behaves as if the file is loaded from an APF-authorized library and raises PermissionError exception when the operation fails.

zos_util.disable_apf(filepath)
    unsets APF-authorized attribute on an executable program file. It behaves the same as removing the file from an APF-authorized library and raises PermissionError exception when the operation fails.

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
