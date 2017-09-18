#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif
#include "primitive.h"

#ifndef PBRT_ACCELERATORS_OCTREE_H
#define PBRT_ACCELERATORS_OCTREE_H

namespace pbrt
{
	class OctreeAccel : public Aggregate
	{
	public:
		OctreeAccel(const std::vector<std::shared_ptr<Primitive>> &p, short maxDepth);
		OctreeAccel(const std::vector<std::shared_ptr<Primitive>> &p, Bounds3f bounds, short maxDepth, short depth);

		~OctreeAccel();

		Bounds3f WorldBound() const override;
		bool Intersect(const Ray &ray, SurfaceInteraction *isect) const override;
		bool IntersectP(const Ray &ray) const override;
	
	private:
		bool _isLeaf = true;
		short _depth;
		short _maxDepth;
		short _branchingFactor = 10;
		std::vector<OctreeAccel*> _nodes;
		std::vector<std::shared_ptr<Primitive>> _children;
		Bounds3f _bounds;

		void createNodes(const std::vector<std::shared_ptr<Primitive>> &p);
	};

	std::shared_ptr<OctreeAccel> CreateOctreeAccelerator(const std::vector<std::shared_ptr<Primitive>> &prims, const ParamSet &ps);
}

#endif  // PBRT_ACCELERATORS_OCTREE_H