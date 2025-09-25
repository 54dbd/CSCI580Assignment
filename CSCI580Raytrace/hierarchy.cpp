#include <algorithm>
#include "hierarchy.h"

// Reorder the entries vector so that adjacent entries tend to be nearby.
// You may want to implement box.cpp first.
void Hierarchy::Reorder_Entries()
{
    if(!entries.size()) return;
    // DONE;//sort box entries to help speed up tracing
    // 计算所有包围盒的中心点
    std::vector<std::pair<int, vec3>> centers; // (index, center
    centers.reserve(entries.size());
    for (size_t i = 0; i < entries.size(); ++i) {
        vec3 center = 0.5 * (entries[i].box.lo + entries[i].box.hi);
        centers.emplace_back(i, center);
    }
    // 按 x 坐标排序
    std::sort(centers.begin(), centers.end(), [](const auto& a, const auto& b) {
        return a.second[0] < b.second[0];
    });
    // 重排 entries
    std::vector<Entry> sorted_entries;
    sorted_entries.reserve(entries.size());
    for (const auto& p : centers) {
        sorted_entries.push_back(entries[p.first]);
    }
    entries = std::move(sorted_entries);


}

// Populate tree from entries.
void Hierarchy::Build_Tree()
{
    if(!entries.size()) return;
    // DONE;//build bound box heirarchy
    const int n = static_cast<int>(entries.size());
    const int T = 2 * n - 1; // 总节点数
    tree.resize(T);
    // 1. 初始化叶子节点
    for (int i = 0; i < n; ++i) {
        tree[T - n + i] = entries[i].box;
    }
    // 2. 自底向上构建内部节点
    for (int i = T - n - 1; i >= 0; --i) {
        const int L = 2 * i + 1;
        const int R = 2 * i + 2;
        tree[i] = tree[L].Union(tree[R]);

    }


}

// Return a list of candidates (indices into the entries list) whose
// bounding boxes intersect the ray.
void Hierarchy::Intersection_Candidates(const Ray& ray, std::vector<int>& candidates) const
{
    TODO; //find all bounding boxes along ray





}
