#include "minimumcutproblemsolver.hpp"

#include <numeric>

c3picko::Node::Node(
    const std::vector<std::shared_ptr<c3picko::Node>> &neighbours)
    : index_(neighbours.size()), neighbours_(neighbours) {}

int c3picko::Node::NumberOfCollisions() { return neighbours_.size(); }

std::vector<std::shared_ptr<c3picko::Node>> c3picko::Node::neighbours() const {
  return neighbours_;
}

c3picko::MinimumCutProblemSolver::MinimumCutProblemSolver(
    const std::vector<Colony> &colonies)
    : colonies_(colonies), nodes_(MapColoniesToNodes(colonies)) {}

std::vector<c3picko::Colony> c3picko::MinimumCutProblemSolver::Solve(
    EvaluationPolicy eval_policy) {
  int h = c3picko::MinimumCutProblemSolver::GenerateHeuristic();
  std::vector<Node> binary_heap(nodes_);

  while (h > 0) {
    ExpandCurrentState(binary_heap);
    switch (eval_policy) {
      case kGreedyMax:
        binary_heap = EvaluateGreedyMax(binary_heap);
        break;
      case kGreedyMin:;
        break;
      case kRandomBetweenMinAndMax:;
    }
  }
}

std::vector<c3picko::Node> c3picko::MinimumCutProblemSolver::ExpandCurrentState(
    std::vector<Node> &old_state) {
  std::make_heap(old_state.begin(), old_state.end());
}

std::vector<c3picko::Node> c3picko::MinimumCutProblemSolver::EvaluateGreedyMax(
    std::vector<Node> &current_state) {
  std::pop_heap(current_state.begin(), current_state.end());
  Node &removed_node = current_state.back();
  UpdateNodesNeighbours(removed_node, current_state);
  current_state.pop_back();
}

int c3picko::MinimumCutProblemSolver::GenerateHeuristic() {
  return std::accummulate(nodes_.begin(), nodes_.end(), 0);
}

std::vector<c3picko::Node> c3picko::MinimumCutProblemSolver::MapColoniesToNodes(
    const std::vector<c3picko::Colony> &colonies) {
  // TODO implement
}

void c3picko::MinimumCutProblemSolver::UpdateNodesNeighbours(
    c3picko::Node &node, std::vector<Node> &current_state) {
  std::for_each(node.neighbours().begin(), node.neighbours().end(),
                [](Node &node) -> {

                })
}

void c3picko::MinimumCutProblemSolver::UpdateHeuristic(
    int &h, c3picko::Node &removed_node) {
  h -= removed_node.NumberOfCollisions() * 2;
}
