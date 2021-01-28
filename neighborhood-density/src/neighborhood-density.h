// -----------------------------------------------------------------------------
//
// Copyright (C) The BioDynaMo Project.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
//
// See the LICENSE file distributed with this work for details.
// See the NOTICE file distributed with this work for additional information
// regarding copyright ownership.
//
// -----------------------------------------------------------------------------
#ifndef NEIGHBORHOOD_DENSITY_H_
#define NEIGHBORHOOD_DENSITY_H_

#include "biodynamo.h"
#include "core/operation/operation_registry.h"
#include "core/operation/reduction_op.h"

namespace bdm {

struct SimParam : public ParamGroup {
  BDM_PARAM_GROUP_HEADER(SimParam, 1);

  double diameter = 10;
  double interaction_radius = 20;
  double min_bound = 0;
};

struct CheckNumNeighbors : public Functor<void, Agent*, int*> {
  CheckNumNeighbors() {
    auto* sparam = Simulation::GetActive()->GetParam()->Get<SimParam>();
    radius_ = sparam->interaction_radius;
  }

  struct IncrementPerNeighbor : Functor<void, const Agent*, double> {
    explicit IncrementPerNeighbor(int* tl_result) : tl_result_(tl_result) {}
    void operator()(const Agent* neighbor, double squared_distance) override {
      (*tl_result_)++;
    }

    int* tl_result_ = nullptr;
  };

  void operator()(Agent* agent, int* tl_result) {
    auto* ctxt = Simulation::GetActive()->GetExecutionContext();
    IncrementPerNeighbor count_neighbors(tl_result);
    ctxt->ForEachNeighborWithinRadius(count_neighbors, *agent, radius_);
  }

  double radius_ = 0.0;
};

inline int Simulate(int argc, const char** argv) {
  auto opts = CommandLineOptions(argc, argv);
  opts.AddOption<uint64_t>("m, max-bound", "50", "The maximum dimension length");
  opts.AddOption<uint64_t>("p, population", "2000", "Population");
  opts.AddOption<uint64_t>("i, iterations", "5", "Number of timesteps");
  uint64_t max_bound = opts.Get<uint64_t>("max-bound");
  uint64_t population = opts.Get<uint64_t>("population");
  uint64_t iterations = opts.Get<uint64_t>("iterations");
  Param::RegisterParamGroup(new SimParam());
  Simulation simulation(&opts);

  auto* op = NewOperation("ReductionOpInt");
  auto* op_impl = op->GetImplementation<ReductionOp<int>>();
  op_impl->Initialize(new CheckNumNeighbors(), new SumReduction<int>());
  simulation.GetScheduler()->ScheduleOp(op);

  auto* sparam = simulation.GetParam()->Get<SimParam>();

  auto agent_builder = [&](Double3 position) {
    auto cell = new Cell(position);
    cell->SetDiameter(sparam->diameter);
    return cell;
  };
  ModelInitializer::CreateAgentsRandom(sparam->min_bound, max_bound,
                                       population, agent_builder);

  // Run simulation for one timestep
  auto start = Timing::Timestamp();
  simulation.GetScheduler()->Simulate(iterations);
  auto stop = Timing::Timestamp();
  std::cout << (stop - start) << std::endl;

  // std::cout << "Initial density = "
  //           << static_cast<float>(op_impl->GetResults()[0]) /
  //                  simulation.GetResourceManager()->GetNumAgents()
  //           << " neighbors / agent" << std::endl;

  float avg_density_over_time = 0;
  for (auto res : op_impl->GetResults()) {
    avg_density_over_time += static_cast<float>(res) /
                             simulation.GetResourceManager()->GetNumAgents();
  }

  avg_density_over_time = avg_density_over_time / iterations;

  // std::cout << "Average density = " << avg_density_over_time
  //           << " neighbors / agent / timestep" << std::endl;

  // std::cout << "Simulation completed successfully!" << std::endl;
  return 0;
}

}  // namespace bdm

#endif  // NEIGHBORHOOD_DENSITY_H_
