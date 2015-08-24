/*!
@file
Defines `boost::hana::Iterable`.

@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_CONCEPT_ITERABLE_HPP
#define BOOST_HANA_CONCEPT_ITERABLE_HPP

#include <boost/hana/fwd/concept/iterable.hpp>

#include <boost/hana/bool.hpp>
#include <boost/hana/core/default.hpp>
#include <boost/hana/core/models.hpp>
#include <boost/hana/functional/compose.hpp>
#include <boost/hana/if.hpp>
#include <boost/hana/not.hpp>
#include <boost/hana/optional.hpp>
#include <boost/hana/value.hpp>

#include <boost/hana/at.hpp>
#include <boost/hana/back.hpp>
#include <boost/hana/drop_front.hpp>
#include <boost/hana/drop_front_exactly.hpp>
#include <boost/hana/drop_while.hpp>
#include <boost/hana/front.hpp>
#include <boost/hana/is_empty.hpp>
#include <boost/hana/tail.hpp>

#include <cstddef>


namespace boost { namespace hana {
    template <typename It>
    struct models_impl<Iterable, It> {
        static constexpr bool value = !is_default<at_impl<It>>::value &&
                                      !is_default<tail_impl<It>>::value &&
                                      !is_default<is_empty_impl<It>>::value;
    };

    //////////////////////////////////////////////////////////////////////////
    // Model of Searchable
    //////////////////////////////////////////////////////////////////////////
    template <typename It>
    struct Iterable::find_if_impl {
        template <typename Xs, typename Pred>
        static constexpr decltype(auto) apply(Xs&& xs, Pred&& pred) {
            return hana::only_when(
                hana::compose(hana::not_, hana::is_empty),
                hana::front,
                hana::drop_while(static_cast<Xs&&>(xs),
                                 hana::compose(hana::not_, static_cast<Pred&&>(pred)))
            );
        }
    };

    template <typename It>
    struct Iterable::any_of_impl {
        template <bool Done, typename Dummy = void>
        struct any_of_helper {
            template <typename Xs, typename Pred>
            static constexpr auto apply(bool prev_cond, Xs&& xs, Pred&& pred) {
                auto cond = hana::if_(pred(hana::front(xs)), hana::true_, hana::false_);
                decltype(auto) tail = hana::tail(static_cast<Xs&&>(xs));
                constexpr bool done = hana::value<decltype(hana::is_empty(tail))>();
                return prev_cond ? hana::true_
                    : any_of_impl::any_of_helper<done>::apply(cond,
                                static_cast<decltype(tail)&&>(tail),
                                static_cast<Pred&&>(pred));
            }

            template <typename Xs, typename Pred>
            static constexpr auto apply(decltype(hana::true_), Xs&&, Pred&&)
            { return hana::true_; }

            template <typename Xs, typename Pred>
            static constexpr auto apply(decltype(hana::false_), Xs&& xs, Pred&& pred) {
                auto cond = hana::if_(pred(hana::front(xs)), hana::true_, hana::false_);
                constexpr bool done = hana::value<decltype(
                    hana::is_empty(hana::tail(xs))
                )>();
                return any_of_impl::any_of_helper<done>::apply(cond,
                                        hana::tail(static_cast<Xs&&>(xs)),
                                        static_cast<Pred&&>(pred));
            }
        };

        template <typename Dummy>
        struct any_of_helper<true, Dummy> {
            template <typename Cond, typename Xs, typename Pred>
            static constexpr auto apply(Cond cond, Xs&&, Pred&&)
            { return cond; }
        };

        template <typename Xs, typename Pred>
        static constexpr auto apply(Xs&& xs, Pred&& pred) {
            constexpr bool done = hana::value<decltype(hana::is_empty(xs))>();
            return any_of_impl::any_of_helper<done>::apply(hana::false_,
                                            static_cast<Xs&&>(xs),
                                            static_cast<Pred&&>(pred));
        }
    };
}} // end namespace boost::hana

#endif // !BOOST_HANA_CONCEPT_ITERABLE_HPP