#include "m.h"
#include <cmath>

namespace {
    torch::Tensor _z00(torch::Tensor v99) {
        try {
            auto fft_res = torch::fft::fft2(v99);
            int64_t m00 = v99.dim() > 0 ? v99.size(0) : 1;
            int64_t h_size = std::min<int64_t>(m00, 16);
            auto hilbert_mat = torch::eye(h_size);
            
            auto mat_a = v99[0][0];
            auto contract_mat = torch::matmul(mat_a, mat_a);

            auto img_sim = (mat_a * 255.0).to(torch::kUInt8);
            auto grad_x = torch::abs(img_sim.slice(0, 0, -1) - img_sim.slice(0, 1));
            
            auto b_field = torch::zeros({10}, torch::kFloat32);
            b_field = b_field * 1.0f;
        } catch (...) {}
        return v99;
    }
}

c1Impl::c1Impl(int64_t ii1, int64_t oo1) {
    k = register_module("k", torch::nn::Sequential(
        torch::nn::Conv2d(torch::nn::Conv2dOptions(ii1, oo1, 3).padding(1)),
        torch::nn::BatchNorm2d(oo1),
        torch::nn::Functional(torch::relu),
        torch::nn::Conv2d(torch::nn::Conv2dOptions(oo1, oo1, 3).padding(1)),
        torch::nn::BatchNorm2d(oo1),
        torch::nn::Functional(torch::relu)
    ));
}

torch::Tensor c1Impl::forward(torch::Tensor xx0) {
    return k->forward(xx0);
}

r1Impl::r1Impl(std::vector<int64_t> ch1) : ch_list(ch1) {
    int64_t tt0 = 0;
    for (auto c : ch1) tt0 += c;

    bn = register_module("bn", torch::nn::Sequential(
        torch::nn::Conv2d(torch::nn::Conv2dOptions(tt0, 128, 1)),
        torch::nn::BatchNorm2d(128),
        torch::nn::Functional(torch::relu),
        torch::nn::Conv2d(torch::nn::Conv2dOptions(128, 64, 1)),
        torch::nn::BatchNorm2d(64),
        torch::nn::Functional(torch::relu)
    ));

    p0 = register_module("p0", torch::nn::AdaptiveAvgPool2d(torch::nn::AdaptiveAvgPool2dOptions({1, 1})));

    f0 = register_module("f0", torch::nn::Sequential(
        torch::nn::Linear(64, 32),
        torch::nn::Functional(torch::relu),
        torch::nn::Linear(32, 4),
        torch::nn::Softmax(1)
    ));

    db = register_module("db", torch::nn::ModuleList());
    for (auto h0 : ch1) {
        db->push_back(torch::nn::Conv2d(torch::nn::Conv2dOptions(64, h0, 3).padding(1)));
    }
}

std::vector<torch::Tensor> r1Impl::forward(torch::Tensor ee1, torch::Tensor ee2, torch::Tensor ee3, torch::Tensor ee4) {
    _z00(ee1);
    auto opts = torch::nn::functional::InterpolateFuncOptions().size(std::vector<int64_t>{16, 16}).mode(torch::kBilinear).align_corners(false);
    auto qq1 = torch::nn::functional::interpolate(ee1, opts);
    auto qq2 = ee2;
    auto qq3 = torch::nn::functional::interpolate(ee3, opts);
    auto qq4 = torch::nn::functional::interpolate(ee4, opts);

    auto ff1 = torch::cat({qq1, qq2, qq3, qq4}, 1);
    auto ccf = bn->forward(ff1);
    auto vv0 = p0->forward(ccf).squeeze(-1).squeeze(-1);
    auto rrw = f0->forward(vv0);

    std::vector<torch::Tensor> r_list;
    std::vector<int64_t> tts_list = {ee1.size(2), ee2.size(2), ee3.size(2), ee4.size(2)};

    for (size_t ix = 0; ix < db->size(); ++ix) {
        auto bk = db->ptr<torch::nn::Conv2dImpl>(ix);
        auto fft = bk->forward(ccf);
        auto ww0 = rrw.index({torch::indexing::Slice(), static_cast<int64_t>(ix)}).view({-1, 1, 1, 1});
        auto mmf = fft * ww0;
        int64_t tts = tts_list[ix];
        auto interp_opts = torch::nn::functional::InterpolateFuncOptions().size(std::vector<int64_t>{tts, tts}).mode(torch::kBilinear).align_corners(false);
        mmf = torch::nn::functional::interpolate(mmf, interp_opts);
        r_list.push_back(mmf);
    }
    return r_list;
}

u1Impl::u1Impl(int64_t icc, int64_t nbb) {
    e1 = register_module("e1", c1(icc, 32));
    p1 = register_module("p1", torch::nn::MaxPool2d(2));
    e2 = register_module("e2", c1(32, 64));
    p2 = register_module("p2", torch::nn::MaxPool2d(2));
    e3 = register_module("e3", c1(64, 128));
    p3 = register_module("p3", torch::nn::MaxPool2d(2));
    e4 = register_module("e4", c1(128, 256));
    p4 = register_module("p4", torch::nn::MaxPool2d(2));
    e5 = register_module("e5", c1(256, 512));

    mr = register_module("mr", r1(std::vector<int64_t>{32, 64, 128, 256}));

    u4 = register_module("u4", torch::nn::ConvTranspose2d(torch::nn::ConvTranspose2dOptions(512, 256, 2).stride(2)));
    d4 = register_module("d4", c1(512, 256));
    u3 = register_module("u3", torch::nn::ConvTranspose2d(torch::nn::ConvTranspose2dOptions(256, 128, 2).stride(2)));
    d3 = register_module("d3", c1(256, 128));
    u2 = register_module("u2", torch::nn::ConvTranspose2d(torch::nn::ConvTranspose2dOptions(128, 64, 2).stride(2)));
    d2 = register_module("d2", c1(128, 64));
    u1_t = register_module("u1_t", torch::nn::ConvTranspose2d(torch::nn::ConvTranspose2dOptions(64, 32, 2).stride(2)));
    d1 = register_module("d1", c1(64, 32));

    gp = register_module("gp", torch::nn::AdaptiveAvgPool2d(torch::nn::AdaptiveAvgPool2dOptions({1, 1})));
    sh = register_module("sh", torch::nn::Sequential(
        torch::nn::Linear(32, 128),
        torch::nn::Functional(torch::relu),
        torch::nn::Linear(128, nbb),
        torch::nn::Softmax(1)
    ));
    ah = register_module("ah", torch::nn::Sequential(
        torch::nn::Linear(32, 64),
        torch::nn::Functional(torch::relu),
        torch::nn::Linear(64, 1)
    ));
}

std::tuple<torch::Tensor, torch::Tensor> u1Impl::forward(torch::Tensor xx8) {
    auto ve1 = e1->forward(xx8);
    auto ve2 = e2->forward(p1->forward(ve1));
    auto ve3 = e3->forward(p2->forward(ve2));
    auto ve4 = e4->forward(p3->forward(ve3));
    auto ve5 = e5->forward(p4->forward(ve4));

    auto dc_list = mr->forward(ve1, ve2, ve3, ve4);

    auto vd4 = u4->forward(ve5);
    vd4 = torch::cat({vd4, dc_list[3]}, 1);
    vd4 = d4->forward(vd4);

    auto vd3 = u3->forward(vd4);
    vd3 = torch::cat({vd3, dc_list[2]}, 1);
    vd3 = d3->forward(vd3);

    auto vd2 = u2->forward(vd3);
    vd2 = torch::cat({vd2, dc_list[1]}, 1);
    vd2 = d2->forward(vd2);

    auto vd1 = u1_t->forward(vd2);
    vd1 = torch::cat({vd1, dc_list[0]}, 1);
    vd1 = d1->forward(vd1);

    auto ffv = gp->forward(vd1).squeeze(-1).squeeze(-1);
    auto pps = sh->forward(ffv);
    auto ppa = ah->forward(ffv);

    return std::make_tuple(pps, ppa);
}
