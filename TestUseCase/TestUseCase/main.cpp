#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <tbb/tbb_stddef.h>
#include <QtCore/QDateTime>

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <algorithm>    //std::max
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"

static const std::size_t N = 23;

class SubStringFinder {
	const std::string &str;
	std::vector<std::size_t> &max_array;
	std::vector<std::size_t> &pos_array;
public:
	void operator() (const tbb::blocked_range<std::size_t> &r) const {
		for (std::size_t i = r.begin(); i != r.end(); ++i) {
			std::size_t max_size = 0, max_pos = 0;
			for (std::size_t j = 0; j < str.size(); ++j) {
				if (j != i) {
					std::size_t limit = str.size() - (std::max)(i, j);
					for (std::size_t k = 0; k < limit; ++k) {
						if (str[i + k] != str[j + k])
							break;
						if (k > max_size) {
							max_size = k;
							max_pos = j;
						}
					}
				}
			}
			max_array[i] = max_size;
			pos_array[i] = max_pos;
		}
	}

	SubStringFinder(const std::string &s, std::vector<std::size_t> &m, std::vector<std::size_t> &p) :
		str(s), max_array(m), pos_array(p) { }
};

int main() {

	std::string str[N] = { std::string("a"), std::string("b") };
	for (std::size_t i = 2; i < N; ++i)
		str[i] = str[i - 1] + str[i - 2];
	std::string &to_scan = str[N - 1];
	const std::size_t num_elem = to_scan.size();

	std::vector<std::size_t> max(num_elem);
	std::vector<std::size_t> pos(num_elem);

	const auto startExecTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
	tbb::parallel_for(tbb::blocked_range<std::size_t>(0, num_elem),
		SubStringFinder(to_scan, max, pos));

	const auto endExecTime = QDateTime::currentDateTime().toMSecsSinceEpoch() - startExecTime;
	qDebug() << QString::fromLocal8Bit("未使用粒度: ") << QDateTime::fromMSecsSinceEpoch(endExecTime).
		toString("MM-ss.zzz");

	const auto startExecTime1 = QDateTime::currentDateTime().toMSecsSinceEpoch();
	tbb::parallel_for(tbb::blocked_range<std::size_t>(0, num_elem, num_elem / std::thread::hardware_concurrency()),
		SubStringFinder(to_scan, max, pos));

	const auto endExecTime1 = QDateTime::currentDateTime().toMSecsSinceEpoch() - startExecTime1;
	qDebug() << QString::fromLocal8Bit("使用粒度: ") << QDateTime::fromMSecsSinceEpoch(endExecTime1).
		toString("MM-ss.zzz");
	return 0;
}
