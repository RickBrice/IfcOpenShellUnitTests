#include "pch.h"
#include "CppUnitTest.h"
#include <tchar.h>

// Disable warnings coming from IfcOpenShell
#pragma warning(disable:4018 4267 4250 4984 4985)

#include <ifcparse/IfcHierarchyHelper.h>
#include <ifcparse/Ifc4x3_add2.h>
#include <ifcgeom/abstract_mapping.h>

#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define Schema Ifc4x3_add2

namespace IfcRailRoom
{
	TEST_CLASS(Horizontal)
	{
	public:
		std::string curve_type;

		void Test(const char* test_name)
		{
			std::ostringstream os1;
			os1 << "F:/IFC-Rail-Unit-Test-Reference-Code/alignment_testset/IFC-WithGeneratedGeometry/GENERATED__HorizontalAlignment_" << curve_type.c_str() << test_name << ".ifc";
			IfcParse::IfcFile file(os1.str().c_str());
			auto curves = file.instances_by_type<Schema::IfcCompositeCurve>();
			//Assert::AreEqual(1u, curves->size());
			auto curve = (*(curves->begin()))->as<Schema::IfcCompositeCurve>();
			Assert::IsNotNull(curve);

			ifcopenshell::geometry::Settings settings;
			auto mapping = ifcopenshell::geometry::impl::mapping_implementations().construct(&file, settings);

			auto pwf = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::piecewise_function>(mapping->map(curve));

			std::ostringstream os2;
			os2 << "F:/IFC-Rail-Unit-Test-Reference-Code/alignment_testset/ToolboxProcessed/HorizontalAlignment/" << curve_type.c_str() << "/" << curve_type.c_str() << test_name << ".txt";
			std::ifstream ifile(os2.str().c_str());
			std::string str;
			std::getline(ifile, str);
			std::getline(ifile, str);

			double tol = 0.0001;
			double s;
			while (ifile)
			{
				double es, ex, ey;
				ifile >> es >> ex >> ey;

				s = (curve_type == "Cubic") ? ex : es;
				auto m = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(pwf->evaluate(s));

				m->components().col(3).head(3) /= mapping->get_length_unit();
				Eigen::Matrix4d values = m->components();

				double x = values(0, 3); // row, col
				double y = values(1, 3);
				Assert::AreEqual(ex, x, tol);
				Assert::AreEqual(ey, y, tol);
			}


			// validate the ending placement including the vectors
			auto nSegments = curve->Segments()->size();
			auto segments = curve->Segments();
			auto it = segments->begin() + nSegments - 1;
			auto placement = (*it)->as<Ifc4x3_add2::IfcCurveSegment>()->Placement();
			Assert::IsNotNull(placement, _T("IfcAxis2Placement3D not found"));
			auto m1 = ifcopenshell::geometry::taxonomy::dcast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(placement))->components();
			auto m2 = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(pwf->evaluate(s))->components();
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					std::wostringstream os;
					os << _T("(") << i << _T(", ") << j << _T(")");
					Assert::AreEqual(m1(i, j), m2(i, j), tol, os.str().c_str());
				}
			}
		}

		TEST_METHOD(Line)
		{
			curve_type = "Line";
			Test("_100.0_-1000_-300_1_Meter");
			Test("_100.0_-300_-1000_1_Meter");
			Test("_100.0_-300_-inf_1_Meter");
			Test("_100.0_-inf_-300_1_Meter");
			Test("_100.0_1000_300_1_Meter");
			Test("_100.0_300_1000_1_Meter");
			Test("_100.0_300_inf_1_Meter");
			Test("_100.0_inf_300_1_Meter");
		}

		TEST_METHOD(Cubic)
		{
			curve_type = "Cubic";
			Test("_100.0_-1000_-300_1_Meter");
			Test("_100.0_-300_-1000_1_Meter");
			Test("_100.0_-300_-inf_1_Meter");
			Test("_100.0_-inf_-300_1_Meter");
			Test("_100.0_1000_300_1_Meter");
			Test("_100.0_300_1000_1_Meter");
			Test("_100.0_300_inf_1_Meter");
			Test("_100.0_inf_300_1_Meter");
		}

		TEST_METHOD(Bloss)
		{
			curve_type = "BlossCurve";
			Test("_100.0_-1000_-300_1_Meter");
			Test("_100.0_-300_-1000_1_Meter");
			Test("_100.0_-300_-inf_1_Meter");
			Test("_100.0_-inf_-300_1_Meter");
			Test("_100.0_1000_300_1_Meter");
			Test("_100.0_300_1000_1_Meter");
			Test("_100.0_300_inf_1_Meter");
			Test("_100.0_inf_300_1_Meter");
		}

		TEST_METHOD(CircularArc)
		{
			curve_type = "CircularArc";
			Test("_100.0_-1000_-300_1_Meter");
			Test("_100.0_-300_-1000_1_Meter");
			Test("_100.0_-300_-inf_1_Meter");
			Test("_100.0_-inf_-300_1_Meter");
			Test("_100.0_1000_300_1_Meter");
			Test("_100.0_300_1000_1_Meter");
			Test("_100.0_300_inf_1_Meter");
			Test("_100.0_inf_300_1_Meter");
		}

		TEST_METHOD(Clothoid)
		{
			curve_type = "Clothoid";
			Test("_100.0_-1000_-300_1_Meter");
			Test("_100.0_-300_-1000_1_Meter");
			Test("_100.0_-300_-inf_1_Meter");
			Test("_100.0_-inf_-300_1_Meter");
			Test("_100.0_1000_300_1_Meter");
			Test("_100.0_300_1000_1_Meter");
			Test("_100.0_300_inf_1_Meter");
			Test("_100.0_inf_300_1_Meter");
		}

		TEST_METHOD(CosineCurve)
		{
			curve_type = "CosineCurve";
			Test("_100.0_-1000_-300_1_Meter");
			Test("_100.0_-300_-1000_1_Meter");
			Test("_100.0_-300_-inf_1_Meter");
			Test("_100.0_-inf_-300_1_Meter");
			Test("_100.0_1000_300_1_Meter");
			Test("_100.0_300_1000_1_Meter");
			Test("_100.0_300_inf_1_Meter");
			Test("_100.0_inf_300_1_Meter");
		}

		TEST_METHOD(SineCurve)
		{
			curve_type = "SineCurve";
			Test("_100.0_-1000_-300_1_Meter");
			Test("_100.0_-300_-1000_1_Meter");
			Test("_100.0_-300_-inf_1_Meter");
			Test("_100.0_-inf_-300_1_Meter");
			Test("_100.0_1000_300_1_Meter");
			Test("_100.0_300_1000_1_Meter");
			Test("_100.0_300_inf_1_Meter");
			Test("_100.0_inf_300_1_Meter");
		}

		TEST_METHOD(HelmertCurve)
		{
			curve_type = "HelmertCurve";
			Test("_100.0_-1000_-300_1_Meter");
			Test("_100.0_-300_-1000_1_Meter");
			Test("_100.0_-300_-inf_1_Meter");
			Test("_100.0_-inf_-300_1_Meter");
			Test("_100.0_1000_300_1_Meter");
			Test("_100.0_300_1000_1_Meter");
			Test("_100.0_300_inf_1_Meter");
			Test("_100.0_inf_300_1_Meter");
		}

		TEST_METHOD(VienneseBend)
		{
			curve_type = "VienneseBend";
			Test("_100.0_-1000_-300_1_Meter");
			Test("_100.0_-300_-1000_1_Meter");
			Test("_100.0_-300_-inf_1_Meter");
			Test("_100.0_-inf_-300_1_Meter");
			Test("_100.0_1000_300_1_Meter");
			Test("_100.0_300_1000_1_Meter");
			Test("_100.0_300_inf_1_Meter");
			Test("_100.0_inf_300_1_Meter");
		}
	};
}
