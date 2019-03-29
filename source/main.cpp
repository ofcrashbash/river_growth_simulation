/*! \file main.cpp
    \brief Entry point of program and main loop handler.
    
    Main incorporates user commands handling and managing of inner simulation objects.
    User commands are simple terminal commands(run ./riversim --help to check them all).
    It setups initial conditions and parameters. Initializes all object that are used in
    simulation like: Geometry, Mesh, Triangle, Gmsh and Simualtion, which are all combined
    into the loop.
    Please review code for further details
*/

#include "riversim.hpp"

using namespace River;
using namespace std;
///Entry point of RiverSim program.
/** It glues all together user inputs(commands) and inner objects to run simulation */
int main(int argc, char *argv[])
{
    /**
        Program options
    */
    auto vm = process_program_options(argc, argv);

    if (vm.count("help") || vm.count("version"))
        return 0;

    if (!vm.count("supprchess-signature"))
        print_ascii_signature();

    if(!vm["dev-mode"].as<bool>())
        ForwardRiverEvolution(vm);

    auto river_boundary_id = 3;
    auto boundary_ids = vector<int>{0, 1, 2, river_boundary_id};
    auto region_size = vector<double>{1, 4};
    auto sources_x_coord = vector<double>{0.1};
    auto sources_id = vector<int>{1};

    Model mdl;
    tethex::Mesh border_mesh;
    Border border(border_mesh);
    border.eps = mdl.eps;
    border.MakeRectangular(
        region_size, 
        boundary_ids,
        sources_x_coord,
        sources_id);

    Tree tr(border.GetSourcesPoint(), border.GetSourcesNormalAngle(), border.GetSourcesId());
    //tr.GetBranch(sources_id.at(0)).AddPoint(Polar{0.1, 0});

    //Loop
    for(unsigned i = 0; i < vm["number-of-steps"].as<unsigned>(); ++i){
        auto mesh = BoundaryGenerator(mdl, tr, border, river_boundary_id);

        Triangle tria;
        tria.AreaConstrain = tria.ConstrainAngle = true;
        tria.MaxTriaArea = vm["mesh-max-area"].as<double>();
        tria.MinAngle = vm["mesh-min-angle"].as<double>();
        tria.Verbose = vm["verbose"].as<bool>();
        tria.CustomConstraint = vm.count("test-flag");
        tria.generate(mesh);
        mesh.convert();
        mesh.write(vm["output-mesh"].as<string>());

        //Simulation
        //Deal.II library
        if(vm["simulate"].as<bool>())
        {
            River::Solver sim;
            sim.numOfRefinments = vm["ref-num"].as<int>();
            sim.SetBoundaryRegionValue(boundary_ids, 0.);
            sim.OpenMesh(vm["output-mesh"].as<string>());
            sim.run(0);
            auto tip_point = tr.GetBranch(sources_id.at(0)).TipPoint();
            auto tip_angle = tr.GetBranch(sources_id.at(0)).TipAngle();
            auto series_params = sim.integrate(tip_point, tip_angle);

            cout << Model::next_point(series_params) << endl;
            tr.AddAbsolutePoints({Model::next_point(series_params)}, {sources_id.at(0)});
        }
    }


    auto mesh = BoundaryGenerator(mdl, tr, border, river_boundary_id);

    Triangle tria;
    tria.AreaConstrain = tria.ConstrainAngle = true;
    tria.MaxTriaArea = vm["mesh-max-area"].as<double>();
    tria.MinAngle = vm["mesh-min-angle"].as<double>();
    tria.Verbose = vm["verbose"].as<bool>();
    tria.CustomConstraint = vm.count("test-flag");
    tria.generate(mesh);
    mesh.convert();
    mesh.write(vm["output-mesh"].as<string>());
    
    Gmsh gmsh;
    gmsh.open(vm["output-mesh"].as<string>());
    gmsh.start_ui();
    
    return 0;
}