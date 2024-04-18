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
  std::vector<std::vector<int>::iterator> nextChecks;

  // Subclasses can rewrite how to do these
  virtual void printSingleLine(int component) {
    Rprintf("Job %d: ");
  }
  virtual void printInitial() {Rprintf("\n");}

  void print(int component) {
    int i;
    if (progress[component] == *nextCheck[component])
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
  void increment(int component = 0) {progress[component]++; print(component);}

  // Constructors
  MPbar(int total, int num_bars, int increments = 20) :
  n_end(total), n_bars(num_bars), n_incr(increments) {
    progress = std::vector<int>(n_bars, 0); // Fill with zeros

    // Calculate all progress checkpoints
    checkpoints = std::vector<int>(n_incr);
    for (int i = 0; i < n_incr; i++) {
      checkpoints[i] = i * n_end / n_incr;
    }
    std::vector<int>iterator ptr = checkpoints.begin();
    nextCheck = std::vector<std::vector<int>::iterator>(n_bars, ++ptr);
    // Starting up
    printInitial();
  }
};

#endif
