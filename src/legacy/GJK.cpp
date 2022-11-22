#include "GJK.hpp"

#include <numeric>

namespace algo
{
	namespace GJK
	{
		// --------------------------------------------------------------------------------------------
		glm::vec3 FindFurthestPoint(const Meshable* body, const glm::vec3& direction, const glm::mat4& localToWorld)
		{
			glm::vec3 maxPoint;
			float maxDistance = -FLT_MAX;

			for (const VertexType& vertex : body->getVertices())
			{
				glm::vec3 position = localToWorld * glm::vec4(vertex.position, 1.0);
				float distance = glm::dot(position, direction);
				if (distance > maxDistance)
				{
					maxDistance = distance;
					maxPoint = position;
				}
			}

			return maxPoint;
		}

		// --------------------------------------------------------------------------------------------
		glm::vec3 Support(const Meshable* body1, const Meshable* body2, const glm::vec3& direction)
		{
			return
				FindFurthestPoint(body1, direction, body1->localToWorld()) -
				FindFurthestPoint(body2, -direction, body2->localToWorld());
		}

		// --------------------------------------------------------------------------------------------
		bool SameDirection(const glm::vec3& a, const glm::vec3& b)
		{
			return glm::dot(a, b) > 0;
		}

		// --------------------------------------------------------------------------------------------
		template <int N>
		bool InternalSimplex(Simplex& points, glm::vec3& direction)
		{
			return false;
		}

		// --------------------------------------------------------------------------------------------
		template <> // Line
		bool InternalSimplex<2>(Simplex& points, glm::vec3& direction)
		{
			glm::vec3 a = points[0];
			glm::vec3 b = points[1];

			glm::vec3 ab = b - a;
			glm::vec3 ao = -a;

			if (SameDirection(ab, ao))
			{
				direction = glm::cross(glm::cross(ab, ao), ab);
			}

			else
			{
				points = {a};
				direction = ao;
			}

			return false;
		}

		// --------------------------------------------------------------------------------------------
		template <> // Triangle
		bool InternalSimplex<3>(Simplex& points, glm::vec3& direction)
		{
			glm::vec3 a = points[0];
			glm::vec3 b = points[1];
			glm::vec3 c = points[2];

			glm::vec3 ab = b - a;
			glm::vec3 ac = c - a;
			glm::vec3 ao = -a;

			glm::vec3 abc = glm::cross(ab, ac);

			if (SameDirection(glm::cross(abc, ac), ao))
			{
				if (SameDirection(ac, ao))
				{
					points = {a, c};
					direction = glm::cross(glm::cross(ac, ao), ac);
				}

				else
				{
					points = {a, b};
					return InternalSimplex<2>(points, direction);
				}
			}

			else
			{
				if (SameDirection(glm::cross(ab, abc), ao))
				{
					points = {a, b};
					return InternalSimplex<2>(points, direction);
				}

				else
				{
					if (SameDirection(abc, ao))
					{
						direction = abc;
					}

					else
					{
						points = {a, c, b};
						direction = -abc;
					}
				}
			}

			return false;
		}

		// --------------------------------------------------------------------------------------------
		template <> // Tethaedron
		bool InternalSimplex<4>(Simplex& points, glm::vec3& direction)
		{
			glm::vec3 a = points[0];
			glm::vec3 b = points[1];
			glm::vec3 c = points[2];
			glm::vec3 d = points[3];

			glm::vec3 ab = b - a;
			glm::vec3 ac = c - a;
			glm::vec3 ad = d - a;
			glm::vec3 ao = -a;

			glm::vec3 abc = glm::cross(ab, ac);
			glm::vec3 acd = glm::cross(ac, ad);
			glm::vec3 adb = glm::cross(ad, ab);

			if (SameDirection(abc, ao))
			{
				points = {a, b, c};
				return InternalSimplex<3>(points, direction);
			}

			if (SameDirection(acd, ao))
			{
				points = {a, c, d};
				return InternalSimplex<3>(points, direction);
			}

			if (SameDirection(adb, ao))
			{
				points = {a, d, b};
				return InternalSimplex<3>(points, direction);
			}

			return true;
		}

		// --------------------------------------------------------------------------------------------
		bool NextSimplex(Simplex& points, glm::vec3& direction)
		{
			switch (points.size())
			{
				case 2: return InternalSimplex<2>(points, direction);
				case 3: return InternalSimplex<3>(points, direction);
				case 4: return InternalSimplex<4>(points, direction);
				default: return false;
			}
		}

		// --------------------------------------------------------------------------------------------
		std::optional<Simplex> CollideGJK(const Meshable* body1, const Meshable* body2)
		{
			glm::vec3 support = Support(body1, body2, glm::vec3(1.0, 0.0, 0.0));

			Simplex points{support};
			glm::vec3 direction = -support;

			for (;;)
			{
				support = Support(body1, body2, direction);

				if (glm::dot(support, direction) <= 0)
				{
					return std::nullopt;
				}

				points.insert(points.begin(), support);
				if (points.size() > 4) points.resize(4);

				if (NextSimplex(points, direction))
				{
					return points;
				}
			}
		}

		// --------------------------------------------------------------------------------------------
		std::pair<std::vector<glm::vec4>, size_t> GetFaceNormals(
			const Simplex& points, const std::vector<size_t>& faces)
		{
			std::vector<glm::vec4> normals;
			size_t minTriangle = 0;
			float  minDistance = FLT_MAX;

			for (size_t i = 0; i < faces.size(); i += 3)
			{
				glm::vec3 a = points[faces[i]];
				glm::vec3 b = points[faces[i + 1]];
				glm::vec3 c = points[faces[i + 2]];

				glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
				float distance = glm::dot(normal, a);

				if (distance < 0)
				{
					normal *= -1;
					distance *= -1;
				}

				normals.emplace_back(normal, distance);

				if (distance < minDistance)
				{
					minTriangle = i / 3;
					minDistance = distance;
				}
			}

			return {normals, minTriangle};
		}

		// --------------------------------------------------------------------------------------------
		void AddIfUniqueEdge(
			std::vector<std::pair<size_t, size_t>>& edges, const std::vector<size_t>& faces, size_t a, size_t b)
		{
			auto reverse = std::find(
				edges.begin(),
				edges.end(),
				std::make_pair(faces[b], faces[a])
			);

			if (reverse != edges.end())
			{
				edges.erase(reverse);
			}

			else
			{
				edges.emplace_back(faces[a], faces[b]);
			}
		}

		// --------------------------------------------------------------------------------------------
		CollisionBodyData EPA(const Simplex& simplex, const Meshable* body1, const Meshable* body2)
		{
			std::vector<glm::vec3> points(simplex.begin(), simplex.end());
			std::vector<size_t> faces =
			{
				0, 1, 2,
				0, 3, 1,
				0, 2, 3,
				1, 3, 2
			};

			auto [normals, minFace] = GetFaceNormals(points, faces);

			glm::vec3 minNormal;
			float minDistance = FLT_MAX;

			//while (minDistance == FLT_MAX)
			for (int ii = 0; ii < 100; ++ii)
			{
				minNormal = normals[minFace];
				minDistance = normals[minFace].w;

				glm::vec3 support = Support(body1, body2, minNormal);
				float sDistance = glm::dot(minNormal, support);

				if (abs(sDistance - minDistance) > 0.01f)
				{
					minDistance = FLT_MAX;

					std::vector<std::pair<size_t, size_t>> uniqueEdges;

					for (size_t i = 0; i < normals.size(); i++)
					{
						if (SameDirection(normals[i], support))
						{
							size_t f = i * 3;

							AddIfUniqueEdge(uniqueEdges, faces, f, f + 1);
							AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
							AddIfUniqueEdge(uniqueEdges, faces, f + 2, f);

							faces[f + 2] = faces.back(); faces.pop_back();
							faces[f + 1] = faces.back(); faces.pop_back();
							faces[f] = faces.back(); faces.pop_back();

							normals[i] = normals.back(); normals.pop_back();

							i--;
						}
					}

					std::vector<size_t> newFaces;
					for (auto [edgeIndex1, edgeIndex2] : uniqueEdges)
					{
						newFaces.push_back(edgeIndex1);
						newFaces.push_back(edgeIndex2);
						newFaces.push_back(points.size());
					}

					points.push_back(support);

					auto [newNormals, newMinFace] = GetFaceNormals(points, newFaces);

					float oldMinDistance = FLT_MAX;
					for (size_t i = 0; i < normals.size(); i++)
					{
						if (normals[i].w < oldMinDistance)
						{
							oldMinDistance = normals[i].w;
							minFace = i;
						}
					}

					if (newNormals[newMinFace].w < oldMinDistance)
					{
						minFace = newMinFace + normals.size();
					}

					faces.insert(faces.end(), newFaces.begin(), newFaces.end());
					normals.insert(normals.end(), newNormals.begin(), newNormals.end());
				}

				else break;
			}

			// TODO: verify this
			CollisionBodyData result;
			result.normal = minNormal;

			glm::vec3 barycenter = std::accumulate(points.begin(), points.end(), glm::vec3(0.0)) / (float) points.size();
			result.worldPosition = barycenter;

			/*float coeff = 0.0;
			glm::vec3 weightedOffset = std::accumulate(points.begin(), points.end(), glm::vec3(0.0), [&](glm::vec3 prev, glm::vec3 curr)
																						 {
																							 float k = glm::dot(minNormal, curr - barycenter);
																							 coeff += k;
																							 return prev + curr * k;
																						 });
			result.worldPosition = weightedOffset / coeff;*/

			return result;
		}
	}

  // ----------------------------------------------------------------------------------------------
	CollisionResult solveGJK(const Meshable* body1, const Meshable* body2)
  {
		auto result = GJK::CollideGJK(body1, body2);

		if (!result.has_value())
		{
			return std::nullopt;
		}

		return std::make_pair(
			GJK::EPA(*result, body1, body2),
			GJK::EPA(*result, body2, body1)
		);
  }
}
