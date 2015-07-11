/*!
@file
Defines `boost::hana::partition`.

@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_PARTITION_HPP
#define BOOST_HANA_PARTITION_HPP

#include <boost/hana/fwd/partition.hpp>

#include <boost/hana/at.hpp>
#include <boost/hana/core/datatype.hpp>
#include <boost/hana/core/default.hpp>
#include <boost/hana/core/make.hpp>
#include <boost/hana/core/models.hpp>
#include <boost/hana/detail/by.hpp> // required by fwd decl
#include <boost/hana/detail/constexpr/algorithm.hpp>
#include <boost/hana/detail/constexpr/array.hpp>
#include <boost/hana/detail/dispatch_if.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/unpack.hpp>
#include <boost/hana/value.hpp>

#include <cstddef>
#include <utility>


namespace boost { namespace hana {
    struct Sequence; //! @todo include the forward declaration instead

    //! @cond
    template <typename Xs, typename Pred>
    constexpr auto partition_t::operator()(Xs&& xs, Pred&& pred) const {
        using S = typename datatype<Xs>::type;
        using Partition = BOOST_HANA_DISPATCH_IF(partition_impl<S>,
            _models<Sequence, S>::value
        );

    #ifndef BOOST_HANA_CONFIG_DISABLE_CONCEPT_CHECKS
        static_assert(_models<Sequence, S>::value,
        "hana::partition(xs, pred) requires 'xs' to be a Sequence");
    #endif

        return Partition::apply(static_cast<Xs&&>(xs),
                                static_cast<Pred&&>(pred));
    }
    //! @endcond

    namespace detail {
        template <bool ...B>
        struct partition_indices {
            static constexpr bool results[] = {B..., false}; // avoid empty array
            static constexpr std::size_t left_size =
                detail::constexpr_::count(results, results + sizeof...(B), true);
            static constexpr std::size_t right_size = sizeof...(B) - left_size;

            static constexpr auto compute_left() {
                detail::constexpr_::array<std::size_t, left_size> indices{};
                std::size_t* left = &indices[0];
                for (std::size_t i = 0; i < sizeof...(B); ++i)
                    if (results[i])
                        *left++ = i;
                return indices;
            }

            static constexpr auto compute_right() {
                detail::constexpr_::array<std::size_t, right_size> indices{};
                std::size_t* right = &indices[0];
                for (std::size_t i = 0; i < sizeof...(B); ++i)
                    if (!results[i])
                        *right++ = i;
                return indices;
            }

            static constexpr auto left = compute_left();
            static constexpr auto right = compute_right();

            template <typename S, typename Xs, std::size_t ...l, std::size_t ...r>
            static constexpr auto apply(Xs&& xs, std::index_sequence<l...>,
                                                 std::index_sequence<r...>)
            {
                return hana::make<hana::Pair>(
                    hana::make<S>(hana::at_c<left[l]>(static_cast<Xs&&>(xs))...),
                    hana::make<S>(hana::at_c<right[r]>(static_cast<Xs&&>(xs))...)
                );
            }
        };

        template <typename Pred>
        struct deduce_partition_indices {
            template <typename ...Xs>
            auto operator()(Xs&& ...xs) const -> detail::partition_indices<
                hana::value<decltype(std::declval<Pred>()(static_cast<Xs&&>(xs)))>()...
            > { return {}; }
        };
    }

    template <typename S, bool condition>
    struct partition_impl<S, when<condition>> : default_ {
        template <typename Xs, typename Pred>
        static constexpr auto apply(Xs&& xs, Pred&&) {
            using Indices = decltype(hana::unpack(
                static_cast<Xs&&>(xs), detail::deduce_partition_indices<Pred&&>{}
            ));
            return Indices::template apply<S>(
                static_cast<Xs&&>(xs),
                std::make_index_sequence<Indices::left_size>{},
                std::make_index_sequence<Indices::right_size>{}
            );
        }
    };
}} // end namespace boost::hana

#endif // !BOOST_HANA_PARTITION_HPP