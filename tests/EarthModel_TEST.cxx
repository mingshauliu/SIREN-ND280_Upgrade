
#include <cmath>
#include <math.h>
#include <cstdio>
#include <string>
#include <random>
#include <fstream>
#include <iostream>

#include <gtest/gtest.h>

#include "earthmodel-service/EarthModel.h"
#include "earthmodel-service/Geometry.h"

#include "FakeMaterialModel.h"
#include "FakeEarthModel.h"

using namespace earthmodel;

TEST(Constructor, Default)
{
    EXPECT_NO_THROW(EarthModel A);
}

TEST(DefaultMaterials, VacuumOnly)
{
    EarthModel A;
    MaterialModel materials = A.GetMaterials();

    int material_count = 0;
    while(materials.HasMaterial(material_count)) {
        material_count += 1;
    }
    EXPECT_EQ(material_count, 1);

    std::string name = "VACUUM";

    ASSERT_TRUE(materials.HasMaterial(name));

    int id = materials.GetMaterialId(name);

    ASSERT_EQ(0, id);
    EXPECT_DOUBLE_EQ(7.0/8.0, materials.GetPNERatio(id));

    int component_id = 1000070080;
    std::map<int, double> material_map = materials.GetMaterialMap(id);
    ASSERT_EQ(1, material_map.size());
    ASSERT_EQ(1, material_map.count(component_id));
    EXPECT_DOUBLE_EQ(1.0, material_map[component_id]);
}

/*
    EarthSector sector;
    sector.material_id = materials_.GetMaterialId("VACUUM");
    sector.level = sectors_.size();
    sector.geo = Sphere(Vector3D(0,0,0), std::numeric_limits<double>::infinity(), 0).create();
    sector.density = DensityDistribution1D<RadialAxis1D,ConstantDistribution1D>().create(); // Use the universe_mean_density from GEANT4
    sectors_.push_back(sector);
*/

TEST(DefaultSectors, VacuumOnly)
{
    EarthModel A;
    std::vector<EarthSector> sectors = A.GetSectors();
    ASSERT_EQ(1, sectors.size());
    EarthSector sector = sectors[0];
    EXPECT_EQ(0, sector.material_id);
    EXPECT_EQ(std::numeric_limits<int>::min(), sector.level);
    Sphere geo(Vector3D(0,0,0), std::numeric_limits<double>::infinity(), 0, std::numeric_limits<int>::min());
    DensityDistribution1D<RadialAxis1D,ConstantDistribution1D> density;
    EXPECT_EQ(geo, *sector.geo);
    EXPECT_EQ(density, *sector.density);
}

TEST_F(FakeMaterialModelTest, EarthModelConstructorEmptyModel)
{
    EXPECT_THROW(EarthModel A("", materials_file), char const *);
}

TEST_F(FakeMaterialModelTest, EarthModelConstructorEmptyPathEmptyModel)
{
    EXPECT_THROW(EarthModel A("", "", materials_file), char const *);
}

TEST(Constructor, EarthModelConstructorEmptyModelEmptyMaterials)
{
    EXPECT_THROW(EarthModel A("", ""), char const *);
}

TEST(Constructor, EarthModelConstructorEmptyPathEmptyModelEmptyMaterials)
{
    EXPECT_THROW(EarthModel A("", "", ""), char const *);
}

TEST_F(FakeMaterialModelTest, EarthModelConstructorEmptyPathBadModel)
{
    EXPECT_THROW(EarthModel("", std::tmpnam(nullptr), materials_file), char const *);
}

TEST_F(FakeMaterialModelTest, EarthModelConstructorBadModel)
{
    EXPECT_THROW(EarthModel(std::tmpnam(nullptr), materials_file), char const *);
}

TEST_F(FakeMaterialModelTest, EarthModelConstructorBadModelEmptyMaterial)
{
    EXPECT_THROW(EarthModel(std::tmpnam(nullptr), ""), char const *);
}

TEST_F(FakeLegacyEarthModelTest, LegacyFileLoadWithNoIce)
{
    unsigned int N_rand = 1000;
    for(unsigned int i=0; i<N_rand; ++i) {
        ASSERT_NO_THROW(reset());
        EarthModel A;
        ASSERT_NO_THROW(A.LoadMaterialModel(materials_file));
        double max_depth = 5000;
        max_depth = std::min(max_depth, *std::max_element(layer_radii.begin(), layer_radii.end()));
        double depth = FakeLegacyEarthModelFile::RandomDouble()*max_depth;
        double ice_angle = -1;
        EXPECT_NO_THROW(A.LoadConcentricShellsFromLegacyFile(model_file, depth, ice_angle));
    }
}

TEST_F(FakeLegacyEarthModelTest, LegacyFileLoadWithIce)
{
    unsigned int N_rand = 1000;
    for(unsigned int i=0; i<N_rand; ++i) {
        ASSERT_NO_THROW(reset());
        EarthModel A;
        ASSERT_NO_THROW(A.LoadMaterialModel(materials_file));
        double max_depth = 5000;
        max_depth = std::min(max_depth, *std::max_element(layer_radii.begin(), layer_radii.end()));
        double depth = FakeLegacyEarthModelFile::RandomDouble()*max_depth;
        double ice_angle = FakeLegacyEarthModelFile::RandomDouble()*180;
        EXPECT_NO_THROW(A.LoadConcentricShellsFromLegacyFile(model_file, depth, ice_angle));
    }
}

TEST_F(FakeLegacyEarthModelTest, LegacyFileLayerNames)
{
    unsigned int N_rand = 1000;
    for(unsigned int i=0; i<N_rand; ++i) {
        ASSERT_NO_THROW(reset());
        EarthModel A;
        ASSERT_NO_THROW(A.LoadMaterialModel(materials_file));
        double max_depth = 5000;
        max_depth = std::min(max_depth, *std::max_element(layer_radii.begin(), layer_radii.end()));
        double depth = FakeLegacyEarthModelFile::RandomDouble()*max_depth;
        double ice_angle = FakeLegacyEarthModelFile::RandomDouble()*180;
        ASSERT_NO_THROW(A.LoadConcentricShellsFromLegacyFile(model_file, depth, ice_angle));
        std::vector<EarthSector> sectors = A.GetSectors();
        EXPECT_EQ(layer_names.size(), sectors.size() - 1);
        unsigned int n_layers = std::min(layer_names.size(), sectors.size() - 1);
        for(unsigned int j=0; j<n_layers; ++j) {
            EXPECT_EQ(layer_names[j], sectors[j+1].name);
        }
    }
}

/*
    std::vector<std::string> layer_names;
    std::vector<double> layer_thicknesses;
    std::vector<double> layer_radii;
    std::vector<Polynom> layer_densities;
    std::vector<std::string> layer_materials;
struct EarthSector {
    std::string name;
    int material_id;
    int level;
    std::shared_ptr<const Geometry> geo;
    std::shared_ptr<const DensityDistribution> density;
};
*/

TEST_F(FakeLegacyEarthModelTest, LegacyFileLayerMaterials)
{
    unsigned int N_rand = 1000;
    for(unsigned int i=0; i<N_rand; ++i) {
        ASSERT_NO_THROW(reset());
        EarthModel A;
        ASSERT_NO_THROW(A.LoadMaterialModel(materials_file));
        double max_depth = 5000;
        max_depth = std::min(max_depth, *std::max_element(layer_radii.begin(), layer_radii.end()));
        double depth = FakeLegacyEarthModelFile::RandomDouble()*max_depth;
        double ice_angle = FakeLegacyEarthModelFile::RandomDouble()*180;
        ASSERT_NO_THROW(A.LoadConcentricShellsFromLegacyFile(model_file, depth, ice_angle));
        std::vector<EarthSector> sectors = A.GetSectors();
        EXPECT_EQ(layer_materials.size(), sectors.size() - 1);
        unsigned int n_layers = std::min(layer_materials.size(), sectors.size() - 1);
        MaterialModel materials = A.GetMaterials();
        for(unsigned int j=0; j<n_layers; ++j) {
            EXPECT_EQ(layer_materials[j], materials.GetMaterialName(sectors[j+1].material_id));
        }
    }
}

TEST_F(FakeLegacyEarthModelTest, LegacyFileSectorTypes)
{
    unsigned int N_rand = 1000;
    for(unsigned int i=0; i<N_rand; ++i) {
        ASSERT_NO_THROW(reset());
        EarthModel A;
        ASSERT_NO_THROW(A.LoadMaterialModel(materials_file));
        double max_depth = 5000;
        max_depth = std::min(max_depth, *std::max_element(layer_radii.begin(), layer_radii.end()));
        double depth = FakeLegacyEarthModelFile::RandomDouble()*max_depth;
        double ice_angle = FakeLegacyEarthModelFile::RandomDouble()*180;
        ASSERT_NO_THROW(A.LoadConcentricShellsFromLegacyFile(model_file, depth, ice_angle));
        std::vector<EarthSector> sectors = A.GetSectors();
        unsigned int n_layers = sectors.size()-1;
        ASSERT_EQ(n_layers, layer_thicknesses.size());
        double max_radius = 0.0;
        Vector3D origin;
        for(unsigned int j=0; j<n_layers; ++j) {
            unsigned int sector_index = j+1;
            unsigned int layer_index = j;
            std::shared_ptr<const Geometry> geo = sectors[sector_index].geo;
            Sphere const* sphere = dynamic_cast<Sphere const*>(geo.get());
            ASSERT_TRUE(sphere);
            double ice_offset = sphere->GetPosition().magnitude();

            if(ice_offset == 0.0) {
                EXPECT_GT(sphere->GetRadius(), max_radius);
                EXPECT_LE(sphere->GetInnerRadius(), max_radius);
            }
            else {
                EXPECT_GT(sphere->GetRadius()+ice_offset, max_radius);
                if(sphere->GetInnerRadius() > 0)
                    EXPECT_LE(sphere->GetInnerRadius()+ice_offset, max_radius);
            }
            double layer_thickness = layer_thicknesses[layer_index];
            double layer_radius = layer_radii[layer_index];
            if(ice_offset == 0.0) {
                ASSERT_DOUBLE_EQ(layer_radius, sphere->GetRadius());
            }
            else {
                ASSERT_DOUBLE_EQ(layer_radius, sphere->GetRadius() + ice_offset);
            }
            if(ice_offset == 0.0) {
                EXPECT_NEAR(layer_thickness, sphere->GetRadius() - max_radius, std::max(std::abs(layer_thickness), std::abs(sphere->GetRadius() - max_radius))*1e-6);
            }
            else {
                EXPECT_NEAR(layer_thickness, sphere->GetRadius() - max_radius + ice_offset, std::max(std::abs(layer_thickness), std::abs(sphere->GetRadius() - max_radius + ice_offset))*1e-6);
            }
            if(ice_offset == 0.0) {
                max_radius = sphere->GetRadius();
            }
            else {
                max_radius = sphere->GetRadius() + ice_offset;
            }
            origin = sphere->GetPosition();
        }
    }
}

TEST_F(FakeLegacyEarthModelTest, LegacyFileConstantIntegralInternal)
{
    unsigned int N_rand = 1000;
    for(unsigned int i=0; i<N_rand; ++i) {
        ASSERT_NO_THROW(reset(1, 1));
        EarthModel A;
        ASSERT_NO_THROW(A.LoadMaterialModel(materials_file));
        double max_depth = 5000;
        max_depth = std::min(max_depth, *std::max_element(layer_radii.begin(), layer_radii.end()));
        double depth = FakeLegacyEarthModelFile::RandomDouble()*max_depth;
        double ice_angle = -1;
        ASSERT_NO_THROW(A.LoadConcentricShellsFromLegacyFile(model_file, depth, ice_angle));
        std::vector<EarthSector> sectors = A.GetSectors();
        ASSERT_EQ(2, sectors.size());
        EarthSector sector = sectors[1];
        Sphere const * sphere = dynamic_cast<Sphere const *>(sector.geo.get());
        ASSERT_TRUE(sphere);
        double max_radius = sphere->GetRadius();
        double min_radius = sphere->GetInnerRadius();
        Vector3D p0 = RandomVector(max_radius, min_radius);
        Vector3D p1 = RandomVector(max_radius, min_radius);
        DensityDistribution1D<RadialAxis1D,ConstantDistribution1D> const * density = dynamic_cast<DensityDistribution1D<RadialAxis1D,ConstantDistribution1D> const *>(sector.density.get());
        ASSERT_TRUE(density);
        double rho = density->Evaluate(Vector3D());
        double sum = A.GetColumnDepthInCGS(p0, p1);
        EXPECT_DOUBLE_EQ((p1 - p0).magnitude()*rho, sum);
    }
}

TEST_F(FakeLegacyEarthModelTest, LegacyFileConstantIntegralNested)
{
    unsigned int N_rand = 1000;
    for(unsigned int i=0; i<N_rand; ++i) {
        ASSERT_NO_THROW(reset(2, 1));
        EarthModel A;
        ASSERT_NO_THROW(A.LoadMaterialModel(materials_file));
        double max_depth = 5000;
        max_depth = std::min(max_depth, *std::max_element(layer_radii.begin(), layer_radii.end()));
        double depth = FakeLegacyEarthModelFile::RandomDouble()*max_depth;
        double ice_angle = -1;
        ASSERT_NO_THROW(A.LoadConcentricShellsFromLegacyFile(model_file, depth, ice_angle));
        std::vector<EarthSector> sectors = A.GetSectors();
        ASSERT_EQ(3, sectors.size());
        EarthSector sector_0 = sectors[1];
        EarthSector sector_1 = sectors[2];
        Sphere const * sphere_0 = dynamic_cast<Sphere const *>(sector_0.geo.get());
        Sphere const * sphere_1 = dynamic_cast<Sphere const *>(sector_1.geo.get());
        ASSERT_TRUE(sphere_0);
        ASSERT_TRUE(sphere_1);
        EXPECT_GE(sphere_1->GetRadius(), sphere_0->GetRadius());
        double max_radius = sphere_1->GetRadius();
        double min_radius = sphere_0->GetInnerRadius();
        Vector3D p0 = RandomVector(max_radius, min_radius);
        Vector3D p1 = RandomVector(max_radius, min_radius);
        double distance = (p1-p0).magnitude();
        DensityDistribution1D<RadialAxis1D,ConstantDistribution1D> const * density_0 = dynamic_cast<DensityDistribution1D<RadialAxis1D,ConstantDistribution1D> const *>(sector_0.density.get());
        DensityDistribution1D<RadialAxis1D,ConstantDistribution1D> const * density_1 = dynamic_cast<DensityDistribution1D<RadialAxis1D,ConstantDistribution1D> const *>(sector_1.density.get());
        ASSERT_TRUE(density_0);
        ASSERT_TRUE(density_1);
        double rho_0 = density_0->Evaluate(Vector3D());
        double rho_1 = density_1->Evaluate(Vector3D());
        ASSERT_LE(p0.magnitude(), max_radius);
        ASSERT_LE(p1.magnitude(), max_radius);
        bool in_0 = p0.magnitude() <= sphere_0->GetRadius();
        bool in_1 = p1.magnitude() <= sphere_0->GetRadius();
        double integral = 0.0;
        double sum = A.GetColumnDepthInCGS(p0, p1);
        if(in_0 and in_1) {
            integral = rho_0 * (p1-p0).magnitude();
            ASSERT_DOUBLE_EQ(integral, sum);
        }
        else {
            Vector3D direction = p1 - p0;
            direction.normalize();
            std::vector<Geometry::Intersection> intersections = sphere_0->Intersections(p0, direction);
            if(intersections.size() > 1) {
                double dist_0 = intersections[0].distance;
                double dist_1 = intersections[1].distance;
                if(dist_0 > 0 and dist_1 > 0) {
                    double near = std::min(dist_0, dist_1);
                    if((not in_0) and in_1) {
                        integral += rho_1*near;
                        integral += rho_0*((p1-p0).magnitude() - near);
                        ASSERT_DOUBLE_EQ(integral, sum);
                    } else if(in_0 and not in_1) {
                        integral += rho_0*near;
                        integral += rho_1*((p1-p0).magnitude() - near);
                        ASSERT_DOUBLE_EQ(integral, sum);
                    } else if((not in_0) and (not in_1)) {
                        double far = std::max(std::max(dist_0, 0.0), std::max(dist_1, 0.0));
                        if(near < distance) {
                            integral += rho_1*near;
                            if(far < distance) {
                                integral += rho_0*(far-near);
                                integral += rho_1*(distance-far);
                            } else {
                                integral += rho_0*(distance - near);
                            }
                        } else {
                            integral += rho_1*distance;
                        }
                        ASSERT_DOUBLE_EQ(integral, sum);
                    }
                }
                else if (dist_0 <= 0 and dist_1 <= 0) {
                    integral = rho_1 * (p1-p0).magnitude();
                    ASSERT_DOUBLE_EQ(integral, sum);
                }
                else {
                    double dist = std::max(dist_0, dist_1);
                    if((not in_0) and in_1) {
                        assert(false);
                    } else if(in_0 and not in_1) {
                        integral += rho_0*dist;
                        integral += rho_1*((p1-p0).magnitude() - dist);
                        ASSERT_DOUBLE_EQ(integral, sum);
                    } else if((not in_0) and (not in_1)) {
                        assert(false);
                    }
                }
            }
            else {
                integral = rho_1 * (p1-p0).magnitude();
                ASSERT_DOUBLE_EQ(integral, sum);
            }
        }
        ASSERT_DOUBLE_EQ(integral, sum);
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
