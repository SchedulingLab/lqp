#ifndef LQP_SOLVER_H
#define LQP_SOLVER_H

#include <memory>
#include <chrono>
#include <filesystem>

#include "Api.h"
#include "Problem.h"
#include "Solution.h"

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

  class LQP_API SolverResult {
  public:
    SolverResult(SolverStatus status);
    SolverResult(SolverStatus status, Solution solution);

    SolverStatus status() const;

    bool has_solution() const;
    Solution solution();

  private:
    SolverStatus m_status;
    Solution m_solution;
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
    Solver() = default;
    virtual ~Solver();

    Solver(const Solver&) = delete;
    Solver& operator=(const Solver&) = delete;

    Solver(Solver&&) = default;
    Solver& operator=(Solver&&) = default;

    virtual bool available() const = 0;
    virtual SolverResult solve(const Problem& problem, const SolverConfig& config = SolverConfig()) = 0;

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
    SolverResult solve(const Problem& problem, const SolverConfig& config) override;
  };

}


#endif // LQP_SOLVER_H
