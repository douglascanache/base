#include "skp_helpers.hpp"

#include <array>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

using namespace bath;

struct Palette {
  SUMaterialRef concrete;
  SUMaterialRef gravel;
  SUMaterialRef rebar;
  SUMaterialRef steel;
  SUMaterialRef galvanized;
  SUMaterialRef wood;
  SUMaterialRef cement;
  SUMaterialRef membrane;
  SUMaterialRef wall;
  SUMaterialRef pvc;
  SUMaterialRef tile;
  SUMaterialRef sanitary;
  SUMaterialRef chrome;
  SUMaterialRef glass;
  SUMaterialRef door;
  SUMaterialRef sewer;
  SUMaterialRef cold;
  SUMaterialRef hot;
  SUMaterialRef electric;
  SUMaterialRef black;
  SUMaterialRef label;
};

struct Builder {
  SUEntitiesRef entities;
  Palette p;
  bool ok = true;

  void Mesh(const std::string& name, const bath::Mesh& m, SUMaterialRef material, SULayerRef tag) {
    if (ok) ok = AddMesh(entities, name, m, material, tag);
  }
  void Box(const std::string& name, double x,double y,double z,double w,double d,double h,
           SUMaterialRef material,SULayerRef tag) {
    Mesh(name, MakeBox(x,y,z,w,d,h), material, tag);
  }
  void Pipe(const std::string& name, Point a,Point b,double radius,int segments,
            SUMaterialRef material,SULayerRef tag) {
    Mesh(name, MakeCylinderBetween(a,b,radius,segments), material, tag);
  }
  void Cylinder(const std::string& name,double x,double y,double z,double r,double h,int segments,
                SUMaterialRef material,SULayerRef tag) {
    Mesh(name, MakeVerticalCylinder(x,y,z,r,h,segments), material, tag);
  }
};

std::string StagePrefix(int stage) {
  std::ostringstream out;
  out << "ETAPA_" << std::setw(2) << std::setfill('0') << stage << "_";
  return out.str();
}

std::string StageName(int stage) {
  static const char* names[] = {
    "",
    "01_Fundaciones",
    "02_Riostras_y_anclajes",
    "03_Alcantarillado_enterrado",
    "04_Radier_y_base_de_piso",
    "05_Estructura_metalica",
    "06_Entrepiso_superior",
    "07_Perfileria_de_muros",
    "08_Instalaciones_interiores",
    "09_Revestimientos",
    "10_Artefactos_y_terminaciones",
    "11_Modelo_final"
  };
  return names[stage];
}

void AddDigit(Builder& b, int digit, double x, double y, double z, double scale,
              SULayerRef tag, const std::string& prefix) {
  // Seven segment digit lying vertically in the X-Z plane.
  static const bool seg[10][7] = {
    {1,1,1,1,1,1,0},{0,1,1,0,0,0,0},{1,1,0,1,1,0,1},{1,1,1,1,0,0,1},{0,1,1,0,0,1,1},
    {1,0,1,1,0,1,1},{1,0,1,1,1,1,1},{1,1,1,0,0,0,0},{1,1,1,1,1,1,1},{1,1,1,1,0,1,1}
  };
  const double t=0.06*scale, w=0.42*scale, h=0.75*scale, d=0.04*scale;
  auto horiz=[&](int id,double xx,double zz){b.Box(prefix+"NUM_H"+std::to_string(id),xx,y,zz,w,d,t,b.p.label,tag);};
  auto vert=[&](int id,double xx,double zz){b.Box(prefix+"NUM_V"+std::to_string(id),xx,y,zz,t,d,h/2.0-t/2.0,b.p.label,tag);};
  if(seg[digit][0]) horiz(0,x,z+h-t);
  if(seg[digit][1]) vert(1,x+w-t,z+h/2.0);
  if(seg[digit][2]) vert(2,x+w-t,z+t);
  if(seg[digit][3]) horiz(3,x,z);
  if(seg[digit][4]) vert(4,x,z+t);
  if(seg[digit][5]) vert(5,x,z+h/2.0);
  if(seg[digit][6]) horiz(6,x,z+h/2.0-t/2.0);
}

void AddStageMarker(Builder& b, int stage, double ox, double oy, SULayerRef tag) {
  const std::string pre=StagePrefix(stage);
  b.Box(pre+"Placa_numero",ox-0.10,oy-0.52,0.00,1.00,0.10,1.02,b.p.black,tag);
  if(stage<10) {
    AddDigit(b,stage,ox+0.23,oy-0.535,0.12,1.0,tag,pre);
  } else {
    AddDigit(b,1,ox+0.08,oy-0.535,0.12,0.9,tag,pre+"D1_");
    AddDigit(b,stage-10,ox+0.48,oy-0.535,0.12,0.9,tag,pre+"D2_");
  }
  b.Box(pre+"Base_presentacion",ox-0.18,oy-0.18,-0.035,1.56,2.56,0.035,b.p.label,tag);
}

void AddFootings(Builder& b, const std::string& pre, double ox,double oy,SULayerRef tag) {
  const std::array<std::array<double,2>,4> pts={{{-0.20,-0.20},{0.90,-0.20},{-0.20,1.90},{0.90,1.90}}};
  for(int i=0;i<4;++i){
    const double x=ox+pts[i][0], y=oy+pts[i][1];
    b.Box(pre+"Emplantillado_"+std::to_string(i+1),x-0.03,y-0.03,-0.67,0.56,0.56,0.05,b.p.gravel,tag);
    b.Box(pre+"Fundacion_"+std::to_string(i+1),x,y,-0.62,0.50,0.50,0.62,b.p.concrete,tag);
    for(double dx:{0.10,0.40}) for(double dy:{0.10,0.40})
      b.Cylinder(pre+"Barra_fundacion",x+dx,y+dy,-0.56,0.006,0.73,12,b.p.rebar,tag);
    for(int k=0;k<4;++k){
      double off=0.09+k*0.10;
      b.Pipe(pre+"Malla_fundacion_X",P(x+0.06,y+off,-0.50),P(x+0.44,y+off,-0.50),0.005,10,b.p.rebar,tag);
      b.Pipe(pre+"Malla_fundacion_Y",P(x+off,y+0.06,-0.50),P(x+off,y+0.44,-0.50),0.005,10,b.p.rebar,tag);
    }
  }
}

void AddRiostras(Builder& b,const std::string& pre,double ox,double oy,SULayerRef tag){
  b.Box(pre+"Riostra_frontal",ox+0.00,oy+0.00,-0.30,1.20,0.20,0.30,b.p.concrete,tag);
  b.Box(pre+"Riostra_trasera",ox+0.00,oy+2.00,-0.30,1.20,0.20,0.30,b.p.concrete,tag);
  b.Box(pre+"Riostra_izquierda",ox+0.00,oy+0.20,-0.30,0.20,1.80,0.30,b.p.concrete,tag);
  b.Box(pre+"Riostra_derecha",ox+1.00,oy+0.20,-0.30,0.20,1.80,0.30,b.p.concrete,tag);
  for(double z:{-0.245,-0.065}){
    b.Pipe(pre+"Acero_riostra_F",P(ox+0.08,oy+0.07,z),P(ox+1.12,oy+0.07,z),0.006,12,b.p.rebar,tag);
    b.Pipe(pre+"Acero_riostra_T",P(ox+0.08,oy+2.13,z),P(ox+1.12,oy+2.13,z),0.006,12,b.p.rebar,tag);
    b.Pipe(pre+"Acero_riostra_I",P(ox+0.07,oy+0.13,z),P(ox+0.07,oy+2.07,z),0.006,12,b.p.rebar,tag);
    b.Pipe(pre+"Acero_riostra_D",P(ox+1.13,oy+0.13,z),P(ox+1.13,oy+2.07,z),0.006,12,b.p.rebar,tag);
  }
  const std::array<std::array<double,2>,4> cols={{{0.00,0.00},{1.10,0.00},{0.00,2.10},{1.10,2.10}}};
  for(int i=0;i<4;++i){
    const double cx=ox+cols[i][0]+0.05, cy=oy+cols[i][1]+0.05;
    b.Box(pre+"Grout_placa_"+std::to_string(i+1),cx-0.105,cy-0.105,0.100,0.21,0.21,0.010,b.p.cement,tag);
    b.Box(pre+"Placa_base_"+std::to_string(i+1),cx-0.10,cy-0.10,0.110,0.20,0.20,0.010,b.p.steel,tag);
    for(double dx:{-0.073,0.073}) for(double dy:{-0.073,0.073})
      b.Cylinder(pre+"Perno_M16",cx+dx,cy+dy,0.075,0.008,0.095,12,b.p.steel,tag);
  }
}

void AddBuriedDrainage(Builder& b,const std::string& pre,double ox,double oy,SULayerRef tag){
  b.Pipe(pre+"Colector_110",P(ox+0.25,oy+1.95,-0.10),P(ox+0.25,oy+2.70,-0.12),0.055,24,b.p.sewer,tag);
  b.Pipe(pre+"Ramal_WC_110",P(ox+0.36,oy+0.92,-0.06),P(ox+0.25,oy+1.95,-0.10),0.055,24,b.p.sewer,tag);
  b.Pipe(pre+"Ducha_vertical_50",P(ox+0.62,oy+1.72,0.15),P(ox+0.62,oy+1.72,-0.07),0.025,20,b.p.sewer,tag);
  b.Pipe(pre+"Ramal_ducha_50",P(ox+0.62,oy+1.72,-0.07),P(ox+0.25,oy+1.92,-0.08),0.025,20,b.p.sewer,tag);
  b.Pipe(pre+"Lavamanos_40",P(ox+1.02,oy+0.42,0.35),P(ox+0.34,oy+1.90,0.18),0.020,18,b.p.sewer,tag);
  b.Pipe(pre+"Bajante_110",P(ox+0.25,oy+1.95,-0.10),P(ox+0.25,oy+1.95,2.95),0.055,24,b.p.sewer,tag);
  b.Pipe(pre+"Ventilacion_50",P(ox+0.34,oy+1.95,0.18),P(ox+0.34,oy+1.95,3.05),0.025,20,b.p.sewer,tag);
}

void AddRadier(Builder& b,const std::string& pre,double ox,double oy,SULayerRef tag,bool finished){
  b.Box(pre+"Base_estabilizada",ox+0.00,oy+0.00,-0.15,1.20,2.20,0.15,b.p.gravel,tag);
  b.Box(pre+"Barrera_humedad",ox+0.00,oy+0.00,-0.003,1.20,2.20,0.003,b.p.membrane,tag);
  b.Box(pre+"Radier_10cm",ox+0.00,oy+0.00,0.00,1.20,2.20,0.10,b.p.concrete,tag);
  if(finished){
    b.Box(pre+"Mortero_piso",ox+0.08,oy+0.08,0.10,1.04,2.04,0.025,b.p.cement,tag);
    b.Box(pre+"Impermeabilizacion_piso",ox+0.08,oy+0.08,0.125,1.04,2.04,0.003,b.p.membrane,tag);
    int n=1;
    for(double y=oy+0.10;y<oy+2.10-1e-6;y+=0.60){
      for(double x=ox+0.10;x<ox+1.10-1e-6;x+=0.30){
        b.Box(pre+"Ceramica_"+std::to_string(n++),x,y,0.128,0.296,0.596,0.010,b.p.tile,tag);
      }
    }
  }
}

void AddSteelFrame(Builder& b,const std::string& pre,double ox,double oy,SULayerRef tag){
  const std::array<std::array<double,2>,4> c={{{0,0},{1.10,0},{0,2.10},{1.10,2.10}}};
  for(int i=0;i<4;++i) b.Box(pre+"Columna_100x100_"+std::to_string(i+1),ox+c[i][0],oy+c[i][1],0.12,0.10,0.10,2.38,b.p.steel,tag);
  b.Box(pre+"Viga_frontal",ox+0.00,oy+0.00,2.40,1.20,0.10,0.10,b.p.steel,tag);
  b.Box(pre+"Viga_trasera",ox+0.00,oy+2.10,2.40,1.20,0.10,0.10,b.p.steel,tag);
  b.Box(pre+"Viga_izquierda",ox+0.00,oy+0.10,2.40,0.10,2.00,0.10,b.p.steel,tag);
  b.Box(pre+"Viga_derecha",ox+1.10,oy+0.10,2.40,0.10,2.00,0.10,b.p.steel,tag);
  b.Pipe(pre+"Arriostre_izquierdo",P(ox+0.05,oy+0.25,0.35),P(ox+0.05,oy+1.85,2.30),0.010,12,b.p.steel,tag);
  b.Pipe(pre+"Arriostre_derecho",P(ox+1.15,oy+1.85,0.35),P(ox+1.15,oy+0.25,2.30),0.010,12,b.p.steel,tag);
}

void AddUpperFloor(Builder& b,const std::string& pre,double ox,double oy,SULayerRef tag){
  int n=1;
  for(double y:{0.16,0.52,0.88,1.24,1.60,1.96}) b.Box(pre+"Vigueta_"+std::to_string(n++),ox+0.10,oy+y,2.40,1.00,0.05,0.10,b.p.steel,tag);
  b.Box(pre+"Terciado_21mm",ox,oy,2.50,1.20,2.20,0.021,b.p.wood,tag);
  b.Box(pre+"Placa_cementicia",ox,oy,2.521,1.20,2.20,0.010,b.p.cement,tag);
  b.Box(pre+"Membrana_superior",ox,oy,2.531,1.20,2.20,0.003,b.p.membrane,tag);
}

void AddWallFraming(Builder& b,const std::string& pre,double ox,double oy,SULayerRef tag){
  const double z=0.14,h=2.26;
  // Soleras laterales y trasera.
  b.Box(pre+"Solera_I_inf",ox+0.02,oy+0.10,z,0.06,2.00,0.025,b.p.galvanized,tag);
  b.Box(pre+"Solera_I_sup",ox+0.02,oy+0.10,2.375,0.06,2.00,0.025,b.p.galvanized,tag);
  b.Box(pre+"Solera_D_inf",ox+1.12,oy+0.10,z,0.06,2.00,0.025,b.p.galvanized,tag);
  b.Box(pre+"Solera_D_sup",ox+1.12,oy+0.10,2.375,0.06,2.00,0.025,b.p.galvanized,tag);
  for(double y:{0.10,0.50,0.90,1.30,1.70,2.06}){
    b.Box(pre+"Montante_I",ox+0.03,oy+y,z+0.025,0.04,0.035,h-0.05,b.p.galvanized,tag);
    b.Box(pre+"Montante_D",ox+1.13,oy+y,z+0.025,0.04,0.035,h-0.05,b.p.galvanized,tag);
  }
  b.Box(pre+"Solera_T_inf",ox+0.10,oy+2.12,z,1.00,0.06,0.025,b.p.galvanized,tag);
  b.Box(pre+"Solera_T_sup",ox+0.10,oy+2.12,2.375,1.00,0.06,0.025,b.p.galvanized,tag);
  for(double x:{0.10,0.45,0.80,1.06}) b.Box(pre+"Montante_T",ox+x,oy+2.13,z+0.025,0.035,0.04,h-0.05,b.p.galvanized,tag);
  // Frente con vano de puerta.
  b.Box(pre+"Jamba_I",ox+0.215,oy+0.03,z,0.035,0.04,2.03,b.p.galvanized,tag);
  b.Box(pre+"Jamba_D",ox+0.95,oy+0.03,z,0.035,0.04,2.03,b.p.galvanized,tag);
  b.Box(pre+"Dintel",ox+0.215,oy+0.03,2.145,0.77,0.04,0.06,b.p.galvanized,tag);
  // Columna falsa.
  b.Box(pre+"Columna_falsa_frente",ox+0.10,oy+1.80,z,0.30,0.012,2.15,b.p.galvanized,tag);
  b.Box(pre+"Columna_falsa_lateral",ox+0.388,oy+1.80,z,0.012,0.30,2.15,b.p.galvanized,tag);
}

void AddInteriorServices(Builder& b,const std::string& pre,double ox,double oy,SULayerRef tag){
  b.Pipe(pre+"AF_montante",P(ox+0.15,oy+1.88,0.18),P(ox+0.15,oy+1.88,2.86),0.010,14,b.p.cold,tag);
  b.Pipe(pre+"AC_montante",P(ox+0.19,oy+1.88,0.18),P(ox+0.19,oy+1.88,2.86),0.010,14,b.p.hot,tag);
  b.Pipe(pre+"AF_lavamanos",P(ox+0.15,oy+1.88,0.62),P(ox+1.00,oy+0.42,0.62),0.010,14,b.p.cold,tag);
  b.Pipe(pre+"AC_lavamanos",P(ox+0.19,oy+1.88,0.67),P(ox+0.96,oy+0.42,0.67),0.010,14,b.p.hot,tag);
  b.Pipe(pre+"AF_ducha",P(ox+0.15,oy+1.88,1.05),P(ox+0.56,oy+2.07,1.05),0.010,14,b.p.cold,tag);
  b.Pipe(pre+"AC_ducha",P(ox+0.19,oy+1.88,1.10),P(ox+0.64,oy+2.07,1.10),0.010,14,b.p.hot,tag);
  b.Pipe(pre+"AF_WC",P(ox+0.15,oy+1.88,0.40),P(ox+0.20,oy+1.03,0.40),0.010,14,b.p.cold,tag);
  b.Pipe(pre+"Conduit_interruptor",P(ox+1.075,oy+0.34,1.10),P(ox+1.075,oy+0.34,2.28),0.010,12,b.p.electric,tag);
  b.Pipe(pre+"Conduit_luz",P(ox+1.075,oy+0.34,2.28),P(ox+0.60,oy+1.02,2.28),0.010,12,b.p.electric,tag);
  b.Pipe(pre+"Conduit_extractor",P(ox+1.075,oy+0.34,2.18),P(ox+1.075,oy+1.38,2.10),0.010,12,b.p.electric,tag);
  b.Box(pre+"Caja_interruptor",ox+1.075,oy+0.27,1.02,0.02,0.12,0.16,b.p.black,tag);
  b.Box(pre+"Caja_enchufe",ox+1.075,oy+0.52,1.12,0.02,0.12,0.16,b.p.black,tag);
}

void AddCladding(Builder& b,const std::string& pre,double ox,double oy,SULayerRef tag){
  const double z=0.14,h=2.26;
  // Corte didáctico: muro izquierdo, trasero y mitad del derecho.
  b.Box(pre+"Fibro_exterior_I",ox+0.00,oy+0.08,z,0.008,2.04,h,b.p.wall,tag);
  b.Box(pre+"Fibro_exterior_T",ox+0.08,oy+2.192,z,1.04,0.008,h,b.p.wall,tag);
  b.Box(pre+"Fibro_exterior_D_parcial",ox+1.192,oy+1.15,z,0.008,0.97,h,b.p.wall,tag);
  b.Box(pre+"PVC_interior_I",ox+0.092,oy+0.10,z,0.008,2.00,h,b.p.pvc,tag);
  b.Box(pre+"PVC_interior_T",ox+0.10,oy+2.10,z,1.00,0.008,h,b.p.pvc,tag);
  b.Box(pre+"PVC_interior_D_parcial",ox+1.10,oy+1.15,z,0.008,0.95,h,b.p.pvc,tag);
  // Frente alrededor de la puerta.
  b.Box(pre+"Frente_I",ox+0.08,oy,z,0.17,0.008,2.05,b.p.wall,tag);
  b.Box(pre+"Frente_D",ox+0.95,oy,z,0.17,0.008,2.05,b.p.wall,tag);
  b.Box(pre+"Dintel_frente",ox+0.25,oy,2.197,0.70,0.008,0.203,b.p.wall,tag);
  b.Box(pre+"Columna_falsa_PVC",ox+0.10,oy+1.80,z,0.30,0.012,2.15,b.p.pvc,tag);
}

void AddFixtures(Builder& b,const std::string& pre,double ox,double oy,SULayerRef tag,bool final_stage){
  // WC simplificado pero redondeado.
  b.Mesh(pre+"WC_pedestal",MakeEllipticFrustum(ox+0.36,oy+0.91,0.14,0.115,0.20,0.155,0.245,0.27,28),b.p.sanitary,tag);
  b.Mesh(pre+"WC_taza",MakeEllipticFrustum(ox+0.36,oy+0.93,0.41,0.155,0.245,0.19,0.285,0.10,30),b.p.sanitary,tag);
  b.Mesh(pre+"WC_aro",MakeEllipticTorus(ox+0.36,oy+0.93,0.515,0.15,0.235,0.026,32,9),b.p.sanitary,tag);
  b.Box(pre+"WC_estanque",ox+0.19,oy+1.135,0.50,0.34,0.18,0.46,b.p.sanitary,tag);
  // Lavamanos.
  b.Mesh(pre+"Lavamanos_pedestal",MakeEllipticFrustum(ox+0.87,oy+0.39,0.14,0.075,0.085,0.105,0.115,0.60,26),b.p.sanitary,tag);
  b.Mesh(pre+"Lavamanos_cubeta",MakeEllipticFrustum(ox+0.87,oy+0.39,0.74,0.165,0.125,0.22,0.165,0.11,30),b.p.sanitary,tag);
  b.Cylinder(pre+"Griferia_base",ox+0.99,oy+0.39,0.85,0.025,0.09,18,b.p.chrome,tag);
  // Ducha y mampara.
  b.Box(pre+"Receptaculo",ox+0.10,oy+1.22,0.14,1.00,0.85,0.06,b.p.sanitary,tag);
  b.Box(pre+"Mampara",ox+0.56,oy+1.205,0.20,0.54,0.010,1.86,b.p.glass,tag);
  b.Pipe(pre+"Barra_ducha",P(ox+0.61,oy+2.065,1.18),P(ox+0.61,oy+2.065,2.02),0.012,16,b.p.chrome,tag);
  b.Pipe(pre+"Brazo_ducha",P(ox+0.61,oy+2.065,2.02),P(ox+0.61,oy+1.92,2.02),0.012,16,b.p.chrome,tag);
  b.Pipe(pre+"Rociador",P(ox+0.52,oy+1.92,2.02),P(ox+0.70,oy+1.92,2.02),0.035,22,b.p.chrome,tag);
  // Puerta abierta.
  const double a=-35.0*kPi/180.0;
  b.Mesh(pre+"Puerta_abierta",MakeRotatedBoxZ(ox+0.25,oy+0.105,0.16,0.70,0.04,2.00,a),b.p.door,tag);
  if(final_stage){
    b.Box(pre+"Espejo",ox+1.067,oy+0.195,1.075,0.008,0.45,0.67,b.p.glass,tag);
    b.Pipe(pre+"Toallero",P(ox+1.065,oy+0.82,1.28),P(ox+1.065,oy+1.25,1.28),0.010,16,b.p.black,tag);
    b.Pipe(pre+"Portarrollos",P(ox+0.14,oy+0.69,0.72),P(ox+0.14,oy+0.86,0.72),0.012,16,b.p.black,tag);
    b.Cylinder(pre+"Lampara",ox+0.60,oy+1.02,2.25,0.095,0.03,28,b.p.pvc,tag);
    b.Box(pre+"Extractor",ox+1.045,oy+1.28,1.98,0.05,0.20,0.20,b.p.pvc,tag);
    int n=1;
    for(double x=ox+0.10;x<ox+1.10-1e-6;x+=0.20){
      b.Box(pre+"Cielo_PVC_"+std::to_string(n++),x,oy+0.10,2.27,0.197,2.00,0.012,b.p.pvc,tag);
    }
  }
}

void BuildStage(Builder& b,int stage,double ox,double oy,SULayerRef tag){
  const std::string pre=StagePrefix(stage);
  AddStageMarker(b,stage,ox,oy,tag);
  if(stage>=1) AddFootings(b,pre,ox,oy,tag);
  if(stage>=2) AddRiostras(b,pre,ox,oy,tag);
  if(stage>=3) AddBuriedDrainage(b,pre,ox,oy,tag);
  if(stage>=4) AddRadier(b,pre,ox,oy,tag,stage>=10);
  if(stage>=5) AddSteelFrame(b,pre,ox,oy,tag);
  if(stage>=6) AddUpperFloor(b,pre,ox,oy,tag);
  if(stage>=7) AddWallFraming(b,pre,ox,oy,tag);
  if(stage>=8) AddInteriorServices(b,pre,ox,oy,tag);
  if(stage>=9) AddCladding(b,pre,ox,oy,tag);
  if(stage>=10) AddFixtures(b,pre,ox,oy,tag,stage>=11);
}

int main(int argc,char** argv){
  const std::string output=argc>1?argv[1]:"bano_secuencia_constructiva.skp";
  const int requested_stage=argc>2?std::stoi(argv[2]):0; // 0 = all stages in one file

  SUInitialize();
  SUModelRef model=SU_INVALID;
  if(!Check(SUModelCreate(&model),"SUModelCreate")) return 1;
  SUEntitiesRef entities=SU_INVALID;
  if(!Check(SUModelGetEntities(model,&entities),"SUModelGetEntities")) return 1;

  Palette p{
    CreateMaterial(model,"Hormigon H25",145,145,142),
    CreateMaterial(model,"Base estabilizada",145,126,100),
    CreateMaterial(model,"Acero de refuerzo",118,52,42),
    CreateMaterial(model,"Acero estructural",48,55,61),
    CreateMaterial(model,"Perfileria galvanizada",170,180,186),
    CreateMaterial(model,"Terciado estructural",171,120,72),
    CreateMaterial(model,"Mortero y placa cementicia",191,190,181),
    CreateMaterial(model,"Impermeabilizacion",42,132,184,190),
    CreateMaterial(model,"Fibrocemento exterior",218,214,202),
    CreateMaterial(model,"PVC interior",242,240,232),
    CreateMaterial(model,"Ceramica",194,199,197),
    CreateMaterial(model,"Loza sanitaria",250,250,247),
    CreateMaterial(model,"Cromo",173,185,190),
    CreateMaterial(model,"Vidrio",119,190,212,90),
    CreateMaterial(model,"Puerta",102,72,49),
    CreateMaterial(model,"PVC sanitario",105,83,62),
    CreateMaterial(model,"Agua fria",40,104,213),
    CreateMaterial(model,"Agua caliente",211,58,45),
    CreateMaterial(model,"Canalizacion electrica",239,190,45),
    CreateMaterial(model,"Accesorios negros",38,39,40),
    CreateMaterial(model,"Marcadores de etapa",213,146,45)
  };

  Builder b{entities,p,true};
  if(requested_stage>=1 && requested_stage<=11){
    SULayerRef tag=CreateLayer(model,StageName(requested_stage));
    BuildStage(b,requested_stage,0.0,0.0,tag);
  }else{
    // 3 columns x 4 rows. Each stage is cumulative and has its own Tag.
    for(int stage=1;stage<=11;++stage){
      const int col=(stage-1)%3;
      const int row=(stage-1)/3;
      const double ox=col*2.70;
      const double oy=row*3.10;
      SULayerRef tag=CreateLayer(model,StageName(stage));
      BuildStage(b,stage,ox,oy,tag);
    }
  }

  if(!b.ok){SUModelRelease(&model);SUTerminate();return 1;}
  const SUResult result=SUModelSaveToFileWithVersion(model,output.c_str(),SUModelVersion_SU2021);
  if(!Check(result,"SUModelSaveToFileWithVersion")){SUModelRelease(&model);SUTerminate();return 1;}
  SUModelRelease(&model);
  SUTerminate();
  return 0;
}
