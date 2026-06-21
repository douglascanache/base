#pragma once

#include <SketchUpAPI/SketchUp.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace bath {

struct Point {
  double x;
  double y;
  double z;
};

struct Triangle {
  Point a;
  Point b;
  Point c;
};

using Mesh = std::vector<Triangle>;

constexpr double kMetersToInches = 39.37007874015748;
constexpr double kPi = 3.14159265358979323846;

inline Point P(double x, double y, double z) { return Point{x, y, z}; }
inline Point Add(Point a, Point b) { return P(a.x + b.x, a.y + b.y, a.z + b.z); }
inline Point Sub(Point a, Point b) { return P(a.x - b.x, a.y - b.y, a.z - b.z); }
inline Point Mul(Point a, double s) { return P(a.x * s, a.y * s, a.z * s); }
inline double Dot(Point a, Point b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
inline Point Cross(Point a, Point b) {
  return P(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}
inline double Length(Point a) { return std::sqrt(Dot(a, a)); }
inline Point Normalize(Point a) {
  const double len = Length(a);
  return len > 1e-12 ? Mul(a, 1.0 / len) : P(0, 0, 0);
}

inline bool Check(SUResult result, const char* operation) {
  if (result == SU_ERROR_NONE) return true;
  std::cerr << operation << " fallo con codigo SUResult=" << static_cast<int>(result) << "\n";
  return false;
}

inline void Append(Mesh& dst, const Mesh& src) {
  dst.insert(dst.end(), src.begin(), src.end());
}

inline Mesh MakeBox(double x, double y, double z, double w, double d, double h) {
  std::array<Point, 8> v = {
      P(x, y, z), P(x+w, y, z), P(x+w, y+d, z), P(x, y+d, z),
      P(x, y, z+h), P(x+w, y, z+h), P(x+w, y+d, z+h), P(x, y+d, z+h)};
  const int f[][3] = {
      {0,2,1},{0,3,2}, {4,5,6},{4,6,7},
      {0,1,5},{0,5,4}, {1,2,6},{1,6,5},
      {2,3,7},{2,7,6}, {3,0,4},{3,4,7}};
  Mesh out;
  out.reserve(12);
  for (const auto& tri : f) out.push_back({v[tri[0]], v[tri[1]], v[tri[2]]});
  return out;
}

inline Mesh MakeRotatedBoxZ(double ox, double oy, double z, double w, double d, double h,
                            double angle_radians) {
  const double c = std::cos(angle_radians);
  const double s = std::sin(angle_radians);
  auto rot = [&](double x, double y, double zz) {
    return P(ox + c*x - s*y, oy + s*x + c*y, zz);
  };
  std::array<Point, 8> v = {
      rot(0,0,z), rot(w,0,z), rot(w,d,z), rot(0,d,z),
      rot(0,0,z+h), rot(w,0,z+h), rot(w,d,z+h), rot(0,d,z+h)};
  const int f[][3] = {
      {0,2,1},{0,3,2}, {4,5,6},{4,6,7},
      {0,1,5},{0,5,4}, {1,2,6},{1,6,5},
      {2,3,7},{2,7,6}, {3,0,4},{3,4,7}};
  Mesh out;
  for (const auto& tri : f) out.push_back({v[tri[0]], v[tri[1]], v[tri[2]]});
  return out;
}

inline Mesh MakeCylinderBetween(Point a, Point b, double radius, int segments = 20) {
  const Point axis = Sub(b, a);
  const double length = Length(axis);
  if (length < 1e-9) return {};
  const Point n = Normalize(axis);
  const Point helper = std::fabs(n.z) < 0.9 ? P(0,0,1) : P(1,0,0);
  const Point u = Normalize(Cross(n, helper));
  const Point v = Normalize(Cross(n, u));

  std::vector<Point> ring_a, ring_b;
  ring_a.reserve(segments);
  ring_b.reserve(segments);
  for (int i = 0; i < segments; ++i) {
    const double t = 2.0 * kPi * static_cast<double>(i) / static_cast<double>(segments);
    const Point offset = Add(Mul(u, radius * std::cos(t)), Mul(v, radius * std::sin(t)));
    ring_a.push_back(Add(a, offset));
    ring_b.push_back(Add(b, offset));
  }

  Mesh out;
  out.reserve(segments * 4);
  for (int i = 0; i < segments; ++i) {
    const int j = (i + 1) % segments;
    out.push_back({ring_a[i], ring_a[j], ring_b[j]});
    out.push_back({ring_a[i], ring_b[j], ring_b[i]});
    out.push_back({a, ring_a[j], ring_a[i]});
    out.push_back({b, ring_b[i], ring_b[j]});
  }
  return out;
}

inline Mesh MakeVerticalCylinder(double cx, double cy, double z, double radius,
                                 double height, int segments = 20) {
  return MakeCylinderBetween(P(cx,cy,z), P(cx,cy,z+height), radius, segments);
}

inline Mesh MakeEllipticFrustum(double cx, double cy, double z,
                                double rx_bottom, double ry_bottom,
                                double rx_top, double ry_top,
                                double height, int segments = 28) {
  std::vector<Point> lower, upper;
  lower.reserve(segments);
  upper.reserve(segments);
  for (int i = 0; i < segments; ++i) {
    const double a = 2.0 * kPi * static_cast<double>(i) / static_cast<double>(segments);
    lower.push_back(P(cx + rx_bottom*std::cos(a), cy + ry_bottom*std::sin(a), z));
    upper.push_back(P(cx + rx_top*std::cos(a), cy + ry_top*std::sin(a), z + height));
  }
  const Point bottom_center = P(cx,cy,z);
  const Point top_center = P(cx,cy,z+height);
  Mesh out;
  for (int i = 0; i < segments; ++i) {
    const int j = (i + 1) % segments;
    out.push_back({lower[i], lower[j], upper[j]});
    out.push_back({lower[i], upper[j], upper[i]});
    out.push_back({bottom_center, lower[j], lower[i]});
    out.push_back({top_center, upper[i], upper[j]});
  }
  return out;
}

inline Mesh MakeEllipsoid(double cx, double cy, double cz,
                          double rx, double ry, double rz,
                          int around = 28, int vertical = 14) {
  std::vector<std::vector<Point>> rings;
  for (int j = 1; j < vertical; ++j) {
    const double phi = -0.5*kPi + kPi*static_cast<double>(j)/static_cast<double>(vertical);
    std::vector<Point> ring;
    for (int i = 0; i < around; ++i) {
      const double theta = 2.0*kPi*static_cast<double>(i)/static_cast<double>(around);
      ring.push_back(P(cx + rx*std::cos(phi)*std::cos(theta),
                       cy + ry*std::cos(phi)*std::sin(theta),
                       cz + rz*std::sin(phi)));
    }
    rings.push_back(ring);
  }
  const Point bottom = P(cx,cy,cz-rz);
  const Point top = P(cx,cy,cz+rz);
  Mesh out;
  if (rings.empty()) return out;
  for (int i = 0; i < around; ++i) {
    const int n = (i+1)%around;
    out.push_back({bottom, rings.front()[n], rings.front()[i]});
    out.push_back({top, rings.back()[i], rings.back()[n]});
  }
  for (size_t j = 0; j+1 < rings.size(); ++j) {
    for (int i = 0; i < around; ++i) {
      const int n = (i+1)%around;
      out.push_back({rings[j][i], rings[j][n], rings[j+1][n]});
      out.push_back({rings[j][i], rings[j+1][n], rings[j+1][i]});
    }
  }
  return out;
}

inline Mesh MakeEllipticTorus(double cx, double cy, double cz,
                              double major_x, double major_y, double minor,
                              int around = 32, int tube = 10) {
  Mesh out;
  std::vector<std::vector<Point>> pts(around, std::vector<Point>(tube));
  for (int i = 0; i < around; ++i) {
    const double u = 2.0*kPi*static_cast<double>(i)/static_cast<double>(around);
    const Point radial = Normalize(P(std::cos(u)/std::max(major_x,1e-6),
                                     std::sin(u)/std::max(major_y,1e-6), 0));
    for (int j = 0; j < tube; ++j) {
      const double v = 2.0*kPi*static_cast<double>(j)/static_cast<double>(tube);
      pts[i][j] = P(cx + major_x*std::cos(u) + minor*std::cos(v)*radial.x,
                    cy + major_y*std::sin(u) + minor*std::cos(v)*radial.y,
                    cz + minor*std::sin(v));
    }
  }
  for (int i = 0; i < around; ++i) {
    const int ni = (i+1)%around;
    for (int j = 0; j < tube; ++j) {
      const int nj = (j+1)%tube;
      out.push_back({pts[i][j], pts[ni][j], pts[ni][nj]});
      out.push_back({pts[i][j], pts[ni][nj], pts[i][nj]});
    }
  }
  return out;
}

inline Mesh MakePolygonPrism(double cx, double cy, double z, double radius,
                             double height, int sides = 6, double rotation = 0.0) {
  std::vector<Point> lower, upper;
  for (int i = 0; i < sides; ++i) {
    const double a = rotation + 2.0*kPi*static_cast<double>(i)/static_cast<double>(sides);
    lower.push_back(P(cx + radius*std::cos(a), cy + radius*std::sin(a), z));
    upper.push_back(P(cx + radius*std::cos(a), cy + radius*std::sin(a), z+height));
  }
  const Point bc=P(cx,cy,z), tc=P(cx,cy,z+height);
  Mesh out;
  for (int i=0;i<sides;++i) {
    int j=(i+1)%sides;
    out.push_back({lower[i],lower[j],upper[j]});
    out.push_back({lower[i],upper[j],upper[i]});
    out.push_back({bc,lower[j],lower[i]});
    out.push_back({tc,upper[i],upper[j]});
  }
  return out;
}

inline SUMaterialRef CreateMaterial(SUModelRef model, const std::string& name,
                                    unsigned char r, unsigned char g,
                                    unsigned char b, unsigned char a = 255) {
  SUMaterialRef material = SU_INVALID;
  if (!Check(SUMaterialCreate(&material), "SUMaterialCreate")) return material;
  Check(SUMaterialSetName(material, name.c_str()), "SUMaterialSetName");
  SUColor color{r,g,b,a};
  Check(SUMaterialSetColor(material, &color), "SUMaterialSetColor");
  if (a < 255) {
    Check(SUMaterialSetUseOpacity(material, true), "SUMaterialSetUseOpacity");
    Check(SUMaterialSetOpacity(material, static_cast<double>(a)/255.0), "SUMaterialSetOpacity");
  }
  const SUMaterialRef list[] = {material};
  Check(SUModelAddMaterials(model, 1, list), "SUModelAddMaterials");
  return material;
}

inline SULayerRef CreateLayer(SUModelRef model, const std::string& name) {
  SULayerRef layer = SU_INVALID;
  if (!Check(SULayerCreate(&layer), "SULayerCreate")) return layer;
  Check(SULayerSetName(layer, name.c_str()), "SULayerSetName");
  const SULayerRef list[] = {layer};
  Check(SUModelAddLayers(model, 1, list), "SUModelAddLayers");
  return layer;
}

inline bool AddMesh(SUEntitiesRef parent, const std::string& name,
                    const Mesh& triangles, SUMaterialRef material,
                    SULayerRef layer) {
  if (triangles.empty()) return true;
  SUGroupRef group = SU_INVALID;
  if (!Check(SUGroupCreate(&group), "SUGroupCreate")) return false;
  if (!Check(SUEntitiesAddGroup(parent, group), "SUEntitiesAddGroup")) return false;
  if (!Check(SUGroupSetName(group, name.c_str()), "SUGroupSetName")) return false;

  SUEntitiesRef group_entities = SU_INVALID;
  if (!Check(SUGroupGetEntities(group, &group_entities), "SUGroupGetEntities")) return false;

  SUGeometryInputRef geometry = SU_INVALID;
  if (!Check(SUGeometryInputCreate(&geometry), "SUGeometryInputCreate")) return false;

  size_t vertex_index = 0;
  for (const Triangle& triangle : triangles) {
    const Point points[3] = {triangle.a, triangle.b, triangle.c};
    SULoopInputRef loop = SU_INVALID;
    if (!Check(SULoopInputCreate(&loop), "SULoopInputCreate")) return false;
    for (const Point& p : points) {
      SUPoint3D point{p.x*kMetersToInches, p.y*kMetersToInches, p.z*kMetersToInches};
      if (!Check(SUGeometryInputAddVertex(geometry, &point), "SUGeometryInputAddVertex")) return false;
      if (!Check(SULoopInputAddVertexIndex(loop, vertex_index), "SULoopInputAddVertexIndex")) return false;
      ++vertex_index;
    }
    if (!Check(SUGeometryInputAddFace(geometry, &loop, nullptr), "SUGeometryInputAddFace")) return false;
  }

  if (!Check(SUEntitiesFill(group_entities, geometry, true), "SUEntitiesFill")) return false;
  if (!Check(SUGeometryInputRelease(&geometry), "SUGeometryInputRelease")) return false;

  const SUDrawingElementRef element = SUGroupToDrawingElement(group);
  if (SUIsValid(material)) Check(SUDrawingElementSetMaterial(element, material), "SetMaterial");
  if (SUIsValid(layer)) Check(SUDrawingElementSetLayer(element, layer), "SetLayer");
  return true;
}

}  // namespace bath
