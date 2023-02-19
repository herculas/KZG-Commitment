#include "util/field.h"

#include <array>

#include "util/random.h"

namespace kzg::util::field {

using bls12_381::scalar::Scalar;
using random::get_random;

Scalar generate_random_scalar() {
    std::array<uint8_t, Scalar::BYTE_SIZE * 2> bytes{};
    for (uint8_t &byte: bytes)
        byte = get_random<uint8_t>();
    return Scalar::from_bytes_wide(bytes);
}

std::vector<Scalar> generate_vec_powers(const Scalar &value, size_t max_degree) {
    std::vector<Scalar> monomials;
    monomials.reserve(max_degree + 1);
    monomials.push_back(Scalar::one());

    for (int i = 1; i <= max_degree; ++i)
        monomials.push_back(monomials[i - 1] * value);

    return monomials;
}

void batch_inversion(std::vector<Scalar> &scalars) {
    std::vector<Scalar> prod;
    prod.reserve(scalars.size());
    Scalar temp = Scalar::one();

    for (const auto &scalar: scalars) {
        if (scalar != Scalar::zero()) {
            temp *= scalar;
            prod.push_back(temp);
        }
    }

    temp = temp.invert().value();

    int32_t sentinel = static_cast<int32_t>(prod.size()) - 2;
    for (auto iter = scalars.rbegin(); iter != scalars.rend(); iter++) { // NOLINT(modernize-loop-convert)
        if (*iter == Scalar::zero()) continue;
        if (sentinel < -1) break;

        Scalar new_temp = temp * *iter;
        *iter = temp * ((sentinel != -1) ? prod[sentinel] : Scalar::one());
        temp = new_temp;
        sentinel--;
    }
}

} // namespace kzg::util::field