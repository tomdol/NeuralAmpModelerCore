#include "activations.h"

nam::activations::ActivationTanh _TANH = nam::activations::ActivationTanh();
nam::activations::ActivationFastTanh _FAST_TANH = nam::activations::ActivationFastTanh();
nam::activations::ActivationHardTanh _HARD_TANH = nam::activations::ActivationHardTanh();
nam::activations::ActivationReLU _RELU = nam::activations::ActivationReLU();
nam::activations::ActivationLeakyReLU _LEAKY_RELU = nam::activations::ActivationLeakyReLU();
nam::activations::ActivationSigmoid _SIGMOID = nam::activations::ActivationSigmoid();

bool nam::activations::Activation::using_fast_tanh = false;

std::unordered_map<std::string, nam::activations::Activation*> nam::activations::Activation::_activations = {
  {"Tanh", &_TANH}, {"Hardtanh", &_HARD_TANH},   {"Fasttanh", &_FAST_TANH},
  {"ReLU", &_RELU}, {"LeakyReLU", &_LEAKY_RELU}, {"Sigmoid", &_SIGMOID}};

nam::activations::Activation* tanh_bak = nullptr;

nam::activations::Activation* nam::activations::Activation::get_activation(const std::string name)
{
  if (_activations.find(name) == _activations.end())
    return nullptr;

  return _activations[name];
}

void nam::activations::Activation::enable_fast_tanh()
{
  nam::activations::Activation::using_fast_tanh = true;

  if (_activations["Tanh"] != _activations["Fasttanh"])
  {
    tanh_bak = _activations["Tanh"];
    _activations["Tanh"] = _activations["Fasttanh"];
  }
}

void nam::activations::Activation::disable_fast_tanh()
{
  nam::activations::Activation::using_fast_tanh = false;

  if (_activations["Tanh"] == _activations["Fasttanh"])
  {
    _activations["Tanh"] = tanh_bak;
  }
}

void nam::activations::Activation::apply(Eigen::Block<Eigen::MatrixXf, -1, -1, true> block) const
{
  apply(block.data(), block.rows() * block.cols());
}

void nam::activations::Activation::apply(float* data, long size) const
{
  throw std::runtime_error{
    "The base class of the activations hierarchy does not have an implementation and should not be called"};
}

std::unique_ptr<nam::activations::Activation> nam::activations::make_activation(std::string_view activation_name)
{
  using namespace std::string_view_literals;

  static std::unordered_map<std::string_view, std::function<std::unique_ptr<Activation>()>> factories{
    {"ReLU"sv, []() { return std::make_unique<ActivationReLU>(); }},
    {"Sigmoid"sv, []() { return std::make_unique<ActivationSigmoid>(); }},
    {"Fasttanh"sv, []() { return std::make_unique<ActivationFastTanh>(); }},
    {"LeakyReLU"sv, []() { return std::make_unique<ActivationLeakyReLU>(); }},
    {"Tanh"sv, []() { return std::make_unique<ActivationTanh>(); }},
    {"Hardtanh"sv, []() { return std::make_unique<ActivationHardTanh>(); }},
  };

  if (activation_name == "Tanh" && nam::activations::Activation::using_fast_tanh)
  {
    activation_name = "Fasttanh"sv;
  }

  if (factories.contains(activation_name))
  {
    return factories.at(activation_name)();
    //                                  ^^
    //  here's where the lambda factory is called
  }
  else
  {
    throw std::runtime_error{
      std::string{"The specified activation is not supported, please check your model: "}.append(activation_name)};
  }
}
