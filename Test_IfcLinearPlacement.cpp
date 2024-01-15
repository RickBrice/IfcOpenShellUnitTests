#include "pch.h"
#include "CppUnitTest.h"

// Disable warnings coming from IfcOpenShell
#pragma warning(disable:4018 4267 4250 4984 4985)

#include <ifcparse/IfcHierarchyHelper.h>
#include <ifcparse/Ifc4x3_add2.h>
#include <ifcgeom/abstract_mapping.h>
#include <boost/math/constants/constants.hpp>

const double PI = boost::math::constants::pi<double>();

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define Schema Ifc4x3_add2

void setup_project(IfcHierarchyHelper<Schema>& file)
{
	auto project = file.addProject();
	project->setName(std::string("TestProject"));

	//// set up project units for feet
	//// the call to file.addProject() sets up length units as millimeter.
	//auto units_in_context = project->UnitsInContext();
	//auto units = units_in_context->Units();
	//auto begin = units->begin();
	//auto iter = begin;
	//auto end = units->end();
	//for (; iter != end; iter++)
	//{
	//	auto unit = *iter;
	//	if (unit->as<Schema::IfcSIUnit>() && unit->as<Schema::IfcSIUnit>()->UnitType() == Schema::IfcUnitEnum::IfcUnit_LENGTHUNIT)
	//	{
	//		auto dimensions = new Schema::IfcDimensionalExponents(1, 0, 0, 0, 0, 0, 0);
	//		file.addEntity(dimensions);

	//		auto conversion_factor = new Schema::IfcMeasureWithUnit(new Schema::IfcLengthMeasure(304.80), unit->as<Schema::IfcSIUnit>());
	//		file.addEntity(conversion_factor);

	//		auto conversion_based_unit = new Schema::IfcConversionBasedUnit(dimensions, Schema::IfcUnitEnum::IfcUnit_LENGTHUNIT, "FEET", conversion_factor);
	//		file.addEntity(conversion_based_unit);

	//		units->remove(unit); // remove the millimeter unit
	//		units->push(conversion_based_unit); // add the feet unit
	//		units_in_context->setUnits(units); // update the UnitsInContext

	//		break; // Done!, the length unit was found, so break out of the loop
	//	}
	//}

	auto geometric_representation_context = file.getRepresentationContext(std::string("Model")); // creates the representation context if it doesn't already exist
}

namespace IfcOpenShellUnitTests
{
	TEST_CLASS(IfcLinearPlacement)
	{
	public:
		enum class Curve { IfcPolyLine, IfcIndexedPolyCurve };

		struct Options
		{
			Curve curve = Curve::IfcPolyLine;
			bool bUseSegments = false;
			bool bMultipleSegments = true;
		};

		void Test2D(const Options& options)
		{
			IfcHierarchyHelper<Schema> file;
			setup_project(file);

			ifcopenshell::geometry::Settings settings;
			auto mapping = ifcopenshell::geometry::impl::mapping_implementations().construct(&file, settings);

			std::vector<std::vector<double>> curve_points;
			curve_points.push_back(std::vector<double>{0, 0});
			curve_points.push_back(std::vector<double>{10, 0});
			curve_points.push_back(std::vector<double>{20, 10});
			curve_points.push_back(std::vector<double>{30, 10});

			Schema::IfcCurve* curve;
			if (options.curve == Curve::IfcPolyLine)
			{
				typename aggregate_of<typename Schema::IfcCartesianPoint>::ptr points(new aggregate_of<typename Schema::IfcCartesianPoint>());
				for (auto& curve_point : curve_points)
				{
					points->push(new Schema::IfcCartesianPoint(curve_point));
				}
				curve = new Schema::IfcPolyline(points);
			}
			else
			{
				auto points = new Schema::IfcCartesianPointList2D(curve_points, boost::none);
				if (options.bUseSegments)
				{
					// note: STEP uses a 1-based index
					typename aggregate_of<typename Schema::IfcSegmentIndexSelect>::ptr segments(new aggregate_of<typename Schema::IfcSegmentIndexSelect>());
					if (options.bMultipleSegments)
					{
						int npoints = curve_points.size();
						for (int i = 0; i < npoints - 1; i++)
						{
							std::vector<int> indicies{ i + 1,i + 2 };
							segments->push(new Schema::IfcLineIndex(indicies));
						}
					}
					else
					{
						std::vector<int> indicies;
						int npoints = curve_points.size();
						for (int i = 0; i < npoints; i++)
						{
							indicies.push_back(i + 1);
						}
						segments->push(new Schema::IfcLineIndex(indicies));
					}
					curve = new Schema::IfcIndexedPolyCurve(points, segments, boost::none);
				}
				else
				{
					curve = new Schema::IfcIndexedPolyCurve(points, boost::none, boost::none);
				}
			}

			// distances along polyline for testing
			std::vector<double> dist{ 5.0, // mid-point first leg
											  17.071067812,// mid-point second leg
											  29.14213562 // mid-point last leg
			};

			// create point by distance expressions
			for (auto& d : dist)
			{
				auto pde = new Schema::IfcPointByDistanceExpression(
					new Schema::IfcLengthMeasure(d),
					boost::none, boost::none, boost::none,
					curve);

				auto pl = new Schema::IfcAxis2PlacementLinear(pde, nullptr, nullptr);
				auto lp = new Schema::IfcLinearPlacement(nullptr, pl, nullptr);
				file.addEntity(lp);
			}

			// expected results
			std::vector<Eigen::Matrix4d> expected_values;
			Eigen::Matrix4d e1;
			e1.row(0) << 1., 0., 0., 5.;
			e1.row(1) << 0., 1., 0., 0.;
			e1.row(2) << 0., 0., 1., 0.;
			e1.row(3) << 0., 0., 0., 1.;

			Eigen::Matrix4d e2;
			auto dx = cos(PI / 4.0);
			auto dy = sin(PI / 4.0);
			e2.row(0) << dx, -dy, 0., 15.0;
			e2.row(1) << dy, dx, 0, 5;
			e2.row(2) << 0, 0, 1, 0;
			e2.row(3) << 0, 0, 0, 1;

			Eigen::Matrix4d e3;
			e3.row(0) << 1., 0., 0., 25.;
			e3.row(1) << 0., 1., 0., 10.;
			e3.row(2) << 0., 0., 1., 0.;
			e3.row(3) << 0., 0., 0., 1.;

			expected_values.emplace_back(e1);
			expected_values.emplace_back(e2);
			expected_values.emplace_back(e3);

			// test mapping of placements
			int i = 0;
			auto placements = file.instances_by_type<Schema::IfcLinearPlacement>();
			for (auto& placement : *placements)
			{
				auto m = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(placement));
				m->components().col(3).head(3) /= mapping->get_length_unit();

				Eigen::Matrix4d values = m->components();

				auto d = values.determinant();
				Assert::AreEqual(1.0, d, 0.0000001);

				auto& expected = expected_values[i++];
				for (int col = 0; col < 4; col++)
				{
					for (int row = 0; row < 4; row++)
					{
						Assert::AreEqual(expected(row, col), values(row, col), 0.001);
					}
				}
			}
		}

		void Test3D(const Options& options)
		{
			IfcHierarchyHelper<Schema> file;
			setup_project(file);

			ifcopenshell::geometry::Settings settings;
			auto mapping = ifcopenshell::geometry::impl::mapping_implementations().construct(&file, settings);

			std::vector<std::vector<double>> curve_points;
			curve_points.push_back(std::vector<double>{0, 0, 0});
			curve_points.push_back(std::vector<double>{10, 0, 10});
			curve_points.push_back(std::vector<double>{20, 10, -10});
			curve_points.push_back(std::vector<double>{30, 10, 0});

			Schema::IfcCurve* curve;
			if (options.curve == Curve::IfcPolyLine)
			{
				typename aggregate_of<typename Schema::IfcCartesianPoint>::ptr points(new aggregate_of<typename Schema::IfcCartesianPoint>());
				for (auto& curve_point : curve_points)
				{
					points->push(new Schema::IfcCartesianPoint(curve_point));
				}
				curve = new Schema::IfcPolyline(points);
			}
			else
			{
				auto points = new Schema::IfcCartesianPointList3D(curve_points, boost::none);
				if (options.bUseSegments)
				{
					// note: STEP uses a 1-based index
					typename aggregate_of<typename Schema::IfcSegmentIndexSelect>::ptr segments(new aggregate_of<typename Schema::IfcSegmentIndexSelect>());
					if (options.bMultipleSegments)
					{
						int npoints = curve_points.size();
						for (int i = 0; i < npoints - 1; i++)
						{
							std::vector<int> indicies{ i + 1,i + 2 };
							segments->push(new Schema::IfcLineIndex(indicies));
						}
					}
					else
					{
						std::vector<int> indicies;
						int npoints = curve_points.size();
						for (int i = 0; i < npoints; i++)
						{
							indicies.push_back(i + 1);
						}
						segments->push(new Schema::IfcLineIndex(indicies));
					}
					curve = new Schema::IfcIndexedPolyCurve(points, segments, boost::none);
				}
				else
				{
					curve = new Schema::IfcIndexedPolyCurve(points, boost::none, boost::none);
				}
			}

			// distances along polyline for testing
			std::vector<double> dist{ 5.0, // point in first leg
											  17.,// point in second leg
											  45.0 // point in last leg
			};

			// create point by distance expressions
			for (auto& d : dist)
			{
				auto pde = new Schema::IfcPointByDistanceExpression(
					new Schema::IfcLengthMeasure(d),
					boost::none, boost::none, boost::none,
					curve);

				auto pl = new Schema::IfcAxis2PlacementLinear(pde, nullptr, nullptr);
				auto lp = new Schema::IfcLinearPlacement(nullptr, pl, nullptr);
				file.addEntity(lp);
			}

			// expected results
			std::vector<Eigen::Matrix4d> expected_values;
			Eigen::Matrix4d e1;
			e1.row(0) << 0.7071, 0., -0.7071, 3.5355;
			e1.row(1) << 0., 1., 0., 0.;
			e1.row(2) << 0.7071, 0., 0.7071, 3.5355;
			e1.row(3) << 0., 0., 0., 1.;

			Eigen::Matrix4d e2;
			e2.row(0) << 0.4082, -0.7071, 0.5773, 11.1667;
			e2.row(1) << 0.4082, 0.7071, 0.5773, 1.1667;
			e2.row(2) << -0.8165, 0., 0.5773, 7.6665;
			e2.row(3) << 0., 0., 0., 1.;

			Eigen::Matrix4d e3;
			e3.row(0) << 0.7071, 0., -0.7071, 24.4993;
			e3.row(1) << 0., 1., 0., 10.;
			e3.row(2) << 0.7071, 0., 0.7071, -5.5007;
			e3.row(3) << 0., 0., 0., 1.;

			expected_values.emplace_back(e1);
			expected_values.emplace_back(e2);
			expected_values.emplace_back(e3);

			// test mapping of placements
			int i = 0;
			auto placements = file.instances_by_type<Schema::IfcLinearPlacement>();
			for (auto& placement : *placements)
			{
				auto m = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(placement));
				m->components().col(3).head(3) /= mapping->get_length_unit();

				Eigen::Matrix4d values = m->components();

				auto d = values.determinant();
				Assert::AreEqual(1.0, d, 0.0000001);

				auto& expected = expected_values[i++];
				for (int col = 0; col < 4; col++)
				{
					for (int row = 0; row < 4; row++)
					{
						Assert::AreEqual(expected(row, col), values(row, col), 0.001);
					}
				}
			}
		}

		TEST_METHOD(BasisCurve_IfcPolyline_2D)
		{
			Options options;
			options.curve = Curve::IfcPolyLine;
			Test2D(options);
		}

		TEST_METHOD(BasisCurve_IfcIndexedPolyCurve_2D)
		{
			Options options;
			options.curve = Curve::IfcIndexedPolyCurve;
			options.bUseSegments = false;
			Test2D(options);

			options.bUseSegments = true;
			options.bMultipleSegments = true;
			Test2D(options);

			options.bUseSegments = false;
			options.bMultipleSegments = false;
			Test2D(options);
		}


		TEST_METHOD(BasisCurve_IfcPolyline_3D)
		{
			Options options;
			options.curve = Curve::IfcPolyLine;
			Test3D(options);
		}

		TEST_METHOD(BasisCurve_IfcIndexedPolyCurve_3D)
		{
			Options options;
			options.curve = Curve::IfcIndexedPolyCurve;
			options.bUseSegments = false;
			Test3D(options);

			options.bUseSegments = true;
			options.bMultipleSegments = true;
			Test3D(options);

			options.bUseSegments = false;
			options.bMultipleSegments = false;
			Test3D(options);
		}

		TEST_METHOD(BasisCurve_IfcOffsetCurveByDistances)
		{
			IfcHierarchyHelper<Schema> file;
			setup_project(file);

			ifcopenshell::geometry::Settings settings;
			auto mapping = ifcopenshell::geometry::impl::mapping_implementations().construct(&file, settings);


			std::vector<std::vector<double>> curve_points;
			curve_points.push_back(std::vector<double>{0, 0});
			curve_points.push_back(std::vector<double>{10, 0});
			curve_points.push_back(std::vector<double>{20, 10});
			curve_points.push_back(std::vector<double>{30, 10});

			Schema::IfcCurve* curve;
			typename aggregate_of<typename Schema::IfcCartesianPoint>::ptr points(new aggregate_of<typename Schema::IfcCartesianPoint>());
			for (auto& curve_point : curve_points)
			{
				points->push(new Schema::IfcCartesianPoint(curve_point));
			}
			curve = new Schema::IfcPolyline(points);

			double offset_y = 10.;
			typename aggregate_of<Schema::IfcPointByDistanceExpression>::ptr offsets(new aggregate_of<typename Schema::IfcPointByDistanceExpression>());
			offsets->push(new Schema::IfcPointByDistanceExpression(new Schema::IfcLengthMeasure(0.0), offset_y, boost::none, boost::none, curve));
			auto offset_curve = new Schema::IfcOffsetCurveByDistances(curve, offsets, boost::none);

			// distances along offset curve for testing
			std::vector<double> dist{ 5.0, // mid-point first leg
											  17.071067812,// mid-point second leg
											  29.14213562 // mid-point last leg
			};

			// create point by distance expressions
			for (auto& d : dist)
			{
				auto pde = new Schema::IfcPointByDistanceExpression(
					new Schema::IfcLengthMeasure(d),
					boost::none, boost::none, boost::none,
					offset_curve);

				auto pl = new Schema::IfcAxis2PlacementLinear(pde, nullptr, nullptr);
				auto lp = new Schema::IfcLinearPlacement(nullptr, pl, nullptr);
				file.addEntity(lp);
			}

			// expected results
			std::vector<Eigen::Matrix4d> expected_values;
			Eigen::Matrix4d e1;
			e1.row(0) << 1., 0., 0., 5.;
			e1.row(1) << 0., 1., 0., 0. + offset_y;
			e1.row(2) << 0., 0., 1., 0.;
			e1.row(3) << 0., 0., 0., 1.;

			Eigen::Matrix4d e2;
			auto dx = cos(PI / 4.0);
			auto dy = sin(PI / 4.0);
			e2.row(0) << dx, -dy, 0., 15.0 - dx*offset_y;
			e2.row(1) << dy, dx, 0, 5 + dy*offset_y;
			e2.row(2) << 0, 0, 1, 0;
			e2.row(3) << 0, 0, 0, 1;

			Eigen::Matrix4d e3;
			e3.row(0) << 1., 0., 0., 25.;
			e3.row(1) << 0., 1., 0., 10. + offset_y;
			e3.row(2) << 0., 0., 1., 0.;
			e3.row(3) << 0., 0., 0., 1.;

			expected_values.emplace_back(e1);
			expected_values.emplace_back(e2);
			expected_values.emplace_back(e3);

			// test mapping of placements
			int i = 0;
			auto placements = file.instances_by_type<Schema::IfcLinearPlacement>();
			for (auto& placement : *placements)
			{
				auto m = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(placement));
				m->components().col(3).head(3) /= mapping->get_length_unit();

				Eigen::Matrix4d values = m->components();

				auto d = values.determinant();
				Assert::AreEqual(1.0, d, 0.0000001);

				auto& expected = expected_values[i++];
				for (int col = 0; col < 4; col++)
				{
					for (int row = 0; row < 4; row++)
					{
						Assert::AreEqual(expected(row, col), values(row, col), 0.001);
					}
				}
			}
		}
	};
}
