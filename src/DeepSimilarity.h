//
//  DeepSimilarity.h
//  AudioSearchCLI
//
//  Created by Michael Donovan on 5/25/17.
//  Copyright © 2017 Michael Donovan. All rights reserved.
//

#ifndef DeepSimilarity_h
#define DeepSimilarity_h

#include "boost/filesystem.hpp"
#include "/usr/local/Cellar/python/2.7.13/Frameworks/Python.framework/Versions/2.7/include/python2.7/Python.h"

#define SCRIPT_PATH "/Users/michael/InteractiveAudioLab/siamese_demo_code"
namespace Distance
{
using boost::filesystem::path;
using namespace std;
static double deepSimilarity (path a, path b)
{
    Py_InitializeEx (0);
    PyRun_SimpleString ("import sys, os");
    //    PyRun_SimpleString ("print sys.path");
    string pathAppend = string ("sys.path.append('") + string (SCRIPT_PATH) + string ("')");
    PyRun_SimpleString (pathAppend.c_str ());
    pathAppend = string ("sys.path.append('") + "/Users/michael/InteractiveAudioLab/caffe/python" + string ("')");
    PyRun_SimpleString (pathAppend.c_str ());

    PyObject* moduleString = PyString_FromString ((char*) "keras2caffe_demo");
    PyObject* module = PyImport_Import (moduleString);
    if (module == nullptr)
    {
        PyErr_Print ();
    }

    Py_DecRef (moduleString);

    PyObject* function = PyObject_GetAttrString (module, (char*) "siamese_similarity");

    PyObject* args = PyTuple_New (2);
    PyTuple_SET_ITEM (args, 0, PyString_FromString (a.string ().c_str ()));
    PyTuple_SET_ITEM (args, 1, PyString_FromString (b.string ().c_str ()));

    PyObject* output = PyObject_CallObject (function, args);
    if (output == nullptr)
    {
        PyErr_Print ();
        throw runtime_error ("");
    }

    double result;
    if (PyFloat_Check (output))
    {
        result = PyFloat_AsDouble (output);
    }
    else
    {
        DBG ("function did not return float");
        throw runtime_error ("");
    }

    Py_Finalize ();
    return result;
}
}

#endif /* DeepSimilarity_h */
