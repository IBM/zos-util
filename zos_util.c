/*
 * © Copyright IBM Corporation 2020
 */

/*
 * Copyright © 2001-2020 Python Software Foundation; All Rights Reserved
 */

#define PY_SSIZE_T_CLEAN
#define _OPEN_SYS_FILE_EXT 1
#include <Python.h>
#include <object.h>
#include <sys/stat.h>

#define BINARY_CCSID 65535
#define UNTAG_CCSID 0
#define ISO8859_CCSID 819

static PyObject *__setccsid(PyObject *self, PyObject *args, PyObject *kwargs,
                            int ccsid) {
  char *path;
  int txtflag = 1;
  int res;
  attrib_t attr;

  memset(&attr, 0, sizeof(attr));
  attr.att_filetagchg = 1;

  if (ccsid == UNTAG_CCSID || ccsid == BINARY_CCSID) {
    if (!PyArg_ParseTuple(args, "s", &path))
      return NULL;
    attr.att_filetag.ft_txtflag = 0;
  } else {

    static char *keywords[] = {"filepath", "ccsid", "set_txtflag", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|ip", keywords, &path,
                                     &ccsid, &txtflag))
      return NULL;
    attr.att_filetag.ft_txtflag = txtflag;
  }

  attr.att_filetag.ft_ccsid = ccsid;
  res = __chattr(path, &attr, sizeof(attr));
  if (res < 0) {
    PyErr_SetFromErrno(PyExc_OSError);
    return NULL;
  }

  Py_RETURN_NONE;
}

static PyObject *__settxtflag(PyObject *self, PyObject *args, int txtflag) {
  char *path;
  int res;
  struct stat st;
  attrib_t attr;

  if (!PyArg_ParseTuple(args, "s", &path))
    return NULL;

  memset(&attr, 0, sizeof(attr));
  attr.att_filetagchg = 1;

  // get old ccsid
  res = stat(path, &st);
  if (res < 0) {
    PyErr_SetFromErrno(PyExc_OSError);
    return NULL;
  }
  unsigned short ccsid = st.st_tag.ft_ccsid;

  attr.att_filetag.ft_ccsid = ccsid;
  attr.att_filetag.ft_txtflag = txtflag;

  res = __chattr(path, &attr, sizeof(attr));
  if (res < 0) {
    PyErr_SetFromErrno(PyExc_OSError);
    return NULL;
  }

  Py_RETURN_NONE;
}

static PyObject *__set_apf_auth(PyObject *self, PyObject *args, int is_apf) {
  char *path;
  int res;
  attrib_t attr;

  if (!PyArg_ParseTuple(args, "s", &path))
    return NULL;

  memset(&attr, 0, sizeof(attr));
  attr.att_setgen = 1;
  attr.att_apfauthmask = 1;
  attr.att_apfauth = is_apf;

  res = __chattr(path, &attr, sizeof(attr));
  if (res < 0) {
    PyErr_SetFromErrno(PyExc_OSError);
    return NULL;
  }

  Py_RETURN_NONE;
}

static PyObject *_chtag_impl(PyObject *self, PyObject *args, PyObject *kwargs) {
  return __setccsid(self, args, kwargs, ISO8859_CCSID);
}

static PyObject *_tag_binary_impl(PyObject *self, PyObject *args) {
  return __setccsid(self, args, NULL, BINARY_CCSID);
}

static PyObject *_untag_impl(PyObject *self, PyObject *args) {
  return __setccsid(self, args, NULL, UNTAG_CCSID);
}

static PyObject *_tag_text_impl(PyObject *self, PyObject *args) {
  return __settxtflag(self, args, 1);
}

static PyObject *_tag_mixed_impl(PyObject *self, PyObject *args) {
  return __settxtflag(self, args, 0);
}

static PyObject *_get_tag_impl(PyObject *self, PyObject *args) {
  struct stat st;
  char *path;
  int res;

  if (!PyArg_ParseTuple(args, "s", &path))
    return NULL;

  res = stat(path, &st);
  if (res < 0) {
    PyErr_SetFromErrno(PyExc_OSError);
    return NULL;
  }

  unsigned short ccsid = st.st_tag.ft_ccsid;
  int txtflag = st.st_tag.ft_txtflag;

  return Py_BuildValue("(HN)", ccsid, PyBool_FromLong((long)(txtflag)));
}

static PyObject *_enable_apf_auth_impl(PyObject *self, PyObject *args) {
  return __set_apf_auth(self, args, 1);
}

static PyObject *_disable_apf_auth_impl(PyObject *self, PyObject *args) {
  return __set_apf_auth(self, args, 0);
}

PyDoc_STRVAR(
    chtag_doc,
    "chtag(filepath, ccsid=819, set_txtflag=True)\n"
    "--\n"
    "\n"
    "Chtag allows you to change information in a file tag, A file tag is "
    "composed\n"
    "composed of a numeric coded character set identifier(ccsid) and a text "
    "flag\n"
    "(set_txtflag) codeset: \n "
    "\t Represents the coded character set in which text data is\n"
    "\t encoded. The code set can be used for uniformly encoded text files or\n"
    "\t files that contain mixed text/binary data.\n"
    "set_txtflag: \n"
    "\t set_txtflag = True indicates the file has uniformly encoded text data\n"
    "\t set_txtflag = False indicates the file has non-uniformly encoded text "
    "data\n");

PyDoc_STRVAR(untag_doc, "untag(filepath)\n"
                        "--\n"
                        "\n"
                        "Removes any tagging information associated with the "
                        "file and sets the status\n"
                        "of the file to 'untagged'.");

PyDoc_STRVAR(get_tag_info_doc, "get_tag_info(filepath)\n"
                               "--\n"
                               "\n"
                               "Return a tuple (ccsid, set_txtflag) of file "
                               "tag information associated with the\n"
                               "file.");

PyDoc_STRVAR(tag_binary_doc, "tag_binary(filepath)\n"
                             "--\n"
                             "\n"
                             "Change the file tag to binary mode. Indicates "
                             "that the file contains only \n"
                             "binary (non-uniformly encoded) data.\n ");

PyDoc_STRVAR(
    tag_text_doc,
    "tag_text(filepath)\n"
    "--\n"
    "\n"
    "Change the file tag to text mode, Indicates that the specified file "
    "contains\n"
    "pure (uniformly encoded) text data.\n"
    "The existing ccsid that is associated with the file is retained.\n"
    "Note: the ccsid MUST be set before apply this function\n ");

PyDoc_STRVAR(
    tag_mixed_doc,
    "tag_mixed(filepath)\n"
    "--\n"
    "\n"
    "Change the file tag to mixed mode, Indicates that the file contains mixed "
    "text\n"
    "and binary data.\n"
    "The existing ccsid that is associated with the file is retained.\n"
    "Note: the ccsid MUST be set before apply this function\n");

PyDoc_STRVAR(enable_apf_auth_doc,
             "enable_apf(filepath)\n"
             "--\n"
             "\n"
             "Set APF-authorized attribute to an executable program file(load "
             "module), it\n"
             "behaves as if loaded from an APF-authorized library.\n"
             "For example, if this program is exec()ed at the job step level "
             "and the program\n"
             "is linked with the AC=1 attribute, the program will be executed "
             "as APF-authorized.\n"
             "Rule: To specify any of these attributes, the user must be the "
             "owner of the file\n"
             "or have superuser authority.\n");

PyDoc_STRVAR(disable_apf_auth_doc,
             "disable_apf(filepath)\n"
             "--\n"
             "\n"
             "Unset APF-authorized attribute to an executable program file,it "
             "behaves as \n"
             "remove file from an APF-authorized library.\n"
             "Rule: To specify any of these attributes, the user must be the "
             "owner of the\n"
             "file or have superuser authority.\n");

static PyMethodDef ZosUtilMethods[] = {
    {"chtag", (PyCFunction)_chtag_impl, METH_VARARGS | METH_KEYWORDS,
     chtag_doc},
    {"untag", (PyCFunction)_untag_impl, METH_VARARGS, untag_doc},
    {"tag_binary", (PyCFunction)_tag_binary_impl, METH_VARARGS, tag_binary_doc},
    {"tag_mixed", (PyCFunction)_tag_mixed_impl, METH_VARARGS, tag_mixed_doc},
    {"tag_text", (PyCFunction)_tag_text_impl, METH_VARARGS, tag_text_doc},
    {"get_tag_info", (PyCFunction)_get_tag_impl, METH_VARARGS,
     get_tag_info_doc},
    {"disable_apf", (PyCFunction)_disable_apf_auth_impl, METH_VARARGS,
     disable_apf_auth_doc},
    {"enable_apf", (PyCFunction)_enable_apf_auth_impl, METH_VARARGS,
     enable_apf_auth_doc},
    {NULL, NULL, 0, NULL}, // sentinel
};

static PyModuleDef zos_util = {
    PyModuleDef_HEAD_INIT,
    "zos.util",
    "This module provides a portable way of using Zos utility",
    -1,
    ZosUtilMethods,
};

PyMODINIT_FUNC PyInit_zos_util() {
  PyObject *module;

  module = PyModule_Create(&zos_util);
  if (module == NULL) {
    return NULL;
  }
  return module;
}
