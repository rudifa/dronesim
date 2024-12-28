#ifndef msplot_H
#define msplot_H

#include <vector>
#include <iostream>

#include "../simple_svg/simple_svg_1.0.0.hpp"

using namespace svg;

class MSPlot
{
    struct SubplotFrame
    {
        std::vector<Point> data;
        std::string label;
        Color color;
        int x_pos;
        int y_pos;
        int full_width;
        int full_height;

        struct Plot
        {
            int x_pos;
            int y_pos;
            int width;
            int height;
            const std::vector<Point>& data;
            const std::string& label;
            const Color& color;

            Plot(int x, int y, int w, int h, const std::vector<Point>& d, const std::string& l, const Color& c)
                : x_pos(x), y_pos(y), width(w), height(h), data(d), label(l), color(c) {}

            Group render() const
            {
                Group group;

                // Find min and max values for x and y
                double x_min = std::numeric_limits<double>::max();
                double x_max = std::numeric_limits<double>::lowest();
                double y_min = std::numeric_limits<double>::max();
                double y_max = std::numeric_limits<double>::lowest();

                for (const auto& point : data) {
                    x_min = std::min(x_min, point.x);
                    x_max = std::max(x_max, point.x);
                    y_min = std::min(y_min, point.y);
                    y_max = std::max(y_max, point.y);
                }

                // Add data polyline
                Polyline polyline(Stroke(1, color));
                for (const auto& point : data) {
                    double scaled_x = x_pos + (point.x - x_min) / (x_max - x_min) * width;
                    double scaled_y = y_pos + height - (point.y - y_min) / (y_max - y_min) * height;
                    polyline << Point(scaled_x, scaled_y);
                }
                group << polyline;

                // Add x and y axes
                group << Line(Point(x_pos, y_pos), Point(x_pos + width, y_pos), Stroke(1, Color::Black)); // X-axis
                group << Line(Point(x_pos, y_pos + height), Point(x_pos + width, y_pos + height), Stroke(1, Color::Black)); // X-axis

                group << Line(Point(x_pos, y_pos), Point(x_pos, y_pos + height), Stroke(1, Color::Black)); // Y-axis
                group << Line(Point(x_pos + width, y_pos), Point(x_pos + width, y_pos + height), Stroke(1, Color::Black)); // Y-axis

                // Add axis ticks and values
                const int n = 5;
                for (int i = 0; i <= n; i++)
                {
                    double x_val = x_min + (x_max - x_min) * i / n;
                    double x_pos_tick = x_pos + width * i / n;
                    // Add tick mark and value
                    group << Line(Point(x_pos_tick, y_pos + height),
                                    Point(x_pos_tick, y_pos + height + 5),
                                    Stroke(1, Color::Black));
                    group << Text(Point(x_pos_tick, y_pos + height + 15),
                                    std::to_string(x_val),
                                    Fill(Color::Black),
                                    Font(10, "Arial"));
                }

                // Add labels
                Text x_label(Point(x_pos + width / 2, y_pos + height + 20), "X-axis", Fill(Color::Black), Font(12, "Arial"));
                group << x_label;

                Text y_label(Point(x_pos - 10, y_pos + height / 2), "Y-axis", Fill(Color::Black), Font(12, "Arial"));
                y_label.setRotation(-90);
                group << y_label;

                Text title(Point(x_pos + width / 2, y_pos - 10), label, Fill(Color::Black), Font(14, "Arial"));
                group << title;

                return group;
            }
        };

        Group render() const
        {
            Group group;

            const int margin = 40; // Add margins for labels
            int plot_width = full_width - 2 * margin;
            int plot_height = full_height - 2 * margin;

            // Create and render the Plot
            Plot plot(x_pos + margin, y_pos + margin, plot_width, plot_height,
                      data, label, color);
            group << plot.render();

            // Add border around the entire subplot
            group << Rectangle(Point(x_pos, y_pos),
                               x_pos + full_width,
                               y_pos + full_height,
                               Fill(), Stroke(1, color, true));

            return group;
        }

        // We can keep this operator for convenience, but it's not strictly necessary
        friend Document &operator<<(Document &svg, const SubplotFrame &subplot)
        {
            svg << subplot.render();
            return svg;
        }
    };

    public:

    class Figure
    {
        std::vector<SubplotFrame> subplots;
        int width;
        int height;
        Document svg;

    public:
        // Use TopLeft layout so that (0, 0) is the top left corner of the SVG
        // but specify coordinates in user space (origin at bottom left, +y axis upwards)
        Figure(int w, int h)
            : width(w), height(h),
              svg("", Layout(Dimensions(w, h), Layout::TopLeft))
        {
        }

        void addSubplot(int rows, int cols, int position)
        {
            if (position >= rows * cols) {
                throw std::out_of_range("Subplot position exceeds grid size");
            }

            SubplotFrame plot;
            plot.full_width = width / cols;
            plot.full_height = height / rows;
            plot.x_pos = (position % cols) * plot.full_width;
            plot.y_pos = (position / cols) * plot.full_height;
            subplots.push_back(plot);
        }

        void plot(const std::vector<double> &x, const std::vector<double> &y,
                const std::string &label = "", const Color &color = Color::Blue)
        {
            std::cerr << "Figure::plot, " << label  << std::endl;

            if (subplots.empty()) {
                throw std::runtime_error("No subplot available. Call addSubplot first.");
            }
            if (x.size() != y.size()) {
                throw std::invalid_argument("X and Y vectors must have same size");
            }
            if (x.empty()) {
                throw std::invalid_argument("Data vectors cannot be empty");
            }

            auto &subplot = getCurrentSubplot();
            subplot.data.clear(); // Clear any existing data
            for (size_t i = 0; i < x.size(); i++) {
                subplot.data.push_back({x[i], y[i]});
            }
            subplot.label = label;
            subplot.color = color;
        }

        SubplotFrame &getCurrentSubplot()
        {
            return subplots.back();
        }

        std::string toString()
        {
            Group figure_group;
            for (const auto &subplot : subplots)
            {
                figure_group << subplot.render();
            }
            svg << figure_group;

            return svg.toString();
        }

        bool save(const std::string& filename)
        {
            if (subplots.empty())
            {
                std::cerr << "No plots to save" << std::endl;
            }

            std::ofstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Failed to open file: " << filename << std::endl;
                return false;
            }

        //     std::cerr << "Saving:\n"
        //               << toString()
        //    << std::endl;

            file << toString();
            file.close();
            std::cerr << filename << " saved successfully" << std::endl;
            return true;
        }
    };
};
#endif // msplot_H
