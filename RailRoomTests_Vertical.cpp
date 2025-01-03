#include "pch.h"
#include "CppUnitTest.h"
#include <tchar.h>

// Disable warnings coming from IfcOpenShell
#pragma warning(disable:4018 4267 4250 4984 4985)

#include <ifcparse/IfcHierarchyHelper.h>
#include <ifcparse/Ifc4x3_add2.h>
#include <ifcgeom/abstract_mapping.h>
#include <ifcgeom/function_item_evaluator.h>

#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define Schema Ifc4x3_add2

namespace IfcRailRoom
{
	TEST_CLASS(Vertical)
	{
	public:
		std::string curve_type;

		void Test(const char* test_name)
		{
			std::ostringstream os1;
			os1 << "F:/IFC-Rail-Unit-Test-Reference-Code/alignment_testset/IFC-WithGeneratedGeometry/GENERATED__VerticalAlignment_" << curve_type.c_str() << test_name << ".ifc";
			IfcParse::IfcFile file(os1.str().c_str());
			auto curves = file.instances_by_type<Schema::IfcGradientCurve>();
			//Assert::AreEqual(1u, curves->size());
			auto curve = (*(curves->begin()))->as<Schema::IfcGradientCurve>();
			Assert::IsNotNull(curve);

			ifcopenshell::geometry::Settings settings;
			auto mapping = ifcopenshell::geometry::impl::mapping_implementations().construct(&file, settings);

			auto fn = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::function_item>(mapping->map(curve));
			ifcopenshell::geometry::function_item_evaluator evaluator(settings,fn);

			std::ostringstream os2;
			os2 << "F:/IFC-Rail-Unit-Test-Reference-Code/alignment_testset/ToolboxProcessed/VerticalAlignment/" << curve_type.c_str() << "/" << curve_type.c_str() << test_name << ".txt";
			std::ifstream ifile(os2.str().c_str());
			std::string str;
			std::getline(ifile, str);
			std::getline(ifile, str);

			double tol = 0.0001;
			double s;
			while (ifile)
			{
				int i;
				double ex, ez, es;
				ifile >> i >> es >> ex >> ez;
				s = es;
				auto m = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(evaluator.evaluate(es));
				m->components().col(3).head(3) /= mapping->get_length_unit();
				Eigen::Matrix4d values = m->components();

				double x = values(0, 3); // row, col
				//double y = values(1, 3);
				double z = values(2, 3);
				Assert::AreEqual(es, x, tol);
				//Assert::AreEqual(ey, y, tol);
				Assert::AreEqual(ez, z, tol);
			}

			// validate the ending placement including the vectors
			auto placement = curve->EndPoint();
			Assert::IsNotNull(placement, _T("IfcAxis2Placement3D not found"));
			auto m1 = ifcopenshell::geometry::taxonomy::dcast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(placement))->components();
			auto m2 = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(evaluator.evaluate(s))->components();
			//for (int i = 0; i < 4; i++)
			//{
			//	for (int j = 0; j < 4; j++)
			//	{
			//		std::wostringstream os;
			//		os << _T("(") << i << _T(", ") << j << _T(")");
			//		Assert::AreEqual(m1(i, j), m2(i, j), tol, os.str().c_str());
			//	}
			//}
		}

		TEST_METHOD(ConstantGradient)
		{
			curve_type = "ConstantGradient";
			Test("_100.0_10.0_-0.5_-1.0_1_Meter");
			Test("_100.0_10.0_-0.5_0.0_1_Meter");
			Test("_100.0_10.0_-1.0_-0.5_1_Meter");
			Test("_100.0_10.0_0.0_-0.5_1_Meter");
			Test("_100.0_10.0_0.0_0.5_1_Meter");
			Test("_100.0_10.0_0.5_0.0_1_Meter");
			Test("_100.0_10.0_0.5_1.0_1_Meter");
			Test("_100.0_10.0_1.0_0.5_1_Meter");
		}

		TEST_METHOD(ParabolicArc)
		{
			curve_type = "ParabolicArc";
			Test("_100.0_10.0_-0.5_-1.0_1_Meter");
			Test("_100.0_10.0_-0.5_0.0_1_Meter");
			Test("_100.0_10.0_-1.0_-0.5_1_Meter");
			Test("_100.0_10.0_0.0_-0.5_1_Meter");
			Test("_100.0_10.0_0.0_0.5_1_Meter");
			Test("_100.0_10.0_0.5_0.0_1_Meter");
			Test("_100.0_10.0_0.5_1.0_1_Meter");
			Test("_100.0_10.0_1.0_0.5_1_Meter");
		}

		TEST_METHOD(CircularArc)
		{
			curve_type = "CircularArc";
			Test("_100.0_10.0_-0.5_-1.0_1_Meter");
			Test("_100.0_10.0_-0.5_0.0_1_Meter");
			Test("_100.0_10.0_-1.0_-0.5_1_Meter");
			Test("_100.0_10.0_0.0_-0.5_1_Meter");
			Test("_100.0_10.0_0.0_0.5_1_Meter");
			Test("_100.0_10.0_0.5_0.0_1_Meter");
			Test("_100.0_10.0_0.5_1.0_1_Meter");
			Test("_100.0_10.0_1.0_0.5_1_Meter");
		}

		TEST_METHOD(Clothoid_NoResultsAvailable)
		{
			// There aren't results for vertical clothoid yet
			return;

			curve_type = "Clothoid";
			Test("_100.0_10.0_-0.5_-1.0_1_Meter");
			Test("_100.0_10.0_-0.5_0.0_1_Meter");
			Test("_100.0_10.0_-1.0_-0.5_1_Meter");
			Test("_100.0_10.0_0.0_-0.5_1_Meter");
			Test("_100.0_10.0_0.0_0.5_1_Meter");
			Test("_100.0_10.0_0.5_0.0_1_Meter");
			Test("_100.0_10.0_0.5_1.0_1_Meter");
			Test("_100.0_10.0_1.0_0.5_1_Meter");
		}
	};
}
