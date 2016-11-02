#include <cstdlib>
#include <iostream>
#include <ctime>
#include <time.h>
#include <iomanip>
#include <vector>
#include <fstream>
using namespace std;

string abc[] = {
  "A", "B" , "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", 
  "N", "O", "P", "Q", "R", "S", "T", "U", "W", "X", "Y", "Z"};


class Upgma  {
    public:
      std::size_t row, col;
      float ** matriz;
      float value;
      vector<string> header;
      string file;

      Upgma (string file_name, int row, int col) 
        : file(file_name), row(row), col(col) {
          this->matriz = createMatriz(row, col);
          this->LoadMatriz();
      }

      float** createMatriz(size_t x, size_t y) {
          float ** m;
          m = new float*[x];
          for (size_t i=0; i<x; ++i) {
              m[i] = new float[y];
              for (size_t j=0; j<y; ++j)
                m[i][j] = 0;
          }
          return m;
      }

      vector<int> getMin(vector<string> header, float** matriz,
          size_t row, size_t col) {

          int positions[2] = {1, 0};
          this->value = matriz[1][0];
          bool flag = false;
          for (int x = 1; x < row; ++x) {
            for (int y = 0; y < x; ++y) {
                if ( flag && matriz[x][y] < this->value ) {
                  positions[0] = x;
                  positions[1] = y;
                  this->value = matriz[x][y];
                }
                else
                  flag = true;
            }
          }
          vector<int> pos;
          for (int u=0; u<2; ++u)
              pos.push_back(positions[u]);
          cout << "\n" << setw(6) << "Min: " << this->value << ", position("<<pos.at(0) << 
              ", " <<pos.at(1)<< "), new: " << header[pos.at(0)] << header[pos.at(1)];
          return pos;
      }

      void getCluster() {
          std::ofstream out("tree.txt");
          cout << "\n Original Matriz: \n";
          printMatriz(this->matriz, this->header, row, col);
          int clusters = row;
          float ** currentMatriz = this->matriz;
          float ** dynamic_matriz;
          string result;
          vector<string> dynamic_header = this->header;
          while(clusters > 2) {
              cout << "\n **************************************** \n";
              vector<int> min_position = getMin(
                dynamic_header, currentMatriz, clusters, clusters);
              vector<string> temporal_header;
              for (int u=0; u<dynamic_header.size(); ++u) {
                  if (u != min_position.at(0) && u != min_position.at(1))
                      temporal_header.push_back(dynamic_header.at(u));
              }
              temporal_header.push_back(
                  dynamic_header.at(
                    min_position.at(0)) + dynamic_header.at(min_position.at(1)));
              result = "("+dynamic_header.at(min_position.at(0))
                + "," + dynamic_header.at(min_position.at(1))+"):"+std::to_string(this->value);
              out << result << endl;
              clusters --;        
              dynamic_matriz = createMatriz(clusters, clusters);
              size_t temp_x = 0;
              // set new matriz
              cout <<  endl << " Clusters: " << clusters;
              for (size_t x=0; x<clusters+1; ++x) {
                  size_t temp_y = 0;
                  if (x!=min_position.at(0) && x!=min_position.at(1)) {
                    for (size_t y=0; y<clusters+1; ++y) {
                        if (y!=min_position.at(0) && y!=min_position.at(1)) {
                            dynamic_matriz[temp_x][temp_y] = currentMatriz[x][y];
                            temp_y++;
                        }
                    }
                    temp_x++;
                  }
              }
              if (clusters == 2) {
                  float sum = 0;
                  for (size_t u=1; u<3; ++u) {
                      sum += currentMatriz[0][u];
                  }
                  dynamic_matriz[1][0] = sum/2;
                  dynamic_matriz[0][1] = dynamic_matriz[1][0];
                  string temp = "(("+temporal_header.at(0)
                      + "," + temporal_header.at(1)+"):"+std::to_string(dynamic_matriz[1][0])
                      + ");";
                  out <<  temp;                 
              }
              else {
                  size_t u = 0;
                  for (size_t x=0; x<clusters+1; ++x) {
                      if (x!=min_position.at(0) && x!=min_position.at(1) ) {
                        float sum = (
                            currentMatriz[min_position.at(0)][x] + currentMatriz[min_position.at(1)][x]);
                        dynamic_matriz[temp_x][u] = sum/2;
                        dynamic_matriz[u][temp_x] = dynamic_matriz[temp_x][u];
                        u++;
                      }
                  }
              }
              cout << "\n New Matriz: \n";
              printMatriz(dynamic_matriz, temporal_header, clusters, clusters);
              currentMatriz = dynamic_matriz;
              dynamic_header = temporal_header;
          }
          out.close();
      }

      void LoadMatriz() {
        ifstream in(this->file);
        if (!in) {
            cout << "Cannot open file.\n";
            return;
        }
        string line;
        for( size_t x = 0; getline( in, line ); ) {
            int i = 0;
            stringstream ssin(line);
            while (ssin.good() && i < this->row){
              ssin >> this->matriz[x][i];
              this->matriz[i][x] =  this->matriz[x][i];
              ++i;
            }
            header.push_back(abc[x]);
            ++x;            
        }
        in.close();
      }

      void printMatriz(float ** matriz, vector<string> header, size_t row, size_t col) {
          cout << "\n" << setw(7) << "-";
          for (size_t x = 0; x < header.size(); ++x)
              cout << setw(7) << header.at(x);
          cout << endl;   
          for (size_t x = 0; x < row; ++x) {
              cout << setw(7) << header.at(x);
              for (size_t y = 0; y < col; ++y) {
                cout << setw(7) << *(*(matriz + x) + y);
              }
              cout << endl;
          }
      }
      ~Upgma() {
          for (size_t i = 0; i < this->row; ++i) {
            delete[] this->matriz[i];
          }
          delete[] this->matriz;
      }
};

int main() {
    Upgma *Cluster = new Upgma("data.txt", 11, 11);
    Cluster->getCluster();
    return 0;
}
