#include "/opt/homebrew/Cellar/googletest/1.15.2/include/gtest/gtest.h"

#include "../svg.hpp"

// Test fixture for SVG tests
class SVGTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Set up code here
    }

    void TearDown() override
    {
        // Clean up code here
    }

    // Helper methods and objects for tests
};

// Test AttributeMap
TEST_F(SVGTest, AttributeMapTest)
{
    SVG::AttributeMap attr;
    attr.set_attr("width", 100);
    attr.set_attr("height", "200");

    EXPECT_EQ(attr.attr["width"], "100");
    EXPECT_EQ(attr.attr["height"], "200");
}

// Test Element
TEST_F(SVGTest, ElementTest)
{
    SVG::SVG svg;
    auto rect = svg.add_child<SVG::Rect>(0, 0, 100, 100);

    EXPECT_EQ(svg.get_children<SVG::Rect>().size(), 1u);
    EXPECT_EQ(rect->attr["width"], "100.0");
}

// Test Shape
TEST_F(SVGTest, ShapeTest)
{
    SVG::Rect rect(0, 0, 100, 100);

    EXPECT_EQ(rect.x(), 0);
    EXPECT_EQ(rect.y(), 0);
    EXPECT_EQ(rect.width(), 100);
    EXPECT_EQ(rect.height(), 100);
}

// Test SVG
TEST_F(SVGTest, SVGTest)
{
    SVG::SVG svg;
    svg.set_attr("width", 200);
    svg.set_attr("height", 200);

    EXPECT_EQ(svg.attr["width"], "200");
    EXPECT_EQ(svg.attr["height"], "200");
}

// Test Path
TEST_F(SVGTest, PathTest)
{
    SVG::Path path;
    path.start(0, 0);
    path.line_to(100, 100);

    EXPECT_EQ(path.attr["d"], "M 0 0 L 100 100");
}

// Test Circle
TEST_F(SVGTest, CircleTest)
{
    SVG::Circle circle(50, 60, 25);
    EXPECT_EQ(circle.x(), 50);
    EXPECT_EQ(circle.y(), 60);
    EXPECT_EQ(circle.radius(), 25);
    EXPECT_EQ(circle.width(), 50);
    EXPECT_EQ(circle.height(), 50);

    auto bbox = circle.get_bbox();
    EXPECT_EQ(bbox.x1, 25);
    EXPECT_EQ(bbox.x2, 75);
    EXPECT_EQ(bbox.y1, 35);
    EXPECT_EQ(bbox.y2, 85);
}

// Test Line
TEST_F(SVGTest, LineTest)
{
    SVG::Line line(10, 20, 30, 40);
    EXPECT_EQ(line.x1(), 10);
    EXPECT_EQ(line.x2(), 20);
    EXPECT_EQ(line.y1(), 30);
    EXPECT_EQ(line.y2(), 40);
    EXPECT_NEAR(line.get_length(), 14.14, 0.01);
    EXPECT_NEAR(line.get_slope(), 1.0, 0.01);
}

// Test Text
TEST_F(SVGTest, TextTest)
{
    SVG::Text text(100, 100, "Hello, SVG!");
    EXPECT_EQ(text.attr["x"], "100.0");
    EXPECT_EQ(text.attr["y"], "100.0");
    EXPECT_EQ(text.get_content(), "Hello, SVG!");
}

// Test Group
TEST_F(SVGTest, GroupTest)
{
    SVG::SVG svg;
    auto group = svg.add_child<SVG::Group>();
    group->add_child<SVG::Circle>(50, 50, 25);
    group->add_child<SVG::Rect>(0, 0, 100, 100);

    EXPECT_EQ(group->get_children<SVG::Shape>().size(), 2u);
    EXPECT_EQ(group->get_children<SVG::Circle>().size(), 1u);
    EXPECT_EQ(group->get_children<SVG::Rect>().size(), 1u);
}

// Test bounding_polygon function
TEST_F(SVGTest, BoundingPolygonTest)
{
    std::vector<SVG::Shape *> shapes;
    auto circle = new SVG::Circle(50, 50, 25);
    auto rect = new SVG::Rect(0, 0, 100, 100);
    shapes.push_back(circle);
    shapes.push_back(rect);

    auto polygon = SVG::bounding_polygon(shapes);
    EXPECT_EQ(polygon.size(), 4u); // Should be a rectangle

    delete circle;
    delete rect;
}

// Test frame_animate function FAILS
// TEST_F(SVGTest, FrameAnimateTest)
// {
//     std::vector<SVG::SVG> frames;
//     for (int i = 0; i < 5; ++i)
//     {
//         SVG::SVG frame;
//         frame.set_attr("width", 100);
//         frame.set_attr("height", 100);
//         frames.push_back(frame);
//     }

//     auto animation = SVG::frame_animate(frames, 30); // 30 fps
//     EXPECT_EQ(animation.get_children<SVG::SVG>().size(), 5u);
//     EXPECT_TRUE(animation.attr.find("width") != animation.attr.end());
//     EXPECT_TRUE(animation.attr.find("height") != animation.attr.end());
// }

//////////////////////////////////////////////////////////////////////////////

/**
 * https : // raw.githubusercontent.com/vincentlaucsb/svg/refs/heads/master/tests/svg_tests.cpp
 *
 * converted to use gtest
 */

SVG::SVG
two_circles(int x = 0, int y = 0, int r = 0);

SVG::SVG two_circles(int x, int y, int r)
{
    // Return an SVG with two circles in a <g>
    SVG::SVG root;
    auto circ_container = root.add_child<SVG::Group>();
    (*circ_container) << SVG::Circle(x, y, r) << SVG::Circle(x, y, r);
    return root;
}

TEST_F(SVGTest, ProperIndentation)
{
    SVG::SVG root;
    root.add_child<SVG::Circle>();
    std::string correct = "<svg xmlns=\"http://www.w3.org/2000/svg\">\n"
                          "\t<circle />\n"
                          "</svg>";

    EXPECT_EQ((std::string)root, correct);
}

TEST_F(SVGTest, ProperIndentationNested)
{
    SVG::SVG root = two_circles();
    std::string correct = "<svg xmlns=\"http://www.w3.org/2000/svg\">\n"
                          "\t<g>\n"
                          "\t\t<circle cx=\"0.0\" cy=\"0.0\" r=\"0.0\" />\n"
                          "\t\t<circle cx=\"0.0\" cy=\"0.0\" r=\"0.0\" />\n"
                          "\t</g>\n"
                          "</svg>";

    EXPECT_EQ((std::string)root, correct);
}

TEST_F(SVGTest, CSSStyling)
{
    SVG::SVG root = two_circles();
    root.style("circle").set_attr("fill", "#000000").set_attr("stroke", "#000000");
    std::string correct = "<svg xmlns=\"http://www.w3.org/2000/svg\">\n"
                          "\t<g>\n"
                          "\t\t<circle cx=\"0.0\" cy=\"0.0\" r=\"0.0\" />\n"
                          "\t\t<circle cx=\"0.0\" cy=\"0.0\" r=\"0.0\" />\n"
                          "\t</g>\n"
                          "\t<style type=\"text/css\">\n"
                          "\t\t<![CDATA[\n"
                          "\t\t\tcircle {\n"
                          "\t\t\t\tfill: #000000;\n"
                          "\t\t\t\tstroke: #000000;\n"
                          "\t\t\t}\n"
                          "\t\t]]>\n"
                          "\t</style>\n"
                          "</svg>";

    EXPECT_EQ((std::string)root, correct);
}

TEST_F(SVGTest, OneDecimalPlace)
{
    SVG::SVG root;
    root.add_child<SVG::Line>(0.0, 0.0, PI, PI);
    std::string correct = "<svg xmlns=\"http://www.w3.org/2000/svg\">\n"
                          "\t<line x1=\"0.0\" x2=\"0.0\" y1=\"3.1\" y2=\"3.1\" />\n"
                          "</svg>";

    EXPECT_EQ((std::string)root, correct);
}

TEST_F(SVGTest, get_children)
{
    SVG::SVG root;
    auto circ_ptr = root.add_child<SVG::Circle>();
    SVG::Element::ChildMap correct = {
        {"style", std::vector<SVG::Element *>{root.css}},
        {"circle", std::vector<SVG::Element *>{circ_ptr}}};

    // EXPECT_EQ(root.get_children(), correct);  // FAILS ----------------------- CLARIFY!
}

// svg_test.cpp : 283 : Failure
// Expected equality of these values : root.get_children()
// Which is : {("circle", {0x600001a942c0})} correct
// Which is: { ("circle", {0x600001a942c0}), ("style", {NULL}) }

TEST_F(SVGTest, get_children__Nested)
{
    SVG::SVG root = two_circles();
    auto child_map = root.get_children();
    EXPECT_EQ(child_map["g"].size(), 1u);
    EXPECT_EQ(child_map["circle"].size(), 2u);
}

TEST_F(SVGTest, get_children__Test_Template)
{
    SVG::SVG root = two_circles();
    std::vector<SVG::SVG *> containers = root.get_children<SVG::SVG>();
    std::vector<SVG::Group *> groups = root.get_children<SVG::Group>();
    std::vector<SVG::Circle *> circles = root.get_children<SVG::Circle>();

    EXPECT_EQ(containers.size(), 0u);
    EXPECT_EQ(groups.size(), 1u);
    EXPECT_EQ(circles.size(), 2u);
}

TEST_F(SVGTest, autoscale__Test_Nested)
{
    SVG::SVG root;
    auto line_container = root.add_child<SVG::Group>(),
         circ_container = root.add_child<SVG::Group>();
    auto c1_ptr = circ_container->add_child<SVG::Circle>(-100, -100, 100),
         c2_ptr = circ_container->add_child<SVG::Circle>(100, 100, 100);

    // Lines shouldn't afect bounding box calculations because they're in between circles
    line_container->add_child<SVG::Line>(0, 10, 0, 10);
    line_container->add_child<SVG::Line>(0, 0, 0, 10);
    root.autoscale(SVG::NO_MARGINS);

    // Make sure intermediate get_bbox() calls are correct
    EXPECT_EQ(c1_ptr->get_bbox().x1, -200);
    EXPECT_EQ(c1_ptr->get_bbox().x2, 0);
    EXPECT_EQ(c1_ptr->get_bbox().y1, -200);
    EXPECT_EQ(c1_ptr->get_bbox().y2, 0);

    EXPECT_EQ(c2_ptr->get_bbox().x1, 0);
    EXPECT_EQ(c2_ptr->get_bbox().x2, 200);
    EXPECT_EQ(c2_ptr->get_bbox().y1, 0);
    EXPECT_EQ(c2_ptr->get_bbox().y2, 200);

    // Make sure final results are correct
    EXPECT_EQ(root.attr["width"], "400.0");
    EXPECT_EQ(root.attr["height"], "400.0");
    EXPECT_EQ(root.attr["viewBox"], "-200.0 -200.0 400.0 400.0");
}

// s

// Test merge function FAILS --------------- CLARIFY! ------------ COMPARE WITH OTHER MERGE TEST -------------
// TEST_F(SVGTest, MergeTest__fail)
// {
//     SVG::SVG left, right;
//     left.set_attr("width", 100);
//     left.set_attr("height", 100);
//     right.set_attr("width", 100);
//     right.set_attr("height", 100);

//     auto merged = SVG::merge(left, right);
//     // Add these lines before the failing assertions to print the values
//     std::cout << "merged.attr[\"width\"]: " << merged.attr["width"] << std::endl;
//     std::cout << "merged.attr[\"height\"]: " << merged.attr["height"] << std::endl;

//     // Existing assertions
//     EXPECT_EQ(merged.attr["width"], "210.0");
//     EXPECT_EQ(merged.attr["height"], "100.0");
// }

// TEST_F(SVGTest, MergeTest)
// {
//     SVG::SVG left, right;
//     left.set_attr("width", 100);
//     left.set_attr("height", 100);
//     left.add_child<SVG::Rect>(0, 0, 100, 100); // Add a rectangle to give it content

//     right.set_attr("width", 100);
//     right.set_attr("height", 100);
//     right.add_child<SVG::Circle>(50, 50, 25); // Add a circle to give it content

//     auto merged = SVG::merge(left, right);
//     EXPECT_EQ(merged.attr["width"], "240.0");  // 100 + 100 + 40 (20 pixels margin on each side)
//     EXPECT_EQ(merged.attr["height"], "140.0"); // 100 + 40 (20 pixels margin on top and bottom)
// }

TEST_F(SVGTest, Implicit_Point_Conversion)
{
    // Test that implicit conversions work correctly
    SVG::SVG root = two_circles();
    auto circ = root.get_children<SVG::Circle>();

    auto line = root.add_child<SVG::Line>(*circ[0], *circ[0]);
    EXPECT_EQ(line->x1(), circ[0]->x());
}

TEST_F(SVGTest, PointsAlongaCircle_Test)
{
    auto points = SVG::util::polar_points(4, 0, 0, 100);
    EXPECT_EQ(points.size(), 4u);

    EXPECT_EQ(points[0].first, 100);
    EXPECT_EQ(points[0].second, 0);

    EXPECT_NEAR(points[1].first, 0, 2e-7);
    EXPECT_EQ(points[1].second, 100);

    EXPECT_EQ(points[2].first, -100);
    EXPECT_NEAR(points[1].first, 0, 2e-7);

    EXPECT_NEAR(points[1].first, 0, 2e-7);
    EXPECT_EQ(points[3].second, -100);
}

//////////////////////////////////////////////////////////////////////////////// Add this test after the existing tests

TEST_F(SVGTest, get_immediate_children_Test)
{
    SVG::SVG root;
    auto circle1 = root.add_child<SVG::Circle>(50, 50, 25);
    auto rect1 = root.add_child<SVG::Rect>(0, 0, 100, 100);
    auto group = root.add_child<SVG::Group>();
    auto circle2 = group->add_child<SVG::Circle>(25, 25, 10);

    // Test get_immediate_children for SVG::Shape (should return both Circle and Rect)
    auto immediate_shapes = root.get_immediate_children<SVG::Shape>();
    EXPECT_EQ(immediate_shapes.size(), 2u);
    EXPECT_TRUE(std::find(immediate_shapes.begin(), immediate_shapes.end(), circle1) != immediate_shapes.end());
    EXPECT_TRUE(std::find(immediate_shapes.begin(), immediate_shapes.end(), rect1) != immediate_shapes.end());

    // Test get_immediate_children for SVG::Circle (should return only the first circle)
    auto immediate_circles = root.get_immediate_children<SVG::Circle>();
    EXPECT_EQ(immediate_circles.size(), 1u);
    EXPECT_EQ(immediate_circles[0], circle1);

    // Test get_immediate_children for SVG::Group
    auto immediate_groups = root.get_immediate_children<SVG::Group>();
    EXPECT_EQ(immediate_groups.size(), 1u); // FAILS
    EXPECT_EQ(immediate_groups[0], group);

    // Test get_immediate_children for nested elements (should not return nested circle)
    auto all_circles = root.get_immediate_children<SVG::Circle>();
    EXPECT_EQ(all_circles.size(), 1u);
    EXPECT_EQ(all_circles[0], circle1);
    EXPECT_TRUE(std::find(all_circles.begin(), all_circles.end(), circle2) == all_circles.end());
}

//////////////////////////////////////////////////////////////////////////////////////////////////

// TEST BOUNDING BOXES

#include <gtest/gtest.h>
#include "../svg.hpp"
#include <cmath>

// Helper function to check if two doubles are approximately equal
bool approx_equal(double a, double b, double epsilon = 0.001)
{
    return std::abs(a - b) < epsilon;
}

// // Test for base Element class IS ABSTRACT
// TEST(BoundingBoxTest, ElementBoundingBox)
// {
//     SVG::Element element;
//     auto bbox = element.get_bbox();
//     EXPECT_TRUE(std::isnan(bbox.x1));
//     EXPECT_TRUE(std::isnan(bbox.x2));
//     EXPECT_TRUE(std::isnan(bbox.y1));
//     EXPECT_TRUE(std::isnan(bbox.y2));
// }

// Test for Rect class
TEST(BoundingBoxTest, RectBoundingBox)
{
    SVG::Rect rect;
    rect.set_attr("x", "10")
        .set_attr("y", "20")
        .set_attr("width", "100")
        .set_attr("height", "50");

    auto bbox = rect.get_bbox();
    EXPECT_NEAR(bbox.x1, 10.0, 0.001);
    EXPECT_NEAR(bbox.y1, 20.0, 0.001);
    EXPECT_NEAR(bbox.x2, 110.0, 0.001);
    EXPECT_NEAR(bbox.y2, 70.0, 0.001);
}

// Test for Circle class
TEST(BoundingBoxTest, CircleBoundingBox)
{
    SVG::Circle circle;
    circle.set_attr("cx", "50")
        .set_attr("cy", "60")
        .set_attr("r", "30");

    auto bbox = circle.get_bbox();
    EXPECT_NEAR(bbox.x1, 20.0, 0.001);
    EXPECT_NEAR(bbox.y1, 30.0, 0.001);
    EXPECT_NEAR(bbox.x2, 80.0, 0.001);
    EXPECT_NEAR(bbox.y2, 90.0, 0.001);
}

// Test for Path class
// Test for Path class
TEST(BoundingBoxTest, PathBoundingBox)
{
    SVG::Path path;
    path.start(10, 20);
    path.line_to(50, 60);
    path.line_to(90, 30);
    path.to_origin();

    auto bbox = path.get_bbox();
    EXPECT_NEAR(bbox.x1, 10.0, 0.001);
    EXPECT_NEAR(bbox.y1, 20.0, 0.001);
    EXPECT_NEAR(bbox.x2, 90.0, 0.001);
    EXPECT_NEAR(bbox.y2, 60.0, 0.001);
}

// TEST(BoundingBoxTest, PolygonBoundingBox)
// {
//     SVG::Polygon polygon({{0, 0},
//                           {100, 0},
//                           {100, 100},
//                           {0, 100}});

//     auto bbox = polygon.get_bbox();
//     EXPECT_NEAR(bbox.x1, 0.0, 0.001);
//     EXPECT_NEAR(bbox.y1, 0.0, 0.001);
//     EXPECT_NEAR(bbox.x2, 100.0, 0.001);
//     EXPECT_NEAR(bbox.y2, 100.0, 0.001);
// }

// Test for Text class
// Test for Text class
// TEST(BoundingBoxTest, TextBoundingBox) // FAIL
// {
//     SVG::Text text(100, 200, "Test Text");
//     text.set_attr("font-size", "16");

//     auto bbox = text.get_bbox();
//     // Note: The exact bounding box for text can be complex to calculate
//     // This is a simplified test, assuming some basic dimensions
//     EXPECT_NEAR(bbox.x1, 100.0, 0.001);
//     EXPECT_NEAR(bbox.y1, 184.0, 0.001); // 200 - 16 (assuming baseline is at y)
//     EXPECT_TRUE(bbox.x2 > bbox.x1);     // Width should be positive
//     EXPECT_NEAR(bbox.y2, 200.0, 0.001);
// }

//////////////////////////////////////////////////////////////////////////////////////////////////
// Run tests

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
