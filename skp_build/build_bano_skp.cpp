#include <SketchUpAPI/SketchUp.h>

#include <array>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

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

constexpr double kMetersToInches = 39.37007874015748;
constexpr double kPi = 3.14159265358979323846;

bool Check(SUResult result, const char* operation) {
  if (result == SU_ERROR_NONE) return true;
  std::cerr << operation << " fallo con codigo SUResult=" << static_cast<int>(result) << "\n";
  return false;
}

Point P(double x, double y, double z) { return Point{x, y, z}; }

std::vector<Triangle> MakeBox(double x, double y, double z, double w, double d, double h) {
  std::array<Point, 8> v = {
      P(x, y, z), P(x + w, y, z), P(x + w, y + d, z), P(x, y + d, z),
      P(x, y, z + h), P(x + w, y, z + h), P(x + w, y + d, z + h), P(x, y + d, z + h)};
  const int idx[][3] = {
      {0,2,1},{0,3,2}, {4,5,6},{4,6,7},
      {0,1,5},{0,5,4}, {1,2,6},{1,6,5},
      {2,3,7},{2,7,6}, {3,0,4},{3,4,7}};
  std::vector<Triangle> out;
  out.reserve(12);
  for (const auto& f : idx) out.push_back({v[f[0]], v[f[1]], v[f[2]]});
  return out;
}

std::vector<Triangle> MakeCylinder(double cx, double cy, double z, double radius,
                                   double height, int segments = 20) {
  std::vector<Point> bottom, top;
  bottom.reserve(segments);
  top.reserve(segments);
  for (int i = 0; i < segments; ++i) {
    const double angle = 2.0 * kPi * static_cast<double>(i) / static_cast<double>(segments);
    const double x = cx + radius * std::cos(angle);
    const double y = cy + radius * std::sin(angle);
    bottom.push_back(P(x, y, z));
    top.push_back(P(x, y, z + height));
  }
  const Point bc = P(cx, cy, z);
  const Point tc = P(cx, cy, z + height);
  std::vector<Triangle> out;
  out.reserve(segments * 4);
  for (int i = 0; i < segments; ++i) {
    const int j = (i + 1) % segments;
    out.push_back({bottom[i], bottom[j], top[j]});
    out.push_back({bottom[i], top[j], top[i]});
    out.push_back({bc, bottom[j], bottom[i]});
    out.push_back({tc, top[i], top[j]});
  }
  return out;
}

bool AddMesh(SUEntitiesRef parent, const std::string& name, const std::vector<Triangle>& triangles) {
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
      SUPoint3D point{p.x * kMetersToInches, p.y * kMetersToInches, p.z * kMetersToInches};
      if (!Check(SUGeometryInputAddVertex(geometry, &point), "SUGeometryInputAddVertex")) return false;
      if (!Check(SULoopInputAddVertexIndex(loop, vertex_index), "SULoopInputAddVertexIndex")) return false;
      ++vertex_index;
    }
    if (!Check(SUGeometryInputAddFace(geometry, &loop, nullptr), "SUGeometryInputAddFace")) return false;
  }

  if (!Check(SUEntitiesFill(group_entities, geometry, true), "SUEntitiesFill")) return false;
  if (!Check(SUGeometryInputRelease(&geometry), "SUGeometryInputRelease")) return false;
  return true;
}

bool AddBox(SUEntitiesRef entities, const std::string& name,
            double x, double y, double z, double w, double d, double h) {
  return AddMesh(entities, name, MakeBox(x, y, z, w, d, h));
}

bool AddCylinder(SUEntitiesRef entities, const std::string& name,
                 double cx, double cy, double z, double radius, double height, int segments = 20) {
  return AddMesh(entities, name, MakeCylinder(cx, cy, z, radius, height, segments));
}

int main(int argc, char** argv) {
  const std::string output = argc > 1 ? argv[1] : "bano_1x2_preliminar.skp";

  if (!Check(SUInitialize(), "SUInitialize")) return 1;

  SUModelRef model = SU_INVALID;
  if (!Check(SUModelCreate(&model), "SUModelCreate")) return 1;

  SUEntitiesRef entities = SU_INVALID;
  if (!Check(SUModelGetEntities(model, &entities), "SUModelGetEntities")) return 1;

  bool ok = true;
  auto box = [&](const std::string& n, double x, double y, double z, double w, double d, double h) {
    if (ok) ok = AddBox(entities, n, x, y, z, w, d, h);
  };
  auto cyl = [&](const std::string& n, double x, double y, double z, double r, double h, int s = 20) {
    if (ok) ok = AddCylinder(entities, n, x, y, z, r, h, s);
  };

  // Cuatro fundaciones de referencia, riostras y radier.
  box("Fundacion_1", -0.20, -0.20, -0.60, 0.50, 0.50, 0.60);
  box("Fundacion_2",  0.90, -0.20, -0.60, 0.50, 0.50, 0.60);
  box("Fundacion_3", -0.20,  1.90, -0.60, 0.50, 0.50, 0.60);
  box("Fundacion_4",  0.90,  1.90, -0.60, 0.50, 0.50, 0.60);
  box("Riostra_frontal",    0.00, 0.00, -0.30, 1.20, 0.20, 0.30);
  box("Riostra_trasera",    0.00, 2.00, -0.30, 1.20, 0.20, 0.30);
  box("Riostra_izquierda",  0.00, 0.20, -0.30, 0.20, 1.80, 0.30);
  box("Riostra_derecha",    1.00, 0.20, -0.30, 0.20, 1.80, 0.30);
  box("Radier_10cm",        0.00, 0.00,  0.00, 1.20, 2.20, 0.10);
  box("Ceramica_piso",      0.08, 0.08,  0.10, 1.04, 2.04, 0.012);

  // Columnas metalicas 100 x 100 mm.
  box("Columna_1_100x100", 0.00, 0.00, 0.10, 0.10, 0.10, 2.40);
  box("Columna_2_100x100", 1.10, 0.00, 0.10, 0.10, 0.10, 2.40);
  box("Columna_3_100x100", 0.00, 2.10, 0.10, 0.10, 0.10, 2.40);
  box("Columna_4_100x100", 1.10, 2.10, 0.10, 0.10, 0.10, 2.40);

  // Marco superior, viguetas y placas del piso superior.
  box("Viga_superior_frontal",   0.00, 0.00, 2.40, 1.20, 0.10, 0.10);
  box("Viga_superior_trasera",   0.00, 2.10, 2.40, 1.20, 0.10, 0.10);
  box("Viga_superior_izquierda", 0.00, 0.10, 2.40, 0.10, 2.00, 0.10);
  box("Viga_superior_derecha",   1.10, 0.10, 2.40, 0.10, 2.00, 0.10);
  const double joists[] = {0.20, 0.60, 1.00, 1.40, 1.80, 2.05};
  for (int i = 0; i < 6; ++i) {
    box("Vigueta_superior_" + std::to_string(i + 1), 0.10, joists[i], 2.40, 1.00, 0.05, 0.10);
  }
  box("Terciado_estructural_21mm", 0.00, 0.00, 2.50, 1.20, 2.20, 0.021);
  box("Placa_cementicia_superior", 0.00, 0.00, 2.521, 1.20, 2.20, 0.010);

  // Muros livianos y vano de puerta.
  box("Muro_izquierdo",          0.00, 0.08, 0.10, 0.08, 2.04, 2.30);
  box("Muro_derecho",            1.12, 0.08, 0.10, 0.08, 2.04, 2.30);
  box("Muro_trasero",            0.08, 2.12, 0.10, 1.04, 0.08, 2.30);
  box("Muro_frontal_izquierdo",  0.08, 0.00, 0.10, 0.17, 0.08, 2.00);
  box("Muro_frontal_derecho",    0.95, 0.00, 0.10, 0.17, 0.08, 2.00);
  box("Dintel_puerta",           0.25, 0.00, 2.10, 0.70, 0.08, 0.30);
  box("PVC_interior_izquierdo",  0.08, 0.10, 0.10, 0.008, 2.00, 2.20);
  box("PVC_interior_derecho",    1.112,0.10, 0.10, 0.008, 2.00, 2.20);
  box("PVC_interior_trasero",    0.10, 2.112,0.10, 1.00, 0.008,2.20);
  box("Puerta_70x200",           0.25,-0.04, 0.10, 0.70, 0.04, 2.00);

  // Columna falsa y tuberias.
  box("Columna_falsa_instalaciones", 0.08, 1.82, 0.10, 0.28, 0.30, 2.20);
  cyl("Bajante_110mm",                 0.22, 1.97, 0.10, 0.055, 2.75, 20);
  cyl("Agua_fria",                     0.12, 1.88, 0.10, 0.010, 2.75, 12);
  cyl("Agua_caliente",                 0.16, 1.88, 0.10, 0.010, 2.75, 12);

  // Artefactos sanitarios simplificados.
  box("Receptaculo_ducha",     0.10, 1.22, 0.112, 1.00, 0.85, 0.07);
  box("Mampara_vidrio",        0.58, 1.20, 0.18,  0.52, 0.012,1.85);
  box("WC_base",               0.14, 0.63, 0.112, 0.38, 0.58, 0.38);
  box("WC_estanque",           0.16, 1.02, 0.42,  0.34, 0.18, 0.55);
  box("Lavamanos_compacto",    0.68, 0.28, 0.82,  0.40, 0.32, 0.12);
  box("Pedestal_lavamanos",    0.83, 0.37, 0.112, 0.12, 0.14, 0.708);
  box("Cielo_falso_PVC",       0.08, 0.08, 2.24,  1.04, 2.04, 0.012);
  cyl("Extractor_100mm",       1.118,0.72, 1.86,  0.05, 0.04, 20);

  // Pasadas preparadas para el baño superior.
  cyl("Pasada_WC_superior",           0.22, 1.97, 2.531, 0.055, 0.25, 20);
  cyl("Pasada_agua_fria_superior",    0.12, 1.88, 2.531, 0.010, 0.25, 12);
  cyl("Pasada_agua_caliente_superior",0.16, 1.88, 2.531, 0.010, 0.25, 12);

  if (!ok) {
    SUModelRelease(&model);
    SUTerminate();
    return 1;
  }

  const SUResult save_result = SUModelSaveToFileWithVersion(
      model, output.c_str(), SUModelVersion_SU2021);
  if (!Check(save_result, "SUModelSaveToFileWithVersion")) {
    SUModelRelease(&model);
    SUTerminate();
    return 1;
  }

  SUModelRelease(&model);
  SUTerminate();
  std::cout << "Archivo SKP creado: " << output << "\n";
  return 0;
}
