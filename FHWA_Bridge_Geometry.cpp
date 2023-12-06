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

			// Example 5.7 - coordinate of point at Station 130+00
			// Distance from start is 3000 ft
			{
				auto pde = new Schema::IfcPointByDistanceExpression(
					new Schema::IfcNonNegativeLengthMeasure(3000.),
					boost::none, boost::none, boost::none,
					curve);

				auto pl = new Schema::IfcAxis2PlacementLinear(pde, nullptr, nullptr);
				auto lp = new Schema::IfcLinearPlacement(nullptr, pl, nullptr);
				file.addEntity(lp);
			}

			// Example 5.8, offset 10 ft right
			{
				auto pde = new Schema::IfcPointByDistanceExpression(
					new Schema::IfcNonNegativeLengthMeasure(3000.),
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
					new Schema::IfcNonNegativeLengthMeasure(dist),
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
			Assert::AreEqual(11u, placements->size());

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

				double dist = *object_placement->as<Schema::IfcLinearPlacement>()->RelativePlacement()->as<Schema::IfcAxis2PlacementLinear>()->Location()->as<Schema::IfcPointByDistanceExpression>()->DistanceAlong()->as<Schema::IfcNonNegativeLengthMeasure>();

				Assert::AreEqual(std::get<0>(expected_values[i]), dist - 1200.0, 0.001);
				Assert::AreEqual(std::get<1>(expected_values[i]), elev, 0.001);
				Assert::AreEqual(std::get<2>(expected_values[i]), slope, 0.00001);

				i++;
			}
		}
	};
}
