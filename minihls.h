#pragma once

#include <string>
#include "algorithm.h"

using namespace dbhc;
using namespace std;

std::string tab(const int i) {
  string s = "";
  for (int k = 0; k < i; k++) {
    s += "  ";
  }
  return s;
}

class Port {
  public:

    string name;
    int width;
    bool is_in;

    string get_name() {
      return name;
    }
};

class module_type {

};

class module_instance {

  public:

    string name;
    module_type* tp;
    bool internal;

    vector<Port> ports() const {
      return {};
    }

    string get_name() const {
      return name;
    }

    bool is_internal() const {
      return internal;
    }

    bool is_external() const {
      return !internal;
    }
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

  string name;

  block() : un(0) {}

  std::string unique_name(const string& pre) {
    int n = un;
    un++;
    return pre + "_" + to_string(n);
  }

  set<module_instance*> instance_set() {
    set<module_instance*> insts;
    for (auto i : instances) {
      insts.insert(i.second);
    }
    return insts;
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

  module_instance* add_external_inst(const std::string& name, module_type* tp) {
    auto inst = new module_instance();
    inst->internal = false;
    instances[name] = inst;
    return inst;
  }

  module_instance* add_inst(const std::string& name, module_type* tp) {
    auto inst = new module_instance();
    inst->name = name;
    inst->internal = true;
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

static inline
void asap_schedule(block& blk) {
}

static inline
void finish_binding(block& blk) {
}

static inline
std::string sep_list(const std::vector<std::string>& strs, const std::string& ldelim, const std::string& rdelim, const std::string& sep) {
  string res = ldelim;

  if (strs.size() > 0) {
    for (size_t i = 0; i < strs.size(); i++) {
      res += strs[i];
      if (strs.size() > 1 && i < strs.size() - 1) {
        res += sep;
      }
    }
  }
  res += rdelim;

  return res;
}

template<typename T>
static inline
std::string sep_list(const std::vector<T>& vals, const std::string& ldelim, const std::string& rdelim, const std::string& sep) {
  vector<string> strs;
  for (auto v : vals) {
    ostringstream ss;
    ss << v;
    strs.push_back(ss.str());
  }
  return sep_list(vals, ldelim, rdelim, sep);
}

static inline
std::string comma_list(const std::vector<std::string>& strs) {
  return sep_list(strs, "", "", ", ");
}

static inline
void emit_verilog(block& blk) {
  ofstream out(blk.name + ".v");
  vector<string> pts{"input clk", "input rst"};
  for (auto m : blk.instance_set()) {
    if (m->is_external()) {
      for (auto pt : m->ports()) {
        pts.push_back(m->get_name() + "_" + pt.get_name());
      }
    }
  }

  out << "module " << blk.name << "(" << comma_list(pts) << ");" << endl;
  for (auto m : blk.instance_set()) {
    if (m->is_internal()) {
      out << tab(1) << "// " << m->get_name() << endl;
    }
  }
  out << "endmodule" << endl;
  out.close();
}

static inline
void compile(block& blk) {
  finish_binding(blk);
  asap_schedule(blk);
  emit_verilog(blk);
}

