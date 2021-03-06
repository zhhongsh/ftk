#include <ftk/hypermesh/regular_simplex_mesh.hh>
#include <ftk/basic/distributed_union_find.hh>

#ifndef DIM
  #define DIM 3
#endif

struct intersection_t {
  template <class Archive> void serialize(Archive & ar) {
    ar(eid, x[0], x[1], x[2], val);
  }

  // std::vector<float> x; // the spacetime coordinates of the trajectory
  float x[DIM];

  // std::vector<float> corner; // the spacetime coordinates of the left corner of the element

  float val; // scalar value at the intersection
  
  std::string eid; // element id
  std::set<std::string> related_elements; 
};

namespace diy
{
  template<>
  struct Serialization<intersection_t>
  {
      static void save(BinaryBuffer& bb, const intersection_t& msg)
      {
          diy::save(bb, msg.x);
          // diy::save(bb, msg.corner);
          diy::save(bb, msg.val);
          diy::save(bb, msg.eid);
          diy::save(bb, msg.related_elements);
      }

      static void load(BinaryBuffer& bb, intersection_t& msg)
      {
          diy::load(bb, msg.x);
          // diy::load(bb, msg.corner);
          diy::load(bb, msg.val);
          diy::load(bb, msg.eid);
          diy::load(bb, msg.related_elements);
      }
  };
}

// template<int DIM>
struct point_t{

  int&  operator[](unsigned i)                          { return corner[i]; }
  int   operator[](unsigned i) const                    { return corner[i]; }

  int corner[DIM]; // the spacetime coordinates of the left corner of the element
  // std::vector<int> corner; 

  // point_t(int DIM): corner(DIM) {}
};

namespace diy
{
  template<>
  struct Serialization<point_t>
  {
      static void save(BinaryBuffer& bb, const point_t& msg)
      {
          diy::save(bb, msg.corner);
      }

      static void load(BinaryBuffer& bb, point_t& msg)
      {
          diy::load(bb, msg.corner);
      }
  };
}

// ==========================================================

// DIY Block for distributed critical point tracking
struct Block_Critical_Point : public Block_Union_Find {
  Block_Critical_Point() : intersections(), Block_Union_Find() {

  }


  void get_sets(diy::mpi::communicator& world, diy::Master& master, diy::ContiguousAssigner& assigner, std::vector<std::set<std::string>>& results);

public:
  std::map<std::string, intersection_t> intersections; // Points on trajectories of critical points
  
  // Data for load balancing
  std::vector<intersection_t> features; 
  std::vector<point_t> points; 

  std::vector<diy::ContinuousBounds>   block_bounds;                       // all block bounds

};

// ==========================================================

struct Message_Critical_Point : public Message_Union_Find {
  Message_Critical_Point() {

  }

  void send_intersection(const std::pair<std::string, intersection_t>& pair) {
    tag = "intersection"; 

    strs.push_back(pair.first); 

    auto& I = pair.second; 
    
    strs.push_back(I.eid); 
    strs.push_back(std::to_string(I.val)); 
    
    // strs.push_back(std::to_string(I.x[0])); 
    // strs.push_back(std::to_string(I.x[1])); 
    // strs.push_back(std::to_string(I.x[2])); 

    // for(int i = 0; i < I.x.size(); ++i) {
    //   strs.push_back(std::to_string(I.x[i])); 
    // }

    for(int i = 0; i < DIM; ++i) {
      strs.push_back(std::to_string(I.x[i])); 
    }
  }

  void receive_intersection(std::pair<std::string, intersection_t>& pair) {
    pair.first = strs[0]; 
    
    intersection_t& I = pair.second; 
    
    I.eid = strs[1];
    I.val = std::stof(strs[2]); 

    // I.x[0] = std::stof(strs[3]); 
    // I.x[1] = std::stof(strs[4]); 
    // I.x[2] = std::stof(strs[5]); 

    for(int i = 3; i < strs.size(); ++i) {
      // I.x.push_back(std::stof(strs[i])); 
      I.x[i-3] = std::stof(strs[i]); 
    }
  }

};

namespace diy
{
  template<>
  struct Serialization<Message_Critical_Point>
  {
      static void save(BinaryBuffer& bb, const Message_Critical_Point& msg)
      {
          diy::save(bb, msg.tag);
          // diy::save(bb, msg.str);
          diy::save(bb, msg.strs);
      }

      static void load(BinaryBuffer& bb, Message_Critical_Point& msg)
      {
          diy::load(bb, msg.tag);
          // diy::load(bb, msg.str);
          diy::load(bb, msg.strs);
      }
  };
}

// ==========================================================


// Generate sets of elements




void send_to_target_processes(Block_Critical_Point* b, diy::Master& master, diy::ContiguousAssigner& assigner, std::vector<std::string>& eles_to_send, std::vector<int>& target_gids) {
  diy::all_to_all(master, assigner, [&](Block_Critical_Point* b, const diy::ReduceProxy& srp) {
    // Block_Critical_Point* b = static_cast<Block_Critical_Point*>(_b);
    if (srp.round() == 0) {
      diy::RegularContinuousLink* link = static_cast<diy::RegularContinuousLink*>(srp.master()->link(srp.master()->lid(srp.gid())));

      for(int i = 0; i < eles_to_send.size(); ++i) {
        auto& ele = eles_to_send[i];
        auto& target_gid = target_gids[i]; 

        std::string root = b->parent(ele); 
        std::pair<std::string, std::string> local_pair(ele, root); 

        srp.enqueue(srp.out_link().target(target_gid), local_pair);
      }

    } else {
      std::vector<int> in; // gids of incoming neighbors in the link
      srp.incoming(in);

      // for all neighbor blocks
      // dequeue data received from this neighbor block in the last exchange
      for (unsigned i = 0; i < in.size(); ++i) {
        while(srp.incoming(in[i])) {
          std::pair<std::string, std::string> local_pair; 
          srp.dequeue(in[i], local_pair);

          b->add(local_pair.first); 
          b->set_parent(local_pair.first, local_pair.second);           
        }
      }
    }
  });


  diy::all_to_all(master, assigner, [&](Block_Critical_Point* b, const diy::ReduceProxy& srp) {
    // Block_Critical_Point* b = static_cast<Block_Critical_Point*>(_b);
    if (srp.round() == 0) {
      diy::RegularContinuousLink* link = static_cast<diy::RegularContinuousLink*>(srp.master()->link(srp.master()->lid(srp.gid())));

      for(int i = 0; i < eles_to_send.size(); ++i) {
        auto& ele = eles_to_send[i];
        auto& target_gid = target_gids[i]; 

        intersection_t& intersection = b->intersections.find(ele)->second;
        intersection.related_elements.clear(); 
        srp.enqueue(srp.out_link().target(target_gid), intersection);
      }

    } else {
      std::vector<int> in; // gids of incoming neighbors in the link
      srp.incoming(in);

      // for all neighbor blocks
      // dequeue data received from this neighbor block in the last exchange
      for (unsigned i = 0; i < in.size(); ++i) {
        while(srp.incoming(in[i])) {
          intersection_t intersection; 
          srp.dequeue(in[i], intersection);

          b->intersections.insert(std::make_pair(intersection.eid, intersection));      
        }
      }
    }
  });

  // Erase elements that have sent
  for(auto& ele : eles_to_send) {
    b->eles.erase(ele); 
  }
}



// Method 1:
  // Send to p0

// Get sets of elements
void get_sets_on_p0(Block_Critical_Point* b, diy::mpi::communicator& world, diy::Master& master, diy::ContiguousAssigner& assigner, std::vector<std::string>& eles_to_send, std::vector<int>& target_gids) {
  int target_gid = 0;
  master.foreach([&](Block_Critical_Point* b, const diy::Master::ProxyWithLink& cp) {
    int gid = cp.gid(); 

    for(auto& ele : b->eles) {
      std::string root = b->parent(ele); 

      if(target_gid != gid) {
        eles_to_send.push_back(ele); 
        target_gids.push_back(target_gid); 
      }
    }
  }); 
}


// Method 2:
// Gather all element-root information to the process of the root
  // Since the roots have all children, can directly use the information to reconstruct the sets; the only lacking part is the intersection. 
  // Step one: send to root
  // Step two: gather on root

// Get sets of elements on processes of roots
void get_sets_on_roots(Block_Critical_Point* b, diy::mpi::communicator& world, diy::Master& master, diy::ContiguousAssigner& assigner, std::vector<std::string>& eles_to_send, std::vector<int>& target_gids) {
  master.foreach([&](Block_Critical_Point* b, const diy::Master::ProxyWithLink& cp) {
    int gid = cp.gid(); 

    for(auto& ele : b->eles) {
      std::string root = b->parent(ele); 
      int gid_root = b->get_gid(root); 

      if(gid_root != gid) {
        eles_to_send.push_back(ele); 
        target_gids.push_back(gid_root); 
      }
    }
  }); 
}


// Method 3:
// Gather all element-root information to the process of the root
  // Step one: send to redistributed processes
  // Step two: gather on redistributed processes

// Get sets of elements by redistributing data
void get_sets_redistributed(Block_Critical_Point* b, diy::mpi::communicator& world, diy::Master& master, diy::ContiguousAssigner& assigner, std::vector<std::string>& eles_to_send, std::vector<int>& target_gids) {
  int nblocks = world.size();
  master.foreach([&](Block_Critical_Point* b, const diy::Master::ProxyWithLink& cp) {
    int gid = cp.gid(); 

    for(auto& ele : b->eles) {
      std::string root = b->parent(ele); 
      int gid_root = hash_string(root, nblocks); 

      if(gid_root != gid) {
        eles_to_send.push_back(ele); 
        target_gids.push_back(gid_root); 
      }
    }
  }); 
}

// Get sets of elements
inline void Block_Critical_Point::get_sets(diy::mpi::communicator& world, diy::Master& master, diy::ContiguousAssigner& assigner, std::vector<std::set<std::string>>& results) {

  std::vector<std::string> eles_to_send ;
  std::vector<int> target_gids;

  // get_sets_on_p0(this, world, master, assigner, eles_to_send, target_gids); 
  get_sets_on_roots(this, world, master, assigner, eles_to_send, target_gids); 
  // get_sets_redistributed(this, world, master, assigner, eles_to_send, target_gids); 


  // ===============================

  Block_Critical_Point* b = this;
  send_to_target_processes(b, master, assigner, eles_to_send, target_gids); 

  std::map<std::string, std::set<std::string>> root2set; 
  for(auto& ele : b->eles) {
    assert(b->is_root(b->parent(ele))); 

    root2set[b->parent(ele)].insert(ele);  
  }

  for(auto ite = root2set.begin(); ite != root2set.end(); ++ite) {
    results.push_back(ite->second);   
  }

  #if ISDEBUG
    for(int i = 0; i < results.size(); ++i) {
      std::cout<<"Set "<<i<<":"<<std::endl; 
      std::set<std::string>& ele_set = results[i]; 
      for(auto& ele : ele_set) {
        std::cout<<ele<<" "; 
      }
      std::cout<<std::endl; 
    }
  #endif
}

// ===================================================================
// Functions for adding edges/unions to the block

bool is_in_mesh(const ftk::regular_simplex_mesh_element& f, const ftk::lattice& _lattice) { 
  // If the corner of the face is contained by the core lattice _lattice, we consider the element belongs to _lattice

  for (int i = 0; i < f.corner.size(); ++i){
    if (f.corner[i] < _lattice.start(i) || f.corner[i] > _lattice.upper_bound(i)) {
      return false; 
    }
  }

  return true;
}

bool is_in_mesh(const std::string& eid, const ftk::lattice& _lattice, ftk::regular_simplex_mesh& m, int feature_dim) { 
  ftk::regular_simplex_mesh_element f = ftk::regular_simplex_mesh_element(m, feature_dim, eid); 
  
  return is_in_mesh(f, _lattice); 
}

  // If the intersection is contained in the lattice
bool is_in_mesh(const intersection_t& intersection, const ftk::lattice& _lattice, ftk::regular_simplex_mesh& m, int feature_dim) { 
  return is_in_mesh(intersection.eid, _lattice, m, feature_dim); 
}

bool is_in_mesh(ftk::regular_simplex_mesh_element& f, diy::ContinuousBounds& block_bound) {
  for (int i = 0; i < f.corner.size(); ++i){
    if(f.corner[i] < block_bound.min[i] || f.corner[i] > block_bound.max[i]) {
       return false;
    }
  }

  return true;
}


// Add union edges to the block
void add_unions(std::map<std::string, intersection_t>* intersections, ftk::regular_simplex_mesh& block_m, const ftk::regular_simplex_mesh_element& f) {
  if (!f.valid()) return; // check if the 3-simplex is valid

  const auto elements = f.sides();
  std::set<std::string> features;
  std::set<std::string> features_in_block;  
  std::map<std::string, ftk::regular_simplex_mesh_element> id2element; 

  for (const auto& ele : elements) {
    std::string eid = ele.to_string(); 
    if(intersections->find(eid) != intersections->end()) {
      features.insert(eid); 
      id2element.insert(std::make_pair(eid, ele)); 

      if(is_in_mesh(ele, block_m.get_lattice())) {
        features_in_block.insert(eid); 
      }
      
    }
  }

  if(features.size()  > 1) {
    if(features_in_block.size() > 1) {
      // When features are local, we just need to relate to the first feature element

      #if MULTITHREAD
        std::lock_guard<std::mutex> guard(mutex);
      #endif

      std::string first_feature = *(features_in_block.begin()); 
      for(std::set<std::string>::iterator ite_i = std::next(features_in_block.begin(), 1); ite_i != features_in_block.end(); ++ite_i) {
        std::string curr_feature = *ite_i; 

        if(first_feature < curr_feature) { // Since only when the id of related_ele < ele, then the related_ele can be the parent of ele
          intersections->find(curr_feature)->second.related_elements.insert(first_feature); 
        } else {
          intersections->find(first_feature)->second.related_elements.insert(curr_feature); 
        }
        
      }
    }

    if(features_in_block.size() == 0 || features.size() == features_in_block.size()) {
      return ;
    }
  
    // When features are across processors, we need to relate all local feature elements to all remote feature elements
    for(auto& feature: features) {
      if(features_in_block.find(feature) == features_in_block.end()) { // if the feature is not in the block
        #if MULTITHREAD 
          std::lock_guard<std::mutex> guard(mutex); // Use a lock for thread-save. 
        #endif

        for(auto& feature_in_block : features_in_block) {

          if(feature < feature_in_block) { // When across processes, also, since only when the id of related_ele < ele, then the related_ele can be the parent of ele
            intersections->find(feature_in_block)->second.related_elements.insert(feature); 
          }

        }
      }
    }
  }
}

void add_related_elements_to_intersections(std::map<std::string, intersection_t>* intersections, ftk::regular_simplex_mesh& m, ftk::regular_simplex_mesh& block_m, int feature_dim) {
    // std::cout<<"Start Adding Elements to Blocks: "<<world.rank()<<std::endl; 

  std::vector<ftk::regular_simplex_mesh_element> eles_with_intersections;
  for(auto& pair : *intersections) {
    auto& eid = pair.first;
    // ftk::regular_simplex_mesh_element f = ftk::regular_simplex_mesh_element(m, 0, eid); 
    auto&f = eles_with_intersections.emplace_back(m, feature_dim, eid); 
  }

  // std::cout<<"Finish Adding Elements to Blocks: "<<world.rank()<<std::endl; 

  // Connected Component Labeling by using union-find. 

  // std::cout<<"Start Adding Union Operations of Elements to Blocks: "<<world.rank()<<std::endl; 

  // // Method one:
  //   // For dense critical points
  //   // Enumerate each 3-d element to connect 2-d faces that contain critical points  
  // _m_ghost.element_for(3, add_unions, nthreads); 

  // Method two:
    // For sparse critical points
    // Enumerate all critical points, find their higher-order geometry; to connect critical points in this higher-order geometry
  std::set<std::string> visited_hypercells;
  for(auto& e : eles_with_intersections) { 
    const auto hypercells = e.side_of();
    for(auto& hypercell : hypercells) {
      std::string id_hypercell = hypercell.to_string(); 
      if(visited_hypercells.find(id_hypercell) == visited_hypercells.end()) {
        visited_hypercells.insert(id_hypercell); 
        add_unions(intersections, block_m, hypercell); 
      }
    }
  }

  // std::cout<<"Finish Adding Union Operations of Elements to Blocks: "<<world.rank()<<std::endl; 
}


// Note: will eliminate features of intersections in ghost cells
void add_points_to_block(ftk::regular_simplex_mesh& m, ftk::regular_simplex_mesh& block_m, Block_Critical_Point* b, int feature_dim) {
  // std::vector<intersection_t> points; 
  // int DIM = m.nd(); 

  // for(auto& intersection : b->intersections.begin();) {
  auto ite = b->intersections.begin();
  while(ite != b->intersections.end()) {
    auto& intersection = (*ite); 

    ftk::regular_simplex_mesh_element f = ftk::regular_simplex_mesh_element(m, feature_dim, intersection.first);

    if(is_in_mesh(f, block_m.get_lattice())) {
      point_t point;
      // point.corner.resize(DIM); 
      for(int i = 0; i < DIM; ++i) {
        point.corner[i] = f.corner[i];
      }
      b->points.push_back(point); 

      ++ite;
    } else{
      ite = b->intersections.erase(ite);
    }
  }
}

// ================================================================
// Functions for load balancing


void load_balancing_resize_bounds(diy::mpi::communicator& world, diy::Master& master, diy::ContiguousAssigner& assigner, ftk::regular_simplex_mesh& m, int gid, Block_Critical_Point* b) {
  // int DIM = m.nd(); 

  bool wrap = false; 
  int hist = 128; //32; 512
  
  diy::ContinuousBounds domain(DIM);
  for(int i = 0; i < DIM; ++i) {
    domain.min[i] = m.lb(i); domain.max[i] = m.ub(i); 
  }

  for(int i = 0; i < DIM; ++i) {
    hist = std::min(hist, (int)(domain.max[i] - domain.min[i] + 1)); 
  }

  diy::RegularContinuousLink* link = new diy::RegularContinuousLink(DIM, domain, domain);
  master.add(gid, b, link); 

  diy::kdtree<Block_Critical_Point, point_t>(master, assigner, DIM, domain, &Block_Critical_Point::points, 2*hist, wrap);
      // For weighted kdtree, look at kdtree.hpp diy::detail::KDTreePartition<Block,Point>::compute_local_histogram, pass and weights along with particles

  // Everybody sends their bounds to everybody else
  diy::all_to_all(master, assigner, [&](Block_Critical_Point* b, const diy::ReduceProxy& srp) {
    // Block_Critical_Point* b = static_cast<Block_Critical_Point*>(_b);
    if (srp.round() == 0) {
      diy::RegularContinuousLink* link = static_cast<diy::RegularContinuousLink*>(srp.master()->link(srp.master()->lid(srp.gid())));
      for (int i = 0; i < world.size(); ++i) {
        srp.enqueue(srp.out_link().target(i), link->bounds());
      }
    } else {
      b->block_bounds.resize(srp.in_link().size());
      for (int i = 0; i < srp.in_link().size(); ++i) {
        int _gid = srp.in_link().target(i).gid;

        assert(i == _gid);

        srp.dequeue(_gid, b->block_bounds[_gid]);
      }
    }
  });
}

void load_balancing_redistribute_data(diy::mpi::communicator& world, diy::Master& master, diy::ContiguousAssigner& assigner, ftk::regular_simplex_mesh& m, int gid, Block_Critical_Point* b, int feature_dim) {
  diy::all_to_all(master, assigner, [&](Block_Critical_Point* b, const diy::ReduceProxy& srp) {
    // Block_Critical_Point* b = static_cast<Block_Critical_Point*>(_b);
    if (srp.round() == 0) {
      diy::RegularContinuousLink* link = static_cast<diy::RegularContinuousLink*>(srp.master()->link(srp.master()->lid(srp.gid())));

      for(auto& intersection : b->intersections) {
        ftk::regular_simplex_mesh_element f = ftk::regular_simplex_mesh_element(m, feature_dim, intersection.first);

        // if this point is within multiple processes' ranges; probably on a boundary, select the largest gid
        int target_gid = -1;
        for(int rgid = world.size() - 1; rgid >= 0; --rgid) {
          if(is_in_mesh(f, b->block_bounds[rgid])) {
            target_gid = rgid; 
            break ;
          }
        }

        if(target_gid == gid) {
          b->features.push_back(intersection.second);   
        } else {
          srp.enqueue(srp.out_link().target(target_gid), intersection.second);
        }
      }

    } else {
      std::vector<int> in; // gids of incoming neighbors in the link
      srp.incoming(in);

      // for all neighbor blocks
      // dequeue data received from this neighbor block in the last exchange
      for (unsigned i = 0; i < in.size(); ++i) {
        while(srp.incoming(in[i])) {
          intersection_t intersection; 
          srp.dequeue(in[i], intersection);

          b->features.push_back(intersection); 
        }
      }
    }
  });
}


void init_block_after_load_balancing(diy::mpi::communicator& world, diy::Master& master, diy::ContiguousAssigner& assigner, ftk::regular_simplex_mesh& m, int gid, Block_Critical_Point* b, int feature_dim) {
  // int DIM = m.nd(); 
  // , diy::RegularContinuousLink* link

  b->intersections.clear(); 
  for(auto feature : b->features) {
    b->intersections.insert(std::make_pair(feature.eid, feature)); 

    b->add(feature.eid); 
    b->set_gid(feature.eid, gid);
  }

  for(auto& feature : b->features) {
    // std::cout<<p.related_elements.size()<<std::endl;

    for(auto& related_ele : feature.related_elements) {
      // std::cout<<related_ele<<std::endl;

      b->add_related_element(feature.eid, related_ele); 

      ftk::regular_simplex_mesh_element f = ftk::regular_simplex_mesh_element(m, feature_dim, related_ele);

      if(!b->has_gid(related_ele)) { // If the block id of this feature is unknown, search the block id of this feature

        // if this point is within multiple processes' ranges; probably on a boundary, select the largest gid
        for(int rgid = b->block_bounds.size() - 1; rgid >= 0; --rgid) {
          if(rgid == gid) {
            continue;
          }

          if(is_in_mesh(f, b->block_bounds[rgid])) {
            b->set_gid(related_ele, rgid);
            break ;
          }
        }

        assert(b->has_gid(related_ele)); // If not, Error! Cannot find the gid of the related element!
        // if(!b->has_gid(related_ele)) {
        //   std::cout<<"Error! Cannot find the gid of the related element! "<<std::endl;
        //   exit(0);
        // }

      }

    }
  }
}

void init_block_without_load_balancing(std::vector<std::tuple<ftk::lattice, ftk::lattice>>& lattice_partitions, ftk::regular_simplex_mesh& m, int gid, Block_Critical_Point* b, int feature_dim) {

  for(auto& intersection : b->intersections) {
    ftk::regular_simplex_mesh_element f = ftk::regular_simplex_mesh_element(m, feature_dim, intersection.first);
    auto& _lattice = std::get<0>(lattice_partitions[gid]); 
    if(is_in_mesh(f, _lattice)) { // the feature is in this partition
      b->features.push_back(intersection.second);   
    }
  }

  b->intersections.clear(); // *
  for(auto feature : b->features) {
    b->intersections.insert(std::make_pair(feature.eid, feature)); // *

    b->add(feature.eid); 
    b->set_gid(feature.eid, gid);
  }

  for(auto& feature : b->features) {
    // std::cout<<p.related_elements.size()<<std::endl;

    for(auto& related_ele : feature.related_elements) {
      // std::cout<<related_ele<<std::endl;

      b->add_related_element(feature.eid, related_ele); 

      ftk::regular_simplex_mesh_element f = ftk::regular_simplex_mesh_element(m, feature_dim, related_ele);

      if(!b->has_gid(related_ele)) { // If the block id of this feature is unknown, search the block id of this feature
        for(int i = 0; i < lattice_partitions.size(); ++i) {
          int rgid = i;
          if(rgid == gid) { // We know the feature is not in this partition
            continue;
          }

          auto& _lattice = std::get<0>(lattice_partitions[i]); 
          if(is_in_mesh(f, _lattice)) { // the feature is in mith partition
            b->set_gid(related_ele, rgid); // Set gid of this feature to mi  
          }

        }
      }

    }
  }
}

