#pragma once

#include <string>
#include "algorithm.h"

using namespace dbhc;
using namespace std;

#define INT_INF 999999

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

    string system_verilog_type_string() const {
      return "logic [" + to_string(width - 1) + ":0]";
    }

    string get_name() {
      return name;
    }
};

Port outpt(const string& s, const int w) {
  return {s, w, false};
}

Port inpt(const string& s, const int w) {
  return {s, w, true};
}

class module_type {
  public:

    string name;
    vector<Port> ports;
};

class module_instance {

  public:

    string name;
    module_type* tp;
    bool internal;

    vector<Port> ports() const {
      return tp->ports;
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
    string name;
    module_instance* unit;
    vector<instruction_instance*> operands;

    string get_name() const {
      return name;
    }


    module_instance* get_unit() const {
      return unit;
    }

    void bind_unit(module_instance* u) {
      unit = u;
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

class micro_architecture {
  public:
    schedule sched;
    map<instr*, map<int, string> > source_registers;
    map<instr*, map<int, string> > source_wires;
};

template<typename N, typename E>
class DirectedGraph {
  public:
    map<N, set<pair<N, E > > > out_edges;

    set<pair<pair<N, N>, E> > edges() const {
      set<pair<pair<N, N>, E> > es;

      for (auto edge_set : out_edges) {
        for (auto e : edge_set.second) {
          es.insert({{edge_set.first, e.first}, e.second});
        }
      }
      return es;
    }

    set<N> vertices() const {
      set<N> verts;
      for (auto e : out_edges) {
        verts.insert(e.first);
      }
      return verts;
    }

    void add_edge(const N& a, const N& b, const E& v) {
      out_edges[a].insert({b, v});
    }

    void add_node(const N& v) {
      out_edges[v] = {};
    }
};

template<typename N, typename E>
void diff_lte(DirectedGraph<N, E>& g, const N& a, const N& b, const E& v) {
  g.add_edge(b, a, v);
}

template<typename N, typename E>
void diff_eq(DirectedGraph<N, E>& g, const N& a, const N& b, const E& v) {
  diff_lte(g, a, b, v);
  diff_lte(g, a, b, -v);
}

string endstr(instr* i) {
  return "$end_" + i->get_name();
}

string startstr(instr* i) {
  return "$start_" + i->get_name();
}

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

  set<instr*> bound_instrs(module_instance* inst) {
    set<instr*> bnd;
    for (auto instr : instrs) {
      if (instr.second->get_unit() == inst) {
        bnd.insert(instr.second);
      }
    }
    return bnd;
  }

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

  // Primitive difference logic solver. For details see:
  // https://www.cs.upc.edu/~erodri/webpage/cps/theory/sat/SMT-DL/slides.pdf
  void asap_schedule() {
    cout << "Scheduling" << endl;

    DirectedGraph<string, int> constraints;
    constraints.add_node("$base");
    for (pair<string, instr*> instr : instrs) {
      cout << "Adding instruction to graph" << endl;
      auto i = instr.second;
      constraints.add_node(startstr(i));
      constraints.add_node(endstr(i));

      constraints.add_edge("$base", startstr(i), 0);
      constraints.add_edge("$base", endstr(i), 0);

      // Assume latency 0 for now
      diff_eq(constraints, startstr(i), endstr(i), 1);
    }
    
    for (pair<string, instr*> instr : instrs) {
      for (auto op : instr.second->operands) {
        diff_lte(constraints,
            endstr(op),
            startstr(instr.second),
            0);
      }
    }

    map<string, int> distance;
    map<string, string> predecessor;
    for (auto v : constraints.vertices()) {
      distance[v] = INT_INF;
      predecessor[v] = "";
    }
    distance["$base"] = 0;

    for (int i = 1; i < constraints.vertices().size(); i++) {
      for (auto e : constraints.edges()) {
        auto u = e.first.first;
        auto v = e.first.second;
        auto w = e.second;
        if (distance[u] + w < distance[v]) {
          distance[v] = distance[u] + w;
          predecessor[v] = u;
        }
      }
    }

    int min_distance = INT_INF;
    for (auto v : constraints.vertices()) {
      cout << tab(1) << v << " -> " << distance[v] << endl;
      if (distance[v] < min_distance) {
        min_distance = distance[v];
      }
    }

    for (auto e : constraints.edges()) {
      auto u = e.first.first;
      auto v = e.first.second;
      auto w = e.second;
      if (distance[u] + w < distance[v]) {
        cout << "Error: Graph contains a negative weight cycle with: " << u << ", and " << v << endl;
        assert(false);
      }
    }

    int shift = -min_distance;
    schedule sched;
    for (auto instr : instrs) {
      auto i = instr.second;
      sched.start_times[i] = distance[startstr(i)] + shift;
      sched.end_times[i] = distance[endstr(i)] + shift;
    }

    cout << "Done with scheduling" << endl;
    for (auto instrP : instrs) {
      auto instr = instrP.second;
      cout << "Start of " << startstr(instr) << " -> " << sched.start_times[instr] << endl;
      cout << "End of " << startstr(instr) << " -> " << sched.end_times[instr] << endl;
    }
  }

  instr* add_instr(const std::string& name, instruction_type* tp) {
    vector<instr*> noargs;
    return add_instr(name, tp, noargs);
  }
  
  instr* add_instr(const std::string& name, instruction_type* tp,
      const vector<instr*>& args) {

    assert(!contains_key(name, instrs));

    auto instr = new instruction_instance();
    instr->name = name;
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

  module_type* add_module_type(const std::string& name, const vector<Port>& pts) {
    auto inst = new module_type();
    inst->name = name;
    inst->ports = pts;
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
    inst->name = name;
    inst->tp = tp;
    inst->internal = false;
    instances[name] = inst;
    return inst;
  }

  module_instance* add_inst(const std::string& name, module_type* tp) {
    auto inst = new module_instance();
    inst->name = name;
    inst->tp = tp;
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
  blk.asap_schedule();
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
  vector<string> pts{"input clk", "input rst", "input start", "output done"};
  for (auto m : blk.instance_set()) {
    if (m->is_external()) {
      for (auto pt : m->ports()) {
        pts.push_back(pt.system_verilog_type_string() + " " + m->get_name() + "_" + pt.get_name());
      }
    }
  }

  out << "module " << blk.name << "(" << comma_list(pts) << ");" << endl;
  for (auto m : blk.instance_set()) {
    if (m->is_internal()) {
      out << tab(1) << "// " << m->get_name() << endl;
    }

    out << tab(1) << "// Bindings to " << m->get_name() << endl;
    for (auto bound_instr : blk.bound_instrs(m)) {
      out << tab(2) << "// " << bound_instr->get_name() << endl;
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

