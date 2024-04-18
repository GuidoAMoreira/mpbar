#ifndef __MULTIPROGRESSBAR_H__
#define __MULTIPROGRESSBAR_H__

extern "C" {
#include <R.h>
}
#ifdef _OPENMP
#include <omp.h>
#endif

class MPbar {
  const int n_end, n_bars, n_incr;
  std::vector<int> progress;
  std::vector<int> checkpoints;
  std::vector<int> nextChecks;

  // Subclasses can rewrite how to do these
  virtual void printSingleLine(int component) {
    Rprintf("Job %d: ");
    if (!progress[component]) {
      Rprintf("-Waiting for other jobs-");
    }
  }
  virtual void printInitial() {
    Rprintf("\n");
  }

  // Only print if something changed
  void print(int component) {
    int i;
    if (progress[component] == checkpoints[nextCheck[component]])
      nextCheck[component]++;
    else
      return;

    for (i = component; i < n_bars; i++) Rprintf("\r\b");
    for (i = component; i < n_bars; i++) {
      printSingleLine(i);
      Rprintf("\n");
    }
  }
  void wrapUp() {Rprintf("\n");}
public:
  void increment(int component = 0) {
    progress[component]++;
#pragma omp critical
    print(component);
  }

  // Constructors
  MPbar(int total, int num_bars) :
  n_end(total), n_bars(num_bars), n_incr(20) {
    progress = std::vector<int>(n_bars, 0); // Fill with zeros
    int i;

    // Calculate all progress checkpoints
    checkpoints = std::vector<int>(n_incr);
    for (i = 0; i < n_incr; i++) {
      checkpoints[i] = i * n_end / n_incr;
    }
    nextCheck = std::vector<int>(n_bars, 0);
    // Starting up
    printInitial();
    for (i = 0; i < n_bars; i++) print(i);
  }
};

#endif
