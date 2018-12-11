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

#include "stryke_thread.hpp"
#include "stryke_multifile.hpp"
#include "stryke_template.hpp"

using namespace stryke;

void bench_thread(int number_line) {
  fs::create_directories("data");
  auto start1 = std::chrono::high_resolution_clock::now();
  {
    OrcWriterThread<OrcWriterMulti, DateNumber, Double> writer_multi({"A2", "B2"}, "data", "date", 1000000, 100);
    for (int i = 0; i < number_line; ++i) {
      writer_multi.write(300 * 30 + i / 100000., 11111.111111 + i / 10000.);
    }
    std::chrono::duration<double, std::milli> elapsed1 = std::chrono::high_resolution_clock::now() - start1;
    std::cout << std::setw(40) << std::left << "thread:  processing time : description : " << elapsed1.count() << " ms\n";
  }
  std::chrono::duration<double, std::milli> elapsed2 = std::chrono::high_resolution_clock::now() - start1;
  std::cout << std::setw(40) << std::left << "thread after full release:  processing time : description : " << elapsed2.count() << " ms\n";
  fs::remove_all("data");
}

int main(int argc, char const *argv[]) {

  int number_line = 1000000;
  bench_thread(number_line);


  return 0;
}