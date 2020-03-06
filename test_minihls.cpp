#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "minihls.h"

using namespace std;

module_type wire_mod(const int w) {
  return module_type();
}

instruction_type wire_read(const int w) {
  return instruction_type();
}

module_type* uadd_type(block& blk, const int width) {
  string name = "uadd_" + to_string(width);
  if (blk.has_module_type(name)) {
    return blk.get_module_type(name);
  }

  vector<Port> pts{inpt("in0", width),
    inpt("in1", width),
    outpt("out", width)};

  return blk.add_module_type(name, pts);
}
module_type* wire_type(block& blk, const int width) {
  string name = "wire_" + to_string(width);
  if (blk.has_module_type(name)) {
    return blk.get_module_type(name);
  }

  vector<Port> pts{inpt("in", width), outpt("out", width)};
  return blk.add_module_type(name, pts);
}

module_instance* get_wire(block& blk, const string& name, int width) {
  if (blk.has_inst(name)) {
    return blk.get_inst(name);
  }

  module_type* wtp = wire_type(blk, width);

  return blk.add_external_inst(name, wtp);
}

instruction_type* uadd_instr(block& blk, int width) {
  string name = "uadd_instr_" + to_string(width);
  if (blk.has_instruction_type(name)) {
    return blk.get_instruction_type(name);
  }

  return blk.add_instruction_type(name);
}

instruction_binding* uadd_binding(block& blk, int width) {
  string name = "uadd_binding_" + to_string(width);
  if (blk.has_instruction_binding(name)) {
    return blk.get_instruction_binding(name);
  }

  return blk.add_instruction_binding(name, uadd_instr(blk, width), uadd_type(blk, width), "out", {{0, "in0"}, {1, "in1"}});
}

instruction_type* rd_wire_instr(block& blk, int width) {
  string name = "rd_wire_instr_" + to_string(width);
  if (blk.has_instruction_type(name)) {
    return blk.get_instruction_type(name);
  }

  return blk.add_instruction_type(name);
}

instruction_binding* rd_wire_binding(block& blk, int width) {
  string name = "rd_wire_binding_" + to_string(width);
  if (blk.has_instruction_binding(name)) {
    return blk.get_instruction_binding(name);
  }

  return blk.add_instruction_binding(name, rd_wire_instr(blk, width), wire_type(blk, width), "out", {});
}

instruction_type* wr_wire_instr(block& blk, int width) {
  string name = "wr_wire_instr_" + to_string(width);
  if (blk.has_instruction_type(name)) {
    return blk.get_instruction_type(name);
  }

  return blk.add_instruction_type(name);
}

instruction_binding* wr_wire_binding(block& blk, int width) {
  string name = "wr_wire_binding_" + to_string(width);
  if (blk.has_instruction_binding(name)) {
    return blk.get_instruction_binding(name);
  }

  return blk.add_instruction_binding(name, wr_wire_instr(blk, width), wire_type(blk, width), "", {{0, "in"}});
}

instr* wire_write(block& blk, const string& arg_name, int width, instr* arg) {
  module_instance* arg_wire =
    get_wire(blk, arg_name, width);

  instruction_binding* rd_wire = 
    wr_wire_binding(blk, width);

  instruction_type* instr_tp =
    wr_wire_instr(blk, width);

  auto instr = blk.add_instr(blk.unique_name("wr"), instr_tp, {arg});
  instr->bind_procedure(rd_wire);
  instr->bind_unit(arg_wire);

  return instr;
}

instr* uadd(block& blk, const string& arg_name, int width, const vector<instr*>& args) {
  instruction_binding* add_val = 
    uadd_binding(blk, width);

  instruction_type* instr_tp =
    uadd_instr(blk, width);

  auto instr = blk.add_instr(blk.unique_name(arg_name), instr_tp, args);

  return instr;

}

instr* wire_read(block& blk, const string& arg_name, int width) {
  module_instance* arg_wire =
    get_wire(blk, arg_name, width);

  instruction_binding* rd_wire = 
    rd_wire_binding(blk, width);

  instruction_type* instr_tp =
    rd_wire_instr(blk, width);

  auto instr = blk.add_instr(blk.unique_name("rd"), instr_tp);
  instr->bind_procedure(rd_wire);
  instr->bind_unit(arg_wire);

  return instr;
}

TEST_CASE("Create program") {
  block blk;
  blk.name = "wire_blk";

  auto rdin = wire_read(blk, "val_in", 32);
  auto wrout = wire_write(blk, "val_out", 32, rdin);

  compile(blk);
}


TEST_CASE("One addition") {
  block blk;
  blk.name = "adder";

  auto a = wire_read(blk, "a", 32);
  auto b = wire_read(blk, "b", 32);
  auto sum = uadd(blk, "sum", 32, {a, b});
  auto wrout = wire_write(blk, "c", 32, sum);

  compile(blk);
}
