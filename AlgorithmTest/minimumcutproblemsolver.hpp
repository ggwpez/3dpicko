#ifndef MINIMUMCUTPROBLEMSOLVER_HPP
#define MINIMUMCUTPROBLEMSOLVER_HPP

#include <algorithm>
#include <memory>
#include <vector>

namespace c3picko {
class Colony;

enum EvaluationPolicy { kGreedyMax, kGreedyMin, kRandomBetweenMinAndMax };

class Node {
 public:
  Node(const std::vector<std::shared_ptr<Node>> &neighbours);
  int NumberOfCollisions();
  std::vector<std::shared_ptr<Node>> neighbours() const;

 private:
  const int index_;
  const std::vector<std::shared_ptr<Node>> neighbours_;
};

class MinimumCutProblemSolver {
 public:
  MinimumCutProblemSolver(const std::vector<Colony> &colonies);

  std::vector<Colony> Solve(
      EvaluationPolicy eval_policy);  // TODO allow to pass solving policy

  std::vector<Node> ExpandCurrentState(std::vector<Node> &old_state);

  std::vector<Node> EvaluateGreedyMax(std::vector<Node> &current_state);

  int GenerateHeuristic();

 private:
  const std::vector<Colony> colonies_;
  const std::vector<Node> nodes_;

  std::vector<Node> MapColoniesToNodes(const std::vector<Colony> &colonies);
  void UpdateNodesNeighbours(Node &node, std::vector<Node> &current_state);
  void UpdateHeuristic(int &h, Node &removed_node);
};
}  // namespace c3picko

#endif  // MINIMUMCUTPROBLEMSOLVER_HPP
