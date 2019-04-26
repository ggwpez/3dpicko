#ifndef MINIMUMCUTPROBLEMSOLVER_HPP
#define MINIMUMCUTPROBLEMSOLVER_HPP

#include <functional>
#include <memory>
#include <random>
#include <unordered_map>
#include <vector>
#include "include/colony.h"

namespace c3picko {
class Colony;

class Node {
 public:
  Node(const std::unordered_map<int, Node*>& neighbours);
  int NumberOfCollisions() const;
  std::unordered_map<int, Node*> neighbours();
  int index() const;

  static bool orderByNumberOfCollisionsSmaller(Node const& a, Node const& b);
  static bool orderByNumberOfCollisionsGreater(Node const& a, Node const& b);

 private:
  int index_;
  std::unordered_map<int, Node*> neighbours_;
};

class MinimumCutProblemSolver {
 public:
  MinimumCutProblemSolver(const std::vector<Colony>& colonies);

  std::vector<Colony> Solve(int number_of_random_tries);

 private:
  const std::vector<Colony> colonies_;
  const std::vector<Node> nodes_;

  std::tuple<int, std::vector<Node>, int> SolveGreedyMax(
	  int h, std::vector<Node> binary_heap);
  std::tuple<int, std::vector<Node>, int> SolveGreedyMin(
	  int h, std::vector<Node> binary_heap);
  std::tuple<int, std::vector<Node>> SolveRandomBetweenMinAndMax(
	  int h, std::vector<Node> binary_heap,
	  const int amount_of_min_number_of_collisions,
	  const int amount_of_max_number_of_collisions, std::mt19937_64& rng);

  void ExpandCurrentState(
	  std::vector<Node>& old_state,
	  std::function<bool(const Node& node1, const Node& node2)> cmp);
  int EvaluateGreedyMax(std::vector<Node>& current_state, int& h);
  int EvaluateGreedyMin(std::vector<Node>& current_state, int& h);
  void EvaluateRandomBetweenMinAndMax(
	  std::vector<Node>& current_state, int& h,
	  const int amount_of_min_number_of_collisions,
	  const int amount_of_max_number_of_collisions, std::mt19937_64& rng);
  int GetAmountOfMaxOrMinNumberOfCollisions(
	  const std::vector<int>& number_of_collisions);
  std::vector<Node> MapColoniesToNodes(const std::vector<Colony>& colonies);
  std::vector<Colony> MapNodesToColonies(
	  const std::vector<Node>& nodes_solution);
  void UpdateNodesNeighbours(Node& node, std::vector<Node>& current_state);
  int GenerateHeuristic();
  void UpdateHeuristic(int& h, Node& removed_node);
};
}  // namespace c3picko

#endif  // MINIMUMCUTPROBLEMSOLVER_HPP
