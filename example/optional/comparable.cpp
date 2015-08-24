/*
@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#include <boost/hana/assert.hpp>
#include <boost/hana/optional.hpp>
namespace hana = boost::hana;


BOOST_HANA_CONSTANT_CHECK(hana::nothing == hana::nothing);
static_assert(hana::just('x') == hana::just('x'), "");
static_assert(hana::just('x') != hana::just('y'), "");
BOOST_HANA_CONSTANT_CHECK(hana::just('x') != hana::nothing);

int main() { }