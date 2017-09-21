#ifndef PATH_FINDING_STRUCT_H
#define PATH_FINDING_STRUCT_H

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <utility>
#include <iostream>
#include <climits>
#include <cfloat>
    
using namespace std;

struct int_cost{
    double cost;
    unsigned streetsegid;
    unsigned intersectionid;
};

struct imspooked {
    unsigned id;
    unsigned from_node;
    unsigned from_edge;
    double cost;
    imspooked(){id = UINT_MAX; from_node = UINT_MAX; from_edge = UINT_MAX; cost = DBL_MAX;}
    imspooked(unsigned _id, unsigned _from_node, unsigned _from_edge, double _cost)
    {id = _id; from_node = _from_node; from_edge = _from_edge; cost = _cost;}
    bool operator<(const imspooked& rhs)const{return (cost < rhs.cost);}
};

struct prim {
    unsigned id;
    double cost;
    vector<unsigned> parent;
    unsigned visited;
    unsigned type;
    vector<unsigned> child;
    vector<unsigned> path;
    prim(unsigned _id, double _cost, unsigned _visited, unsigned _type){
        id = _id;
        cost = _cost;
        parent = _parent;
        visited = _visited;
        type = _type;
    }
    bool operator<(const prim& a)const{
//        if(id == a.id){
//            cout << "using id to compare, YAY" << endl;
//            return false;
//        }
//        else if(cost < a.cost)  return true;
//
//        return false;
        return (cost<a.cost);
    }
};

struct compare{
    bool operator()(const prim& lhs, const prim& rhs){
        return lhs.cost > rhs.cost;
    }
};


void populatePathFind();

double heuristic1(unsigned id1,unsigned id2);

vector<unsigned> return_streetSegID_from_intIDs(const vector<unsigned>& intersectionids);

bool check_all_street_segs_connected(const vector<unsigned>& street_segs);

bool check_turn_penalty(unsigned id1,unsigned id2);

bool found_MST(const unordered_map< unsigned,pair< prim,vector<unsigned> > > pickup,
               const unordered_map< unsigned,pair< prim,vector<unsigned> > > dropoff);

void highlight_MST(const unordered_map< unsigned,pair< prim,vector<unsigned> > > pickup,
                   const unordered_map< unsigned,pair< prim,vector<unsigned> > > dropoff);

void draw_MST(vector<unsigned> street_segs);

vector<unsigned> hamiltonian(const vector< pair<unsigned,unsigned> > result,
                 const unordered_map< unsigned,pair< prim,vector<unsigned> > > pickup,
                 const unordered_map< unsigned,pair< prim,vector<unsigned> > > dropoff);

#endif /* PATH_FINDING_STRUCT_H */

