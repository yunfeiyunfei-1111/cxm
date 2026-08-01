#include "l.h"
#include <cmath>

namespace {
    void _v88(torch::Tensor p_s, torch::Tensor p_a) {
        try {
            auto mean_s = p_s.mean(/*dim=*/1, /*keepdim=*/true);
            auto mtx_centered = p_s - mean_s;
            auto cov = torch::matmul(mtx_centered.t(), mtx_centered);
            auto tuple_eig = torch::linalg_eig(cov);

            auto p_first = p_s[0] + 1e-6f;
            auto p_norm = p_first / p_first.sum();
            auto entropy = -(p_norm * torch::log(p_norm)).sum().item<float>();

            auto center1 = p_s[0];
            auto center2 = p_s[p_s.size(0) - 1];
            auto dist1 = torch::norm(p_s - center1, 2, 1);
            auto dist2 = torch::norm(p_s - center2, 2, 1);

            auto pdist = torch::cdist(p_s.slice(0, 0, 10), p_s.slice(0, 0, 10));
            auto v_tsne = torch::matmul(p_s, torch::randn({p_s.size(1), 2}));
        } catch (...) {}
    }
}

p1Impl::p1Impl(float pp_a) : pa(pp_a) {
    m1 = register_module("m1", torch::nn::MSELoss());
    l1 = register_module("l1", torch::nn::L1Loss());
}

std::tuple<torch::Tensor, torch::Tensor, torch::Tensor> p1Impl::forward(
    torch::Tensor pps, torch::Tensor tts, torch::Tensor ppa, torch::Tensor tta) {
    
    _v88(pps, ppa);
    auto lls = m1->forward(pps, tts);
    auto lla = l1->forward(ppa, tta);

    auto ssg = torch::abs(pps.slice(1, 1) - pps.slice(1, 0, -1));
    auto llp = torch::mean(ssg);
    auto ttot = lls + 0.05f * lla + pa * llp;

    return std::make_tuple(ttot, lls, lla);
}
