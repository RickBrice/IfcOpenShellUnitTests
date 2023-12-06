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
	TEST_CLASS(ACCA_Sleepers)
	{
	public:

		TEST_METHOD(LinearPlacement)
		{
			IfcParse::IfcFile file("../../Files/ACCA_sleepers-linear-placement-cant-explicit.ifc");

			ifcopenshell::geometry::Settings settings;
			auto mapping = ifcopenshell::geometry::impl::mapping_implementations().construct(&file, settings);

			auto segmented_ref_curves = file.instances_by_type<Schema::IfcSegmentedReferenceCurve>();
			auto segmented_ref_curve = *segmented_ref_curves->begin();

			auto pde = new Schema::IfcPointByDistanceExpression(
				new Schema::IfcNonNegativeLengthMeasure(425.0),
				boost::none, boost::none, boost::none,
				segmented_ref_curve);

			auto pl = new Schema::IfcAxis2PlacementLinear(pde, nullptr, nullptr);
			auto lp = new Schema::IfcLinearPlacement(nullptr, pl, nullptr);
			file.addEntity(lp);

			// iterator over the placements
			auto placements = file.instances_by_type<Schema::IfcLinearPlacement>();
			auto iter = placements->begin();
			{
				auto placement = *(iter);

				auto m = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(placement));

				auto d = m->ccomponents().determinant();
				Assert::AreEqual(0.99995, d, 0.00001); // incorrect input leads to determinant being a little off

				auto RefDirX = m->ccomponents().col(0)(0);
				auto RefDirY = m->ccomponents().col(0)(1);
				auto RefDirZ = m->ccomponents().col(0)(2);
				Assert::AreEqual(0.99994132362816945, RefDirX, 0.0001);
				Assert::AreEqual(-0.0041664462931909289, RefDirY, 0.0001);
				Assert::AreEqual(0.0099995012890844882, RefDirZ, 0.0001);

				auto AxisDirX = m->ccomponents().col(2)(0);
				auto AxisDirY = m->ccomponents().col(2)(1);
				auto AxisDirZ = m->ccomponents().col(2)(2);
				Assert::AreEqual(-0.0010900472969021056, AxisDirX, 0.0001);
				Assert::AreEqual(-0.28978322821015978, AxisDirY, 0.0001);
				Assert::AreEqual(0.95709168445086223, AxisDirZ, 0.0001);

				auto x = m->ccomponents().col(3)(0);
				auto y = m->ccomponents().col(3)(1);
				auto z = m->ccomponents().col(3)(2);

				Assert::AreEqual(424.99995652662165, x, 0.001);
				Assert::AreEqual(-0.034722183303109985, y, 0.001);
				Assert::AreEqual(0.25000003129438392, z, 0.001);
			}

			iter++;

			{
				auto placement = *(iter);

				auto m = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(placement));

				auto d = m->ccomponents().determinant();
				Assert::AreEqual(0.99995, d, 0.00001); // incorrect input leads to determinant being a little off

				auto RefDirX = m->ccomponents().col(0)(0);
				auto RefDirY = m->ccomponents().col(0)(1);
				auto RefDirZ = m->ccomponents().col(0)(2);
				Assert::AreEqual(0.99993984923984913, RefDirX, 0.0001);
				Assert::AreEqual(-0.0045064260958536297, RefDirY, 0.0001);
				Assert::AreEqual(0.0099995012890844899, RefDirZ, 0.0001);

				auto AxisDirX = m->ccomponents().col(2)(0);
				auto AxisDirY = m->ccomponents().col(2)(1);
				auto AxisDirZ = m->ccomponents().col(2)(2);
				Assert::AreEqual(-0.0011505876482260603, AxisDirX, 0.0001);
				Assert::AreEqual(-0.30101714069267704, AxisDirY, 0.0001);
				Assert::AreEqual(0.95361803525167710, AxisDirZ, 0.0001);

				auto x = m->ccomponents().col(3)(0);
				auto y = m->ccomponents().col(3)(1);
				auto z = m->ccomponents().col(3)(2);

				Assert::AreEqual(425.99994710799535, x, 0.001);
				Assert::AreEqual(-0.039057725772016598, y, 0.001);
				Assert::AreEqual(0.26000003254615928, z, 0.001);
			}

			iter++;
			{
				// this is the manually defined placement using valid input
				auto placement = *(iter);

				auto m = ifcopenshell::geometry::taxonomy::cast<ifcopenshell::geometry::taxonomy::matrix4>(mapping->map(placement));

				auto d = m->ccomponents().determinant();
				Assert::AreEqual(1.0, d, 0.0000001);


				auto RefDirX = m->ccomponents().col(0)(0);
				auto RefDirY = m->ccomponents().col(0)(1);
				auto RefDirZ = m->ccomponents().col(0)(2);
				Assert::AreEqual(0.99994132362816945, RefDirX, 0.0001);
				Assert::AreEqual(-0.0041664462931909289, RefDirY, 0.0001);
				Assert::AreEqual(0.0099995012890844882, RefDirZ, 0.0001);

				auto AxisDirX = m->ccomponents().col(2)(0);
				auto AxisDirY = m->ccomponents().col(2)(1);
				auto AxisDirZ = m->ccomponents().col(2)(2);
				Assert::AreEqual(-0.011206784726867754, AxisDirX, 0.0001);
				Assert::AreEqual(-0.28972551595900053, AxisDirY, 0.0001);
				Assert::AreEqual(0.95704416480033805, AxisDirZ, 0.0001);

				auto x = m->ccomponents().col(3)(0);
				auto y = m->ccomponents().col(3)(1);
				auto z = m->ccomponents().col(3)(2);

				Assert::AreEqual(424.99995652662165, x, 0.001);
				Assert::AreEqual(-0.034722183303109985, y, 0.001);
				Assert::AreEqual(0.25000003129438392, z, 0.001);
			}
		}
	};
}
