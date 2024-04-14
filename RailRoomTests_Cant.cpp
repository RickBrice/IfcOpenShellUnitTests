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
	TEST_CLASS(Cant)
	{
	public:
		std::string curve_type;

		void Test(const char* test_name)
		{
			std::ostringstream os1;
			os1 << "F:/IFC-Rail-Unit-Test-Reference-Code/alignment_testset/IFC-WithGeneratedGeometry/GENERATED__CantAlignment_" << curve_type.c_str() << test_name << ".ifc";
			IfcParse::IfcFile file(os1.str().c_str());
			auto curves = file.instances_by_type<Schema::IfcSegmentedReferenceCurve>();
			//Assert::AreEqual(1u, curves->size());
			auto curve = (*(curves->begin()))->as<Schema::IfcSegmentedReferenceCurve>();
			Assert::IsNotNull(curve);

			ifcopenshell::geometry::Settings settings;
			auto mapping = ifcopenshell::geometry::impl::mapping_implementations().construct(&file, settings);

			auto pwf = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::piecewise_function>(mapping->map(curve));

			std::ostringstream os2;
			os2 << "F:/IFC-Rail-Unit-Test-Reference-Code/alignment_testset/ToolboxProcess-C/CantAlignment/" << curve_type.c_str() << "/" << curve_type.c_str() << test_name << "-2CS.txt";
			std::ifstream ifile(os2.str().c_str());
			std::string str;
			std::getline(ifile, str);
			std::getline(ifile, str);

			double tol = 0.00001;
			double s; // so we have the last value after the loop
			while (ifile)
			{
				double es, ex, ey, ez;
				std::string ee;
				ifile >> es >> ex >> ey >> ez >> ee;

				if (ifile.fail()) break;

				s = std::stod(ee.substr(1, ee.size() - 2));

				//double s = (curve_type == "Cubic") ? ex : es;
				auto m = ifcopenshell::geometry::taxonomy::make<ifcopenshell::geometry::taxonomy::matrix4>(pwf->evaluate(s));

				m->components().col(3).head(3) /= mapping->get_length_unit();
				Eigen::Matrix4d values = m->components();

				double x = values(0, 3); // row, col
				double y = values(1, 3);
				double z = values(2, 3);
				Assert::AreEqual(ex, x, tol);
				Assert::AreEqual(ey, y, tol);
				Assert::AreEqual(ez, z, tol);
			}

			// validate the ending placement including the vectors
			auto placement = curve->EndPoint();
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

		//TEST_METHOD(Line)
		//{
		//	curve_type = "Line";
		//	Test("_100.0_-1000_-300_1_Meter");
		//	Test("_100.0_-300_-1000_1_Meter");
		//	Test("_100.0_-300_-inf_1_Meter");
		//	Test("_100.0_-inf_-300_1_Meter");
		//	Test("_100.0_1000_300_1_Meter");
		//	Test("_100.0_300_1000_1_Meter");
		//	Test("_100.0_300_inf_1_Meter");
		//	Test("_100.0_inf_300_1_Meter");
		//}

		//TEST_METHOD(Cubic)
		//{
		//	curve_type = "Cubic";
		//	Test("_100.0_-1000_-300_1_Meter");
		//	Test("_100.0_-300_-1000_1_Meter");
		//	Test("_100.0_-300_-inf_1_Meter");
		//	Test("_100.0_-inf_-300_1_Meter");
		//	Test("_100.0_1000_300_1_Meter");
		//	Test("_100.0_300_1000_1_Meter");
		//	Test("_100.0_300_inf_1_Meter");
		//	Test("_100.0_inf_300_1_Meter");
		//}

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

		TEST_METHOD(ConstantCant)
		{
			curve_type = "ConstantCant";
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

		TEST_METHOD(LinearTransition)
		{
			curve_type = "LinearTransition";
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
