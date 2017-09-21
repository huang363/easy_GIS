#include "m1.h"
#include "m3.h"
#include "StreetsDatabaseAPI.h"
#include "../inc/path_finding_struct.h"
#include "../inc/KDMap.h"
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>
#include <queue>
#include <utility> 
#include <iostream>
#include <climits>
#include <cfloat>
#include <set>
#include <unordered_map>

//#define DEBUGME
#define VERBOSE
#define RELEASE

using namespace std;


extern unsigned numIntersections;
extern vector <string> streetNames;
extern vector <StreetSegmentInfo> searchByStreetSeg;
extern vector <LatLon> latLonIntersection;
extern unordered_multimap <string,unsigned> POINames;
extern vector <LatLon> latLonPointOfInterest;
extern vector < vector<int_cost> > searchDataStruct;
extern vector <double>streetSegTravelTime;

double compute_path_travel_time(const vector<unsigned>& path,const double turn_penalty){
    double time = 0;

    unsigned id1;
    unsigned id2;
    
    if(path.size()== 0){
        return 0;
    }else{
        time = streetSegTravelTime[path.front()];
        for(auto itr = path.begin()+1;itr<path.end();itr++){
            time += streetSegTravelTime[*(itr)];
            
            if(itr != path.begin()){
                id1 = searchByStreetSeg[*(itr-1)].streetID;
                id2 = searchByStreetSeg[*(itr)].streetID;
                if(id1 != id2){
                    time += turn_penalty;
                }
            }       
        }
    }
    return time;
}

vector<unsigned> find_path_between_intersections(const unsigned intersect_id_start,
                                                 const unsigned intersect_id_end,
                                                 const double turn_penalty){
    
    //open and closed set definitions
    vector<bool> closed(numIntersections,false);
    multiset<imspooked> open;

    //graph definition
    vector<imspooked> graph;
    graph.resize(numIntersections);

    //containers for use in algorithm
    vector<int_cost> adjacent_nodes;
    imspooked curr_node;

    //insert starting node into open set
    imspooked start_node(intersect_id_start,UINT_MAX,UINT_MAX,0);
    open.insert(start_node);
    
    while(!open.empty()){
                
        //pop prioritized node
        curr_node = *open.begin();
        open.erase(open.begin());
        
        //final node visited => path found
        if(closed[intersect_id_end] == true){

            vector<unsigned> result;

            curr_node = graph[intersect_id_end];
            while(curr_node.from_edge != UINT_MAX){
                result.insert(result.begin(),curr_node.from_edge);
                curr_node = graph[curr_node.from_node];
            }

            return result;
        }

        if(closed[curr_node.id] != true){ //if intersection not yet visited
            
            //set visited to true and update graph
            closed[curr_node.id] = true;
            graph[curr_node.id] = curr_node;

            //find adjacent intersection info
            adjacent_nodes = searchDataStruct[curr_node.id];
            //iterate through adjacent intersections and process
            for(auto next_node = adjacent_nodes.begin(); next_node != adjacent_nodes.end(); next_node++){
                
                //calculate cost to the next
                double cost = curr_node.cost + next_node->cost;
                if(curr_node.id != intersect_id_start)
                    cost += turn_penalty*check_turn_penalty(curr_node.from_edge,next_node->streetsegid);

                imspooked new_node(next_node->intersectionid,curr_node.id,next_node->streetsegid,cost);
                open.insert(new_node);
                
            } //end iterating through adjacent intersections

        } //if already visited, do nothing

    } //priority queue empty, exit while
    
    //path not found, return empty vector
    vector<unsigned> result;
    return result;

}

vector<unsigned> find_path_to_point_of_interest(const unsigned intersection_id_start,
                                                const string point_of_interest_name,
                                                const double turn_penalty){
    

    auto ids = POINames.equal_range(point_of_interest_name);
    double cost = DBL_MAX;
    struct stuff{bool operator()(const pair<unsigned,double>& a, const pair<unsigned,double>& b)const{return a.second > b.second;}};
    priority_queue < pair < unsigned,double >, vector < pair < unsigned,double > >, stuff > compare;
    vector <unsigned> result;

    LatLon position = latLonIntersection[intersection_id_start];
    for(auto itr = ids.first;itr != ids.second;itr++){
        unsigned POI_id = itr->second;
        LatLon position_poi = latLonPointOfInterest[POI_id];
        double distance = find_distance_between_two_points(position_poi, position);
        compare.push(make_pair(POI_id,distance));
    }
    
    unsigned checks = 10;
    while(checks != 0){
        unsigned current = compare.top().first;
        LatLon position_poi = latLonPointOfInterest[current];
        unsigned intersection_id_end = find_closest_intersection(position_poi);
        vector <unsigned> adj_intersect = find_adjacent_intersections(intersection_id_end);
        unsigned intersection = UINT_MAX;
        double distance = DBL_MAX;
        double adj;
        for(auto itr = adj_intersect.begin();itr!=adj_intersect.end();itr++){
            adj = find_distance_between_two_points(position,latLonIntersection[*itr]);
            if(distance>adj){
                distance = adj;
                intersection = *itr;
            }
        }
        if(intersection != UINT_MAX){
            distance = find_distance_between_two_points(latLonIntersection[intersection],position_poi);
            adj = find_distance_between_two_points(latLonIntersection[intersection_id_end],position_poi);
        }
        
        if(distance<adj){
            intersection_id_end = intersection;
        }
        vector<unsigned> option = find_path_between_intersections(intersection_id_start,intersection_id_end,turn_penalty);
        double temp_cost = compute_path_travel_time(option,turn_penalty);
        
        if(option.size() != 0){
            if(temp_cost<cost){
                result = option;
                cost = temp_cost;
            }
        }
        compare.pop();
        checks--;
    }

    return result;
    
}

double heuristic1(unsigned id1,unsigned id2){
    LatLon current = latLonIntersection[id1];
    LatLon destination = latLonIntersection[id2];
    return find_distance_between_two_points(current, destination);
}

bool check_turn_penalty(unsigned prev_seg,unsigned next_seg){
    StreetSegmentInfo seg1 = searchByStreetSeg[prev_seg];
    StreetSegmentInfo seg2 = searchByStreetSeg[next_seg];
    
    if(seg1.streetID == seg2.streetID){
        return false;
    }
    else{
        return true;
    }
}