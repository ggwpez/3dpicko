#include "minimumcutproblemsolver.hpp"

#include <limits>
#include <numeric>
#include <random>

c3picko::Node::Node(const std::unordered_map<int, Node*>& neighbours)
    : index_(neighbours.size()), neighbours_(neighbours) {}

int c3picko::Node::NumberOfCollisions() const { return neighbours_.size(); }

std::unordered_map<int, c3picko::Node*> c3picko::Node::neighbours() {
  return neighbours_;
}

int c3picko::Node::index() const { return index_; }

c3picko::MinimumCutProblemSolver::MinimumCutProblemSolver(
    const std::vector<Colony>& colonies)
    : colonies_(colonies), nodes_(MapColoniesToNodes(colonies)) {}

std::vector<c3picko::Colony> c3picko::MinimumCutProblemSolver::Solve(
    int number_of_random_tries) {
  int amount_of_min_number_of_collisions;
  int amount_of_max_number_of_collisions;

  int h = c3picko::MinimumCutProblemSolver::GenerateHeuristic();

  int current_least_steps = std::numeric_limits<int>::max();
  std::vector<Node> optimal_solution;

  std::tuple<int, std::vector<c3picko::Node>, int> solution_greedy_min =
      SolveGreedyMin(h, nodes_);
  if (std::get<0>(solution_greedy_min) < current_least_steps) {
    current_least_steps = std::get<0>(solution_greedy_min);
    optimal_solution = std::get<1>(solution_greedy_min);
  }
  amount_of_min_number_of_collisions = std::get<2>(solution_greedy_min);

  std::tuple<int, std::vector<c3picko::Node>, int> solution_greedy_max =
      SolveGreedyMax(h, nodes_);
  current_least_steps = std::get<0>(solution_greedy_max);
  optimal_solution = std::get<1>(solution_greedy_max);
  amount_of_max_number_of_collisions = std::get<2>(solution_greedy_max);

  std::random_device rd;  // only used once to initialise (seed) engine
  std::mt19937 rng(
      rd());  // random-number engine used (Mersenne-Twister in this case)

  for (int i = 0; i < number_of_random_tries; ++i) {
    std::tuple<int, std::vector<c3picko::Node>>
        solution_random_between_min_max = SolveRandomBetweenMinAndMax(
            h, nodes_, amount_of_min_number_of_collisions,
            amount_of_max_number_of_collisions, rng);
    if (std::get<0>(solution_random_between_min_max) < current_least_steps) {
      current_least_steps = std::get<0>(solution_random_between_min_max);
      optimal_solution = std::get<1>(solution_random_between_min_max);
    }
  }

  return MapNodesToColonies(optimal_solution);
}

std::tuple<int, std::vector<c3picko::Node>, int>
c3picko::MinimumCutProblemSolver::SolveGreedyMax(
    int h, std::vector<Node> binary_heap) {
  int i = 0;
  std::vector<int> number_of_collisions;
  while (h > 0) {
    ExpandCurrentState(binary_heap, [](const Node& node1, const Node& node2) {
      return node1.NumberOfCollisions() > node2.NumberOfCollisions();
    });
    number_of_collisions.push_back(EvaluateGreedyMax(binary_heap, h));
    ++i;
  }
  return std::make_tuple(
      i, binary_heap,
      GetAmountOfMaxOrMinNumberOfCollisions(number_of_collisions));
}

std::tuple<int, std::vector<c3picko::Node>, int>
c3picko::MinimumCutProblemSolver::SolveGreedyMin(
    int h, std::vector<Node> binary_heap) {
  int i = 0;
  std::vector<int> number_of_collisions;
  while (h > 0) {
    ExpandCurrentState(binary_heap, [](const Node& node1, const Node& node2) {
      return node1.NumberOfCollisions() < node2.NumberOfCollisions();
    });
    number_of_collisions.push_back(EvaluateGreedyMin(binary_heap, h));
    ++i;
  }

  return std::make_tuple(
      i, binary_heap,
      GetAmountOfMaxOrMinNumberOfCollisions(number_of_collisions));
}

std::tuple<int, std::vector<c3picko::Node>>
c3picko::MinimumCutProblemSolver::SolveRandomBetweenMinAndMax(
    int h, std::vector<Node> binary_heap,
    const int amount_of_min_number_of_collisions,
    const int amount_of_max_number_of_collisions, const std::mt19937& rng) {
  int i = 0;
  while (h > 0) {
    ExpandCurrentState(binary_heap, [](const Node& node1, const Node& node2) {
      return node1.NumberOfCollisions() < node2.NumberOfCollisions();
    });
    EvaluateRandomBetweenMinAndMax(binary_heap, h,
                                   amount_of_min_number_of_collisions,
                                   amount_of_max_number_of_collisions, rng);
    ++i;
  }

  return std::make_tuple(i, binary_heap);
}

void c3picko::MinimumCutProblemSolver::ExpandCurrentState(
    std::vector<Node>& old_state,
    std::function<bool(const Node&, const Node&)> cmp) {
  std::make_heap(old_state.begin(), old_state.end(), cmp);
}

int c3picko::MinimumCutProblemSolver::EvaluateGreedyMax(
    std::vector<Node>& current_state, int& h) {
  std::pop_heap(current_state.begin(), current_state.end());
  Node& removed_node = current_state.back();
  UpdateNodesNeighbours(removed_node, current_state);
  UpdateHeuristic(h, removed_node);
  int number_of_collisions = removed_node.NumberOfCollisions();
  current_state.pop_back();
  return number_of_collisions;
}

int c3picko::MinimumCutProblemSolver::EvaluateGreedyMin(
    std::vector<c3picko::Node>& current_state, int& h) {
  std::pop_heap(current_state.begin(), current_state.end());
  Node& removed_node = current_state.back();
  UpdateNodesNeighbours(removed_node, current_state);
  UpdateHeuristic(h, removed_node);
  int number_of_collisions = removed_node.NumberOfCollisions();
  current_state.pop_back();
  return number_of_collisions;
}

void c3picko::MinimumCutProblemSolver::EvaluateRandomBetweenMinAndMax(
    std::vector<c3picko::Node>& current_state, int& h,
    const int amount_of_min_number_of_collisions,
    const int amount_of_max_number_of_collisions, const std::mt19937& rng) {
  int range_min;
  int range_max;
  if (amount_of_min_number_of_collisions + amount_of_max_number_of_collisions >=
      current_state.size()) {
    // in case of bigger than: every collision causing colony causes the same
    // amount of collisions, hence min and max are the same; in case of equal:
    // we can only choose the min or max value; conclusion: we choose any other
    // node apart from the ones we already started with in greedy evalutation
    range_min = 1;
    range_max = current_state.size() - 1;
  } else {
    range_min = amount_of_min_number_of_collisions + 1;
    range_max = current_state.size() - amount_of_max_number_of_collisions;
  }

  std::uniform_int_distribution<int> uni(
      range_min,
      range_max);  // ordered from min to max
                   // value, see lamba in call on
                   // ExpandCurrentState in
                   // SolverRandomBetweenMinAndMax
  auto random_integer = uni(rng);

  for (int i = 0; i < random_integer; ++i)
    std::pop_heap(current_state.begin(), current_state.end());

  Node& removed_node = current_state.back();
  UpdateNodesNeighbours(removed_node, current_state);
  UpdateHeuristic(h, removed_node);
  current_state.pop_back();
}

int c3picko::MinimumCutProblemSolver::GetAmountOfMaxOrMinNumberOfCollisions(
    const std::vector<int>& number_of_collisions) {
  int min_or_max_number_of_collisions = number_of_collisions.front();
  int counter = 0;

  while (number_of_collisions.at(counter++) ==
         min_or_max_number_of_collisions) {
  }

  return counter;
}

std::vector<c3picko::Node> c3picko::MinimumCutProblemSolver::MapColoniesToNodes(
    const std::vector<c3picko::Colony>& colonies) {
  // TODO implement
}

std::vector<c3picko::Colony>
c3picko::MinimumCutProblemSolver::MapNodesToColonies(
    const std::vector<c3picko::Node>& nodes_solution) {
  // TODO implement
}

void c3picko::MinimumCutProblemSolver::UpdateNodesNeighbours(
    c3picko::Node& node, std::vector<Node>& current_state) {
  auto lambda = [&node, &current_state](std::pair<int, Node*> neighbour) {
    for (int i = 0; i < current_state.size(); ++i) {
      Node& current_node = current_state.at(i);
      if (neighbour.first == current_node.index()) {
        current_node.neighbours().erase(node.index());
        break;
      }
    }
  };
  std::for_each(node.neighbours().begin(), node.neighbours().end(), lambda);
}

int c3picko::MinimumCutProblemSolver::GenerateHeuristic() {
  return std::accummulate(nodes_.begin(), nodes_.end(), 0);
}

void c3picko::MinimumCutProblemSolver::UpdateHeuristic(
    int& h, c3picko::Node& removed_node) {
  h -= removed_node.NumberOfCollisions() * 2;
}
