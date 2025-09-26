#include <algorithm>
#include <functional>
#include "hierarchy.h"

// Reorder the entries vector so that adjacent entries tend to be nearby.
// You may want to implement box.cpp first.
void Hierarchy::Reorder_Entries()
{
    if(!entries.size()) return;
    //DONE
    // Sort entries by the center of their bounding boxes
    // This helps with spatial locality for better cache performance
    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        vec3 center_a = (a.box.lo + a.box.hi) * 0.5;
        vec3 center_b = (b.box.lo + b.box.hi) * 0.5;
        
        // Sort by x-coordinate first, then y, then z
        if (center_a[0] != center_b[0]) return center_a[0] < center_b[0];
        if (center_a[1] != center_b[1]) return center_a[1] < center_b[1];
        return center_a[2] < center_b[2];
    });
}

// Populate tree from entries.
void Hierarchy::Build_Tree()
{
    if(!entries.size()) return;
    //DONE

    // Clear existing tree
    tree.clear();
    
    int n = entries.size();
    // For n entries, we need 2*n-1 tree nodes
    tree.resize(2 * n - 1);
    
    // Initialize leaf nodes (last n elements) with entry bounding boxes
    for (int i = 0; i < n; i++) {
        tree[n - 1 + i] = entries[i].box;
    }
    
    // Build internal nodes bottom-up
    for (int i = n - 2; i >= 0; i--) {
        int left_child = 2 * i + 1;
        int right_child = 2 * i + 2;
        
        if (right_child < tree.size()) {
            // Both children exist
            tree[i] = tree[left_child].Union(tree[right_child]);
        } else if (left_child < tree.size()) {
            // Only left child exists
            tree[i] = tree[left_child];
        }
    }
}

// Return a list of candidates (indices into the entries list) whose
// bounding boxes intersect the ray.
void Hierarchy::Intersection_Candidates(const Ray& ray, std::vector<int>& candidates) const
{
    //DONE

    candidates.clear();
    if (tree.empty()) return;

    // Recursive helper function to traverse the tree
    std::function<void(int)> traverse = [&](int node_index) {
        if (node_index >= tree.size()) return;
        
        // Check if ray intersects this node's bounding box
        if (!tree[node_index].Intersection(ray)) return;
        
        int n = entries.size();
        
        // If this is a leaf node (in the last n elements)
        if (node_index >= n - 1) {
            int entry_index = node_index - (n - 1);
            if (entry_index >= 0 && entry_index < n) {
                candidates.push_back(entry_index);
            }
        } else {
            // This is an internal node, check children
            int left_child = 2 * node_index + 1;
            int right_child = 2 * node_index + 2;
            
            traverse(left_child);
            traverse(right_child);
        }
    };
    
    // Start traversal from root
    traverse(0);
}
