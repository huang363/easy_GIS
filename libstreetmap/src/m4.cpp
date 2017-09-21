#include "m4.h"
#include "m3.h"
#include "m1.h"
#include "../inc/m1_globals.h"
#include "StreetsDatabaseAPI.h"
#include <vector>
#include <queue>
#include <set>
#include <unordered_map>
#include <algorithm>
#include "../inc/path_finding_struct.h"
#include <climits>
#include <cfloat>

using namespace std;

std::vector<unsigned> traveling_courier(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty){
    
    cout << "starting travelling salesman" << endl;
    
    //store whether it has been visited or not and drop off locations
    unordered_map < unsigned, pair <prim,vector<unsigned> > > pickup;
    //store whether it has been visited or not and the pickup location
    unordered_map < unsigned, pair <prim,vector<unsigned> > > dropoff;

    set <prim> open;
    vector < vector <unsigned> > storePath;
    vector <unsigned> result;
    vector < pair<unsigned,unsigned> > ham;
    
    double size = 0;
    double size2 = 0;
    for(auto itr = deliveries.begin();itr!=deliveries.end();itr++){
        auto notfind1 = pickup.find(itr->pickUp);
        auto notfind2 = dropoff.find(itr->dropOff);
        if(pickup.empty() || notfind1 == pickup.end()){
            vector <unsigned> insert;
            insert.push_back(itr->dropOff);
            prim temp(itr->pickUp,1000000000000-size,false,1);
            pickup.insert(make_pair(itr->pickUp,make_pair(temp,insert)));
            size++;
            size++;
        }else if(notfind1 != pickup.end()){
            vector<unsigned> itsfuckedup = notfind1->second.second;
            auto what = find(itsfuckedup.begin(),itsfuckedup.end(),itr->dropOff);
            if(what == itsfuckedup.end()){
                notfind1->second.second.push_back(itr->dropOff);
            }else{
                cout<<"it repeats"<<endl;
            }    
        }
        if(dropoff.empty() || notfind2 == dropoff.end()){
            vector <unsigned> insert;
            insert.push_back(itr->pickUp);
            size2++;
            prim temp(itr->dropOff,1000000000000-size2,false,0);
            dropoff.insert(make_pair(itr->dropOff,make_pair(temp,insert)));
            size2++;
        }else if(notfind2 != dropoff.end()){
            vector<unsigned> itsfuckedup = notfind2->second.second;
            if(find(itsfuckedup.begin(),itsfuckedup.end(),itr->pickUp) != itsfuckedup.end()){
                cout<<"it repeats"<<endl;
            }else{
                notfind2->second.second.push_back(itr->pickUp);
            }
        }
    }
    cout<<"pickup locations:"<<size<<endl;
    cout<<"dropoff locations:"<<size2<<endl;
      
    
    //initializing the open set id is inserted, parents and cost is set to UINT_MAX, except for the cost of first pickup location
    prim testcompare(0,DBL_MAX,UINT_MAX,false,0);
    for(auto i = pickup.begin(); i != pickup.end();i++){

        if(i == pickup.begin()){
            testcompare = i->second.first;
            prim temp = i->second.first;
            temp.cost = 0;
            open.insert(temp);
        }else{
            open.insert(i->second.first);
        }
    }
    
    
    if(open.begin()->id != testcompare.id){
        if(open.begin()->id == 0){
            cout<<"did not insert first item"<<endl;
        }else{
            cout<<"wrong comparison equation!"<<endl;
        }
    }
    
    size = 0;
    for(auto debug = open.begin();debug != open.end();debug++){
//        cout<<debug->id<<"  "<<debug->cost<<endl;
        size++;
    } 
    cout<<"size of the openset is "<<size<<endl;

    unsigned iteration = 0;
    unsigned counter;
//    while(counter < 300){
    while(!open.empty()){
        
//        if(iteration == 3){
//            return result;
//        }
//        if(open.begin()->visited == 10){
//            counter ++;
//            continue;
//        }
        counter = 0;
        auto current = open.begin();
        result.push_back(current->id);
        ham.push_back(make_pair(current->id,current->type));
        cout<<"current node:"<<current->id<<endl;
        priority_queue <prim,vector<prim>,compare> neighbors;       
        
        cout << ++iteration << endl;
        
        //make sure to note that this location has been visited in the pickup and dropoff map 
        if(current->type == 1){
            auto closed = pickup.find(current->id);
            if(closed != pickup.end()){
                closed->second.first.visited = 1;
            }else{
                cout<<"problem finding stuff in unordered map"<<endl;
            }
        }else if(current->type == 0){
            auto closed = dropoff.find(current->id);
            if(closed != dropoff.end()){
                closed->second.first.visited = 1;
            }else{
                cout<<"problem finding stuff in unordered map"<<endl;
            }
        }
        
        bool found = found_MST(pickup,dropoff);
        if(found){
            //highlighting the streets so I know what the hell is physically going on
            highlight_MST(pickup,dropoff);
            //does other stuff to convert MST to actual path
        }else{
            
            //push in the dropoff location according to the current pickup location
            if(current->type == 1){
                auto find = pickup.find(current->id);
                vector <unsigned> drop = find->second.second;
                for(auto addDropOff = drop.begin();addDropOff != drop.end();addDropOff++){
                    auto insert = dropoff.find(*addDropOff);
                    auto findopen = open.begin();
                    bool found = false;
                    for(findopen = open.begin(); findopen != open.end();findopen++){
                        if(findopen->id == *addDropOff){
                            found = true;
                            break;
                        }
                    }
                    if(find != dropoff.end()){
                        if(!found){
                            cout<<"insertion size:"<<open.size()<<endl;
                            open.insert(insert->second.first);
                            cout<<"insertion size:"<<open.size()<<endl;
                        }else{
                            cout<<"drop off already exists in the open set"<<endl;
                        }
                        
                    }else{
                        cout<<"dude dropoff is somehow out of bounds"<<endl;
                    }        
                }
            }else{
                cout<<"type is weird"<<endl;
            }

//            size = 0;
//            prim bye(10000,0,UINT_MAX,false,0);
            for(auto openset = open.begin();openset != open.end();openset++){
                if(openset == open.begin()) {
                    continue;
                }

                if(current->id == openset->id){
                    cout << "same node" << endl;
                    cout << current->cost << "," << openset->cost << endl;
//                    bye = *openset;
                    continue;
                }
                
                LatLon point1 = latLonIntersection[current->id];
                LatLon point2 = latLonIntersection[openset->id];
                double distance = find_distance_between_two_points(point1,point2);

    //            if(openset->cost > distance){
                prim temp = *openset;
                temp.cost = distance;
                neighbors.push(temp); 
    //            }
                size++;
            }
//            if(bye.cost != 10000){
//                open.erase(bye);
//            }
            cout<<"size of the openset is "<<open.size()<<endl;
            
            double range = 0;
            priority_queue <prim,vector<prim>,compare> check;
            
            
            
            if(!neighbors.empty()){
                cout << neighbors.top().id << "," << neighbors.top().cost << endl;
                range = 1.5 * neighbors.top().cost;
                cout << range << endl;
            }
            
            
            
            while(!neighbors.empty() && neighbors.top().cost < range){

                prim temp = neighbors.top();
                double prev_cost = temp.cost;
                check.push(temp);
                cout << "calculate the cost of possible solutions" << endl;

                vector<unsigned> path = find_path_between_intersections(temp.id,current->id,turn_penalty);
                double cost = compute_path_travel_time(path,turn_penalty);

                temp.cost = cost;
                temp.parent.push_back(current->id);
                temp.path = path;
                cout << "node:" <<temp.id<<" cost:"<<cost<<endl;
                check.push(temp);

                neighbors.pop();
                
            }
            
            if(neighbors.empty())
                cout << "NEIGHBORS EMPTY";
            else
                cout << neighbors.top().id<<endl;
            
            if(!neighbors.empty()){
                if(check.empty())
                    cout << "its empty man" << endl;
                prim theNode = check.top();
                cout<<"The optimal cost:"<<theNode.cost<<" node:"<<theNode.id<<endl;
                unsigned parent;
                parent = theNode.parent.push_back(parent);
                if(pickup.find(parent)!=pickup.end()){
                    auto child = pickup.find(parent);
                    child->second.first.child.push_back(theNode.id);
                }else if(dropoff.find(parent)!= dropoff.end()){
                    auto child = dropoff.find(parent);
                    child->second.first.child.push_back(theNode.id);
                }else{
                    cout<<"could not find parent"<<endl;
                }
                if(theNode.type == 1){
                    auto iterator = pickup.find(theNode.id);
                    auto findopen = open.begin();
                    bool found = false;
                    for(findopen = open.begin(); findopen != open.end();findopen++){
                        if(findopen->id == theNode.id){
                            found = true;
                            break;
                        }
                        
                    }
                    if(found){
                        if(iterator != pickup.end()){
                            if(findopen->cost > theNode.cost){

                                //insert into open
                                open.erase(*findopen);
                                open.insert(theNode);
                                
                            }
                                //update pickup unordered map
                                iterator->second.first = theNode;
                        }else{
                            cout<<"unordered map search problem"<<endl;
                        }
                    }else{
                        cout<<"set is fucked up"<<endl;
                        for(auto debug = open.begin();debug != open.end();debug++){
                            cout<<debug->id<<"  "<<debug->cost<<endl;
                        }
                    }
                }else if(theNode.type == 0){
                    auto iterator = dropoff.find(theNode.id);
                    auto findopen = open.begin();
                    bool found = false;
                    for(findopen = open.begin(); findopen != open.end();findopen++){
                        if(findopen->id == theNode.id){
                            found = true;
                            break;
                        }
                        
                    }
                    if(found){
                        if(iterator != pickup.end()){
                            if(findopen->cost > theNode.cost){

                                //insert into open
                                open.erase(*findopen);
                                open.insert(theNode);
                            }
                                //update pickup unordered map
                                iterator->second.first = theNode;
                        }else{
                            cout<<"unordered map search problem"<<endl;
                        }
                    }else{
                        cout<<"set is fucked up"<<endl;
                        for(auto debug = open.begin();debug != open.end();debug++){
                            cout<<debug->id<<"  "<<debug->cost<<endl;
                        }
                    }
                }else{
                    cout<<"its fucked up"<<endl;
                }
            }

            //take current off the open list     
        }
//        prim store(current->id,1000000000000000-iteration,current->parent,10,current->type);
        open.erase(current);
//        open.insert(store);
        cout<<endl;
    }
    
    
    cout << "exists properly" << endl;
    return result;
}

bool found_MST(const unordered_map< unsigned,pair< prim,vector<unsigned> > > pickup,
               const unordered_map< unsigned,pair< prim,vector<unsigned> > > dropoff){
    for(auto loop = pickup.begin();loop != pickup.end();loop++){
        if(loop->second.first.visited != true)
            return false;
            
    }
    for(auto loop = dropoff.begin();loop != dropoff.end();loop++){
        if(loop->second.first.visited != true)
            return false;
    }
    return true;
}

void highlight_MST(const unordered_map< unsigned,pair< prim,vector<unsigned> > > pickup,
                   const unordered_map< unsigned,pair< prim,vector<unsigned> > > dropoff){
    for(auto loop = pickup.begin();loop != pickup.end();loop++){
        unsigned first = loop->first;
        prim temp = loop->second.first;
        if(temp.cost != UINT_MAX){
//            unsigned second = temp.parent;
//            vector <unsigned> intersect;
//            intersect.push_back(first);
//            intersect.push_back(second);
//            vector <unsigned> streetSegs = return_streetSegID_from_intIDs(intersect);
            if(!loop->second.first.path.empty()){
                draw_MST(loop->second.first.path);
                cout<<"cost:"<<loop->second.first.path.size()<<endl;
                cout<<"it draws"<<endl;
            }else{
                cout<<"not drawing"<<endl;
            }
            
        }
    }
    for(auto loop = dropoff.begin();loop != dropoff.end();loop++){
        unsigned first = loop->first;
        prim temp = loop->second.first;
        if(temp.parent != UINT_MAX){
//            unsigned second = temp.parent;
//            vector <unsigned> intersect;
//            intersect.push_back(first);
//            intersect.push_back(second);
//            vector <unsigned> streetSegs = return_streetSegID_from_intIDs(intersect);
            if(!loop->second.first.path.empty()){
                draw_MST(loop->second.first.path);
                cout<<"cost:"<<loop->second.first.path.size()<<endl;
                cout<<"it draws"<<endl;
            }else{
                cout<<"not drawing"<<endl;
            }
        }     
    }
}

vector<unsigned> hamiltonian(const vector< pair<unsigned,unsigned> > result,
                             const unordered_map< unsigned,pair< prim,vector<unsigned> > > pickup,
                             const unordered_map< unsigned,pair< prim,vector<unsigned> > > dropoff){
//    vector <unsigned> path;
//    for(auto itr = result.begin();itr != result.end();itr++){
//        itr++;
//            if(itr->second.first.child == itr++){
//                if( itr->second == 1 ){
//                unsigned node = itr->first;
//                auto populate = pickup.find(node);
//                if(populate->second.first.parent != UINT_MAX){
//                    if(populate != pickup.end() && populate->second.first.parent != UINT_MAX)
//                        path.insert(path.end(),populate->second.first.path.begin(),populate->second.first.path.begin());
//                }else{
//
//                }
//            }else if( itr->second == 0 ){
//                unsigned node = itr->first;
//                auto populate = dropoff.find(node);
//                if(populate->second.first.parent != UINT_MAX){
//                    if(populate != pickup.end() && populate->second.first.parent != UINT_MAX)
//                        path.insert(path.end(),populate->second.first.path.begin(),populate->second.first.path.begin());
//                }else{
//
//                }
//            }
//        }
//        
//
//    }
    
    
    
    return path;
}