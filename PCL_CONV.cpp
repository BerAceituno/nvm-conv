/*********************************************************************/
/* File: PCL_CONV.cpp                                                */
/* Last Edition: 11/01/2016, 09:03 PM.                               */
/*********************************************************************/
/* Programmed by:                                                    */
/* Bernardo Aceituno C                                               */
/*********************************************************************/
/*Point Cloud (pcl) models format converter software                 */
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
        cout << "           -i  .nvm or .ply reconstruction input file." << endl;
        cout << "           -f  model output format (ply, csv or pcd)" << endl;
        cout << "           -cc .csv cameras info output file. [Only if input is .nvm]" << endl;
        cout << "           -t  use tabs in .csv files as separation." << endl;
        cout << "   - Example:" << endl;
        cout << "           $ ./pcl-conv -i model.nvm -f ply -cc" << endl;

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
            else if(ext == "pcd") pcd = 1;
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
    ofstream outpcd;

    in.open(nvm_model,ios::in);

    if(!in.is_open()){
        cout << "ERROR: Invalid input file!" << endl;
        return -1;
    }

    string nvm_model_srt = string(nvm_model);

    //extracts the filename and the filetype
    string filename = nvm_model_srt.substr(nvm_model_srt.find_last_of("/") + 1,nvm_model_srt.length() - 4);
    string type = nvm_model_srt.substr(nvm_model_srt.find_last_of("."),4);

    if(type == ".nvm" || type == ".NVM"){
        int dis = 0;
        int MODELN = 0;

        string buffer;
        getline(in, buffer);
        while(1){
            ostringstream output_name_ply;
            ostringstream output_name_csv;
            ostringstream output_name_cam;
            ostringstream output_name_pcd;

            getline(in, buffer);

            // read and skip the camera section.
            int ncam = 0, npoint = 0;   
            in >> ncam;  
            if(ncam <= 1) break; 

            if(ply == 1){
                outply.close();
                output_name_ply << filename << "_" << (int) MODELN << ".ply";
                outply.open(output_name_ply.str().c_str());
                }
                if(mod == 1){
                    outmod.close();
                    output_name_csv << filename << "_" << (int) MODELN << ".csv";
                    outmod.open(output_name_csv.str().c_str());
                }
                if(pcd == 1){
                    outpcd.close();
                    output_name_pcd << filename << "_" << (int) MODELN << ".pcd";
                    outpcd.open(output_name_pcd.str().c_str());   
                }
                if(cam == 1){
                    outcams.close();
                    output_name_cam << filename << "_" << (int) MODELN << "_CAM" << ".csv";
                    outcams.open(output_name_cam.str().c_str());
            }

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

            if(pcd == 1) outpcd << "# .PCD v.7 - Point Cloud Data file format\n"
                                << "VERSION .7\n"
                                << "FIELDS x y z\n"
                                << "SIZE 4 4 4\n"
                                << "TYPE F F F\n"
                                << "COUNT 1 1 1\n"
                                << "WIDTH " << npoint << "\n"
                                << "HEIGHT 1\n"
                                << "VIEWPOINT 0 0 0 1 0 0 0\n"
                                << "POINTS " << npoint << "\n"
                                << "DATA ascii\n";

            // Load all the vertices
            for(int i = 0; i < npoint; ++i){
                float pt[3]; int cc[3];
                //reads the point coordinate and color
                in  >> pt[0] >> pt[1] >> pt[2] >> cc[0] >> cc[1] >> cc[2];
                //skips the rest of data.
                getline(in, buffer); 

                //writes the points
                if(ply == 1) outply << pt[0] << ' ' << pt[1] << ' ' << pt[2] << ' '
                                    << cc[0] << ' ' << cc[1] << ' ' << cc[2] << '\n';

                if(mod == 1) outmod << pt[0] << cs << pt[1] << cs << pt[2] << cs
                                    << cc[0] << cs << cc[1] << cs << cc[2] << '\n';

                if(pcd == 1) outpcd << pt[0] << ' ' << pt[1] << ' ' << pt[2] << '\n';
            }

            MODELN += 1;
            dis = 0;
            cout << "Conversion " << (int) MODELN << " done!" << endl;
        }
    }
    else if(type == ".ply" || type == ".PLY"){
        string buffer;

        ostringstream output_name_ply;
        ostringstream output_name_csv;
        ostringstream output_name_cam;
        ostringstream output_name_pcd;

        //reads from the input
        int npoint = 0;
        getline(in, buffer);
        getline(in, buffer);
        in >> buffer >> buffer >> npoint; 

        // read the number of points.
        if(npoint <= 0) return -1; 

        //skips the rest of the header
        getline(in, buffer);
        getline(in, buffer);
        getline(in, buffer);
        getline(in, buffer);
        getline(in, buffer);
        getline(in, buffer);
        getline(in, buffer);
        getline(in, buffer);

        if(mod == 1){
                outmod.close();
                output_name_csv << filename << ".csv";
                outmod.open(output_name_csv.str().c_str());
            }
        if(pcd == 1){
            outpcd.close();
            output_name_pcd << filename << ".pcd";
            outpcd.open(output_name_pcd.str().c_str());   
        }

        //writes the pcd header
        if(pcd == 1) outpcd << "# .PCD v.7 - Point Cloud Data file format\n"
                            << "VERSION .7\n"
                            << "FIELDS x y z\n"
                            << "SIZE 4 4 4\n"
                            << "TYPE F F F\n"
                            << "COUNT 1 1 1\n"
                            << "WIDTH " << npoint << "\n"
                            << "HEIGHT 1\n"
                            << "VIEWPOINT 0 0 0 1 0 0 0\n"
                            << "POINTS " << npoint << "\n"
                            << "DATA ascii\n";

        //Load all the vertices
        for(int i = 0; i < npoint; ++i){
            float pt[3]; int cc[3];
            // read the point coordinate and color
            in  >> pt[0] >> pt[1] >> pt[2] >> cc[0] >> cc[1] >> cc[2];
            // skip the rest of data.
            getline(in, buffer); 
            //writes the points
            if(mod == 1) outmod << pt[0] << cs << pt[1] << cs << pt[2] << cs
                                << cc[0] << cs << cc[1] << cs << cc[2] << '\n';

            if(pcd == 1) outpcd << pt[0] << ' ' << pt[1] << ' ' << pt[2] << '\n';
            
            }

        cout << "Conversion done!" << endl;  
    }

    return 0;
}
