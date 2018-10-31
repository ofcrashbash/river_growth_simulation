#include "riversim.hpp"

void gmsh_possibilites(int argc, char *argv[])
{
    const int meshDimension = 2; 

    cout << "init" << endl;
    gmsh::initialize(argc, argv);

    //only this format is supported by deal.ii
    //gmsh::option::setNumber("Mesh.MshFileVersion", 2.2);

    //create new test model
    cout << "add" << endl;
    mdl::add("test");
    cout << "List of models: " << mdl::list << endl;
    //cout << "Model dimenssions: " << mdl::getDimension() << endl;

    
    /*
        defining of geometry
    */
    int geomTag = 1;
    cout << "discr entity" << endl;
    mdl::addDiscreteEntity(meshDimension, geomTag);
    
    //node points
    cout << "set nodes" << endl;
    msh::setNodes(
        meshDimension, 
        geomTag, 
        {1, 2, 3, 4, 5}, 
        {0., 0., 0.,   //node 1 
         0.5, 0., 0.,  //node 2
         1., 0., 0.,   //node 3
         1., 1., 0.,   //node 4
         0., 1., 0.}); //node 5

    //elemets definition(e.g lines)
    //vector<int> elementTypes{1};//1-line, 2 -triangle, see gmsh.pdf for more details
    //vector<vector<int>> elementTags = {{1, 1, 1, 1}};
    //vector<vector<int>> nodeTags = {{1,2,2,3,3,4,4,1}};
    cout << "set elements" << endl;
    msh::setElements(
        meshDimension, 
        geomTag, 
        {1}, 
        {{1, 2, 3, 4, 5}},
        {{1, 2, 2, 3, 3, 4, 4, 5, 5, 1}});

    //Mesh generation
    //msh::generate(meshDimension);
    //gmsh::graphics::draw();
    gmsh::fltk::initialize();
    gmsh::fltk::wait();
    gmsh::fltk::run();
    
    //gmsh::write("playing.msh");
    //finalizing working with gmsh library
    cout << "fina" << endl;
    gmsh::finalize();
}

void gmsh_reverse()
{
    gmsh::initialize();
    gmsh::model::add("square");
    gmsh::model::geo::addPoint(0, 0, 0, 0.1, 1);
    gmsh::model::geo::addPoint(0.5, 0, 0, 0.1, 2);
    gmsh::model::geo::addPoint(0.5, 0.5, 0, 0.1, 6);
    gmsh::model::geo::addPoint(1, 0, 0, 0.1, 3);
    gmsh::model::geo::addPoint(1, 1, 0, 0.1, 4);
    gmsh::model::geo::addPoint(0, 1, 0, 0.1, 5);
    gmsh::model::geo::addLine(1, 2, 1);
    gmsh::model::geo::addLine(2, 3, 2);
    gmsh::model::geo::addLine(3, 4, 3);
    gmsh::model::geo::addLine(4, 5, 4);
    gmsh::model::geo::addLine(5, 1, 4);
    gmsh::model::geo::addCurveLoop({1, 2, 3, 4, 5}, 1);
    gmsh::model::geo::addPlaneSurface({1}, 6);
    gmsh::model::geo::synchronize();
    gmsh::fltk::run();
    //gmsh::model::mesh::generate(2);
    gmsh::write("square.msh");
    gmsh::finalize();
}

void gmsh_to_dealii()
{
    Triangulation<2> triangulation;
    GridIn<2> gridin;
    gridin.attach_triangulation(triangulation);
    std::ifstream f("square.msh");
    gridin.read_msh(f);

}


void dealii_test()
{
    Triangulation<2> triangulation;
    GridGenerator::hyper_cube (triangulation);
    triangulation.refine_global (4);
    ofstream out ("grid-1.eps");
    GridOut grid_out;
    grid_out.write_eps (triangulation, out);
    cout << "Grid written to grid-1.eps" << endl;
}
