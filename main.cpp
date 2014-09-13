#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <cmath>
#include <vector>

#define PI 3.14159265358979323846

using namespace std;

double toRad(double number){
    return ((number * PI) / 180);
}

double getDistance(double lat1,double lon1,double lat2,double lon2) {
    unsigned int R = 6371; // earth's mean radius in km
    double dLat = toRad(lat2-lat1);
    double dLon = toRad(lon2-lon1);
    lat1 = toRad(lat1);
    lat2 = toRad(lat2);
    double a = (sin(dLat/2) * sin(dLat/2)) + cos(lat1) * cos(lat2) * sin(dLon/2) * sin(dLon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    double d = R * c;

   return d;
}

int main(int argc, char* argv[])
{
    unsigned int index;
    string arguments, first, data, final_data, speed;
    double speed_dbl, total_speed, avg_speed;
    bool is_write = false,
            is_output = false,
            is_latitude = true;

    vector<string> buffer_csv;
    vector<double> latitude;
    vector<double> longitude;
    vector<double> elevation;
    vector<double> speed_list;
    vector<double> speed_list_avg;

    if (argc > 0){
        for(index = 0; index < argc; index++){
            if(argv[index] == string("-output") || argv[index] == string("-o")){
                is_output = true;
            }
            if(argv[index] == string("-file") || argv[index] == string("-f")){
                is_write = true;
            }
            if(argv[index] == string("-help") || argv[index] == string("-h") || argv[index] == string("-H")){
                cout << "GPS.exe [path to .gpx file] [Options...] :" << endl;
                cout << "- [\"-f\" or \"-file\"] Create a CSV file containing the latitude, longitude, elevation, speed(km/h) and distance from previous point" << endl;
                cout << "- [\"-o\" or \"-output\"] Show some specfic information as total distance and average speed" << endl;
            }
        }
    }
    arguments = argv[1];

    string file_name = "C:\\Users\\Aztyu\\Desktop\\";
    file_name += arguments;

    size_t index_gpx = file_name.find(".gpx");
    if(index_gpx != string::npos){
        file_name[index_gpx+1] = 'c';
        file_name[index_gpx+2] = 's';
        file_name[index_gpx+3] = 'v';
    }

    string path = "\"" + arguments + "\"";

    ifstream fichier(arguments.c_str(), ios::in);

    if(fichier){
        string contenu;
        bool save_data = false,
            valid_data = false;

        while(getline(fichier, contenu)){
            if (contenu.find("<trkpt") != string::npos){

                //Get the lattitude and longitude
                for(index = 0; index < contenu.size(); index++){
                    if(contenu[index] == '"'){
                        if (save_data){
                            save_data = false;
                            if(is_latitude){
                                latitude.push_back(atof(data.c_str()));
                                is_latitude = false;
                            }else{
                                longitude.push_back(atof(data.c_str()));
                                is_latitude = true;
                            }
                            data.clear();
                        }else{
                            save_data = true;
                        }
                    }else{
                        if (save_data){
                            data += contenu[index];
                        }
                    }
                }

                //Get other info in the 'trkpt' tag
                do{
                    if(contenu.find("<ele") != string::npos){
                        for(index = 0; index < contenu.size(); index++){
                            if(contenu[index] == '>'){
                                save_data = true;
                            }else if(contenu[index] == '<' && save_data){
                                save_data = false;
                                elevation.push_back(atof(data.c_str()));
                            }else{
                                if (save_data){
                                    data += contenu[index];
                                }
                            }
                        }
                    }

                    if(contenu.find("<spe") != string::npos){
                        for(index = 0; index < contenu.size(); index++){
                            if(contenu[index] == '>'){
                                save_data = true;
                            }else if(contenu[index] == '<' && save_data){
                                save_data = false;
                                speed_dbl = atof(speed.c_str());
                                if(speed_dbl > 0.3 || valid_data){
                                    speed_list.push_back(speed_dbl*3.6);
                                    if (!valid_data){
                                        valid_data = true;
                                    }
                                }
                                speed.clear();
                            }else{
                                if (save_data){
                                    speed += contenu[index];
                                }
                            }
                        }
                    }
                    getline(fichier, contenu);
                    save_data = false;
                    data.clear();
                }while (contenu.find("</trkpt") == string::npos);

            }
        }

    }else{
        cout << "Error : Bad file path" << endl;
        cout << arguments;
    }

    fichier.close();

    if(is_write){
       ofstream csv_output(file_name.c_str(), ofstream::out | ofstream::app);
        if(csv_output.is_open()){
            for (index = 0; index < latitude.size(); index++){
                csv_output << latitude[index] << "," << longitude[index] << "," << elevation[index] << "," << speed_list[index];
                if (index+1 > latitude.size()){
                    csv_output << endl;
                }else{
                    csv_output << "," << getDistance(latitude[index], longitude[index], latitude[index+1], longitude[index+1]) << endl;
                }
            }
            csv_output.close();

        }else{
            cout << "error";
        }
    }

    if(is_output){
        /*for (index = 0; index < latitude.size(); index++){
            cout << latitude[index] << "," << longitude[index] << "," << elevation[index] << "," << speed_list[index] << endl;
        }*/

        cout << "La distance entre le depart et l'arrivee est : " << getDistance(latitude[0], longitude[0], latitude[latitude.size()-1], longitude[longitude.size()-1] );
        cout << file_name << endl;

        for(index=0 ; index < speed_list.size() ; index++){
            if(speed_list[index] > 0.001){
                speed_list_avg.push_back(speed_list[index]);
                //cout << speed_list[index] << endl;
                total_speed += speed_list[index];
            }
        }

        avg_speed = total_speed / speed_list_avg.size();
        cout << "La vitesse moyenne est de :" << avg_speed;
    }
    /*cout << latitude[0] << endl;
    cout << longitude[0] << endl;
    cout << latitude[latitude.size()-1] << endl;
    cout << longitude[longitude.size()-1]  << endl;*/



    return 0;
}
