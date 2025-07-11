#line 1 "include/boost/preprocessor/logical/xor.hpp"
# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef BOOST_PREPROCESSOR_LOGICAL_XOR_HPP
# define BOOST_PREPROCESSOR_LOGICAL_XOR_HPP
#
# include <boost/preprocessor/config/config.hpp>
# include <boost/preprocessor/logical/bool.hpp>
# include <boost/preprocessor/logical/bitxor.hpp>
#
# /* BOOST_PP_XOR */
#
# if ~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_EDG()
#    define BOOST_PP_XOR(p, q) BOOST_PP_BITXOR(BOOST_PP_BOOL(p), BOOST_PP_BOOL(q))
# else
#    define BOOST_PP_XOR(p, q) BOOST_PP_XOR_I(p, q)
#    define BOOST_PP_XOR_I(p, q) BOOST_PP_BITXOR(BOOST_PP_BOOL(p), BOOST_PP_BOOL(q))
# endif
#
# endif
