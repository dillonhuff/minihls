#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "minihls.h"

using namespace std;

module_type* float_add_l2_type(block& blk, const int width) {
  string name = "float_add_l2_" + to_string(width);
  if (blk.has_module_type(name)) {
    return blk.get_module_type(name);
  }

  vector<Port> pts{inpt("in0", width),
    inpt("in1", width),
    outpt("out", width)};

  return blk.add_module_type(name, pts);
}

module_type* phi_type(block& blk, const int width) {
  string name = "phi_" + to_string(width);
  if (blk.has_module_type(name)) {
    return blk.get_module_type(name);
  }

  vector<Port> pts{inpt("init_val", width),
    inpt("next_val", width),
    inpt("is_iter_0", 1),
    outpt("out", width)};

  string body = tab(1) + "assign out = is_iter_0 ? init_val : next_val;";

  return blk.add_module_type(name, pts, body);
}

module_type* uadd_type(block& blk, const int width) {
  string name = "uadd_" + to_string(width);
  if (blk.has_module_type(name)) {
    return blk.get_module_type(name);
  }

  vector<Port> pts{inpt("in0", width),
    inpt("in1", width),
    outpt("out", width)};

  string body = tab(1) + "assign out = in0 + in1;";
  return blk.add_module_type(name, pts, body);
}

module_type* wire_type(block& blk, const int width) {
  string name = "wire_" + to_string(width);
  if (blk.has_module_type(name)) {
    return blk.get_module_type(name);
  }

  vector<Port> pts{inpt("in", width), outpt("out", width)};
  string body = tab(1) + "assign out = in;";
  return blk.add_module_type(name, pts, body);
}

module_type* constant_type(block& blk, const int value, const int width) {
  string name = "const_" + to_string(value) + "_" + to_string(width);
  if (blk.has_module_type(name)) {
    return blk.get_module_type(name);
  }

  vector<Port> pts{outpt("out", width)};
  string body = tab(1) + "assign out = " + to_string(width) + "'d" + to_string(value) + ";";
  return blk.add_module_type(name, pts, body);
}

module_instance* get_constant(block& blk, const int value, int width) {
  string name = "val_" + to_string(value) + "_" + to_string(width);
  if (blk.has_inst(name)) {
    return blk.get_inst(name);
  }

  module_type* wtp = constant_type(blk, value, width);

  return blk.add_inst(name, wtp);
}

module_instance* get_wire(block& blk, const string& name, int width) {
  if (blk.has_inst(name)) {
    return blk.get_inst(name);
  }

  module_type* wtp = wire_type(blk, width);

  return blk.add_external_inst(name, wtp);
}

instruction_type* constant_instr(block& blk, const int value, int width) {
  string name = "constant_instr_" + to_string(value) + "_" + to_string(width);
  if (blk.has_instruction_type(name)) {
    return blk.get_instruction_type(name);
  }

  return blk.add_instruction_type(name);
}

instruction_type* float_add_l2_instr(block& blk, int width) {
  string name = "float_add_l2_instr_" + to_string(width);
  if (blk.has_instruction_type(name)) {
    return blk.get_instruction_type(name);
  }

  return blk.add_instruction_type(name);
}

instruction_type* phi_instr(block& blk, int width) {
  string name = "phi_instr_" + to_string(width);
  if (blk.has_instruction_type(name)) {
    return blk.get_instruction_type(name);
  }

  return blk.add_instruction_type(name);
}

instruction_type* uadd_instr(block& blk, int width) {
  string name = "uadd_instr_" + to_string(width);
  if (blk.has_instruction_type(name)) {
    return blk.get_instruction_type(name);
  }

  return blk.add_instruction_type(name);
}

instruction_binding* constant_binding(block& blk, const int value, int width) {
  string name = "constant_binding_" + to_string(value) + "_" + to_string(width);
  if (blk.has_instruction_binding(name)) {
    return blk.get_instruction_binding(name);
  }

  return blk.add_instruction_binding(name,
      constant_instr(blk, value, width),
      constant_type(blk, value, width), "out", {});
}

instruction_binding* phi_binding(block& blk, int width) {
  string name = "phi_binding_" + to_string(width);
  if (blk.has_instruction_binding(name)) {
    return blk.get_instruction_binding(name);
  }

  return blk.add_instruction_binding(name, phi_instr(blk, width), phi_type(blk, width), "out", {{0, "init_val"}, {1, "next_val"}});
}

instruction_binding* uadd_binding(block& blk, int width) {
  string name = "uadd_binding_" + to_string(width);
  if (blk.has_instruction_binding(name)) {
    return blk.get_instruction_binding(name);
  }

  return blk.add_instruction_binding(name, uadd_instr(blk, width), uadd_type(blk, width), "out", {{0, "in0"}, {1, "in1"}});
}

instruction_binding* float_add_l2_binding(block& blk, int width) {
  string name = "float_add_l2_binding_" + to_string(width);
  if (blk.has_instruction_binding(name)) {
    return blk.get_instruction_binding(name);
  }

  return blk.add_instruction_binding(name,
      float_add_l2_instr(blk, width),
      float_add_l2_type(blk, width),
      "out",
      {{0, "in0"}, {1, "in1"}},
      2);
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

instr* phi_node(block& blk, const string& arg_name, int width, instr* args) {
  instruction_binding* add_val = 
    phi_binding(blk, width);

  instruction_type* instr_tp =
    phi_instr(blk, width);

  auto instr = blk.add_instr(blk.unique_name(arg_name), instr_tp, {args});

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

instr* float_add_l2(block& blk, const string& arg_name, int width, const vector<instr*>& args) {
  instruction_binding* add_val = 
    float_add_l2_binding(blk, width);

  instruction_type* instr_tp =
    float_add_l2_instr(blk, width);

  auto instr = blk.add_instr(blk.unique_name(arg_name), instr_tp, args);

  return instr;

}

instr* constant(block& blk, const int value, int width) {
  module_instance* arg_wire =
    get_constant(blk, value, width);

  instruction_binding* rd_wire = 
    constant_binding(blk, value, width);

  instruction_type* instr_tp =
    constant_instr(blk, value, width);

  auto instr = blk.add_instr(blk.unique_name("const_" + to_string(value)), instr_tp);
  instr->bind_procedure(rd_wire);
  instr->bind_unit(arg_wire);

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
  REQUIRE(blk.arch.sched.num_stages() == 1);
}

TEST_CASE("One addition") {
  block blk;
  blk.name = "adder";

  auto a = wire_read(blk, "a", 32);
  auto b = wire_read(blk, "b", 32);
  auto sum = uadd(blk, "sum", 32, {a, b});
  auto wrout = wire_write(blk, "c", 32, sum);

  compile(blk);

  REQUIRE(blk.arch.sched.num_stages() == 1);
}

TEST_CASE("op with latency") {
  block blk;
  blk.name = "latency_op";

  auto a = wire_read(blk, "a", 32);
  auto b = wire_read(blk, "b", 32);
  auto sum = float_add_l2(blk, "sum", 32, {a, b});
  auto wrout = wire_write(blk, "c", 32, sum);

  compile(blk);

  REQUIRE(blk.arch.sched.num_stages() == 3);
}

TEST_CASE("phi node") {
  block blk;
  blk.name = "phi_test";

  auto zero = constant(blk, 0, 32);
  auto one = constant(blk, 1, 32);
  auto i = phi_node(blk, "i", 32, zero);
  auto next_i = uadd(blk, "next_i", 32, {i, one});
  i->operands.push_back(next_i);
  auto wrout = wire_write(blk, "c", 32, i);

  compile(blk);

  REQUIRE(blk.arch.sched.num_stages() == 1);
  int res = 
    system("verilator --cc phi_test.v phi_test_techlib.v --top-module phi_test");
  REQUIRE(res == 0);
}

TEST_CASE("predicated operation") {
  block blk;
  blk.name = "ram_write";

  auto ram = add_dual_port_ram(blk, 256, 8, 1);
  auto zero = constant(blk, 0, 8);
  auto one = constant(blk, 1, 8);

  auto i = phi_node(blk, "i", 8, zero);
  auto next_i = uadd(blk, "next_i", 8, {i, one});
  i->operands.push_back(next_i);
  auto i = write_ram(blk, ram, 256, 8, 1, {i, i});
  auto result = read_ream(blk, ram, 256, 8, 1, {i});
  auto wrout = wire_write(blk, "c", 8, result);

  compile(blk);

  REQUIRE(blk.arch.sched.num_stages() == 1);

  int res = 
    system("verilator --cc ram_write.v ram_write_techlib.v --top-module ram_write");
  REQUIRE(res == 0);
}
