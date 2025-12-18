#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <random>
#include <tuple>
#include <vector>
#include "NAM/activations.h"

using namespace Catch::Matchers;
using namespace nam::activations;

TEST_CASE("Constructing activations")
{
  SECTION("Unsupported activation")
  {
    REQUIRE_THROWS(make_activation("unknown-activation-name"));
  }
}

std::vector<float> random_input_tensor() {
  constexpr const size_t N = 100000;
  std::vector<float> input_tensor(N);

  std::random_device rd;
  std::mt19937 gen{rd()};
  std::uniform_real_distribution<float> float_generator{-1234.0f, 5678.0f};

  for (auto&& elem : input_tensor)
  {
    elem = float_generator(gen);
  }

  return input_tensor;
}

enum class ActivationEnum
{
  ReLU,
  Sigmoid,
  Fasttanh,
  LeakyReLU,
  Tanh,
  Hardtanh
};
using AE = ActivationEnum;

template <typename T, ActivationEnum E>
struct TT // TT = Test Tuple
{
  using Activation_t = T;
  static std::string_view activation_name()
  {
    static std::unordered_map<ActivationEnum, std::string_view> names{
      {ActivationEnum::ReLU, "ReLU"},         {ActivationEnum::Sigmoid, "Sigmoid"},
      {ActivationEnum::Fasttanh, "Fasttanh"}, {ActivationEnum::LeakyReLU, "LeakyReLU"},
      {ActivationEnum::Tanh, "Tanh"},         {ActivationEnum::Hardtanh, "Hardtanh"},
    };

    return names[E];
  }
};

using ActivationsList = std::tuple<TT<ActivationReLU, AE::ReLU>, TT<ActivationSigmoid, AE::Sigmoid>,
                                   TT<ActivationFastTanh, AE::Fasttanh>, TT<ActivationLeakyReLU, AE::LeakyReLU>,
                                   TT<ActivationTanh, AE::Tanh>, TT<ActivationHardTanh, AE::Hardtanh>>;

TEMPLATE_LIST_TEST_CASE("Activation subclasses", "[subclasses]", ActivationsList)
{
  DYNAMIC_SECTION("Benchmarking activation: " << TestType::activation_name())
  {
    auto input_tensor = random_input_tensor();
    typename TestType::Activation_t activation{};

    BENCHMARK("benchmark")
    {
      activation.apply(input_tensor.data(), input_tensor.size());
    };
  }
}

TEMPLATE_LIST_TEST_CASE("Activations wrapped in unique pointers", "[unique_ptr]", ActivationsList)
{
  
  DYNAMIC_SECTION("Benchmarking activation: " << TestType::activation_name())
  {
    auto input_tensor = random_input_tensor();
    const auto activation_unique_ptr = make_activation(TestType::activation_name());

    BENCHMARK("benchmark")
    {
      activation_unique_ptr->apply(input_tensor.data(), input_tensor.size());
    };
  }
}
