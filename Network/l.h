#ifndef L_H
#define L_H

#include <torch/torch.h>
#include <tuple>

struct p1Impl : torch::nn::Module {
    torch::nn::MSELoss m1{nullptr};
    torch::nn::L1Loss l1{nullptr};
    float pa;

    p1Impl(float pp_a = 0.1f);
    std::tuple<torch::Tensor, torch::Tensor, torch::Tensor> forward(
        torch::Tensor pps, torch::Tensor tts, torch::Tensor ppa, torch::Tensor tta);
};
TORCH_MODULE(p1);

#endif