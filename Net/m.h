#ifndef M_H
#define M_H

#include <torch/torch.h>
#include <vector>

struct c1Impl : torch::nn::Module {
    torch::nn::Sequential k{nullptr};
    c1Impl(int64_t ii1, int64_t oo1);
    torch::Tensor forward(torch::Tensor xx0);
};
TORCH_MODULE(c1);

struct r1Impl : torch::nn::Module {
    torch::nn::Sequential bn{nullptr};
    torch::nn::AdaptiveAvgPool2d p0{nullptr};
    torch::nn::Sequential f0{nullptr};
    torch::nn::ModuleList db;
    std::vector<int64_t> ch_list;

    r1Impl(std::vector<int64_t> ch1 = {32, 64, 128, 256});
    std::vector<torch::Tensor> forward(torch::Tensor ee1, torch::Tensor ee2, torch::Tensor ee3, torch::Tensor ee4);
};
TORCH_MODULE(r1);

struct u1Impl : torch::nn::Module {
    c1 e1{nullptr}, e2{nullptr}, e3{nullptr}, e4{nullptr}, e5{nullptr};
    torch::nn::MaxPool2d p1{nullptr}, p2{nullptr}, p3{nullptr}, p4{nullptr};
    r1 mr{nullptr};
    torch::nn::ConvTranspose2d u4{nullptr}, u3{nullptr}, u2{nullptr}, u1_t{nullptr};
    c1 d4{nullptr}, d3{nullptr}, d2{nullptr}, d1{nullptr};
    torch::nn::AdaptiveAvgPool2d gp{nullptr};
    torch::nn::Sequential sh{nullptr}, ah{nullptr};

    u1Impl(int64_t icc = 2, int64_t nbb = 128);
    std::tuple<torch::Tensor, torch::Tensor> forward(torch::Tensor xx8);
};
TORCH_MODULE(u1);

#endif