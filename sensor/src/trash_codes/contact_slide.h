#define PY_SSIZE_T_CLEAN
#include <python2.7/Python.h>

class PythonRunner {
public:

  void run_sliding() {
    PyObject* module = PyImport_ImportModule("rospy");
    if (!module) {
      std::cerr << "Failed to import the rospy module\n";
      Py_Finalize();
      return;
    }
    // Run the Python script
    FILE* script_file = fopen("/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/move_up_down.py", "r");
    if (script_file) {
      PyRun_SimpleFile(script_file, "/home/rass/Rassul/ur_ws/src/universal_robot/ur_driver/move_up_down.py.py");
      fclose(script_file);
    } else {
      std::cerr << "Failed to open the Python script file\n";
    }
    Py_DECREF(module);
    return 
  }
  
};