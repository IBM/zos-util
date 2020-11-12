# © Copyright IBM Corporation 2020

# Copyright © 2001-2020 Python Software Foundation; All Rights Reserved

import zos_util
import subprocess
import os
import grp
import time
import unittest
import tempfile


def check_group_access(name):
    groupIDs = os.getgroups()
    if os.getuid() == 0:
        return True
    # check every group ID
    for id in groupIDs:
        if grp.getgrgid(id).gr_name == name:
            return True
    return False


class TestTag(unittest.TestCase):
    def setUp(self):
        _, self.fname = tempfile.mkstemp()
        self.ISO8859 = b"ISO8859-1"
        self.tag_info_ISO8859 = (819, True)
        self.untagged = b"untagged"
        self.tag_info_untagged = (0, False)
        self.IBM1047 = b"IBM-1047"
        self.tag_info_IBM1047 = (1047, True)
        self.binary = b"binary"
        self.tag_info_binary = (65535, False)

    def tearDown(self):
        os.remove(self.fname)

    def get_tag(self):
        cmd = f'ls -alT {self.fname}'.split()
        run_result = subprocess.run(cmd, capture_output=True)
        output = run_result.stdout.split()[1]
        return output

    def test_tag(self):
        # using default tag
        zos_util.chtag(self.fname)
        tag_info = zos_util.get_tag_info(self.fname)
        output = self.get_tag()
        self.assertEqual(output, self.ISO8859)
        self.assertEqual(tag_info, self.tag_info_ISO8859)

        # remove the tag
        zos_util.untag(self.fname)
        tag_info = zos_util.get_tag_info(self.fname)
        output = self.get_tag()
        self.assertEqual(output, self.untagged)
        self.assertEqual(tag_info, self.tag_info_untagged)

        # change tag to 1047
        zos_util.chtag(self.fname, 1047)
        tag_info = zos_util.get_tag_info(self.fname)
        output = self.get_tag()
        self.assertEqual(output, self.IBM1047)
        self.assertEqual(tag_info, self.tag_info_IBM1047)

        # change tag to 819
        zos_util.chtag(self.fname, 819)
        tag_info = zos_util.get_tag_info(self.fname)
        output = self.get_tag()
        self.assertEqual(output, self.ISO8859)
        self.assertEqual(tag_info, self.tag_info_ISO8859)

        # change tag to 819 and set text flag off
        zos_util.chtag(self.fname, set_txtflag=False)
        tag_info = zos_util.get_tag_info(self.fname)
        output = self.get_tag()
        self.assertEqual(output, self.ISO8859)
        self.assertEqual(tag_info, (819, False))

        # change tag to binary mode
        zos_util.chtag(self.fname, 65535, set_txtflag=False)
        tag_info = zos_util.get_tag_info(self.fname)
        output = self.get_tag()
        self.assertEqual(output, self.binary)
        self.assertEqual(tag_info, self.tag_info_binary)

        # different way for change file to binary mode
        zos_util.untag(self.fname)
        zos_util.tag_binary(self.fname)
        tag_info = zos_util.get_tag_info(self.fname)
        output = self.get_tag()
        self.assertEqual(output, self.binary)
        self.assertEqual(tag_info, self.tag_info_binary)

        # change to text mode
        zos_util.chtag(self.fname)
        zos_util.tag_text(self.fname)
        tag_info = zos_util.get_tag_info(self.fname)
        self.assertEqual(tag_info, (819, True))

        # change to mixed mode
        zos_util.chtag(self.fname)
        zos_util.tag_mixed(self.fname)
        tag_info = zos_util.get_tag_info(self.fname)
        self.assertEqual(tag_info, (819, False))

        # Invalid pass in option with untag
        with self.assertRaises(TypeError):
            zos_util.untag(self.fname, 1047)


@unittest.skipUnless(check_group_access("APF"),
                     "No READ access to the BPX.FILEATTR.APF")
class TestAPF(unittest.TestCase):
    def setUp(self):
        _, self.fname = tempfile.mkstemp()

    def tearDown(self):
        os.remove(self.fname)

    def check_apf(self):
        cmd = f'ls -alE {self.fname}'.split()
        run_result = subprocess.run(cmd, capture_output=True)
        output = run_result.stdout.split()[1]
        return b"a" in output

    def test_apf(self):
        zos_util.enable_apf(self.fname)
        output = self.check_apf()
        self.assertEqual(output, True)

        zos_util.disable_apf(self.fname)
        output = self.check_apf()
        self.assertEqual(output, False)

        zos_util.enable_apf(self.fname)
        output = self.check_apf()
        self.assertEqual(output, True)


if __name__ == '__main__':
    unittest.main()
