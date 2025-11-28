#ifndef NURBS_H
#define NURBS_H

#include <cstdint>
#include <cstring>
#include <vector>

template <typename T> class NURBS {
public:
  NURBS(std::vector<T> controlPoints, std::vector<uint8_t> knots,
        unsigned int degree)
      : _controlPoints(controlPoints), _knots(knots), _degree(degree) {}

  T interpolate(unsigned int t) const {
    const size_t span = findSpan(t);
    const auto basis = bsplineBasis(span, t);

    T value;
    std::memset(&value, 0, sizeof(T));
    for (int i = 0; i <= _degree; ++i) {
      value += _controlPoints[span - i] * basis[i];
    }

    return value;
  }

private:
  /*
   * bsplineBasis and findSpan are based on the implementations of
   * https://github.com/PredatorCZ/HavokLib
   */

  std::vector<float> bsplineBasis(size_t span, float t) const {
    std::vector<float> N(_degree + 1);
    std::fill(N.begin(), N.end(), 0.0f);

    N[0] = 1.0f;

    for (int i = 0; i < _degree; ++i) {
      for (int j = i; j >= 0; --j) {
        float a = (t - _knots[span - j]) /
                  (_knots[span + i + 1 - j] - _knots[span - j]);
        float tmp = N[j] * a;
        N[j + 1] += N[j] - tmp;
        N[j] = tmp;
      }
    }

    return N;
  }

  size_t findSpan(unsigned int t) const {
    if (t >= _knots[_controlPoints.size()])
      return _controlPoints.size() - 1;

    size_t low = _degree;
    size_t high = _controlPoints.size();
    size_t mid = (low + high) / 2;

    while (t < _knots[mid] || t >= _knots[mid + 1]) {
      if (t < _knots[mid]) {
        high = mid;
      } else {
        low = mid;
      }

      mid = (low + high) / 2;
    }

    return mid;
  }

  unsigned int _degree;
  std::vector<T> _controlPoints;
  std::vector<uint8_t> _knots;
};

#endif