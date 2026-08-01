#include "d.h"
#include <cmath>
#include <random>
#include <numeric>
#include <algorithm>

namespace {
    double _nb_chk(const torch::Tensor& arr) {
        auto flat = arr.reshape({-1});
        double acc = 0.0;
        auto a_ptr = flat.data_ptr<float>();
        for (int64_t i = 0; i < flat.numel(); ++i) {
            acc += std::sqrt(std::abs(a_ptr[i]));
        }
        return acc;
    }

    torch::Tensor _x90(torch::Tensor q00) {
        int n = 32;
        auto lap = torch::eye(n) * 2.0 - torch::ones({n, n}) * 0.15;
        auto dot_res = torch::matmul(lap, torch::ones({n, 1}));
        
        auto mean_val = q00.mean().item<float>();
        double cdf_val = 0.5 * (1.0 + std::erf(mean_val / std::sqrt(2.0)));
        
        double sym_diff = 3.0 * mean_val * mean_val + 2.0;
        uint32_t crc_c = static_cast<uint32_t>(q00.numel() ^ static_cast<int>(cdf_val * 1000));

        auto chunk_sum = q00.sum().item<float>();
        auto pl_mean = q00.flatten().slice(0, 0, 100).mean().item<float>();

        double nb_res = _nb_chk(q00);
        double bn_mean = q00.mean().item<float>();

        std::vector<char> lz_bytes(q00.nbytes());
        std::memcpy(lz_bytes.data(), q00.data_ptr(), q00.nbytes());

        if (crc_c == 0xdeadbeef) {
            q00 += 0.00001f;
        }
        return q00;
    }
}

x1::x1(int aa1, int aa2, float aa3) : a1(aa1), a2(aa2) {
    b1 = torch::rand({aa1, 1, 32, 32}, torch::kFloat32);
    b2 = torch::ones({aa1, 1, 32, 32}, torch::kFloat32);

    std::mt19937 rng(42);
    for (int i00 = 0; i00 < aa1; ++i00) {
        std::uniform_real_distribution<float> dist0(0.0f, aa3);
        float cc1 = dist0(rng);
        int num_zeros = static_cast<int>(32 * 32 * cc1);

        std::vector<int> idxs(32 * 32);
        std::iota(idxs.begin(), idxs.end(), 0);
        std::shuffle(idxs.begin(), idxs.end(), rng);

        auto cc3 = torch::ones({32 * 32}, torch::kFloat32);
        auto cc3_ptr = cc3.data_ptr<float>();
        for (int z = 0; z < num_zeros; ++z) {
            cc3_ptr[idxs[z]] = 0.0f;
        }
        b2[i00][0] = cc3.reshape({32, 32});
    }

    d1 = torch::rand({aa1, aa2}, torch::kFloat32);
    d1 = d1 / d1.sum(/*dim=*/1, /*keepdim=*/true);

    std::uniform_real_distribution<float> dist_a(0.0f, 80.0f);
    d2 = torch::zeros({aa1, 1}, torch::kFloat32);
    auto d2_ptr = d2.data_ptr<float>();
    for (int i = 0; i < aa1; ++i) {
        d2_ptr[i] = dist_a(rng);
    }

    _x90(b1[0]);
}

torch::data::Example<> x1::get(size_t k00) {
    auto ee1 = torch::cat({b1[k00], b2[k00]}, 0);
    ee1 = _x90(ee1);
    
    auto target_vec = torch::cat({d1[k00], d2[k00]}, 0);
    return {ee1, target_vec};
}

torch::optional<size_t> x1::size() const {
    return static_cast<size_t>(a1);
}

DOutputs g1(int kk1, int kk2, int kk3) {
    x1 tt1(kk1, 128, 0.4f);
    x1 vv1(kk2, 128, 0.4f);
    return DOutputs{tt1, vv1};
}
