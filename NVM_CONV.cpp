/*********************************************************************/
/* File: NVM_CONV.cpp                                                */
/* Last Edition: 18/11/2015, 00:57 PM.                               */
/*********************************************************************/
/* Programmed by:                                                    */
/* Bernardo Aceituno C                                               */
/*********************************************************************/
/*N-View Match (.nvm) models format converter software               */
/*inputs the .nvm file the .ply model and two .csv files for the     */
/*model and camera positions for visual odometry                     */
/*********************************************************************/

#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]){

    int cam = 0;
    int mod = 0;

    if(string(argv[1]) == "-h"){
        cout << "Help:" << endl;
        cout << "the following is a Conversion software for N-View Match (.nvm) point clouds" << endl;
        cout << "   - Input:" << endl;
        cout << "     the arguments should ve given as:" << endl;
        cout << "           -i .nvm reconstruction input file." << endl;
        cout << "           -o .ply model output file." << endl;
        cout << "           -cm .csv model output file." << endl;
        cout << "           -cc .csv cameras info output file." << endl;
        cout << "   - Example:" << endl;
        cout << "           $ ./NVM_CONV -i model.nvm -o output.ply -cc camera_csv.csv -cm model_csv.csv" << endl;

        return 0;
    }

    if(argc < 5 || argc > 9){
        cout << "ERROR: wrong number of arguments!" << endl;
        return -1;
    }

    char * nvm_model;
    char * ply_model;
    char * csv_model;
    char * csv_camera;
    int i;
    string argument;

    for(i = 1; i < argc; i++){

        argument = string(argv[i]);

        if(argument == "-i") nvm_model = argv[i+1];
        if(argument == "-o") ply_model = argv[i+1];
        if(argument == "-cm"){
            mod = 1;
            csv_model = argv[i+1];
        }
        if(argument == "-cc"){
            cam = 1;
            csv_camera = argv[i+1];
        }
    }

    ifstream in;
    ofstream out;
    ofstream outcams;
    ofstream outmod;

    in.open(nvm_model,ios::in);

    if(!in.is_open()){
        cout << "ERROR: Invalid input file!" << endl;
        return -1;
    }

    out.open(ply_model);
    if(mod == 1) outmod.open(csv_model);
    if(cam == 1) outcams.open(csv_camera);


    //reads from the input
    string buffer;
    getline(in, buffer);
   
    // read and skip the camera section.
    int ncam = 0, npoint = 0;   
    in >> ncam;  
    if(ncam <= 1) return false; 

    for(int i = 0; i < ncam; ++i){
        float f, r; float wxyz[3]; float center[3];
        //reads the information for each camera
        in >> buffer >> f >> wxyz[0] >> wxyz[1] >> wxyz[2] >> center[0] >> center[1] >> center[2] >> r; 
        //skips the rest of the line
        getline(in, buffer);
        //prints the information of the cameras if the input is given
        if(cam == 1) outcams << f << ',' << ' ' << wxyz[0] << ',' << ' ' << wxyz[1] << ',' << ' ' 
                             << wxyz[2] << ',' << ' ' << center[0] << ',' << ' ' << center[1] 
                             << ',' << ' ' << center[2] << ',' << ' ' << r << '\n';
    }

    // read the number of points.
    in >> npoint;   
    if(npoint <= 0) return -1; 

    //writes the PLY header
    out << "ply\nformat ascii 1.0\n"
         << "element vertex " << npoint << '\n'
         << "property float x\n"
         << "property float y\n"
         << "property float z\n"
         << "property uchar diffuse_red\n"
         << "property uchar diffuse_green\n"
         << "property uchar diffuse_blue\n"
         << "end_header\n";
  
    // Load all the vertices
    for(int i = 0; i < npoint; ++i){
        float pt[3]; int cc[3];
        // read the point coordinate and color
        in  >> pt[0] >> pt[1] >> pt[2] >> cc[0] >> cc[1] >> cc[2];
        // skip the rest of data.
        getline(in, buffer); 

        //here you should write a vertex to PLY
        out << pt[0] << ' ' << pt[1] << ' ' << pt[2] << ' '
            << cc[0] << ' ' << cc[1] << ' ' << cc[2] << '\n';

        if(mod == 1) outmod << pt[0] << ',' << ' ' << pt[1] << ',' << ' ' << pt[2] << ',' <<  ' '
                            << cc[0] << ',' << ' ' << cc[1] << ',' << ' ' << cc[2] << '\n';
    }

    cout << "Conversion done!" << endl;

    return 0;
}
