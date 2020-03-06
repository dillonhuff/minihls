#pragma once

#include <string>
#include "algorithm.h"

using namespace dbhc;
using namespace std;


class module_type {

};

class module_instance {

};

class instruction_type {

};

class instruction_binding {

};

class instruction_instance {
  
  public:

    vector<instruction_instance*> operands;

    void bind_unit(module_instance* unit) {

    }

    void bind_procedure(instruction_binding* p) {

    }
};

typedef instruction_instance instr;

class schedule {

  public:

  map<instr*, int> start_times;
  map<instr*, int> end_times;

};

class block {

  int un;
  map<string, instr*> instrs;
  map<string, module_instance*> instances;
  map<string, module_type*> module_types;
  map<string, instruction_binding*> instruction_bindings;
  map<string, instruction_type*> instruction_types;

  public:

  block() : un(0) {}

  std::string unique_name(const string& pre) {
    int n = un;
    un++;
    return pre + "_" + to_string(n);
  }

  void asap_schedule() {

    //schedule sched;

    //set<instr*> active;
    //set<instr*> done;    

    //int time = 0;
    //while (done.size() < instrs.size()) {

    //}

    cout << "Done with scheduling" << endl;
  }

  instr* add_instr(const std::string& name, instruction_type* tp) {
    vector<instr*> noargs;
    return add_instr(name, tp, noargs);
  }
  
  instr* add_instr(const std::string& name, instruction_type* tp,
      const vector<instr*>& args) {

    assert(!contains_key(name, instrs));

    auto instr = new instruction_instance();
    instr->operands = args;
    instrs[name] = instr;

    return instr;
  }

  instruction_binding* add_instruction_binding(const std::string& name) {
    auto inst = new instruction_binding();
    instruction_bindings[name] = inst;
    return inst;
  }

  instruction_binding* get_instruction_binding(const std::string& name) const {
    assert(has_instruction_binding(name));
    return map_find(name, instruction_bindings);
  }

  bool has_instruction_binding(const std::string& name) const {
    return contains_key(name, instruction_bindings);
  }

  instruction_type* add_instruction_type(const std::string& name) {
    auto inst = new instruction_type();
    instruction_types[name] = inst;
    return inst;
  }

  instruction_type* get_instruction_type(const std::string& name) const {
    assert(has_instruction_type(name));
    return map_find(name, instruction_types);
  }

  bool has_instruction_type(const std::string& name) const {
    return contains_key(name, instruction_types);
  }

  module_type* add_module_type(const std::string& name) {
    auto inst = new module_type();
    module_types[name] = inst;
    return inst;
  }

  module_type* get_module_type(const std::string& name) const {
    assert(has_module_type(name));
    return map_find(name, module_types);
  }

  bool has_module_type(const std::string& name) const {
    return contains_key(name, module_types);
  }

  module_instance* add_inst(const std::string& name, module_type* tp) {
    auto inst = new module_instance();
    instances[name] = inst;
    return inst;
  }

  module_instance* get_inst(const std::string& name) const {
    assert(has_inst(name));
    return map_find(name, instances);
  }

  bool has_inst(const std::string& name) const {
    return contains_key(name, instances);
  }

};

