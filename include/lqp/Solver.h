#ifndef LQP_SOLVER_H
#define LQP_SOLVER_H

#include <memory>
#include <chrono>
#include <filesystem>

#include "Api.h"
#include "Problem.h"

namespace lqp {

  enum class SolverStatus {
    Error,
    Optimal,
    Feasible,
    Infeasible,
    NoFeasibleSolution,
    UnboundedSolution,
    Undefined,
    NotSolved,
  };

  struct LQP_API SolverConfig {
    bool use_mip = false;
    bool verbose = true;
    bool presolve = false;
    std::chrono::milliseconds timeout = std::chrono::milliseconds::max();
    std::filesystem::path problem_output;
    std::filesystem::path solution_output;
  };

  class LQP_API Solver {
  public:
    virtual ~Solver();

    virtual bool available() const = 0;
    virtual SolverStatus solve(const Problem& problem, const SolverConfig& config = SolverConfig()) = 0;

  protected:
    static std::vector<Problem::Variable> variables(const Problem& problem);
    static std::vector<Problem::Constraint> constraints(const Problem& problem);
    static Problem::Objective objective(const Problem& problem);
  };

  enum class SolverImplementation {
    Null,
    Glpk,
    Gurobi,
  };

  LQP_API std::unique_ptr<Solver> make_solver(SolverImplementation implementation);

  class LQP_API NullSolver : public Solver {
  public:
    bool available() const override;
    SolverStatus solve(const Problem& problem, const SolverConfig& config) override;
  };

}


#endif // LQP_SOLVER_H
