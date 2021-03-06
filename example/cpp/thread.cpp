//
//  Stryke
//
//  https://github.com/edmBernard/Stryke
//
//  Created by Erwan BERNARD on 08/12/2018.
//
//  Copyright (c) 2018 Erwan BERNARD. All rights reserved.
//  Distributed under the Apache License, Version 2.0. (See accompanying
//  file LICENSE or copy at http://www.apache.org/licenses/LICENSE-2.0)
//

#include "stryke/thread.hpp"
#include "stryke/sequential.hpp"
#include "stryke/core.hpp"

using namespace stryke;

void bench_thread(int number_line, const WriterOptions &options) {
  fs::create_directories("data");
  double max_timer = 0;
  auto start1 = std::chrono::high_resolution_clock::now();
  {
    OrcWriterThread<OrcWriterSequentialDuplicate, DateNumber, Double, TimestampNumber> writer_multi({"A2", "B2", "C2"}, "data", "thread_", options);
    double previous_timer = 0;
    for (int i = 0; i < number_line; ++i) {
      // auto start3 = std::chrono::high_resolution_clock::now() + std::chrono::duration<double, std::micro>(10);
      auto start4 = std::chrono::high_resolution_clock::now();

      writer_multi.write(17875 + i / 1000000., 11111.111111 + i / 10000., 17875 + previous_timer);

      previous_timer = (std::chrono::high_resolution_clock::now() - start4).count() / 1000000000.;
      if (previous_timer > max_timer) {
        max_timer = previous_timer;
      }
      // std::this_thread::sleep_until(start3);
      // std::cout << "previous_timer: " << previous_timer << "s -- " << (std::chrono::high_resolution_clock::now() - start4).count() / 1000000000. << "s\n";
    }
    std::chrono::duration<double, std::milli> elapsed1 = std::chrono::high_resolution_clock::now() - start1;
    std::cout << std::setw(40) << std::left << "thread:  processing time : description : " << elapsed1.count() << " ms\n";
  }
  std::chrono::duration<double, std::milli> elapsed2 = std::chrono::high_resolution_clock::now() - start1;
  std::cout << std::setw(40) << std::left << "thread after full release:  processing time : description : " << elapsed2.count() << " ms\n";
  std::cout << std::setw(40) << std::left << "thread max timer for one row:  processing time : description : " << max_timer << " s\n";
  fs::remove_all("data");
}

int main(int argc, char const *argv[]) {

  int number_line = 1000000;

  stryke::WriterOptions options;
  options.set_batch_size(1000);
  options.set_batch_max(0);
  options.set_stripe_size(10000);

  bench_thread(number_line, options);

  return 0;
}
