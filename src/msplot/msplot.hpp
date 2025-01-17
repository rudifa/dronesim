#ifndef msplot_H
#define msplot_H

#include <iostream>
#include <vector>

#include "../simple_svg/simple_svg_1.0.0.hpp"

using namespace svg;

class MSPlot
{
    struct SubplotFrame
    {
        std::vector<Point> data;
        std::string label;
        std::string xlabel;
        std::string ylabel;
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
            const std::vector<Point> &data;
            const std::string &p_title;
            const std::string &x_label;
            const std::string &y_label;
            const Color &color;

            Plot(int x, int y, int w, int h, const std::vector<Point> &d,
                 const std::string &p_title, const std::string &x_label,
                 const std::string &y_label, const Color &c)
                : x_pos(x),
                  y_pos(y),
                  width(w),
                  height(h),
                  data(d),
                  p_title(p_title),
                  x_label(x_label),
                  y_label(y_label),
                  color(c)
            {
            }

            Group render() const
            {
                Group group;

                // Find min and max values for x and y
                double x_min = std::numeric_limits<double>::max();
                double x_max = std::numeric_limits<double>::lowest();
                double y_min = std::numeric_limits<double>::max();
                double y_max = std::numeric_limits<double>::lowest();

                for (const auto &point : data)
                {
                    x_min = std::min(x_min, point.x);
                    x_max = std::max(x_max, point.x);
                    y_min = std::min(y_min, point.y);
                    y_max = std::max(y_max, point.y);
                }

                // Add x and y axes and frame
                group << Line(Point(x_pos, y_pos), Point(x_pos + width, y_pos),
                              Stroke(1, Color(Color::Black)));  // X-axis
                group << Line(Point(x_pos, y_pos + height),
                              Point(x_pos + width, y_pos + height),
                              Stroke(1, Color(Color::Black)));  // X-axis

                group << Line(Point(x_pos, y_pos), Point(x_pos, y_pos + height),
                              Stroke(1, Color(Color::Black)));  // Y-axis
                group << Line(Point(x_pos + width, y_pos),
                              Point(x_pos + width, y_pos + height),
                              Stroke(1, Color(Color::Black)));  // Y-axis

                // Add x axis ticks and values
                const int n = 5;
                for (int i = 0; i <= n; i++)
                {
                    double x_val = x_min + (x_max - x_min) * i / n;
                    double x_pos_tick = x_pos + width * i / n;
                    // Add tick mark and value
                    group << Line(Point(x_pos_tick, y_pos + height),
                                  Point(x_pos_tick, y_pos + height + 5),
                                  Stroke(1, Color(Color::Black)));
                    group << Text(Point(x_pos_tick, y_pos + height + 15),
                                  formatValue(x_val), Fill(Color::Black),
                                  Font(10, "Arial"));
                }

                // Add y axis ticks and values
                for (int i = 0; i <= n; i++)
                {
                    double y_val = y_min + (y_max - y_min) * i / n;
                    double y_pos_tick = y_pos + height - (height * i / n);
                    // Add tick mark and value
                    group << Line(Point(x_pos, y_pos_tick),
                                  Point(x_pos - 5, y_pos_tick),
                                  Stroke(1, Color(Color::Black)));
                    group << Text(Point(x_pos - 30, y_pos_tick),
                                  formatValue(y_val), Fill(Color::Black),
                                  Font(10, "Arial"));
                }

                // Add labels
                Text xlabel(Point(x_pos + width / 2, y_pos + height + 20),
                            x_label, Fill(Color::Black), Font(12, "Arial"));
                group << xlabel;

                Text ylabel(Point(x_pos - 40, y_pos + height / 2), y_label,
                            Fill(Color::Black), Font(12, "Arial"));
                ylabel.setRotation(-90);
                group << ylabel;

                Text ptitle(Point(x_pos + width / 2, y_pos - 10), p_title,
                            Fill(Color::Black), Font(14, "Arial"));
                group << ptitle;

                // Add data polyline
                Polyline polyline(Stroke(2, color));
                for (const auto &point : data)
                {
                    double scaled_x =
                        x_pos + (point.x - x_min) / (x_max - x_min) * width;
                    double scaled_y =
                        y_pos + height -
                        (point.y - y_min) / (y_max - y_min) * height;
                    polyline << Point(scaled_x, scaled_y);
                }
                group << polyline;

                return group;
            }
        };

        Group render() const
        {
            Group group;

            const int l_margin = 60;   // Left margin
            const int r_margin = 30;   // Right margin
            const int tb_margin = 40;  // Top and bottom margin
            int plot_width = full_width - (l_margin + r_margin);
            int plot_height = full_height - (2 * tb_margin);

            // Create and render the Plot
            Plot plot(x_pos + l_margin, y_pos + tb_margin, plot_width,
                      plot_height, data, label, xlabel, ylabel, color);
            group << plot.render();

            // Add border around the entire subplot
            group << Rectangle(Point(x_pos, y_pos), x_pos + full_width,
                               y_pos + full_height, Fill(),
                               Stroke(1, color, true));

            return group;
        }

        // We can keep this operator for convenience, but it's not strictly
        // necessary
        friend Document &operator<<(Document &svg, const SubplotFrame &subplot)
        {
            svg << subplot.render();
            return svg;
        }
    };

    static std::string formatValue(double value, int precision = 2)
    {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(precision) << value;
        return ss.str();
    }

   public:
    class Figure
    {
        std::vector<SubplotFrame> subplotFrames;
        int width;
        int height;
        Document svg;

       public:
        // Use TopLeft layout so that (0, 0) is the top left corner of the SVG
        // but specify coordinates in user space (origin at bottom left, +y axis
        // upwards)
        Figure(int w, int h)
            : width(w),
              height(h),
              svg("", Layout(Dimensions(w, h), Layout::TopLeft))
        {
        }

        void addSubplot(int rows, int cols, int position)
        {
            if (position >= rows * cols)
            {
                throw std::out_of_range("Subplot position exceeds grid size");
            }

            SubplotFrame frame;
            frame.full_width = width / cols;
            frame.full_height = height / rows;
            frame.x_pos = (position % cols) * frame.full_width;
            frame.y_pos = (position / cols) * frame.full_height;
            subplotFrames.push_back(frame);
        }

        void plot(const std::vector<double> &x, const std::vector<double> &y,
                  const Color &color = Color(Color::Blue))
        {
            if (x.size() != y.size())
            {
                throw std::invalid_argument(
                    "X and Y vectors must have same size");
            }
            if (x.empty())
            {
                throw std::invalid_argument("Data vectors cannot be empty");
            }

            auto &subplot = getCurrentSubplotFrame();
            subplot.data.clear();  // Clear any existing data
            for (size_t i = 0; i < x.size(); i++)
            {
                subplot.data.push_back(Point(x[i], y[i]));
            }
            subplot.color = color;
        }
        void title(const std::string &title)
        {
            auto &subplot = getCurrentSubplotFrame();
            subplot.label = title;
        }

        void xlabel(const std::string &label)
        {
            auto &subplot = getCurrentSubplotFrame();
            subplot.xlabel = label;
        }

        void ylabel(const std::string &label)
        {
            auto &subplot = getCurrentSubplotFrame();
            subplot.ylabel = label;
        }

        SubplotFrame &getCurrentSubplotFrame()
        {
            if (subplotFrames.empty())
            {
                throw std::runtime_error(
                    "No subplot available. Call addSubplot first.");
            }
            return subplotFrames.back();
        }

        std::string toString()
        {
            Group figure_group;
            for (const auto &subplot : subplotFrames)
            {
                figure_group << subplot.render();
            }
            svg << figure_group;

            return svg.toString();
        }

        bool save(const std::string &filename)
        {
            if (subplotFrames.empty())
            {
                std::cerr << "No plots to save" << std::endl;
            }

            std::ofstream file(filename);
            if (!file.is_open())
            {
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
#endif  // msplot_H
