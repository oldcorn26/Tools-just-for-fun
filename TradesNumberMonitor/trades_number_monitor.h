#pragma once
#include <vector>
#include "loop_queue.h"

/*
 * TradesNumberMonitor class is used for monitoring trades number.
 * periods must be sorted in descending order.
 */

class TradesNumberMonitor {
public:
    TradesNumberMonitor(const std::vector<int> &periods, const std::vector<int> &thresholds);
    void add(int trades);
    int peakSum(size_t idx);
    bool checkIfAlert(size_t idx);

private:
    int maxPeriod;
    std::vector<int> periods;
    std::vector<int> thresholds;
    std::vector<int> sums;
    LoopQueue<int> buckets;
};

TradesNumberMonitor::TradesNumberMonitor(const std::vector<int> &periods, const std::vector<int> &thresholds)
        : periods(periods), thresholds(thresholds), maxPeriod(*std::max_element(periods.begin(), periods.end())),
          buckets(maxPeriod), sums(periods.size(), 0) {
}

void TradesNumberMonitor::add(int trades) {
    if (buckets.full()) {
        for (size_t i = 0; i < periods.size(); ++i) {
            sums[i] -= buckets.peakFrontI(maxPeriod - periods[i]);
        }
    }

    buckets.put(trades);

    for (size_t i = 0; i < periods.size(); ++i) {
        if (buckets.full() || periods[i] + buckets.size() > maxPeriod) {
            sums[i] += trades;
        }
    }
}

int TradesNumberMonitor::peakSum(size_t idx) {
    return sums[idx];
}

bool TradesNumberMonitor::checkIfAlert(size_t idx) {
    return sums[idx] >= thresholds[idx];
}
