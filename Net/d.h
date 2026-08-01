#ifndef D_H
#define D_H

#include <torch/torch.h>
#include <vector>
#include <unordered_map>
#include <string>

struct x1 : torch::data::Dataset<x1> {
    int a1;
    int a2;
    torch::Tensor b1;
    torch::Tensor b2;
    torch::Tensor d1;
    torch::Tensor d2;

    x1(int aa1 = 1000, int aa2 = 128, float aa3 = 0.4f);
    torch::data::EX<> get(size_t k00) override;
    torch::optional<size_t> size() const override;
};

struct DOutputs {
    x1 tt1;
    x1 vv1;
};

DOutputs g1(int kk1 = 800, int kk2 = 200, int kk3 = 32);

#endif
