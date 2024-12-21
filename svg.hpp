/**
 * from https://vincentlaucsb.github.io/svg/svg_8hpp_source.html
 * via https://vincentlaucsb.github.io/svg/index.html
 * also https://github.com/vincentlaucsb/svg
 */

#pragma once
#define PI 3.14159265
#define SVG_TYPE_CHECK static_assert(std::is_base_of<Element, T>::value, "Child must be an SVG element.")
#define APPROX_EQUALS(x, y, tol) bool(abs(x - y) < tol)
#include <iostream>
#include <algorithm> // min, max
#include <fstream>   // ofstream
#include <math.h>    // NAN
#include <map>
#include <deque>
#include <vector>
#include <string>
#include <sstream> // stringstream
#include <iomanip> // setprecision
#include <memory>
#include <type_traits> // is_base_of
#include <typeinfo>

/**
 * SVG namespace contains classes and functions related to the Scalable Vector Graphics (SVG) format.
 *
 * get_bbox() returns the bounding box of an element; default is { NaN, NaN, NaN, NaN }
 * => consider making it optional and testing before use
 */

namespace SVG
{
    class AttributeMap;
    class SVG;
    class Shape;

    struct QuadCoord
    {
        double x1;
        double x2;
        double y1;
        double y2;
    };

    using SelectorProperties = std::map<std::string, AttributeMap>;
    using SVGAttrib = std::map<std::string, std::string>;
    using Point = std::pair<double, double>;
    using Margins = QuadCoord;

    inline std::string to_string(const double &value);
    inline std::string to_string(const Point &point);
    inline std::string to_string(const std::map<std::string, AttributeMap> &css, const size_t indent_level = 0);

    std::vector<Point> bounding_polygon(const std::vector<Shape *> &shapes);
    SVG frame_animate(std::vector<SVG> &frames, const double fps);
    const static Margins DEFAULT_MARGINS = {10, 10, 10, 10};
    const static Margins NO_MARGINS = {0, 0, 0, 0};
    SVG merge(SVG &left, SVG &right, const Margins &margins = DEFAULT_MARGINS);

    namespace util
    {
        enum Orientation
        {
            COLINEAR,
            CLOCKWISE,
            COUNTERCLOCKWISE
        };

        inline std::vector<Point> polar_points(int n, int a, int b, double radius);

        template <typename T>
        inline T min_or_not_nan(T first, T second)
        {
            if (isnan(first) && isnan(second))
                return NAN;
            else if (isnan(first) || isnan(second))
                return isnan(first) ? second : first;
            else
                return std::min(first, second);
        }

        template <typename T>
        inline T max_or_not_nan(T first, T second)
        {
            if (isnan(first) && isnan(second))
                return NAN;
            else if (isnan(first) || isnan(second))
                return isnan(first) ? second : first;
            else
                return std::max(first, second);
        }

        inline Orientation orientation(Point &p1, Point &p2, Point &p3)
        {
            double value = ((p2.second - p1.second) * (p3.first - p2.first) -
                            (p2.first - p1.first) * (p3.second - p2.second));

            if (value == 0)
                return COLINEAR;
            else if (value > 0)
                return CLOCKWISE;
            else
                return COUNTERCLOCKWISE;
        }

        inline std::vector<Point> convex_hull(std::vector<Point> &points)
        {
            if (points.size() < 3)
                return {}; // Need at least three points
            std::vector<Point> hull;

            // Find leftmost point (ties don't matter)
            int left = 0;
            for (size_t i = 0; i < points.size(); i++)
                if (points[i].first < points[left].first)
                    left = (int)i;

            // While we don't reach leftmost point
            int current = left, next;
            do
            {
                // Add to convex hull
                hull.push_back(points[current]);

                // Keep moving counterclockwise
                next = (current + 1) % points.size();
                for (size_t i = 0; i < points.size(); i++)
                {
                    // We've found a more counterclockwise point --> update next
                    if (orientation(points[current], points[next], points[i]) == COUNTERCLOCKWISE)
                        next = (int)i;
                }

                current = next;
            } while (current != left);

            return hull;
        }

        inline std::vector<Point> polar_points(int n, int a, int b, double radius)
        {
            std::vector<Point> ret;
            for (double degree = 0; degree < 360; degree += 360 / n)
            {
                ret.push_back(Point(
                    a + radius * cos(degree * (PI / 180)), // 1 degree = pi/180 radians
                    b + radius * sin(degree * (PI / 180))));
            }

            return ret;
        }
    }

    inline std::string to_string(const double &value)
    {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1);
        ss << value;
        return ss.str();
    }

    inline std::string to_string(const Point &point)
    {
        return to_string(point.first) + "," + to_string(point.second);
    }

    class AttributeMap
    {
    public:
        AttributeMap() = default;
        AttributeMap(SVGAttrib _attr) : attr(_attr) {};
        SVGAttrib attr;

        template <typename T>
        AttributeMap &set_attr(const std::string key, T value)
        {
            this->attr[key] = std::to_string(value);
            return *this;
        }
    };

    template <>
    inline AttributeMap &AttributeMap::set_attr(const std::string key, const double value)
    {
        this->attr[key] = to_string(value);
        return *this;
    }

    template <>
    inline AttributeMap &AttributeMap::set_attr(const std::string key, const char *value)
    {
        this->attr[key] = value;
        return *this;
    }

    template <>
    inline AttributeMap &AttributeMap::set_attr(const std::string key, const std::string value)
    {
        this->attr[key] = value;
        return *this;
    }

    class Element : public AttributeMap
    {
    public:
        class BoundingBox : public QuadCoord
        {
        public:
            using QuadCoord::QuadCoord;
            BoundingBox() = default;
            BoundingBox(double a, double b, double c, double d) : QuadCoord({a, b, c, d}) {};

            BoundingBox operator+(const BoundingBox &other)
            {
                using namespace util;
                BoundingBox new_box;
                new_box.x1 = min_or_not_nan(this->x1, other.x1);
                new_box.x2 = max_or_not_nan(this->x2, other.x2);
                new_box.y1 = min_or_not_nan(this->y1, other.y1);
                new_box.y2 = max_or_not_nan(this->y2, other.y2);
                return new_box;
            }
        };
        using ChildList = std::vector<Element *>;
        using ChildMap = std::map<std::string, ChildList>;

        Element() = default;
        Element(const Element &other) = delete; // No copy constructor
        Element(Element &&other) = default;     // Move constructor
        Element(const char *id) : AttributeMap(
                                      SVGAttrib({{"id", id}})) {};
        using AttributeMap::AttributeMap;

        virtual ~Element() = default;

        // Implicit string conversion
        operator std::string() { return this->svg_to_string(0); };

        template <typename T, typename... Args>
        T *add_child(Args &&...args)
        {
            SVG_TYPE_CHECK;
            this->children.push_back(std::make_unique<T>(std::forward<Args>(args)...));
            return (T *)this->children.back().get();
        }

        template <typename T>
        Element &operator<<(T &&node)
        {
            SVG_TYPE_CHECK;
            this->children.push_back(std::make_unique<T>(std::move(node)));
            return *this;
        }

        template <typename T>
        std::vector<T *> get_children()
        {
            SVG_TYPE_CHECK;
            std::vector<T *> ret;
            auto child_elems = this->get_children_helper();

            for (auto &child : child_elems)
                if (typeid(*child) == typeid(T))
                    ret.push_back((T *)child);

            return ret;
        }

        template <typename T>
        std::vector<T *> get_immediate_children()
        {
            SVG_TYPE_CHECK;
            std::vector<T *> ret;
            for (auto &child : this->children)
            {
                // Check if the child is of type T
                if (auto *derived = dynamic_cast<T *>(child.get()))
                {
                    ret.push_back(derived);
                }
            }
            return ret;
        }

        Element *get_element_by_id(const std::string &id);
        std::vector<Element *> get_elements_by_class(const std::string &clsname);
        void autoscale(const Margins &margins = DEFAULT_MARGINS);
        void autoscale(const double margin);
        virtual BoundingBox get_bbox();
        ChildMap get_children();

    protected:
        std::vector<std::unique_ptr<Element>> children;
        std::vector<Element *> get_children_helper();
        void get_bbox(Element::BoundingBox &);
        virtual std::string svg_to_string(const size_t indent_level);
        virtual std::string tag() = 0;
        double find_numeric(const std::string &key)
        {
            if (attr.find(key) != attr.end())
                return std::stof(attr[key]);
            return NAN;
        }
    };

    template <>
    inline Element::ChildList Element::get_immediate_children()
    {
        Element::ChildList ret;
        for (auto &child : this->children)
            ret.push_back(child.get());
        return ret;
    }

    inline Element *Element::get_element_by_id(const std::string &id)
    {
        auto child_elems = this->get_children_helper();
        for (auto &current : child_elems)
            if (current->attr.find("id") != current->attr.end() &&
                current->attr.find("id")->second == id)
                return current;

        return nullptr;
    }

    inline std::vector<Element *> Element::get_elements_by_class(const std::string &clsname)
    {
        std::vector<Element *> ret;
        auto child_elems = this->get_children_helper();

        for (auto &current : child_elems)
        {
            if ((current->attr.find("class") != current->attr.end()) && (current->attr.find("class")->second == clsname))
                ret.push_back(current);
        }

        return ret;
    }

    inline Element::BoundingBox Element::get_bbox()
    {
        return {NAN, NAN, NAN, NAN};
    }

    class Shape : public Element
    {
    public:
        using Element::Element;

        operator Point()
        {
            return std::make_pair(this->x(), this->y());
        }

        virtual std::vector<Point> points()
        {
            auto bbox = this->get_bbox();
            return {
                Point(bbox.x1, bbox.y1), // Top left
                Point(bbox.x2, bbox.y1), // Top right
                Point(bbox.x1, bbox.y2), // Bottom left
                Point(bbox.x2, bbox.y2)  // Bottom right
            };
        }

        virtual double x() { return this->find_numeric("x"); }
        virtual double y() { return this->find_numeric("y"); }
        virtual double width()
        {
            return this->find_numeric("width");
        }
        virtual double height()
        {
            return this->find_numeric("height");
        }
    };

    class SVG : public Shape
    {
    public:
        class Style : public Element
        {
        public:
            Style() = default;
            using Element::Element;
            SelectorProperties css;
            std::map<std::string, SelectorProperties> keyframes;

        protected:
            std::string svg_to_string(const size_t) override;
            std::string tag() override { return "style"; };
        };

        SVG(SVGAttrib _attr =
                {{"xmlns", "http://www.w3.org/2000/svg"}}) : Shape(_attr) {};
        AttributeMap &style(const std::string &key)
        {
            if (!this->css)
                this->css = this->add_child<Style>();
            return this->css->css[key];
        }

        std::map<std::string, AttributeMap> &keyframes(const std::string &key)
        {
            if (!this->css)
                this->css = this->add_child<Style>();
            return this->css->keyframes[key];
        }

        Style *css = nullptr;

    protected:
        std::string tag() override { return "svg"; }
    };

    class Path : public Shape
    {
    public:
        using Shape::Shape;

        template <typename T>
        inline void start(T x, T y)
        {
            this->attr["d"] = "M " + std::to_string(x) + " " + std::to_string(y);
            this->x_start = x;
            this->y_start = y;
        }

        template <typename T>
        inline void line_to(T x, T y)
        {
            if (this->attr.find("d") == this->attr.end())
                start(x, y);
            else
                this->attr["d"] += " L " + std::to_string(x) +
                                   " " + std::to_string(y);
        }

        inline void line_to(std::pair<double, double> coord)
        {
            this->line_to(coord.first, coord.second);
        }

        inline void to_origin()
        {
            this->line_to(x_start, y_start);
        }
        Element::BoundingBox get_bbox() override;

    protected:
        std::string tag() override { return "path"; }

    private:
        double x_start;
        double y_start;
    };

    class Text : public Element
    {
    public:
        Text() = default;
        using Element::Element;

        Text(double x, double y, std::string _content)
        {
            set_attr("x", to_string(x));
            set_attr("y", to_string(y));
            content = _content;
        }

        Text(std::pair<double, double> xy, std::string _content) : Text(xy.first, xy.second, _content) {};

        std::string get_content() const { return content; }

    protected:
        std::string content;
        std::string svg_to_string(const size_t) override;
        std::string tag() override { return "text"; }
    };

    class Group : public Element
    {
    public:
        using Element::Element;

        template <typename T, typename... Args>
        T *add_child(Args &&...args)
        {
            return Element::add_child<T>(std::forward<Args>(args)...);
        }

        template <typename T>
        std::vector<T *> get_children() const
        {
            std::vector<T *> result;
            for (const auto &child : children)
            {
                if (auto casted = dynamic_cast<T *>(child.get()))
                {
                    result.push_back(casted);
                }
            }
            return result;
        }

    protected:
        std::string tag() override { return "g"; }
    };

    class Line : public Shape
    {
    public:
        Line() = default;
        using Shape::Shape;

        Line(double x1, double x2, double y1, double y2) : Shape({{"x1", to_string(x1)},
                                                                  {"x2", to_string(x2)},
                                                                  {"y1", to_string(y1)},
                                                                  {"y2", to_string(y2)}}) {};

        Line(Point x, Point y) : Line(x.first, y.first, x.second, y.second) {};

        double x1() { return std::stof(this->attr["x1"]); }
        double x2() { return std::stof(this->attr["x2"]); }
        double y1() { return std::stof(this->attr["y1"]); }
        double y2() { return std::stof(this->attr["y2"]); }

        double width() override { return std::abs(x2() - x1()); }
        double height() override { return std::abs(y2() - y1()); }
        double get_length() { return std::sqrt(pow(width(), 2) + pow(height(), 2)); }
        double get_slope() { return (y2() - y1()) / (x2() - x1()); }

        std::pair<double, double> along(double percent);

    protected:
        Element::BoundingBox get_bbox() override;
        std::string tag() override { return "line"; }
    };

    class Rect : public Shape
    {
    public:
        Rect() = default;
        using Shape::Shape;

        Rect(
            double x, double y, double width, double height) : Shape({{"x", to_string(x)},
                                                                      {"y", to_string(y)},
                                                                      {"width", to_string(width)},
                                                                      {"height", to_string(height)}}) {};

        Element::BoundingBox get_bbox() override;

    protected:
        std::string tag() override { return "rect"; }
    };

    class Circle : public Shape
    {
    public:
        Circle() = default;
        using Shape::Shape;

        Circle(double cx, double cy, double radius) : Shape({{"cx", to_string(cx)},
                                                             {"cy", to_string(cy)},
                                                             {"r", to_string(radius)}}) {
                                                      };

        Circle(std::pair<double, double> xy, double radius) : Circle(xy.first, xy.second, radius) {};
        double radius() { return this->find_numeric("r"); }
        virtual double x() override { return this->find_numeric("cx"); }
        virtual double y() override { return this->find_numeric("cy"); }
        virtual double width() override { return this->radius() * 2; }
        virtual double height() override { return this->width(); }
        Element::BoundingBox get_bbox() override;

    protected:
        std::string tag() override { return "circle"; }
    };

    class Polygon : public Element
    {
    public:
        Polygon() = default;
        using Element::Element;

        Polygon(const std::vector<Point> &points)
        {
            // Quick and dirty
            std::string &point_str = this->attr["points"];
            for (auto &pt : points)
                point_str += to_string(pt) + " ";
        };
        Element::BoundingBox get_bbox() override;

    protected:
        std::string tag() override
        {
            return "polygon";
        }
    };

    inline Element::BoundingBox Line::get_bbox()
    {
        return {x1(), x2(), y1(), y2()};
    }

    inline Element::BoundingBox Path::get_bbox()
    {
        if (this->attr.find("d") == this->attr.end())
        {
            return {NAN, NAN, NAN, NAN};
        }

        std::istringstream iss(this->attr["d"]);
        std::string token;
        char command;
        double x, y;
        double minX = std::numeric_limits<double>::max();
        double minY = std::numeric_limits<double>::max();
        double maxX = std::numeric_limits<double>::lowest();
        double maxY = std::numeric_limits<double>::lowest();

        while (iss >> command)
        {
            if (command == 'M' || command == 'L')
            {
                iss >> x >> y;
                minX = std::min(minX, x);
                minY = std::min(minY, y);
                maxX = std::max(maxX, x);
                maxY = std::max(maxY, y);
            }
        }

        return {minX, maxX, minY, maxY};
    }

    inline Element::BoundingBox Polygon::get_bbox()
    {
        if (this->attr.find("d") == this->attr.end())
        {
            return {NAN, NAN, NAN, NAN};
        }

        std::istringstream iss(this->attr["d"]);
        std::string token;
        char command;
        double x, y;
        double minX = std::numeric_limits<double>::max();
        double minY = std::numeric_limits<double>::max();
        double maxX = std::numeric_limits<double>::lowest();
        double maxY = std::numeric_limits<double>::lowest();

        while (iss >> command)
        {
            if (command == 'M' || command == 'L')
            {
                iss >> x >> y;
                minX = std::min(minX, x);
                minY = std::min(minY, y);
                maxX = std::max(maxX, x);
                maxY = std::max(maxY, y);
            }
        }

        return {minX, maxX, minY, maxY};
    }

    inline Element::BoundingBox Rect::get_bbox()
    {
        double x = this->x(), y = this->y(),
               width = this->width(), height = this->height();
        return {x, x + width, y, y + height};
    }

    inline Element::BoundingBox Circle::get_bbox()
    {
        double x = this->x(), y = this->y(), radius = this->radius();

        return {
            x - radius,
            x + radius,
            y - radius,
            y + radius};
    }

    inline std::pair<double, double> Line::along(double percent)
    {
        double x_pos, y_pos;

        if (x1() != x2())
        {
            double length = percent * this->get_length();
            double discrim = std::sqrt(4 * pow(length, 2) * (1 / (1 + pow(get_slope(), 2))));

            double x_a = (2 * x1() + discrim) / 2;
            double x_b = (2 * x1() - discrim) / 2;
            x_pos = x_a;

            if ((x_a > x1() && x_a > x2()) || (x_a < x1() && x_a < x2()))
                x_pos = x_b;

            y_pos = get_slope() * (x_pos - x1()) + y1();
        }
        else
        { // Edge case:: Completely vertical lines
            x_pos = x1();

            if (y1() > y2()) // Downward pointing
                y_pos = y1() - percent * this->get_length();
            else
                y_pos = y1() + percent * this->get_length();
        }

        return std::make_pair(x_pos, y_pos);
    }

    inline std::string Element::svg_to_string(const size_t indent_level)
    {
        auto indent = std::string(indent_level, '\t');
        std::string ret = indent + "<" + tag();

        // Set attributes
        for (auto &pair : attr)
            ret += " " + pair.first + "=" + "\"" + pair.second + "\"";

        if (!this->children.empty())
        {
            ret += ">\n";

            // Recursively get strings for child elements
            for (auto &child : children)
                ret += child->svg_to_string(indent_level + 1) + "\n";

            return ret += indent + "</" + tag() + ">";
        }

        return ret += " />";
    }

    inline std::string to_string(const std::map<std::string, AttributeMap> &css, const size_t indent_level)
    {
        auto indent = std::string(indent_level, '\t'), ret = std::string();
        for (auto &selector : css)
        {
            // Loop over each selector's attribute/value pairs
            ret += indent + "\t\t" + selector.first + " {\n";
            for (auto &attr : selector.second.attr)
                ret += indent + "\t\t\t" + attr.first + ": " + attr.second + ";\n";
            ret += indent + "\t\t" + "}\n";
        }
        return ret;
    }

    inline std::string SVG::Style::svg_to_string(const size_t indent_level)
    {
        auto indent = std::string(indent_level, '\t');
        std::string ret = indent + "<style type=\"text/css\">\n" +
                          indent + "\t<![CDATA[\n";

        // Begin CSS stylesheet
        ret += to_string(this->css, indent_level);

        // Animation frames
        for (auto &anim : this->keyframes)
        {
            ret += indent + "\t\t@keyframes " + anim.first + " {\n" +
                   to_string(anim.second, indent_level + 1) +
                   indent + "\t\t" + "}\n";
        }

        ret += indent + "\t]]>\n";
        return ret + indent + "</style>";
    }

    inline std::string Text::svg_to_string(const size_t indent_level)
    {
        auto indent = std::string(indent_level, '\t');
        std::string ret = indent + "<text";
        for (auto &pair : attr)
            ret += " " + pair.first + "=" + "\"" + pair.second + "\"";
        return ret += ">" + this->content + "</text>";
    }

    inline void Element::autoscale(const double margin)
    {
        Element::BoundingBox bbox = this->get_bbox();
        this->get_bbox(bbox);
        double width = abs(bbox.x1) + abs(bbox.x2),
               height = abs(bbox.y1) + abs(bbox.y2);

        this->autoscale({width * margin, width * margin,
                         height * margin, height * margin});
    }

    inline void Element::autoscale(const Margins &margins)
    {
        using std::stof;

        Element::BoundingBox bbox = this->get_bbox();
        this->get_bbox(bbox); // Compute the bounding box (recursive)
        double width = abs(bbox.x1) + abs(bbox.x2) + margins.x1 + margins.x2,
               height = abs(bbox.y1) + abs(bbox.y2) + margins.y1 + margins.y2,
               x1 = bbox.x1 - margins.x1, y1 = bbox.y1 - margins.y1;

        this->set_attr("width", width)
            .set_attr("height", height);

        if (x1 < 0 || y1 < 0)
        {
            std::stringstream viewbox;
            viewbox << std::fixed << std::setprecision(1)
                    << x1 << " " // min-x
                    << y1 << " " // min-y
                    << width << " "
                    << height;
            this->set_attr("viewBox", viewbox.str());
        }
    }

    inline void Element::get_bbox(Element::BoundingBox &box)
    {
        auto this_bbox = this->get_bbox();
        box = this_bbox + box; // Take union of both
        for (auto &child : this->children)
            child->get_bbox(box); // Recursion
    }

    inline Element::ChildMap Element::get_children()
    {
        Element::ChildMap child_map;
        for (auto &child : this->get_children_helper())
            child_map[child->tag()].push_back(child);
        return child_map;
    }

    inline std::vector<Element *> Element::get_children_helper()
    {
        std::deque<Element *> temp;
        std::vector<Element *> ret;

        for (auto &child : this->children)
        {
            temp.push_back(child.get());
        }
        while (!temp.empty())
        {
            ret.push_back(temp.front());
            for (auto &child : temp.front()->children)
            {
                temp.push_back(child.get());
            }
            temp.pop_front();
        }

        return ret;
    };

    inline SVG merge(SVG &left, SVG &right, const Margins &margins)
    {
        SVG ret;

        // Move items
        ret << std::move(left) << std::move(right);

        // Set bounding box of individual pieces
        for (auto &svg_child : ret.get_immediate_children<SVG>())
            svg_child->autoscale(margins);

        // Set x position for child SVG elements, and compute width/height for this
        double x = 0, height = 0;
        for (auto &svg_child : ret.get_immediate_children<SVG>())
        {
            svg_child->set_attr("x", x).set_attr("y", 0);
            x += svg_child->width();
            height = std::max(height, svg_child->height());
        }

        ret.set_attr("width", x).set_attr("height", height);
        return ret;
    }

    inline std::vector<Point> bounding_polygon(std::vector<Shape *> &shapes)
    {
        /* Convert shapes into sets of points, aggregate them, and then calculate
         * convex hull for aggregate set
         */
        std::vector<Point> points;
        for (auto &shp : shapes)
        {
            auto temp_points = shp->points();
            std::move(temp_points.begin(), temp_points.end(), std::back_inserter(points));
        }

        return util::convex_hull(points);
    }

    inline SVG frame_animate(std::vector<SVG> &frames, const double fps)
    {
        SVG root;
        const double duration = (double)frames.size() / fps; // [seconds]
        // const double frame_step = 1.0 / fps;                 // duration of each frame [seconds]
        int current_frame = 0;

        root.style("svg.animated").set_attr("animation-iteration-count", "infinite").set_attr("animation-timing-function", "step-end").set_attr("animation-duration", std::to_string(duration) + "s").set_attr("opacity", 0);

        // Move frames into new SVG
        for (auto &frame : frames)
        {
            std::string frame_id = "frame_" + std::to_string(current_frame);
            frame.set_attr("id", frame_id).set_attr("class", "animated");
            root.style("#" + frame_id).set_attr("animation-name", "anim_" + std::to_string(current_frame));
            current_frame++;
            root << std::move(frame);
        }

        // Set animation frames
        for (size_t i = 0, ilen = frames.size(); i < ilen; i++)
        {
            auto &anim = root.keyframes("anim_" + std::to_string(i));
            double begin_pct = (double)i / frames.size(),
                   end_pct = (double)(i + 1) / frames.size();
            anim["0%"].set_attr("opacity", 0);
            anim[std::to_string(begin_pct * 100) + "%"].set_attr("opacity", 1);
            anim[std::to_string(end_pct * 100) + "%"].set_attr("opacity", 0);
        }

        // Scale and center child SVGs
        double width = 0, height = 0;

        for (auto &child : root.get_immediate_children<SVG>())
        {
            child->autoscale();
            width = std::max(width, child->width());
            height = std::max(height, child->height());
        }

        root.set_attr("viewBox", "0 0 " + std::to_string(width) + " " + std::to_string(height));

        // Center child SVGs
        for (auto &child : root.get_immediate_children<SVG>())
            child->set_attr("x", (width - child->width()) / 2).set_attr("y", (height - child->height()) / 2);

        return root;
    }
}
