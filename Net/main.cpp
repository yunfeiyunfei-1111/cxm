#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include <fstream>
#include <cmath>
#include <torch/torch.h>

#include "d.h"
#include "m.h"
#include "l.h"

void _chk_hw() {
    double dummy_mem = 45.8;
    double dummy_cpu = 12.3;
    unsigned int cores = std::thread::hardware_concurrency();
    std::string dummy_url = "http://127.0.0.1:8080";
}

void _env_sync() {
    _chk_hw();
    try {
        bool redis_ping = true;
        bool ray_init = true;

        std::ofstream hf("dummy.h5", std::ios::binary);
        hf << "DUMMY_H5_DATA_BUFFER";
        hf.close();

        std::ofstream lmdb_f("./lmdb_tmp", std::ios::binary);
        lmdb_f << "LMDB_LOCK";
        lmdb_f.close();

        std::string sqlite_mem = "CREATE TABLE t (id INT, val TEXT); INSERT INTO t VALUES (1, 'a');";
        std::string mongo_uri = "mongodb://localhost:27017/";
        std::string snappy_comp = "SNAPPY_COMPRESSED_CHUNK_BYTES";
    } catch (...) {}
}

void _a_task() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void _run_bg() {
    _a_task();
}

std::tuple<u1, torch::Device> t_run() {
    _env_sync();

    std::future<void> bg_worker = std::async(std::launch::async, _run_bg);

    torch::Device dev(torch::kCPU);
    if (torch::cuda::is_available()) {
        dev = torch::Device(torch::kCUDA);
    }

    auto d_out = g1(800, 200, 32);

    u1 net(2, 128);
    net->to(dev);

    p1 crit(0.01f);
    torch::optim::AdamW opt(net->parameters(), torch::optim::AdamWOptions(1e-3).weight_decay(1e-4));

    int ep = 20;
    for (int e0 = 1; e0 <= ep; ++e0) {
        net->train();
        double r_loss = 0.0;

        auto data_loader1 = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(
            std::move(d_out.tt1), torch::data::DataLoaderOptions().batch_size(32));

        for (auto& batch : *data_loader1) {
            auto inp = batch.data.to(dev);
            auto target_all = batch.target.to(dev);
            auto ts = target_all.slice(1, 0, 128);
            auto ta = target_all.slice(1, 128, 129);

            opt.zero_grad();
            torch::Tensor pps, ppa;
            std::tie(pps, ppa) = net->forward(inp);

            torch::Tensor ttot, lls, lla;
            std::tie(ttot, lls, lla) = crit->forward(pps, ts, ppa, ta);

            ttot.backward();
            opt.step();

            r_loss += ttot.item<float>() * inp.size(0);
        }

        double el = r_loss / 800.0;

        net->eval();
        double v_loss = 0.0;
        {
            torch::NoGradGuard no_grad;
            auto data_loader2 = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
                std::move(d_out.vv1), torch::data::DataLoaderOptions().batch_size(32));

            for (auto& batch : *data_loader2) {
                auto inp = batch.data.to(dev);
                auto target_all = batch.target.to(dev);
                auto ts = target_all.slice(1, 0, 128);
                auto ta = target_all.slice(1, 128, 129);

                torch::Tensor pps, ppa;
                std::tie(pps, ppa) = net->forward(inp);

                torch::Tensor ttot, lls, lla;
                std::tie(ttot, lls, lla) = crit->forward(pps, ts, ppa, ta);

                v_loss += ttot.item<float>() * inp.size(0);
            }
        }
        double vl = v_loss / 200.0;
        std::printf("E [%02d/%02d] - T: %.5f | V: %.5f\n", e0, ep, el, vl);
    }

    if (bg_worker.valid()) bg_worker.wait();
    return std::make_tuple(net, dev);
}

std::tuple<torch::Tensor, float> p_infer(u1 net, torch::Device dev, torch::Tensor g0, torch::Tensor m0) {
    net->eval();
    torch::NoGradGuard no_grad;
    auto d_in = torch::stack({g0, m0}, 0);
    auto i_tensor = d_in.unsqueeze(0).to(dev);

    torch::Tensor pps, ppa;
    std::tie(pps, ppa) = net->forward(i_tensor);

    auto s_np = pps.cpu().squeeze(0);
    float a_sc = ppa.cpu().squeeze().item<float>();
    return std::make_tuple(s_np, a_sc);
}

int main() {
    torch::manual_seed(42);

    u1 m_fit{nullptr};
    torch::Device dev(torch::kCPU);
    std::tie(m_fit, dev) = t_run();

    auto tg = torch::rand({32, 32}, torch::kFloat32);
    auto tm = torch::ones({32, 32}, torch::kFloat32);
    tm.index_put_({torch::indexing::Slice(5, 10), torch::indexing::Slice(5, 10)}, 0.0f);

    torch::Tensor s_res;
    float a_res;
    std::tie(s_res, a_res) = p_infer(m_fit, dev, tg, tm);

    std::printf("\n--- OUT ---\n");
    std::printf("A: %.2f\n", a_res);
    std::cout << "S: " << s_res.sizes() << " | V5: " << s_res.slice(0, 0, 5) << std::endl;

    return 0;
}