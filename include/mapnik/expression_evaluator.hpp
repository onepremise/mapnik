/*****************************************************************************
 *
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2011 Artem Pavlenko
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

#ifndef MAPNIK_EXPRESSION_EVALUATOR_HPP
#define MAPNIK_EXPRESSION_EVALUATOR_HPP

// mapnik
#include <mapnik/attribute.hpp>
#include <mapnik/unicode.hpp>
#include <mapnik/expression_node.hpp>

// boost
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/regex.hpp>
#if defined(BOOST_REGEX_HAS_ICU)
#include <boost/regex/icu.hpp>
#endif

namespace mapnik
{

template <typename T0, typename T1, typename T2>
struct evaluate : boost::static_visitor<T1>
{
    using feature_type = T0;
    using value_type = T1;
    using variable_type = T2;

    explicit evaluate(feature_type const& f, variable_type const& v)
        : feature_(f),
          vars_(v) {}

    value_integer operator() (value_integer val) const
    {
        return val;
    }

    value_double operator() (value_double val) const
    {
        return val;
    }

    value_bool operator() (value_bool val) const
    {
        return val;
    }

    value_null operator() (value_null val) const
    {
        return val;
    }

    value_unicode_string const& operator() (value_unicode_string const& str) const
    {
        return str;
    }

    value_type operator() (attribute const& attr) const
    {
        return attr.value<value_type,feature_type>(feature_);
    }

    value_type operator() (global_attribute const& attr) const
    {
        auto itr = vars_.find(attr.name);
        if (itr != vars_.end())
        {
            return itr->second;
        }
        return value_type();// throw?
    }

    value_type operator() (geometry_type_attribute const& geom) const
    {
        return geom.value<value_type,feature_type>(feature_);
    }

    value_type operator() (binary_node<tags::logical_and> const & x) const
    {
        return (boost::apply_visitor(*this, x.left).to_bool())
            && (boost::apply_visitor(*this, x.right).to_bool());
    }

    value_type operator() (binary_node<tags::logical_or> const & x) const
    {
        return (boost::apply_visitor(*this,x.left).to_bool())
            || (boost::apply_visitor(*this,x.right).to_bool());
    }

    template <typename Tag>
    value_type operator() (binary_node<Tag> const& x) const
    {
        typename make_op<Tag>::type operation;
        return operation(boost::apply_visitor(*this, x.left),
                         boost::apply_visitor(*this, x.right));
    }

    template <typename Tag>
    value_type operator() (unary_node<Tag> const& x) const
    {
        typename make_op<Tag>::type func;
        return func(boost::apply_visitor(*this, x.expr));
    }

    value_type operator() (unary_node<tags::logical_not> const& x) const
    {
        return ! (boost::apply_visitor(*this,x.expr).to_bool());
    }

    value_type operator() (regex_match_node const& x) const
    {
        value_type v = boost::apply_visitor(*this, x.expr);
#if defined(BOOST_REGEX_HAS_ICU)
        return boost::u32regex_match(v.to_unicode(),x.pattern);
#else
        return boost::regex_match(v.to_string(),x.pattern);
#endif

    }

    value_type operator() (regex_replace_node const& x) const
    {
        value_type v = boost::apply_visitor(*this, x.expr);
#if defined(BOOST_REGEX_HAS_ICU)
        return boost::u32regex_replace(v.to_unicode(),x.pattern,x.format);
#else
        std::string repl = boost::regex_replace(v.to_string(),x.pattern,x.format);
        mapnik::transcoder tr_("utf8");
        return tr_.transcode(repl.c_str());
#endif
    }

    feature_type const& feature_;
    variable_type const& vars_;
};

}

#endif // MAPNIK_EXPRESSION_EVALUATOR_HPP
