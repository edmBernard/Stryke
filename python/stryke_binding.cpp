
#include "stryke/core.hpp"
#include "stryke/multifile.hpp"
#include "stryke/options.hpp"
#include "stryke/reader.hpp"
#include "stryke/thread.hpp"
#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <array>
#include <iostream>
#include <string>

namespace py = pybind11;

// ==============================================================
// Type Convertion
// ==============================================================
namespace pybind11 {
namespace detail {

// For safety reason as I'm not so sure of my binding,
// I will comment type with unsafe conversion like Short<->Long
// concern type are :
// Short
// Int
// Float

// For safety reason as I'm not so sure of my binding,
// I will comment type with unsafe conversion like Short<->Long
// template <>
// struct type_caster<stryke::Int> {
// public:
//   PYBIND11_TYPE_CASTER(stryke::Int, _("stryke.Int"));
//   bool load(handle src, bool) {
//     PyObject *source = src.ptr();
//     PyObject *tmp = PyNumber_Long(source);
//     if (!tmp)
//       return false;
//     value.data = PyLong_AsLong(tmp);  // it's maybe not safe to convert long to int
//     Py_DECREF(tmp);
//     return !(value.data == -1 && PyErr_Occurred());
//   }
// };

// For safety reason as I'm not so sure of my binding,
// I will comment type with unsafe conversion like Short<->Long
// template <>
// struct type_caster<stryke::Short> {
// public:
//   PYBIND11_TYPE_CASTER(stryke::Short, _("stryke.Short"));
//   bool load(handle src, bool) {
//     PyObject *source = src.ptr();
//     PyObject *tmp = PyNumber_Long(source);
//     if (!tmp)
//       return false;
//     value.data = PyLong_AsLong(tmp);  // it's maybe not safe to convert long to short
//     Py_DECREF(tmp);
//     return !(value.data == -1 && PyErr_Occurred());
//   }
// };

template <>
struct type_caster<stryke::Long> {
public:
  PYBIND11_TYPE_CASTER(stryke::Long, _("stryke.Long"));
  bool load(handle src, bool) {
    PyObject *source = src.ptr();
    PyObject *tmp = PyNumber_Long(source);
    if (!tmp)
      return false;
    value.data = PyLong_AsLong(tmp);
    value.empty = false;
    Py_DECREF(tmp);
    return !((value.data == -1) && (PyErr_Occurred() != NULL));
  }

  static handle cast(stryke::Long src, return_value_policy, handle) {
    return PyLong_FromLong(src.data);
  }
};

// For safety reason as I'm not so sure of my binding,
// I will comment type with unsafe conversion like Short<->Long
// template <>
// struct type_caster<stryke::Float> {
// public:
//   PYBIND11_TYPE_CASTER(stryke::Float, _("stryke.Float"));
//   bool load(handle src, bool) {
//     PyObject *source = src.ptr();
//     PyObject *tmp = PyNumber_Float(source);
//     if (!tmp)
//       return false;
//     value.data = PyFloat_AsDouble(tmp);  // it's maybe not safe to convert double to float
//     Py_DECREF(tmp);
//     return !(value.data == -1 && PyErr_Occurred());
//   }
// };

template <>
struct type_caster<stryke::Double> {
public:
  PYBIND11_TYPE_CASTER(stryke::Double, _("stryke.Double"));
  bool load(handle src, bool) {
    PyObject *source = src.ptr();
    PyObject *tmp = PyNumber_Float(source);
    if (!tmp)
      return false;
    value.data = PyFloat_AsDouble(tmp);
    value.empty = false;
    Py_DECREF(tmp);
    return !((value.data == -1.0) && (PyErr_Occurred() != NULL));
  }

  static handle cast(stryke::Double src, return_value_policy, handle) {
    return PyFloat_FromDouble(src.data);
  }
};

template <>
struct type_caster<stryke::Boolean> {
public:
  PYBIND11_TYPE_CASTER(stryke::Boolean, _("stryke.Boolean"));
  bool load(handle src, bool) {
    PyObject *source = src.ptr();
    if (PyObject_IsTrue(source) == 1) {
      value.data = true;
    } else {
      value.data = false;
    }
    value.empty = false;
    return !((value.data == false) && (PyErr_Occurred() != NULL));
  }

  static handle cast(stryke::Boolean src, return_value_policy, handle) {
    if (src.data == true) {
      return Py_True;
    } else {
      return Py_False;
    }
  }
};

template <>
struct type_caster<stryke::Date> {
public:
  PYBIND11_TYPE_CASTER(stryke::Date, _("stryke.Date"));
  bool load(handle src, bool) {
    PyObject *source = src.ptr();
    PyObject *tmp = PyObject_Str(source);
    if (!tmp)
      return false;
    PyObject *pyStr = PyUnicode_AsEncodedString(tmp, "utf-8", "Error ~");
    value.data = PyBytes_AsString(pyStr);
    Py_DECREF(tmp);
    return !PyErr_Occurred();
  }

  static handle cast(stryke::Date src, return_value_policy, handle) {
    return PyUnicode_FromString(src.data.c_str());
  }
};

template <>
struct type_caster<stryke::DateNumber> {
public:
  PYBIND11_TYPE_CASTER(stryke::DateNumber, _("stryke.DateNumber"));
  bool load(handle src, bool) {
    PyObject *source = src.ptr();
    PyObject *tmp = PyNumber_Long(source);
    if (!tmp)
      return false;
    value.data = PyLong_AsLong(tmp);
    value.empty = false;
    Py_DECREF(tmp);
    return !((value.data == -1) && (PyErr_Occurred() != NULL));
  }

  static handle cast(stryke::DateNumber src, return_value_policy, handle) {
    return PyLong_FromLong(src.data);
  }
};

template <>
struct type_caster<stryke::Timestamp> {
public:
  PYBIND11_TYPE_CASTER(stryke::Timestamp, _("stryke.Timestamp"));
  bool load(handle src, bool) {
    PyObject *source = src.ptr();
    PyObject *tmp = PyObject_Str(source);
    if (!tmp)
      return false;
    PyObject *pyStr = PyUnicode_AsEncodedString(tmp, "utf-8", "Error ~");
    value.data = PyBytes_AsString(pyStr);
    Py_DECREF(tmp);
    return !PyErr_Occurred();
  }

  static handle cast(stryke::Timestamp src, return_value_policy, handle) {
    return PyUnicode_FromString(src.data.c_str());
  }
};

template <>
struct type_caster<stryke::TimestampNumber> {
public:
  PYBIND11_TYPE_CASTER(stryke::TimestampNumber, _("stryke.TimestampNumber"));
  bool load(handle src, bool) {
    PyObject *source = src.ptr();
    PyObject *tmp = PyNumber_Float(source);
    if (!tmp)
      return false;
    value.data = PyFloat_AsDouble(tmp);
    value.empty = false;
    Py_DECREF(tmp);
    return !((value.data == -1.0) && (PyErr_Occurred() != NULL));
  }

  static handle cast(stryke::TimestampNumber src, return_value_policy, handle) {
    return PyFloat_FromDouble(src.data);
  }
};

} // namespace detail
} // namespace pybind11

// ==============================================================
// Template to automate template binding
// ==============================================================

template <typename... T>
void declare_writer_impl(py::module &m, const std::string &typestr) {
  using Class = stryke::OrcWriterImpl<T...>;
  std::string pyclass_name = std::string("Writer") + typestr;

  py::class_<Class>(m, pyclass_name.c_str())
      .def(py::init<std::array<std::string, sizeof...(T)>, std::string, const stryke::WriterOptions &>(), py::arg("column_names"), py::arg("filename"), py::arg("writer_options"))
      .def("write", (void (Class::*)(T...)) & Class::write);
}

template <typename... T>
void declare_writer_multifile(py::module &m, const std::string &typestr) {
  using Class = stryke::OrcWriterMulti<T...>;
  std::string pyclass_name = std::string("Writer") + typestr;

  py::class_<Class>(m, pyclass_name.c_str())
      .def(py::init<std::array<std::string, sizeof...(T)>, std::string, std::string, const stryke::WriterOptions &>(), py::arg("column_names"), py::arg("root_folder"), py::arg("prefix"), py::arg("writer_options"))
      .def("write", (void (Class::*)(T...)) & Class::write)
      .def("close", (void (Class::*)(T...)) & Class::close);
}

template <typename... T>
void declare_writer_thread(py::module &m, const std::string &typestr) {
  using Class = stryke::OrcWriterThread<stryke::OrcWriterMulti, T...>;
  std::string pyclass_name = std::string("Writer") + typestr;

  py::class_<Class>(m, pyclass_name.c_str())
      .def(py::init<std::array<std::string, sizeof...(T)>, std::string, std::string, const stryke::WriterOptions &>(), py::arg("column_names"), py::arg("root_folder"), py::arg("prefix"), py::arg("writer_options"))
      .def("write", (void (Class::*)(T...)) & Class::write)
      .def("close_async", (void (Class::*)(T...)) & Class::close_async)
      .def("close_sync", (void (Class::*)(T...)) & Class::close_sync);
}

PYBIND11_MODULE(stryke, m) {

  py::class_<stryke::WriterOptions>(m, "WriterOptions")
      .def(py::init<>())
      .def("disable_lock_file", &stryke::WriterOptions::disable_lock_file)
      .def("enable_suffix_timestamp", &stryke::WriterOptions::enable_suffix_timestamp)
      .def("set_batch_size", &stryke::WriterOptions::set_batch_size)
      .def("set_batch_max", &stryke::WriterOptions::set_batch_max)
      .def("set_stripe_size", &stryke::WriterOptions::set_stripe_size)
      .def("set_cron", &stryke::WriterOptions::set_cron);

  // ==============================================================
  // Binding for WriterTemplate
  // ==============================================================

  auto m_simple = m.def_submodule("simple");

  // Basic type
  declare_writer_impl<stryke::Long>(m_simple, "Long1");
  declare_writer_impl<stryke::Double>(m_simple, "Double1");
  declare_writer_impl<stryke::Boolean>(m_simple, "Boolean1");
  declare_writer_impl<stryke::Date>(m_simple, "Date1");
  declare_writer_impl<stryke::DateNumber>(m_simple, "DateN1");
  declare_writer_impl<stryke::Timestamp>(m_simple, "Timestamp1");
  declare_writer_impl<stryke::TimestampNumber>(m_simple, "TimestampN1");
  // Reader
  m_simple.def("readerLong1", &stryke::orcReader<stryke::Long>, "Reader for single column Long");
  m_simple.def("readerDouble1", &stryke::orcReader<stryke::Double>, "Reader for single column Double");
  m_simple.def("readerBoolean1", &stryke::orcReader<stryke::Boolean>, "Reader for single column Boolean");
  m_simple.def("readerDate1", &stryke::orcReader<stryke::Date>, "Reader for single column Date");
  m_simple.def("readerDateN1", &stryke::orcReader<stryke::DateNumber>, "Reader for single column DateN");
  m_simple.def("readerTimestamp1", &stryke::orcReader<stryke::Timestamp>, "Reader for single column Timestamp");
  m_simple.def("readerTimestampN1", &stryke::orcReader<stryke::TimestampNumber>, "Reader for single column TimestampN");

  // 1D Point
  declare_writer_impl<stryke::Timestamp, stryke::Long>(m_simple, "TimestampPoint1l");
  declare_writer_impl<stryke::TimestampNumber, stryke::Long>(m_simple, "TimestampNPoint1l");
  declare_writer_impl<stryke::Date, stryke::Long>(m_simple, "DatePoint1l");
  declare_writer_impl<stryke::DateNumber, stryke::Long>(m_simple, "DateNPoint1l");
  declare_writer_impl<stryke::Timestamp, stryke::Double>(m_simple, "TimestampPoint1d");
  declare_writer_impl<stryke::TimestampNumber, stryke::Double>(m_simple, "TimestampNPoint1d");
  declare_writer_impl<stryke::Date, stryke::Double>(m_simple, "DatePoint1d");
  declare_writer_impl<stryke::DateNumber, stryke::Double>(m_simple, "DateNPoint1d");

  // 2D Point
  declare_writer_impl<stryke::Timestamp, stryke::Long, stryke::Long>(m_simple, "TimestampPoint2l");
  declare_writer_impl<stryke::TimestampNumber, stryke::Long, stryke::Long>(m_simple, "TimestampNPoint2l");
  declare_writer_impl<stryke::Date, stryke::Long, stryke::Long>(m_simple, "DatePoint2l");
  declare_writer_impl<stryke::DateNumber, stryke::Long, stryke::Long>(m_simple, "DateNPoint2l");
  declare_writer_impl<stryke::Timestamp, stryke::Double, stryke::Double>(m_simple, "TimestampPoint2d");
  declare_writer_impl<stryke::TimestampNumber, stryke::Double, stryke::Double>(m_simple, "TimestampNPoint2d");
  declare_writer_impl<stryke::Date, stryke::Double, stryke::Double>(m_simple, "DatePoint2d");
  declare_writer_impl<stryke::DateNumber, stryke::Double, stryke::Double>(m_simple, "DateNPoint2d");

  // 3D Point
  declare_writer_impl<stryke::Timestamp, stryke::Long, stryke::Long, stryke::Long>(m_simple, "TimestampPoint3l");
  declare_writer_impl<stryke::TimestampNumber, stryke::Long, stryke::Long, stryke::Long>(m_simple, "TimestampNPoint3l");
  declare_writer_impl<stryke::Date, stryke::Long, stryke::Long, stryke::Long>(m_simple, "DatePoint3l");
  declare_writer_impl<stryke::DateNumber, stryke::Long, stryke::Long, stryke::Long>(m_simple, "DateNPoint3l");
  declare_writer_impl<stryke::Timestamp, stryke::Double, stryke::Double, stryke::Double>(m_simple, "TimestampPoint3d");
  declare_writer_impl<stryke::TimestampNumber, stryke::Double, stryke::Double, stryke::Double>(m_simple, "TimestampNPoint3d");
  declare_writer_impl<stryke::Date, stryke::Double, stryke::Double, stryke::Double>(m_simple, "DatePoint3d");
  declare_writer_impl<stryke::DateNumber, stryke::Double, stryke::Double, stryke::Double>(m_simple, "DateNPoint3d");

  // Pair of coordinate 2D Point
  declare_writer_impl<stryke::Timestamp, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_simple, "TimestampVec2l");
  declare_writer_impl<stryke::TimestampNumber, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_simple, "TimestampNVec2l");
  declare_writer_impl<stryke::Date, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_simple, "DateVec2l");
  declare_writer_impl<stryke::DateNumber, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_simple, "DateNVec2l");
  declare_writer_impl<stryke::Timestamp, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_simple, "TimestampVec2d");
  declare_writer_impl<stryke::TimestampNumber, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_simple, "TimestampNVec2d");
  declare_writer_impl<stryke::Date, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_simple, "DateVec2d");
  declare_writer_impl<stryke::DateNumber, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_simple, "DateNVec2d");

  // Pair of coordinate 3D Point
  declare_writer_impl<stryke::Timestamp, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_simple, "TimestampVec3l");
  declare_writer_impl<stryke::TimestampNumber, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_simple, "TimestampNVec3l");
  declare_writer_impl<stryke::Date, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_simple, "DateVec3l");
  declare_writer_impl<stryke::DateNumber, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_simple, "DateNVec3l");
  declare_writer_impl<stryke::Timestamp, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_simple, "TimestampVec3d");
  declare_writer_impl<stryke::TimestampNumber, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_simple, "TimestampNVec3d");
  declare_writer_impl<stryke::Date, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_simple, "DateVec3d");
  declare_writer_impl<stryke::DateNumber, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_simple, "DateNVec3d");

  // ==============================================================
  // Binding for WriterMultifile
  // ==============================================================

  auto m_multifile = m.def_submodule("multifile");

  // 1D Point
  declare_writer_multifile<stryke::Timestamp, stryke::Long>(m_multifile, "TimestampPoint1l");
  declare_writer_multifile<stryke::TimestampNumber, stryke::Long>(m_multifile, "TimestampNPoint1l");
  declare_writer_multifile<stryke::Date, stryke::Long>(m_multifile, "DatePoint1l");
  declare_writer_multifile<stryke::DateNumber, stryke::Long>(m_multifile, "DateNPoint1l");
  declare_writer_multifile<stryke::Timestamp, stryke::Double>(m_multifile, "TimestampPoint1d");
  declare_writer_multifile<stryke::TimestampNumber, stryke::Double>(m_multifile, "TimestampNPoint1d");
  declare_writer_multifile<stryke::Date, stryke::Double>(m_multifile, "DatePoint1d");
  declare_writer_multifile<stryke::DateNumber, stryke::Double>(m_multifile, "DateNPoint1d");

  // 2D Point
  declare_writer_multifile<stryke::Timestamp, stryke::Long, stryke::Long>(m_multifile, "TimestampPoint2l");
  declare_writer_multifile<stryke::TimestampNumber, stryke::Long, stryke::Long>(m_multifile, "TimestampNPoint2l");
  declare_writer_multifile<stryke::Date, stryke::Long, stryke::Long>(m_multifile, "DatePoint2l");
  declare_writer_multifile<stryke::DateNumber, stryke::Long, stryke::Long>(m_multifile, "DateNPoint2l");
  declare_writer_multifile<stryke::Timestamp, stryke::Double, stryke::Double>(m_multifile, "TimestampPoint2d");
  declare_writer_multifile<stryke::TimestampNumber, stryke::Double, stryke::Double>(m_multifile, "TimestampNPoint2d");
  declare_writer_multifile<stryke::Date, stryke::Double, stryke::Double>(m_multifile, "DatePoint2d");
  declare_writer_multifile<stryke::DateNumber, stryke::Double, stryke::Double>(m_multifile, "DateNPoint2d");

  // 3D Point
  declare_writer_multifile<stryke::Timestamp, stryke::Long, stryke::Long, stryke::Long>(m_multifile, "TimestampPoint3l");
  declare_writer_multifile<stryke::TimestampNumber, stryke::Long, stryke::Long, stryke::Long>(m_multifile, "TimestampNPoint3l");
  declare_writer_multifile<stryke::Date, stryke::Long, stryke::Long, stryke::Long>(m_multifile, "DatePoint3l");
  declare_writer_multifile<stryke::DateNumber, stryke::Long, stryke::Long, stryke::Long>(m_multifile, "DateNPoint3l");
  declare_writer_multifile<stryke::Timestamp, stryke::Double, stryke::Double, stryke::Double>(m_multifile, "TimestampPoint3d");
  declare_writer_multifile<stryke::TimestampNumber, stryke::Double, stryke::Double, stryke::Double>(m_multifile, "TimestampNPoint3d");
  declare_writer_multifile<stryke::Date, stryke::Double, stryke::Double, stryke::Double>(m_multifile, "DatePoint3d");
  declare_writer_multifile<stryke::DateNumber, stryke::Double, stryke::Double, stryke::Double>(m_multifile, "DateNPoint3d");

  // Pair of coordinate 2D Point
  declare_writer_multifile<stryke::Timestamp, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_multifile, "TimestampVec2l");
  declare_writer_multifile<stryke::TimestampNumber, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_multifile, "TimestampNVec2l");
  declare_writer_multifile<stryke::Date, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_multifile, "DateVec2l");
  declare_writer_multifile<stryke::DateNumber, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_multifile, "DateNVec2l");
  declare_writer_multifile<stryke::Timestamp, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_multifile, "TimestampVec2d");
  declare_writer_multifile<stryke::TimestampNumber, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_multifile, "TimestampNVec2d");
  declare_writer_multifile<stryke::Date, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_multifile, "DateVec2d");
  declare_writer_multifile<stryke::DateNumber, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_multifile, "DateNVec2d");

  // Pair of coordinate 3D Point
  declare_writer_multifile<stryke::Timestamp, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_multifile, "TimestampVec3l");
  declare_writer_multifile<stryke::TimestampNumber, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_multifile, "TimestampNVec3l");
  declare_writer_multifile<stryke::Date, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_multifile, "DateVec3l");
  declare_writer_multifile<stryke::DateNumber, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_multifile, "DateNVec3l");
  declare_writer_multifile<stryke::Timestamp, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_multifile, "TimestampVec3d");
  declare_writer_multifile<stryke::TimestampNumber, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_multifile, "TimestampNVec3d");
  declare_writer_multifile<stryke::Date, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_multifile, "DateVec3d");
  declare_writer_multifile<stryke::DateNumber, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_multifile, "DateNVec3d");

  // ==============================================================
  // Binding for WriterThread
  // ==============================================================

  auto m_thread = m.def_submodule("thread");

  // 1D Point
  declare_writer_thread<stryke::Timestamp, stryke::Long>(m_thread, "TimestampPoint1l");
  declare_writer_thread<stryke::TimestampNumber, stryke::Long>(m_thread, "TimestampNPoint1l");
  declare_writer_thread<stryke::Date, stryke::Long>(m_thread, "DatePoint1l");
  declare_writer_thread<stryke::DateNumber, stryke::Long>(m_thread, "DateNPoint1l");
  declare_writer_thread<stryke::Timestamp, stryke::Double>(m_thread, "TimestampPoint1d");
  declare_writer_thread<stryke::TimestampNumber, stryke::Double>(m_thread, "TimestampNPoint1d");
  declare_writer_thread<stryke::Date, stryke::Double>(m_thread, "DatePoint1d");
  declare_writer_thread<stryke::DateNumber, stryke::Double>(m_thread, "DateNPoint1d");

  // 2D Point
  declare_writer_thread<stryke::Timestamp, stryke::Long, stryke::Long>(m_thread, "TimestampPoint2l");
  declare_writer_thread<stryke::TimestampNumber, stryke::Long, stryke::Long>(m_thread, "TimestampNPoint2l");
  declare_writer_thread<stryke::Date, stryke::Long, stryke::Long>(m_thread, "DatePoint2l");
  declare_writer_thread<stryke::DateNumber, stryke::Long, stryke::Long>(m_thread, "DateNPoint2l");
  declare_writer_thread<stryke::Timestamp, stryke::Double, stryke::Double>(m_thread, "TimestampPoint2d");
  declare_writer_thread<stryke::TimestampNumber, stryke::Double, stryke::Double>(m_thread, "TimestampNPoint2d");
  declare_writer_thread<stryke::Date, stryke::Double, stryke::Double>(m_thread, "DatePoint2d");
  declare_writer_thread<stryke::DateNumber, stryke::Double, stryke::Double>(m_thread, "DateNPoint2d");

  // 3D Point
  declare_writer_thread<stryke::Timestamp, stryke::Long, stryke::Long, stryke::Long>(m_thread, "TimestampPoint3l");
  declare_writer_thread<stryke::TimestampNumber, stryke::Long, stryke::Long, stryke::Long>(m_thread, "TimestampNPoint3l");
  declare_writer_thread<stryke::Date, stryke::Long, stryke::Long, stryke::Long>(m_thread, "DatePoint3l");
  declare_writer_thread<stryke::DateNumber, stryke::Long, stryke::Long, stryke::Long>(m_thread, "DateNPoint3l");
  declare_writer_thread<stryke::Timestamp, stryke::Double, stryke::Double, stryke::Double>(m_thread, "TimestampPoint3d");
  declare_writer_thread<stryke::TimestampNumber, stryke::Double, stryke::Double, stryke::Double>(m_thread, "TimestampNPoint3d");
  declare_writer_thread<stryke::Date, stryke::Double, stryke::Double, stryke::Double>(m_thread, "DatePoint3d");
  declare_writer_thread<stryke::DateNumber, stryke::Double, stryke::Double, stryke::Double>(m_thread, "DateNPoint3d");

  // Pair of coordinate 2D Point
  declare_writer_thread<stryke::Timestamp, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_thread, "TimestampVec2l");
  declare_writer_thread<stryke::TimestampNumber, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_thread, "TimestampNVec2l");
  declare_writer_thread<stryke::Date, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_thread, "DateVec2l");
  declare_writer_thread<stryke::DateNumber, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_thread, "DateNVec2l");
  declare_writer_thread<stryke::Timestamp, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_thread, "TimestampVec2d");
  declare_writer_thread<stryke::TimestampNumber, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_thread, "TimestampNVec2d");
  declare_writer_thread<stryke::Date, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_thread, "DateVec2d");
  declare_writer_thread<stryke::DateNumber, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_thread, "DateNVec2d");

  // Pair of coordinate 3D Point
  declare_writer_thread<stryke::Timestamp, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_thread, "TimestampVec3l");
  declare_writer_thread<stryke::TimestampNumber, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_thread, "TimestampNVec3l");
  declare_writer_thread<stryke::Date, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_thread, "DateVec3l");
  declare_writer_thread<stryke::DateNumber, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long, stryke::Long>(m_thread, "DateNVec3l");
  declare_writer_thread<stryke::Timestamp, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_thread, "TimestampVec3d");
  declare_writer_thread<stryke::TimestampNumber, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_thread, "TimestampNVec3d");
  declare_writer_thread<stryke::Date, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_thread, "DateVec3d");
  declare_writer_thread<stryke::DateNumber, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double, stryke::Double>(m_thread, "DateNVec3d");

  // ==============================================================
  // Custom Binding
  // ==============================================================

  auto m_custom = m.def_submodule("custom");

  declare_writer_thread<stryke::Timestamp, stryke::Int, stryke::Double>(m_custom, "TimestampIntDouble");
}