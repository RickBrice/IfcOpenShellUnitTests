#include "pch.h"
#include "CppUnitTest.h"

// Disable warnings coming from IfcOpenShell
#pragma warning(disable:4018 4267 4250 4984 4985)

#include <ifcparse/IfcHierarchyHelper.h>
#include <ifcparse/Ifc4x3_add2.h>
#include <ifcgeom/abstract_mapping.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define Schema Ifc4x3_add2

namespace IfcOpenShellUnitTests
{
	// Test cases based on FHWA Bridge Geometry Manual
	TEST_CLASS(FHWA_Bridge_Geometry_Alignment_Example)
	{
	public:

		TEST_METHOD(Alignment_Points)
		{
			IfcParse::IfcFile file("../../Files/FHWA_Bridge_Geometry_Alignment_Example.ifc");

			auto curves = file.instances_by_type<Schema::IfcCompositeCurve>();
			Assert::AreEqual(2u, curves->size());
			auto curve = (*(curves->begin()))->as<Schema::IfcCompositeCurve>();
			Assert::IsNotNull(curve);

			// Example 5.1, Point at 110+00
			{
				auto pde = new Schema::IfcPointByDistanceExpression(
					new Schema::IfcLengthMeasure(1000.),
					boost::none, boost::none, boost::none,
					curve);

				auto pl = new Schema::IfcAxis2PlacementLinear(pde, nullptr, nullptr);
				auto lp = new Schema::IfcLinearPlacement(nullptr, pl, nullptr);
				file.addEntity(lp);
			}

			// Example 5.2, Point at 145+00
			{
				auto pde = new Schema::IfcPointByDistanceExpression(
					new Schema::IfcLengthMeasure(4500.),
					boost::none, boost::none, boost::none,
					curve);

				auto pl = new Schema::IfcAxis2PlacementLinear(pde, nullptr, nullptr);
				auto lp = new Schema::IfcLinearPlacement(nullptr, pl, nullptr);
				file.addEntity(lp);
			}

			// Example 5.3, Point at 145+00, 20 ft left
			{
				auto pde = new Schema::IfcPointByDistanceExpression(
					new Schema::IfcLengthMeasure(4500.),
					20.0, boost::none, boost::none,
					curve);

				auto pl = new Schema::IfcAxis2PlacementLinear(pde, nullptr, nullptr);
				auto lp = new Schema::IfcLinearPlacement(nullptr, pl, nullptr);
				file.addEntity(lp);
			}

			// Example 5.3, Point at 145+00, 20 ft right
			{
				auto pde = new Schema::IfcPointByDistanceExpression(
					new Schema::IfcLengthMeasure(4500.),
					-20.0, boost::none, boost::none,
					curve);

				auto pl = new Schema::IfcAxis2PlacementLinear(pde, nullptr, nullptr);
				auto lp = new Schema::IfcLinearPlacement(nullptr, pl, nullptr);
				file.addEntity(lp);
			}

			// Example 5.7 - coordinate of point at Station 130+00
			// Distance from start is 3000 ft
			{
				auto pde = new Schema::IfcPointByDistanceExpression(
					new Schema::IfcLengthMeasure(3000.),
					boost::none, boost::none, boost::none,
					curve);

				auto pl = new Schema::IfcAxis2PlacementLinear(pde, nullptr, nullptr);
				auto lp = new Schema::IfcLinearPlacement(nullptr, pl, nullptr);
				file.addEntity(lp);
			}

			// Example 5.8, offset 10 ft right
			{
				auto pde = new Schema::IfcPointByDistanceExpression(
					new Schema::IfcLengthMeasure(3000.),
					-10.0, boost::none, boost::none,
					curve);

				auto pl = new Schema::IfcAxis2PlacementLinear(pde, nullptr, nullptr);
				auto lp = new Schema::IfcLinearPlacement(nullptr, pl, nullptr);
				file.addEntity(lp);
			}

			ifcopenshell::geometry::Settings settings;
			auto mapping = ifcopenshell::geometry::impl::mapping_implementations().construct(&file, settings);

			// iterator over the placements
			auto placements = file.instances_by_type<Schema::IfcLinearPlacement>();
			auto iter = placements->begin();
			{
				// Example 5.1
				auto placement = *(iter);

				auto m = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(placement));
				m->components().col(3).head(3) /= mapping->get_length_unit();

				auto d = m->ccomponents().determinant();
				Assert::AreEqual(1.0, d, 0.0000001);

				auto x = m->ccomponents().col(3)(0);
				auto y = m->ccomponents().col(3)(1);
				auto z = m->ccomponents().col(3)(2);

				Assert::AreEqual(1339.2529, x, 0.001);
				Assert::AreEqual(1956.2587, y, 0.001);
				Assert::AreEqual(0.0, z, 0.001);
			}

			iter++;

			{
				// Example 5.2
				auto placement = *(iter);

				auto m = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(placement));
				m->components().col(3).head(3) /= mapping->get_length_unit();

				auto d = m->ccomponents().determinant();
				Assert::AreEqual(1.0, d, 0.0000001);

				auto x = m->ccomponents().col(3)(0);
				auto y = m->ccomponents().col(3)(1);
				auto z = m->ccomponents().col(3)(2);

				Assert::AreEqual(3800.5519, x, 0.001);
				Assert::AreEqual(2658.7954, y, 0.001);
				Assert::AreEqual(0.0, z, 0.001);
			}

			iter++;

			{
				// Example 5.3, 20 ft left
				auto placement = *(iter);

				auto m = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(placement));
				m->components().col(3).head(3) /= mapping->get_length_unit();

				auto d = m->ccomponents().determinant();
				Assert::AreEqual(1.0, d, 0.0000001);

				auto x = m->ccomponents().col(3)(0);
				auto y = m->ccomponents().col(3)(1);
				auto z = m->ccomponents().col(3)(2);

				Assert::AreEqual(3781.0626, x, 0.001);
				Assert::AreEqual(2663.2860, y, 0.001);
				Assert::AreEqual(0.0, z, 0.001);
			}

			iter++;

			{
				// Example 5.3, 20 ft right
				auto placement = *(iter);

				auto m = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(placement));
				m->components().col(3).head(3) /= mapping->get_length_unit();

				auto d = m->ccomponents().determinant();
				Assert::AreEqual(1.0, d, 0.0000001);

				auto x = m->ccomponents().col(3)(0);
				auto y = m->ccomponents().col(3)(1);
				auto z = m->ccomponents().col(3)(2);

				Assert::AreEqual(3820.0412, x, 0.001);
				Assert::AreEqual(2654.3048, y, 0.001);
				Assert::AreEqual(0.0, z, 0.001);
			}

			iter++;

			// Example 5.7
			{
				auto placement = *(iter);

				auto m = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(placement));
				m->components().col(3).head(3) /= mapping->get_length_unit();

				auto d = m->ccomponents().determinant();
				Assert::AreEqual(1.0, d, 0.0000001);


				auto x = m->ccomponents().col(3)(0);
				auto y = m->ccomponents().col(3)(1);
				auto z = m->ccomponents().col(3)(2);

				// Eq 5.83 page 61
				Assert::AreEqual(3137.3723, x, 0.001);
				Assert::AreEqual(1382.9424, y, 0.001);
				Assert::AreEqual(0.0, z, 0.001);
			}

			iter++;

			// Example 5.8
			{
				auto placement = *(iter);

				auto m = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(placement));
				m->components().col(3).head(3) /= mapping->get_length_unit();

				auto d = m->ccomponents().determinant();
				Assert::AreEqual(1.0, d, 0.0000001);

				auto x = m->ccomponents().col(3)(0);
				auto y = m->ccomponents().col(3)(1);
				auto z = m->ccomponents().col(3)(2);

				// Eq 5.92 page 62
				Assert::AreEqual(3141.8862, x, 0.001);
				Assert::AreEqual(1374.0192, y, 0.001);
				Assert::AreEqual(0.0, z, 0.001);

				//auto lp = placement->as<Schema::IfcLinearPlacement>();
				//auto rp = lp->RelativePlacement();
				//auto pl = rp->as<Schema::IfcAxis2PlacementLinear>();
				//auto loc = pl->Location();
				//auto pde = loc->as<Schema::IfcPointByDistanceExpression>();
				//pde->setDistanceAlong(new Schema::IfcLengthMeasure(4000.));
				//m = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(placement));
				//m->components().col(3).head(3) /= mapping->get_length_unit();
				//x = m->ccomponents().col(3)(0);
				//y = m->ccomponents().col(3)(1);
				//z = m->ccomponents().col(3)(2);

			}
		}

		// Bridge 1 Pier Elevations
		TEST_METHOD(Bridge1)
		{
			IfcParse::IfcFile file("../../Files/FHWA_Bridge_Geometry_Alignment_Example.ifc");

			// get the profile
			auto curves = file.instances_by_type<Schema::IfcGradientCurve>();
			Assert::AreEqual(1u, curves->size());
			auto gradient_curve = (*(curves->begin()))->as<Schema::IfcGradientCurve>();
			Assert::IsNotNull(gradient_curve);

			// Station, X, Y, Elev, Slope
			// X,Y from Table 6.2
			// Elev, Slope from Table 6.5
			std::vector<std::tuple<double, double, double, double, double>> expected_values{
				{ 11070, 1398.00, 1918.20, 118.73, 0.0175}, // Abut 1
				{ 11200, 1507.10, 1847.51, 121.00, 0.0175}, // Pier 2
				{ 11330, 1616.21, 1776.82, 123.13, 0.0153}, // Pier 3
				{ 11460, 1725.31, 1706.14, 124.97, 0.0130}, // Pier 4
				{ 11590, 1834.41, 1635.45, 126.52, 0.0108}, // Pier 5
				{ 11720, 1943.51, 1564.76, 127.78, 0.0086}, // Pier 6
				{ 11850, 2052.62, 1494.08, 128.74, 0.0063}  // Abut 7
			};

			double start_station = 10000.0;
			for(const auto& ev : expected_values)
			{
				auto station = std::get<0>(ev);
				auto dist = station - start_station;
				auto pde = new Schema::IfcPointByDistanceExpression(
					new Schema::IfcLengthMeasure(dist),
					boost::none, boost::none, boost::none,
					gradient_curve);

				auto pl = new Schema::IfcAxis2PlacementLinear(pde, nullptr, nullptr);
				auto lp = new Schema::IfcLinearPlacement(nullptr, pl, nullptr);
				file.addEntity(lp);
			}

			// set up mapping
			ifcopenshell::geometry::Settings settings;
			auto mapping = ifcopenshell::geometry::impl::mapping_implementations().construct(&file, settings);

			// iterator over the placements
			auto placements = file.instances_by_type<Schema::IfcLinearPlacement>();
			Assert::AreEqual(expected_values.size(), (size_t)placements->size());

			int i = 0;
			for (auto& object_placement : *placements)
			{
				auto m = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(object_placement));
				m->components().col(3).head(3) /= mapping->get_length_unit();

				auto d = m->ccomponents().determinant();
				Assert::AreEqual(1.0, d, 0.0000001);

				auto dx = m->ccomponents().col(0)(0);
				auto dy = m->ccomponents().col(0)(1);
				auto dz = m->ccomponents().col(0)(2);

				auto slope = dz / sqrt(dx * dx + dy * dy);

				auto x = m->ccomponents().col(3)(0);
				auto y = m->ccomponents().col(3)(1);
				auto z = m->ccomponents().col(3)(2);

				double dist = *object_placement->as<Schema::IfcLinearPlacement>()->RelativePlacement()->as<Schema::IfcAxis2PlacementLinear>()->Location()->as<Schema::IfcPointByDistanceExpression>()->DistanceAlong()->as<Schema::IfcLengthMeasure>();

				Assert::AreEqual(std::get<0>(expected_values[i]), dist+start_station, 0.001);
				Assert::AreEqual(std::get<1>(expected_values[i]), x, 0.01);
				Assert::AreEqual(std::get<2>(expected_values[i]), y, 0.01);
				Assert::AreEqual(std::get<3>(expected_values[i]), z, 0.01);
				Assert::AreEqual(std::get<4>(expected_values[i]), slope, 0.0001);

				i++;
			}
		}
		
		// Test using values reported in Table 3.2
		TEST_METHOD(Vertical_Curve)
		{
			IfcParse::IfcFile file("../../Files/FHWA_Bridge_Geometry_Alignment_Example.ifc");

			// get the profile
			auto curves = file.instances_by_type<Schema::IfcGradientCurve>();
			Assert::AreEqual(1u, curves->size());
			auto gradient_curve = (*(curves->begin()))->as<Schema::IfcGradientCurve>();
			Assert::IsNotNull(gradient_curve);

			// create linear placements along vertical curve 1
			for (int i = 0; i < 11; i++)
			{
				double dist = 1200. + i * 1600.0 / 10;
				auto pde = new Schema::IfcPointByDistanceExpression(
					new Schema::IfcLengthMeasure(dist),
					boost::none, boost::none, boost::none,
					gradient_curve);

				auto pl = new Schema::IfcAxis2PlacementLinear(pde, nullptr, nullptr);
				auto lp = new Schema::IfcLinearPlacement(nullptr, pl, nullptr);
				file.addEntity(lp);
			}

			// set up mapping
			ifcopenshell::geometry::Settings settings;
			auto mapping = ifcopenshell::geometry::impl::mapping_implementations().construct(&file, settings);

			// From Table 3.2
			// S, Elev, Grade
			std::vector<std::tuple<double, double, double>> expected_values{
				{   0.0, 121.00, 0.01750},
				{ 160.0, 123.58, 0.01475},
				{ 320.0, 125.72, 0.01200},
				{ 480.0, 127.42, 0.00925},
				{ 640.0, 128.68, 0.00650},
				{ 800.0, 129.50, 0.00375},
				{ 960.0, 129.88, 0.00100},
				{1120.0, 129.82, -0.00175},
				{1280.0, 129.32, -0.00450},
				{1440.0, 128.38, -0.00725},
				{1600.0, 127.00, -0.01000}
			};

			// iterator over the placements
			auto placements = file.instances_by_type<Schema::IfcLinearPlacement>();
			Assert::AreEqual(expected_values.size(), (size_t)placements->size());

			int i = 0;
			for (auto& object_placement : *placements)
			{
				auto m = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(object_placement));
				m->components().col(3).head(3) /= mapping->get_length_unit();

				auto d = m->ccomponents().determinant();
				Assert::AreEqual(1.0, d, 0.0000001);

				auto dx = m->ccomponents().col(0)(0);
				auto dy = m->ccomponents().col(0)(1);
				auto dz = m->ccomponents().col(0)(2);

				auto slope = dz / sqrt(dx * dx + dy * dy);
				auto elev = m->ccomponents().col(3)(2);

				double dist = *object_placement->as<Schema::IfcLinearPlacement>()->RelativePlacement()->as<Schema::IfcAxis2PlacementLinear>()->Location()->as<Schema::IfcPointByDistanceExpression>()->DistanceAlong()->as<Schema::IfcLengthMeasure>();

				Assert::AreEqual(std::get<0>(expected_values[i]), dist - 1200.0, 0.001);
				Assert::AreEqual(std::get<1>(expected_values[i]), elev, 0.001);
				Assert::AreEqual(std::get<2>(expected_values[i]), slope, 0.00001);

				i++;
			}
		}
	};
}
