#include "accelerators/octree.h"
#include "paramset.h"
#include "stats.h"

using namespace std;

namespace pbrt
{

	STAT_MEMORY_COUNTER("Memory/Octree", _treeBytes);
	STAT_COUNTER("Octree/Interior nodes", _interiorNodes);
	STAT_RATIO("Octree/Primitives per leaf nodes", _primitives, _leafNodes);
	STAT_COUNTER("Octree/Tree depth", _treeDepth);

	unsigned short OctreeAccel::_maxPrimitivesPeLeaf;
	unsigned short OctreeAccel::_maxDepth;

	OctreeAccel::OctreeAccel(const vector<shared_ptr<Primitive>>& p, unsigned short maxDepth, unsigned short maxPrimitivesPerLeaf)
	{
		_maxPrimitivesPeLeaf = maxPrimitivesPerLeaf;
		_maxDepth = maxDepth;

		_treeBytes = 0;
		_primitives = p.size();
		_depth = 0;
		float xmax, ymax, zmax;
		xmax = ymax = zmax = -numeric_limits<float>::max();
		float xmin, ymin, zmin;
		xmin = ymin = zmin = numeric_limits<float>::max();
		for (const shared_ptr<Primitive> primitive : p)
		{
			Bounds3f b = primitive->WorldBound();

			if(b.pMax.x > xmax)
			{
				xmax = b.pMax.x;
			}

			if (b.pMax.y > ymax)
			{
				ymax = b.pMax.y;
			}

			if (b.pMax.z > zmax)
			{
				zmax = b.pMax.z;
			}

			if (b.pMin.x < xmin)
			{
				xmin = b.pMin.x;
			}

			if (b.pMin.y < ymin)
			{
				ymin = b.pMin.y;
			}

			if (b.pMin.z < zmin)
			{
				zmin = b.pMin.z;
			}
		}

		_bounds = Bounds3f(Point3f(xmin, ymin, zmin), Point3f(xmax, ymax, zmax));

		createNodes(p);
		_interiorNodes++;

		_treeBytes += sizeof(*this) + _children.size() * sizeof(shared_ptr<Primitive>) + _nodes.size() * sizeof(shared_ptr<OctreeAccel>);
	}

	OctreeAccel::OctreeAccel(const vector<shared_ptr<Primitive>>& p, Bounds3f bounds, unsigned short depth)
	{
		if(_treeDepth < depth)
		{
			_treeDepth = depth;
		}

		_bounds = bounds;
		_depth = depth;

		_children.clear();

		for (const shared_ptr<Primitive> primitive : p)
		{
			Bounds3f b = primitive->WorldBound();

			if( (_bounds.pMin.x > b.pMax.x || b.pMin.x > _bounds.pMax.x) ||
				(_bounds.pMin.y > b.pMax.y || b.pMin.y > _bounds.pMax.y) || 
				(_bounds.pMin.z > b.pMax.z || b.pMin.z > _bounds.pMax.z))
			{
				continue;
			}

			_children.push_back(primitive);
		}

		if (_children.size() >= _maxPrimitivesPeLeaf && _depth < _maxDepth)
		{
			createNodes(_children);
			_children.clear();
		}

		if(_children.size() <= 0)
		{
			_interiorNodes++;
		}
		else
		{
			_leafNodes++;
		}

		_treeBytes += sizeof(*this) + _children.size() * sizeof(shared_ptr<Primitive>) + _nodes.size() * sizeof(shared_ptr<OctreeAccel>);
	}

	OctreeAccel::~OctreeAccel()
	{
		for (OctreeAccel* node : _nodes)
		{
			delete(node);
		}
	}

	Bounds3f OctreeAccel::WorldBound() const
	{
		return _bounds;
	}

	bool OctreeAccel::Intersect(const Ray& ray, SurfaceInteraction* isect) const
	{
		bool hit = false;
		float minDist = numeric_limits<float>::max();

		// not intersecting this section
		if (!_bounds.IntersectP(ray))
		{
			return false;
		}

		if (_isLeaf)
		{
			// no elements in this section
			if (_children.size() <= 0)
			{
				return false;
			}

			float d = numeric_limits<float>::max();

			// if any of the children is hit, return true
			for (const shared_ptr<Primitive> primitive : _children)
			{
				SurfaceInteraction si;
				if (primitive->Intersect(ray, &si))
				{
					float dist = (ray.o - si.p).Length();

					if(dist < d)
					{
						d = dist;
						hit = true;
						*isect = si;
					}
				}
			}
		}
		else
		{
			// recurively check if any of the subtrees get hit.
			for (OctreeAccel* const node : _nodes)
			{
				SurfaceInteraction si;
				if (node->Intersect(ray, &si))
				{
					float dist = (ray.o - si.p).Length();

					if (dist < minDist)
					{
						minDist = dist;
						hit = true;
						*isect = si;
					}
				}
			}
		}

		return hit;
	}

	// not entierly accurate, but faster: as soon as an element is hit, return true
	bool OctreeAccel::IntersectP(const Ray& ray) const
	{
		// not intersecting this section
		if (!_bounds.IntersectP(ray))
		{
			return false;
		}

		if (_isLeaf)
		{
			// no elements in this section
			if (_children.size() <= 0)
			{
				return false;
			}

			// if any of the children is hit, return true
			for (const shared_ptr<Primitive> primitive : _children)
			{
				if(primitive->IntersectP(ray))
				{
					return true;
				}
			}
		}
		else
		{
			// recurively check if any of the subtrees get hit.
			for (OctreeAccel* const node : _nodes)
			{
				if(node->IntersectP(ray))
				{
					return true;
				}
			}
		}

		// passed through this section without hiting any object
		return false;
	}

	void OctreeAccel::createNodes(const vector<shared_ptr<Primitive>>& p)
	{
		_isLeaf = false;

		_nodes.clear();

		float xdist, ydist, zdist;

		xdist = (_bounds.pMax.x - _bounds.pMin.x) / 2;
		ydist = (_bounds.pMax.y - _bounds.pMin.y) / 2;
		zdist = (_bounds.pMax.z - _bounds.pMin.z) / 2;

		Bounds3f b = Bounds3f(_bounds.pMin, Point3f(_bounds.pMin.x + xdist, _bounds.pMin.y + ydist, _bounds.pMin.z + zdist));
		_nodes.push_back(new OctreeAccel(p, b, _depth + 1));

		b.pMin.x += xdist;
		b.pMax.x += xdist;
		_nodes.push_back(new OctreeAccel(p, b, _depth + 1));

		b.pMin.y += ydist;
		b.pMax.y += ydist;
		_nodes.push_back(new OctreeAccel(p, b, _depth + 1));

		b.pMin.x -= xdist;
		b.pMax.x -= xdist;
		_nodes.push_back(new OctreeAccel(p, b, _depth + 1));

		b.pMin.z += zdist;
		b.pMax.z += zdist;
		_nodes.push_back(new OctreeAccel(p, b, _depth + 1));

		b.pMin.y -= ydist;
		b.pMax.y -= ydist;
		_nodes.push_back(new OctreeAccel(p, b, _depth + 1));

		b.pMin.x += xdist;
		b.pMax.x += xdist;
		_nodes.push_back(new OctreeAccel(p, b, _depth + 1));

		b.pMin.y += ydist;
		b.pMax.y += ydist;
		_nodes.push_back(new OctreeAccel(p, b, _depth + 1));
	}

	shared_ptr<OctreeAccel> CreateOctreeAccelerator(const vector<shared_ptr<Primitive>> &prims, const ParamSet &ps)
	{
		int maxDepth = ps.FindOneInt("maxDepth", 10);
		int maxPrimitivesPerLeaf = ps.FindOneInt("maxPrimitivesPerLeaf", 10);
		return make_shared<OctreeAccel>(prims, maxDepth, maxPrimitivesPerLeaf);
	}

}
