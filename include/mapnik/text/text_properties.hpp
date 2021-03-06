/*****************************************************************************
 *
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2013 Artem Pavlenko
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
#ifndef MAPNIK_TEXT_PROPERTIES_HPP
#define MAPNIK_TEXT_PROPERTIES_HPP

// mapnik
#include <mapnik/text/char_properties_ptr.hpp>
#include <mapnik/color.hpp>
#include <mapnik/attribute.hpp>
#include <mapnik/value.hpp>
#include <mapnik/feature.hpp>
#include <mapnik/font_set.hpp>
#include <mapnik/enumeration.hpp>
#include <mapnik/expression.hpp>
#include <mapnik/text/formatting/base.hpp>
#include <mapnik/pixel_position.hpp>
#include <mapnik/symbolizer.hpp>
// stl
#include <map>
#include <functional>
// boost
#include <boost/optional.hpp>
#include <boost/property_tree/ptree_fwd.hpp>

namespace mapnik
{

using fontset_map = std::map<std::string, font_set>;

struct MAPNIK_DECL char_properties
{
    char_properties();
    // Construct object from XML.
    void from_xml(xml_node const& sym, fontset_map const& fontsets);
    // Write object to XML ptree.
    void to_xml(boost::property_tree::ptree& node, bool explicit_defaults, char_properties const& dfl=char_properties()) const;
    std::string face_name;
    boost::optional<font_set> fontset;
    double text_size;
    double character_spacing;
    double line_spacing; //Largest total height (fontsize+line_spacing) per line is chosen
    double text_opacity;
    double halo_opacity;
    unsigned wrap_char;
    text_transform_e text_transform; //Per expression
    color fill;
    color halo_fill;
    double halo_radius;
};

// Properties for building the layout of a single text placement
struct MAPNIK_DECL text_layout_properties
{
    text_layout_properties();

    // Load all values from XML ptree.
    void from_xml(xml_node const &sym);
    // Save all values to XML ptree (but does not create a new parent node!).
    void to_xml(boost::property_tree::ptree & node, bool explicit_defaults,
                text_layout_properties const& dfl = text_layout_properties()) const;

    // Get a list of all expressions used in any placement.
    // This function is used to collect attributes.
    void add_expressions(expression_set & output) const;

    //Per layout options
    symbolizer_base::value_type dx;
    symbolizer_base::value_type dy;
    symbolizer_base::value_type orientation;
    symbolizer_base::value_type text_ratio;
    symbolizer_base::value_type wrap_width;
    symbolizer_base::value_type wrap_before;
    symbolizer_base::value_type rotate_displacement;
    horizontal_alignment_e halign;
    justify_alignment_e jalign;
    vertical_alignment_e valign;
    std::function<pixel_position(double,double)> displacement_evaluator_ =
        [](double dx, double dy) { return pixel_position(dx,dy);};
};

class text_layout;

// Contains all text symbolizer properties which are not directly related to text formatting and layout.
struct MAPNIK_DECL text_symbolizer_properties
{
    text_symbolizer_properties();
    // Load only placement related values from XML ptree.
    void placement_properties_from_xml(xml_node const& sym);
    // Load all values from XML ptree.
    void from_xml(xml_node const& node, fontset_map const& fontsets);
    // Save all values to XML ptree (but does not create a new parent node!).
    void to_xml(boost::property_tree::ptree & node, bool explicit_defaults,
                text_symbolizer_properties const& dfl = text_symbolizer_properties()) const;

    // Takes a feature and produces formated text as output.
    // The output object has to be created by the caller and passed in for thread safety.
    void process(text_layout &output, feature_impl const& feature, attributes const& vars) const;
    // Automatically create processing instructions for a single expression.
    void set_old_style_expression(expression_ptr expr);
    // Sets new format tree.
    void set_format_tree(formatting::node_ptr tree);
    // Get format tree.
    formatting::node_ptr format_tree() const;
    // Get a list of all expressions used in any placement.
    // This function is used to collect attributes.
    void add_expressions(expression_set & output) const;

    // Per symbolizer options
    label_placement_e label_placement;
    // distance between repeated labels on a single geometry
    double label_spacing;
    // distance the label can be moved on the line to fit, if 0 the default is used
    double label_position_tolerance;
    bool avoid_edges;
    double minimum_distance;
    double minimum_padding;
    double minimum_path_length;
    double max_char_angle_delta;
    // Always try render an odd amount of labels
    bool force_odd_labels;
    bool allow_overlap;
    // Only consider geometry with largest bbox (polygons)
    bool largest_bbox_only;
    text_upright_e upright;

    // Default values for text layouts
    text_layout_properties layout_defaults;
    // Default values for char_properties.
    char_properties_ptr format;
private:
    // A tree of formatting::nodes which contain text and formatting information.
    formatting::node_ptr tree_;
};

} //ns mapnik

#endif // MAPNIK_TEXT_PROPERTIES_HPP
