#ifndef MINIMUMCUTPROBLEMSOLVER_HPP
#define MINIMUMCUTPROBLEMSOLVER_HPP

#include <algorithm>
#include <memory>
#include <vector>
#include <unordered_map>

namespace c3picko {
class Colony;

class Node {
 public:
  Node(const std::unordered_map<int, Node*>& neighbours);
  int NumberOfCollisions() const;
  std::unordered_map<int, Node*> neighbours();
  int index() const;


 private:
  const int index_;
  std::unordered_map<int, Node*> neighbours_;
};

class MinimumCutProblemSolver {
 public:
  MinimumCutProblemSolver(const std::vector<Colony>& colonies);

  std::vector<Colony> Solve(int number_of_random_tries);


 private:
  const std::vector<Colony> colonies_;
  const std::vector<Node> nodes_;

  std::tuple<int, std::vector<Node>> SolveGreedyMax(int h, std::vector<Node> binary_heap);
  std::tuple<int, std::vector<Node>> SolveGreedyMin(int h, std::vector<Node> binary_heap);
  std::tuple<int, std::vector<Node>> SolveRandomBetweenMinAndMax(int h, std::vector<Node> binary_heap, const std::mt19937& rng);

  void ExpandCurrentState(std::vector<Node>& old_state, std::function<bool(const Node& node1, const Node& node2)> cmp);
  void EvaluateGreedyMax(std::vector<Node>& current_state, int& h);
  void EvaluateGreedyMin(std::vector<Node>& current_state, int& h);
  void EvaluateRandomBetweenMinAndMax(std::vector<Node>& current_state, int& h, const std::mt19937& rng);

  std::vector<Node> MapColoniesToNodes(const std::vector<Colony>& colonies);
  std::vector<Colony> MapNodesToColonies(const std::vector<Node>& nodes_solution);
  void UpdateNodesNeighbours(Node& node, std::vector<Node>& current_state);
  int GenerateHeuristic();
  void UpdateHeuristic(int& h, Node& removed_node);
};
}  // namespace c3picko

#endif  // MINIMUMCUTPROBLEMSOLVER_HPP
