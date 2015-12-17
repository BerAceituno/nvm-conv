/*********************************************************************/
/* File: NVM_CONV.cpp                                                */
/* Last Edition: 24/11/2015, 09:03 PM.                               */
/*********************************************************************/
/* Programmed by:                                                    */
/* Bernardo Aceituno C                                               */
/*********************************************************************/
/*N-View Match (.nvm) models format converter software               */
/*inputs the .nvm file the .ply model and two .csv files for the     */
/*model and camera positions for visual odometry                     */
/*********************************************************************/

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]){

    int cam = 0;
    int mod = 0;
    int ply = 0;
    int pcd = 0;
    int comma = 1;

    if(string(argv[1]) == "-h"){
        cout << "Help:" << endl;
        cout << "the following is a Conversion software for N-View Match (.nvm) point clouds" << endl;
        cout << "   - Input:" << endl;
        cout << "     the arguments should ve given as:" << endl;
        cout << "           -i  .nvm reconstruction input file." << endl;
        cout << "           -f  model output format (ply, csv of pcd)" << endl;
        cout << "           -cc .csv cameras info output file." << endl;
        cout << "           -t  use tabs in .csv files as separation." << endl;
        cout << "   - Example:" << endl;
        cout << "           $ ./NVM_CONV -i model.nvm -f ply -cc" << endl;

        return 0;
    }

    if(argc < 5){
        cout << "ERROR: wrong number of arguments!" << endl;
        return -1;
    }

    char * nvm_model;
    int i;
    string argument;

    for(i = 1; i < argc; i++){

        argument = string(argv[i]);

        if(argument == "-i") nvm_model = argv[i+1];
        if(argument == "-f"){
            string ext = string(argv[i+1]); 
            if(ext == "ply") ply = 1;
            else if(ext == "csv") mod = 1;
            else return -1;
        }
        if(argument == "-cc"){
            cam = 1;
        }
        if(argument == "-t") comma = 0;
    }

    string cs;

    if(comma == 1) cs = ", ";
    else cs = "\t";

    ifstream in;
    ofstream outply;
    ofstream outcams;
    ofstream outmod;

    in.open(nvm_model,ios::in);

    if(!in.is_open()){
        cout << "ERROR: Invalid input file!" << endl;
        return -1;
    }

    string nvm_model_srt = string(nvm_model);

    string filename = nvm_model_srt.substr(nvm_model_srt.find_last_of("/") + 1,nvm_model_srt.length() - 4);

    int dis = 1;
    int MODELN = 0;
    string buffer;

    while(1){
        ostringstream output_name_ply;
        ostringstream output_name_csv;
        ostringstream output_name_cam;

        if(dis == 1){
            if(ply == 1 && mod == 0){
            outply.close();
            output_name_ply << filename << "_" << (int) MODELN << ".ply";
            outply.open(output_name_ply.str().c_str());
            }
            if(mod == 1){
                outmod.close();
                output_name_csv << filename << "_" << (int) MODELN << ".csv";
                outmod.open(output_name_csv.str().c_str());
            }
            if(cam == 1){
                outcams.close();
                output_name_cam << filename << "_" << (int) MODELN << "_CAM" << ".csv";
                outcams.open(output_name_cam.str().c_str());
            }
            //reads from the input
            getline(in, buffer);
        }
        // read and skip the camera section.
        int ncam = 0, npoint = 0;   
        in >> ncam;  
        if(ncam <= 1) return false; 

        for(int i = 0; i < ncam; ++i){
            float f, r; float wxyz[4]; float center[3];
            //reads the information for each camera
            in >> buffer >> f >> wxyz[0] >> wxyz[1] >> wxyz[2] >> wxyz[3] >> center[0] >> center[1] >> center[2] >> r; 
            //skips the rest of the line
            getline(in, buffer);
            //prints the information of the cameras if the input is given
            if(cam == 1) outcams << f << cs << wxyz[0] << cs << wxyz[1] << cs 
                                 << wxyz[2] << cs << wxyz[3] << cs << center[0] << cs 
                                 << center[1] << cs << center[2] << cs << r << '\n';
        }

        // read the number of points.
        in >> npoint;   
        if(npoint <= 0) break; 

        //writes the PLY header
        if(ply == 1) outply << "ply\nformat ascii 1.0\n"
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
            if(ply == 1) outply << pt[0] << ' ' << pt[1] << ' ' << pt[2] << ' '
                                << cc[0] << ' ' << cc[1] << ' ' << cc[2] << '\n';

            if(mod == 1) outmod << pt[0] << cs << pt[1] << cs << pt[2] << ',' <<  ' '
                                << cc[0] << cs << cc[1] << cs << cc[2] << '\n';
        }

        MODELN += 1;
        dis = 0;
        cout << "Conversion " << (int) MODELN << " done!" << endl;
    }
    return 0;
}
