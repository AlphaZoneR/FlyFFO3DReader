
#include "LoadedObject.hpp"

int main(){
    LoadedObject obj = LoadedObject("Obj_MaCoPrStJo01.o3d");
//    std::vector<float> vertices = obj.getVerticeList();
//    std::vector<int> indices = obj.getIndiceList();
    std::vector< pair < float, float > > a = obj.getUvs();
    cout << obj.getTextureName() << endl;

}


