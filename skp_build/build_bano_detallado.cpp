#include "skp_helpers.hpp"

#include <iomanip>
#include <sstream>

using namespace bath;

std::string N(const std::string& prefix, int index) {
  std::ostringstream out;
  out << prefix << std::setw(2) << std::setfill('0') << index;
  return out.str();
}

int main(int argc, char** argv) {
  const std::string output = argc > 1 ? argv[1] : "bano_1x2_detallado.skp";

  SUInitialize();
  SUModelRef model = SU_INVALID;
  if (!Check(SUModelCreate(&model), "SUModelCreate")) return 1;
  SUEntitiesRef entities = SU_INVALID;
  if (!Check(SUModelGetEntities(model, &entities), "SUModelGetEntities")) return 1;

  // Materiales diferenciados para lectura constructiva y presentación.
  const SUMaterialRef mat_concrete = CreateMaterial(model, "Hormigon H25", 145,145,142);
  const SUMaterialRef mat_gravel = CreateMaterial(model, "Base estabilizada", 145,126,100);
  const SUMaterialRef mat_rebar = CreateMaterial(model, "Acero de refuerzo", 118,52,42);
  const SUMaterialRef mat_steel = CreateMaterial(model, "Acero estructural grafito", 50,57,63);
  const SUMaterialRef mat_galv = CreateMaterial(model, "Perfileria galvanizada", 170,180,186);
  const SUMaterialRef mat_plate = CreateMaterial(model, "Placas y pernos", 72,78,82);
  const SUMaterialRef mat_wood = CreateMaterial(model, "Terciado estructural", 171,120,72);
  const SUMaterialRef mat_cement = CreateMaterial(model, "Placa cementicia", 191,190,181);
  const SUMaterialRef mat_membrane = CreateMaterial(model, "Membrana impermeable", 42,132,184,210);
  const SUMaterialRef mat_fibro = CreateMaterial(model, "Fibrocemento exterior", 218,214,202);
  const SUMaterialRef mat_pvc = CreateMaterial(model, "PVC interior blanco", 242,240,232);
  const SUMaterialRef mat_tile = CreateMaterial(model, "Ceramica antideslizante", 194,199,197);
  const SUMaterialRef mat_grout = CreateMaterial(model, "Fragüe gris claro", 150,154,153);
  const SUMaterialRef mat_sanitary = CreateMaterial(model, "Loza sanitaria", 250,250,247);
  const SUMaterialRef mat_chrome = CreateMaterial(model, "Cromo", 173,185,190);
  const SUMaterialRef mat_glass = CreateMaterial(model, "Vidrio templado", 119,190,212,95);
  const SUMaterialRef mat_door = CreateMaterial(model, "Puerta resistente humedad", 102,72,49);
  const SUMaterialRef mat_sewer = CreateMaterial(model, "PVC sanitario", 105,83,62);
  const SUMaterialRef mat_cold = CreateMaterial(model, "Agua fria", 40,104,213);
  const SUMaterialRef mat_hot = CreateMaterial(model, "Agua caliente", 211,58,45);
  const SUMaterialRef mat_electrical = CreateMaterial(model, "Canalizacion electrica", 239,190,45);
  const SUMaterialRef mat_black = CreateMaterial(model, "Accesorios negros", 38,39,40);
  const SUMaterialRef mat_mirror = CreateMaterial(model, "Espejo", 175,209,219,180);
  const SUMaterialRef mat_green = CreateMaterial(model, "Aislacion lana mineral", 176,191,143,180);

  // Tags profesionales para encender/apagar sistemas en SketchUp Web.
  const SULayerRef tag_found = CreateLayer(model, "01_Fundaciones_y_radier");
  const SULayerRef tag_rebar = CreateLayer(model, "02_Armaduras_y_anclajes");
  const SULayerRef tag_steel = CreateLayer(model, "03_Estructura_metalica");
  const SULayerRef tag_upper = CreateLayer(model, "04_Entrepiso_superior");
  const SULayerRef tag_frame = CreateLayer(model, "05_Perfileria_muros");
  const SULayerRef tag_clad = CreateLayer(model, "06_Revestimientos");
  const SULayerRef tag_sewer = CreateLayer(model, "07_Alcantarillado");
  const SULayerRef tag_water = CreateLayer(model, "08_Agua_fria_y_caliente");
  const SULayerRef tag_elec = CreateLayer(model, "09_Electricidad_y_ventilacion");
  const SULayerRef tag_fixture = CreateLayer(model, "10_Artefactos_sanitarios");
  const SULayerRef tag_access = CreateLayer(model, "11_Accesorios");
  const SULayerRef tag_ceiling = CreateLayer(model, "12_Cielo_falso");

  bool ok = true;
  auto mesh = [&](const std::string& name, const Mesh& m, SUMaterialRef material, SULayerRef tag) {
    if (ok) ok = AddMesh(entities, name, m, material, tag);
  };
  auto box = [&](const std::string& name, double x,double y,double z,double w,double d,double h,
                 SUMaterialRef material,SULayerRef tag) {
    mesh(name, MakeBox(x,y,z,w,d,h), material, tag);
  };
  auto rbox = [&](const std::string& name, double x,double y,double z,double w,double d,double h,double a,
                  SUMaterialRef material,SULayerRef tag) {
    mesh(name, MakeRotatedBoxZ(x,y,z,w,d,h,a), material, tag);
  };
  auto pipe = [&](const std::string& name, Point a,Point b,double radius,int seg,
                  SUMaterialRef material,SULayerRef tag) {
    mesh(name, MakeCylinderBetween(a,b,radius,seg), material, tag);
  };
  auto cyl = [&](const std::string& name,double x,double y,double z,double r,double h,int seg,
                 SUMaterialRef material,SULayerRef tag) {
    mesh(name, MakeVerticalCylinder(x,y,z,r,h,seg), material, tag);
  };

  // ---------------------------------------------------------------------------
  // 01. Fundaciones, riostras, suelo y capas del piso terminado.
  // ---------------------------------------------------------------------------
  const std::array<std::array<double,2>,4> footing_xy = {{{-0.20,-0.20},{0.90,-0.20},{-0.20,1.90},{0.90,1.90}}};
  for (int i=0;i<4;++i) {
    box(N("01_Fundacion_H25_",i+1), footing_xy[i][0],footing_xy[i][1],-0.62,0.50,0.50,0.62,mat_concrete,tag_found);
    box(N("01_Emplantillado_",i+1), footing_xy[i][0]-0.03,footing_xy[i][1]-0.03,-0.67,0.56,0.56,0.05,mat_gravel,tag_found);
  }
  box("01_Riostra_frontal_20x30",0.00,0.00,-0.30,1.20,0.20,0.30,mat_concrete,tag_found);
  box("01_Riostra_trasera_20x30",0.00,2.00,-0.30,1.20,0.20,0.30,mat_concrete,tag_found);
  box("01_Riostra_izquierda_20x30",0.00,0.20,-0.30,0.20,1.80,0.30,mat_concrete,tag_found);
  box("01_Riostra_derecha_20x30",1.00,0.20,-0.30,0.20,1.80,0.30,mat_concrete,tag_found);
  box("01_Base_estabilizada_compactada",0.00,0.00,-0.15,1.20,2.20,0.15,mat_gravel,tag_found);
  box("01_Barrera_humedad_polietileno",0.00,0.00,-0.003,1.20,2.20,0.003,mat_membrane,tag_found);
  box("01_Radier_armado_10cm",0.00,0.00,0.00,1.20,2.20,0.10,mat_concrete,tag_found);
  box("01_Mortero_nivelacion",0.08,0.08,0.10,1.04,2.04,0.028,mat_grout,tag_found);
  box("01_Impermeabilizacion_piso",0.08,0.08,0.128,1.04,2.04,0.003,mat_membrane,tag_found);
  box("01_Base_fragüe_piso",0.10,0.10,0.131,1.00,2.00,0.006,mat_grout,tag_found);

  // Cerámica 30x60 cm con junta visible.
  int tile_id=1;
  const double gap=0.004;
  for (double y=0.10; y<2.10-1e-6; y+=0.60) {
    const double d=std::min(0.60-gap,2.10-y-gap);
    for (double x=0.10; x<1.10-1e-6; x+=0.30) {
      const double w=std::min(0.30-gap,1.10-x-gap);
      if (w>0.03 && d>0.03) box(N("01_Ceramica_30x60_",tile_id++),x,y,0.137,w,d,0.010,mat_tile,tag_found);
    }
  }

  // Armaduras de fundación y riostra; visibles al ocultar el hormigón.
  for (int f=0;f<4;++f) {
    const double fx=footing_xy[f][0], fy=footing_xy[f][1];
    for (double dx : {0.10,0.40}) for (double dy : {0.10,0.40}) {
      cyl(N("02_Barra_vertical_fundacion_",f*4 + (dx>0.2?2:0)+(dy>0.2?1:0)+1),fx+dx,fy+dy,-0.55,0.006,0.70,12,mat_rebar,tag_rebar);
    }
    for (int k=0;k<4;++k) {
      const double off=0.09+k*0.10;
      pipe(N("02_Malla_X_fundacion_",f*4+k+1),P(fx+0.06,fy+off,-0.50),P(fx+0.44,fy+off,-0.50),0.005,10,mat_rebar,tag_rebar);
      pipe(N("02_Malla_Y_fundacion_",f*4+k+1),P(fx+off,fy+0.06,-0.50),P(fx+off,fy+0.44,-0.50),0.005,10,mat_rebar,tag_rebar);
    }
  }
  for (double z : {-0.245,-0.065}) {
    pipe("02_Riostra_frontal_barra",P(0.08,0.07,z),P(1.12,0.07,z),0.006,12,mat_rebar,tag_rebar);
    pipe("02_Riostra_trasera_barra",P(0.08,2.13,z),P(1.12,2.13,z),0.006,12,mat_rebar,tag_rebar);
    pipe("02_Riostra_izquierda_barra",P(0.07,0.13,z),P(0.07,2.07,z),0.006,12,mat_rebar,tag_rebar);
    pipe("02_Riostra_derecha_barra",P(1.13,0.13,z),P(1.13,2.07,z),0.006,12,mat_rebar,tag_rebar);
  }

  // Placas base 200x200x10, grout, pernos M16 y tuercas hexagonales.
  const std::array<std::array<double,2>,4> column_xy = {{{0.00,0.00},{1.10,0.00},{0.00,2.10},{1.10,2.10}}};
  int bolt_id=1;
  for (int i=0;i<4;++i) {
    const double cx=column_xy[i][0]+0.05, cy=column_xy[i][1]+0.05;
    box(N("02_Grout_placa_base_",i+1),cx-0.105,cy-0.105,0.100,0.21,0.21,0.010,mat_grout,tag_rebar);
    box(N("02_Placa_base_200x200x10_",i+1),cx-0.10,cy-0.10,0.110,0.20,0.20,0.010,mat_plate,tag_rebar);
    for (double dx : {-0.073,0.073}) for (double dy : {-0.073,0.073}) {
      cyl(N("02_Perno_M16_",bolt_id),cx+dx,cy+dy,0.075,0.008,0.095,12,mat_plate,tag_rebar);
      mesh(N("02_Tuerca_M16_",bolt_id++),MakePolygonPrism(cx+dx,cy+dy,0.146,0.014,0.010,6,kPi/6.0),mat_plate,tag_rebar);
    }
  }

  // ---------------------------------------------------------------------------
  // 03. Estructura principal de acero y arriostramiento.
  // ---------------------------------------------------------------------------
  for (int i=0;i<4;++i) {
    box(N("03_Columna_100x100x3_",i+1),column_xy[i][0],column_xy[i][1],0.120,0.10,0.10,2.38,mat_steel,tag_steel);
    box(N("03_Cartela_columna_",i+1),column_xy[i][0]-0.025,column_xy[i][1]+0.01,0.12,0.15,0.008,0.18,mat_plate,tag_steel);
  }
  box("03_Viga_frontal_100x50x3",0.00,0.00,2.40,1.20,0.10,0.10,mat_steel,tag_steel);
  box("03_Viga_trasera_100x50x3",0.00,2.10,2.40,1.20,0.10,0.10,mat_steel,tag_steel);
  box("03_Viga_izquierda_100x50x3",0.00,0.10,2.40,0.10,2.00,0.10,mat_steel,tag_steel);
  box("03_Viga_derecha_100x50x3",1.10,0.10,2.40,0.10,2.00,0.10,mat_steel,tag_steel);
  pipe("03_Arriostre_lateral_izq",P(0.05,0.25,0.35),P(0.05,1.85,2.30),0.010,12,mat_steel,tag_steel);
  pipe("03_Arriostre_lateral_der",P(1.15,1.85,0.35),P(1.15,0.25,2.30),0.010,12,mat_steel,tag_steel);

  // ---------------------------------------------------------------------------
  // 04. Entrepiso preparado para el segundo baño.
  // ---------------------------------------------------------------------------
  int joist_id=1;
  for (double y : {0.16,0.52,0.88,1.24,1.60,1.96}) {
    box(N("04_Vigueta_100x50x2_",joist_id++),0.10,y,2.40,1.00,0.05,0.10,mat_steel,tag_upper);
  }
  box("04_Terciado_estructural_21mm",0.00,0.00,2.50,1.20,2.20,0.021,mat_wood,tag_upper);
  box("04_Placa_cementicia_10mm",0.00,0.00,2.521,1.20,2.20,0.010,mat_cement,tag_upper);
  box("04_Impermeabilizacion_superior",0.00,0.00,2.531,1.20,2.20,0.003,mat_membrane,tag_upper);
  box("04_Banda_perimetral_impermeable_izq",0.00,0.00,2.531,0.025,2.20,0.12,mat_membrane,tag_upper);
  box("04_Banda_perimetral_impermeable_der",1.175,0.00,2.531,0.025,2.20,0.12,mat_membrane,tag_upper);
  box("04_Banda_perimetral_impermeable_tras",0.00,2.175,2.531,1.20,0.025,0.12,mat_membrane,tag_upper);

  // ---------------------------------------------------------------------------
  // 05. Perfilería Metalcon 60 mm: soleras, montantes y refuerzos.
  // ---------------------------------------------------------------------------
  const double wall_z=0.147, wall_h=2.253;
  box("05_Solera_inferior_izq",0.020,0.10,wall_z,0.060,2.00,0.025,mat_galv,tag_frame);
  box("05_Solera_superior_izq",0.020,0.10,2.375,0.060,2.00,0.025,mat_galv,tag_frame);
  box("05_Solera_inferior_der",1.120,0.10,wall_z,0.060,2.00,0.025,mat_galv,tag_frame);
  box("05_Solera_superior_der",1.120,0.10,2.375,0.060,2.00,0.025,mat_galv,tag_frame);
  for (double y : {0.10,0.50,0.90,1.30,1.70,2.06}) {
    box("05_Montante_izq",0.030,y,wall_z+0.025,0.040,0.035,wall_h-0.05,mat_galv,tag_frame);
    box("05_Montante_der",1.130,y,wall_z+0.025,0.040,0.035,wall_h-0.05,mat_galv,tag_frame);
  }
  box("05_Solera_inferior_tras",0.10,2.120,wall_z,1.00,0.060,0.025,mat_galv,tag_frame);
  box("05_Solera_superior_tras",0.10,2.120,2.375,1.00,0.060,0.025,mat_galv,tag_frame);
  for (double x : {0.10,0.45,0.80,1.06}) box("05_Montante_trasero",x,2.130,wall_z+0.025,0.035,0.040,wall_h-0.05,mat_galv,tag_frame);
  // Frente y vano de 70 cm.
  box("05_Solera_frontal_izq",0.10,0.020,wall_z,0.15,0.060,0.025,mat_galv,tag_frame);
  box("05_Solera_frontal_der",0.95,0.020,wall_z,0.15,0.060,0.025,mat_galv,tag_frame);
  box("05_Montante_jamba_izq",0.215,0.030,wall_z,0.035,0.040,2.03,mat_galv,tag_frame);
  box("05_Montante_jamba_der",0.950,0.030,wall_z,0.035,0.040,2.03,mat_galv,tag_frame);
  box("05_Dintel_metalcon_puerta",0.215,0.030,2.145,0.770,0.040,0.060,mat_galv,tag_frame);
  // Refuerzos para lavamanos, mampara y accesorios.
  box("05_Refuerzo_lavamanos",1.125,0.20,0.72,0.050,0.55,0.10,mat_wood,tag_frame);
  box("05_Refuerzo_mampara",0.54,1.185,0.20,0.12,0.045,1.95,mat_wood,tag_frame);
  box("05_Refuerzo_toallero",1.125,0.82,1.10,0.050,0.55,0.08,mat_wood,tag_frame);

  // Aislación en paños, ligeramente transparente.
  box("05_Aislacion_muro_izquierdo",0.025,0.13,0.20,0.050,1.94,2.10,mat_green,tag_frame);
  box("05_Aislacion_muro_derecho",1.125,0.13,0.20,0.050,1.94,2.10,mat_green,tag_frame);
  box("05_Aislacion_muro_trasero",0.13,2.125,0.20,0.94,0.050,2.10,mat_green,tag_frame);

  // ---------------------------------------------------------------------------
  // 06. Revestimientos exteriores, interiores, columna falsa y puerta.
  // ---------------------------------------------------------------------------
  box("06_Fibrocemento_exterior_izq",0.000,0.08,wall_z,0.008,2.04,wall_h,mat_fibro,tag_clad);
  box("06_Fibrocemento_exterior_der",1.192,0.08,wall_z,0.008,2.04,wall_h,mat_fibro,tag_clad);
  box("06_Fibrocemento_exterior_tras",0.08,2.192,wall_z,1.04,0.008,wall_h,mat_fibro,tag_clad);
  box("06_Fibrocemento_frente_izq",0.08,0.000,wall_z,0.17,0.008,2.05,mat_fibro,tag_clad);
  box("06_Fibrocemento_frente_der",0.95,0.000,wall_z,0.17,0.008,2.05,mat_fibro,tag_clad);
  box("06_Fibrocemento_dintel",0.25,0.000,2.197,0.70,0.008,0.203,mat_fibro,tag_clad);
  // Membrana hidrófuga detrás del fibrocemento.
  box("06_Membrana_fachada_izq",0.009,0.08,wall_z,0.002,2.04,wall_h,mat_membrane,tag_clad);
  box("06_Membrana_fachada_der",1.189,0.08,wall_z,0.002,2.04,wall_h,mat_membrane,tag_clad);
  box("06_Membrana_fachada_tras",0.08,2.189,wall_z,1.04,0.002,wall_h,mat_membrane,tag_clad);
  // Paneles PVC interiores con juntas verticales.
  int panel=1;
  for (double y=0.10;y<2.10-1e-6;y+=0.25) {
    const double d=std::min(0.247,2.10-y);
    box(N("06_PVC_interior_izq_",panel),0.092,y,wall_z,0.008,d,wall_h,mat_pvc,tag_clad);
    box(N("06_PVC_interior_der_",panel++),1.100,y,wall_z,0.008,d,wall_h,mat_pvc,tag_clad);
  }
  panel=1;
  for (double x=0.10;x<1.10-1e-6;x+=0.25) {
    const double w=std::min(0.247,1.10-x);
    box(N("06_PVC_interior_tras_",panel++),x,2.100,wall_z,w,0.008,wall_h,mat_pvc,tag_clad);
  }
  // Placa cementicia impermeabilizada en zona de ducha.
  box("06_Placa_cementicia_ducha_tras",0.10,2.088,wall_z,1.00,0.010,2.15,mat_cement,tag_clad);
  box("06_Impermeabilizacion_ducha_tras",0.10,2.086,wall_z,1.00,0.002,2.15,mat_membrane,tag_clad);
  box("06_Placa_cementicia_ducha_izq",0.100,1.20,wall_z,0.010,0.90,2.15,mat_cement,tag_clad);
  box("06_Impermeabilizacion_ducha_izq",0.112,1.20,wall_z,0.002,0.90,2.15,mat_membrane,tag_clad);
  // Esquineros y goterones exteriores.
  box("06_Esquinero_exterior_izq_frente",-0.006,0.00,wall_z,0.018,0.018,wall_h,mat_plate,tag_clad);
  box("06_Esquinero_exterior_der_frente",1.188,0.00,wall_z,0.018,0.018,wall_h,mat_plate,tag_clad);
  box("06_Goteron_inferior_izq",-0.005,0.08,wall_z-0.015,0.018,2.04,0.020,mat_plate,tag_clad);
  box("06_Goteron_inferior_der",1.187,0.08,wall_z-0.015,0.018,2.04,0.020,mat_plate,tag_clad);

  // Columna falsa 30x30 cm con tapa de inspección.
  box("06_Columna_falsa_frente",0.10,1.80,wall_z,0.30,0.012,2.15,mat_pvc,tag_clad);
  box("06_Columna_falsa_lateral",0.388,1.80,wall_z,0.012,0.30,2.15,mat_pvc,tag_clad);
  box("06_Tapa_inspeccion_30x40",0.15,1.787,0.42,0.20,0.012,0.40,mat_pvc,tag_clad);
  box("06_Marco_tapa_inspeccion_superior",0.14,1.784,0.82,0.22,0.010,0.015,mat_black,tag_clad);
  box("06_Marco_tapa_inspeccion_inferior",0.14,1.784,0.405,0.22,0.010,0.015,mat_black,tag_clad);

  // Puerta abierta 35 grados, marco, bisagras, cerradura y manillas.
  const double door_angle=-35.0*kPi/180.0;
  box("06_Marco_puerta_jamba_izq",0.225,0.075,wall_z,0.025,0.045,2.05,mat_door,tag_clad);
  box("06_Marco_puerta_jamba_der",0.950,0.075,wall_z,0.025,0.045,2.05,mat_door,tag_clad);
  box("06_Marco_puerta_dintel",0.225,0.075,2.185,0.750,0.045,0.025,mat_door,tag_clad);
  rbox("06_Hoja_puerta_70x200",0.250,0.105,wall_z+0.02,0.70,0.040,2.00,door_angle,mat_door,tag_clad);
  for (double z : {0.40,1.15,1.90}) cyl("06_Bisagra_puerta",0.25,0.105,z,0.008,0.08,12,mat_chrome,tag_clad);
  const Point handle_a=P(0.25+0.58*std::cos(door_angle),0.105+0.58*std::sin(door_angle),1.03);
  const Point handle_b=Add(handle_a,P(-0.04*std::sin(door_angle),0.04*std::cos(door_angle),0));
  pipe("06_Manilla_puerta",handle_a,handle_b,0.010,14,mat_chrome,tag_clad);

  // ---------------------------------------------------------------------------
  // 07. Alcantarillado completo y ventilación.
  // ---------------------------------------------------------------------------
  pipe("07_Colector_110_salida",P(0.25,1.95,-0.10),P(0.25,2.75,-0.12),0.055,24,mat_sewer,tag_sewer);
  pipe("07_Ramal_WC_110",P(0.36,0.92,-0.06),P(0.25,1.95,-0.10),0.055,24,mat_sewer,tag_sewer);
  pipe("07_Bajante_principal_110",P(0.25,1.95,-0.10),P(0.25,1.95,2.95),0.055,24,mat_sewer,tag_sewer);
  pipe("07_Desague_ducha_vertical_50",P(0.62,1.72,0.13),P(0.62,1.72,-0.07),0.025,20,mat_sewer,tag_sewer);
  pipe("07_Ramal_ducha_50",P(0.62,1.72,-0.07),P(0.25,1.92,-0.08),0.025,20,mat_sewer,tag_sewer);
  pipe("07_Desague_lavamanos_40_vertical",P(1.02,0.42,0.70),P(1.02,0.42,0.30),0.020,18,mat_sewer,tag_sewer);
  pipe("07_Ramal_lavamanos_40",P(1.02,0.42,0.30),P(0.34,1.90,0.20),0.020,18,mat_sewer,tag_sewer);
  pipe("07_Ventilacion_sanitaria_50",P(0.34,1.95,0.20),P(0.34,1.95,3.05),0.025,20,mat_sewer,tag_sewer);
  cyl("07_Terminal_ventilacion",0.34,1.95,3.05,0.040,0.035,20,mat_sewer,tag_sewer);
  cyl("07_Registro_limpieza_110",0.25,1.84,0.25,0.060,0.035,24,mat_sewer,tag_sewer);
  cyl("07_Rejilla_ducha",0.62,1.72,0.147,0.045,0.008,28,mat_chrome,tag_sewer);
  cyl("07_Sumidero_piso",0.98,1.12,0.147,0.035,0.008,24,mat_chrome,tag_sewer);

  // ---------------------------------------------------------------------------
  // 08. Agua fría y caliente, derivaciones y llaves de corte.
  // ---------------------------------------------------------------------------
  pipe("08_Montante_agua_fria",P(0.15,1.88,0.18),P(0.15,1.88,2.86),0.010,14,mat_cold,tag_water);
  pipe("08_Montante_agua_caliente",P(0.19,1.88,0.18),P(0.19,1.88,2.86),0.010,14,mat_hot,tag_water);
  // Lavamanos.
  pipe("08_AF_lavamanos_horizontal",P(0.15,1.88,0.62),P(1.00,0.42,0.62),0.010,14,mat_cold,tag_water);
  pipe("08_AC_lavamanos_horizontal",P(0.19,1.88,0.67),P(0.96,0.42,0.67),0.010,14,mat_hot,tag_water);
  // Ducha.
  pipe("08_AF_ducha",P(0.15,1.88,1.05),P(0.56,2.07,1.05),0.010,14,mat_cold,tag_water);
  pipe("08_AC_ducha",P(0.19,1.88,1.10),P(0.64,2.07,1.10),0.010,14,mat_hot,tag_water);
  // WC solo agua fría.
  pipe("08_AF_WC",P(0.15,1.88,0.40),P(0.20,1.03,0.40),0.010,14,mat_cold,tag_water);
  // Llaves de corte generales con cuerpo y maneta.
  for (int i=0;i<2;++i) {
    const double x=i==0?0.15:0.19;
    const SUMaterialRef m=i==0?mat_cold:mat_hot;
    cyl(N("08_Llave_corte_cuerpo_",i+1),x,1.88,0.83,0.020,0.045,18,mat_chrome,tag_water);
    pipe(N("08_Llave_corte_maneta_",i+1),P(x-0.045,1.88,0.87),P(x+0.045,1.88,0.87),0.006,12,m,tag_water);
  }
  // Salidas preparadas para baño superior.
  cyl("08_Salida_AF_superior",0.15,1.88,2.86,0.015,0.08,16,mat_cold,tag_water);
  cyl("08_Salida_AC_superior",0.19,1.88,2.86,0.015,0.08,16,mat_hot,tag_water);

  // ---------------------------------------------------------------------------
  // 09. Electricidad, iluminación, extractor, interruptor y enchufe IP44.
  // ---------------------------------------------------------------------------
  pipe("09_Conduit_subida_interruptor",P(1.075,0.34,1.10),P(1.075,0.34,2.28),0.010,12,mat_electrical,tag_elec);
  pipe("09_Conduit_luz_cielo",P(1.075,0.34,2.28),P(0.60,1.02,2.28),0.010,12,mat_electrical,tag_elec);
  pipe("09_Conduit_extractor",P(1.075,0.34,2.18),P(1.075,1.38,2.10),0.010,12,mat_electrical,tag_elec);
  box("09_Caja_interruptor",1.075,0.27,1.02,0.020,0.12,0.16,mat_black,tag_elec);
  box("09_Tecla_interruptor",1.066,0.30,1.07,0.010,0.06,0.06,mat_pvc,tag_elec);
  box("09_Caja_enchufe_IP44",1.075,0.52,1.12,0.020,0.12,0.16,mat_black,tag_elec);
  cyl("09_Toma_enchufe_1",1.064,0.56,1.19,0.009,0.010,14,mat_black,tag_elec);
  cyl("09_Toma_enchufe_2",1.064,0.61,1.19,0.009,0.010,14,mat_black,tag_elec);
  // Lámpara circular IP65 y extractor con rejilla.
  cyl("09_Base_lampara_IP65",0.60,1.02,2.265,0.105,0.025,32,mat_pvc,tag_elec);
  cyl("09_Difusor_lampara",0.60,1.02,2.240,0.090,0.025,32,mat_mirror,tag_elec);
  pipe("09_Ducto_extractor_100",P(1.08,1.38,2.08),P(1.28,1.38,2.08),0.050,24,mat_pvc,tag_elec);
  box("09_Cuerpo_extractor",1.045,1.28,1.98,0.050,0.20,0.20,mat_pvc,tag_elec);
  for (int i=0;i<5;++i) box(N("09_Rejilla_extractor_",i+1),1.038,1.30+i*0.035,2.00,0.010,0.16,0.008,mat_black,tag_elec);

  // ---------------------------------------------------------------------------
  // 10. Artefactos sanitarios más realistas.
  // ---------------------------------------------------------------------------
  // WC: pie, taza, aro, asiento, estanque, tapa y botón.
  mesh("10_WC_pedestal",MakeEllipticFrustum(0.36,0.91,0.147,0.115,0.20,0.155,0.245,0.27,30),mat_sanitary,tag_fixture);
  mesh("10_WC_taza",MakeEllipticFrustum(0.36,0.93,0.417,0.155,0.245,0.190,0.285,0.10,32),mat_sanitary,tag_fixture);
  mesh("10_WC_aro",MakeEllipticTorus(0.36,0.93,0.520,0.150,0.235,0.026,34,10),mat_sanitary,tag_fixture);
  mesh("10_WC_asiento",MakeEllipticTorus(0.36,0.93,0.535,0.142,0.225,0.017,34,8),mat_pvc,tag_fixture);
  box("10_WC_estanque",0.19,1.135,0.50,0.34,0.18,0.46,mat_sanitary,tag_fixture);
  box("10_WC_tapa_estanque",0.18,1.125,0.96,0.36,0.20,0.025,mat_sanitary,tag_fixture);
  cyl("10_WC_boton_descarga",0.36,1.22,0.985,0.025,0.010,24,mat_chrome,tag_fixture);
  pipe("10_WC_flexible",P(0.20,1.05,0.40),P(0.22,1.16,0.58),0.006,12,mat_chrome,tag_fixture);

  // Lavamanos compacto con pedestal, cubeta, aro, monomando y sifón.
  mesh("10_Lavamanos_pedestal",MakeEllipticFrustum(0.87,0.39,0.147,0.075,0.085,0.105,0.115,0.60,28),mat_sanitary,tag_fixture);
  mesh("10_Lavamanos_cubeta",MakeEllipticFrustum(0.87,0.39,0.745,0.165,0.125,0.220,0.165,0.11,32),mat_sanitary,tag_fixture);
  mesh("10_Lavamanos_borde",MakeEllipticTorus(0.87,0.39,0.855,0.190,0.135,0.022,34,10),mat_sanitary,tag_fixture);
  cyl("10_Griferia_lavamanos_base",0.99,0.39,0.855,0.025,0.09,20,mat_chrome,tag_fixture);
  pipe("10_Griferia_lavamanos_cuello",P(0.99,0.39,0.94),P(0.91,0.39,1.00),0.012,16,mat_chrome,tag_fixture);
  pipe("10_Griferia_lavamanos_salida",P(0.91,0.39,1.00),P(0.87,0.39,0.97),0.012,16,mat_chrome,tag_fixture);
  pipe("10_Manilla_monomando",P(1.00,0.39,0.94),P(1.06,0.39,0.97),0.008,14,mat_chrome,tag_fixture);
  pipe("10_Sifon_lavamanos_vertical",P(0.87,0.39,0.74),P(0.87,0.39,0.55),0.018,18,mat_chrome,tag_fixture);
  pipe("10_Sifon_lavamanos_salida",P(0.87,0.39,0.55),P(1.02,0.42,0.55),0.018,18,mat_chrome,tag_fixture);

  // Ducha: receptáculo, bordes, mampara, grifería, barra, rociador y desagüe.
  box("10_Receptaculo_ducha_base",0.10,1.22,0.147,1.00,0.85,0.045,mat_sanitary,tag_fixture);
  box("10_Receptaculo_borde_frontal",0.10,1.22,0.192,1.00,0.045,0.055,mat_sanitary,tag_fixture);
  box("10_Receptaculo_borde_izq",0.10,1.22,0.192,0.045,0.85,0.055,mat_sanitary,tag_fixture);
  box("10_Receptaculo_borde_der",1.055,1.22,0.192,0.045,0.85,0.055,mat_sanitary,tag_fixture);
  box("10_Receptaculo_borde_tras",0.10,2.025,0.192,1.00,0.045,0.055,mat_sanitary,tag_fixture);
  box("10_Mampara_vidrio_fijo",0.56,1.205,0.247,0.54,0.010,1.83,mat_glass,tag_fixture);
  box("10_Perfil_mampara_vertical",0.555,1.198,0.247,0.018,0.024,1.83,mat_chrome,tag_fixture);
  box("10_Perfil_mampara_superior",0.56,1.198,2.06,0.54,0.024,0.018,mat_chrome,tag_fixture);
  cyl("10_Mezclador_ducha",0.61,2.075,1.08,0.055,0.035,28,mat_chrome,tag_fixture);
  pipe("10_Manilla_ducha",P(0.56,2.065,1.08),P(0.66,2.065,1.08),0.008,14,mat_chrome,tag_fixture);
  pipe("10_Barra_ducha",P(0.61,2.065,1.18),P(0.61,2.065,2.02),0.012,16,mat_chrome,tag_fixture);
  pipe("10_Brazo_rociador",P(0.61,2.065,2.02),P(0.61,1.92,2.02),0.012,16,mat_chrome,tag_fixture);
  pipe("10_Rociador_ducha",P(0.52,1.92,2.02),P(0.70,1.92,2.02),0.035,24,mat_chrome,tag_fixture);
  pipe("10_Flexible_ducha_mano",P(0.67,2.06,1.15),P(0.73,1.97,1.48),0.006,12,mat_chrome,tag_fixture);
  pipe("10_Ducha_mano",P(0.73,1.97,1.48),P(0.73,1.91,1.61),0.015,18,mat_chrome,tag_fixture);

  // ---------------------------------------------------------------------------
  // 11. Espejo y accesorios completos.
  // ---------------------------------------------------------------------------
  box("11_Espejo_marco",1.073,0.18,1.05,0.018,0.48,0.72,mat_black,tag_access);
  box("11_Espejo",1.067,0.195,1.075,0.008,0.45,0.67,mat_mirror,tag_access);
  // Toallero en muro derecho.
  pipe("11_Toallero_barra",P(1.065,0.82,1.28),P(1.065,1.25,1.28),0.010,16,mat_black,tag_access);
  pipe("11_Toallero_soporte_1",P(1.065,0.82,1.28),P(1.095,0.82,1.28),0.009,14,mat_black,tag_access);
  pipe("11_Toallero_soporte_2",P(1.065,1.25,1.28),P(1.095,1.25,1.28),0.009,14,mat_black,tag_access);
  // Portarrollos, rollo y soporte.
  pipe("11_Portarrollos_soporte",P(0.125,0.72,0.72),P(0.175,0.72,0.72),0.009,14,mat_black,tag_access);
  pipe("11_Portarrollos_eje",P(0.14,0.69,0.72),P(0.14,0.86,0.72),0.012,16,mat_black,tag_access);
  pipe("11_Rollo_papel",P(0.14,0.72,0.72),P(0.14,0.83,0.72),0.045,24,mat_pvc,tag_access);
  // Repisa de ducha y jabonera.
  box("11_Repisa_ducha",0.72,2.045,1.32,0.28,0.055,0.025,mat_black,tag_access);
  box("11_Jabonera_lavamanos",0.70,0.20,0.91,0.14,0.10,0.025,mat_black,tag_access);
  cyl("11_Dispensador_jabon",0.76,0.25,0.935,0.030,0.12,20,mat_pvc,tag_access);
  pipe("11_Bomba_dispensador",P(0.76,0.25,1.055),P(0.81,0.25,1.055),0.006,12,mat_black,tag_access);
  // Perchas detrás de la puerta.
  for (int i=0;i<2;++i) {
    pipe(N("11_Percha_",i+1),P(0.14,0.25+i*0.12,1.62),P(0.20,0.25+i*0.12,1.62),0.010,14,mat_black,tag_access);
    pipe(N("11_Percha_gancho_",i+1),P(0.20,0.25+i*0.12,1.62),P(0.20,0.25+i*0.12,1.68),0.008,14,mat_black,tag_access);
  }
  // Escobilla y basurero compacto.
  cyl("11_Base_escobilla_WC",0.78,0.95,0.147,0.055,0.18,24,mat_black,tag_access);
  pipe("11_Mango_escobilla",P(0.78,0.95,0.32),P(0.78,0.95,0.72),0.009,14,mat_black,tag_access);
  mesh("11_Basurero",MakeEllipticFrustum(0.88,0.76,0.147,0.08,0.08,0.10,0.10,0.28,24),mat_black,tag_access);
  box("11_Alfombra_antideslizante",0.56,0.55,0.148,0.42,0.58,0.006,mat_black,tag_access);

  // ---------------------------------------------------------------------------
  // 12. Cielo falso PVC, perfiles, tapa de inspección y molduras.
  // ---------------------------------------------------------------------------
  int omega=1;
  for (double y : {0.15,0.55,0.95,1.35,1.75,2.05}) box(N("12_Perfil_omega_",omega++),0.10,y,2.285,1.00,0.025,0.025,mat_galv,tag_ceiling);
  int strip=1;
  for (double x=0.10;x<1.10-1e-6;x+=0.20) {
    const double w=std::min(0.197,1.10-x);
    box(N("12_Panel_cielo_PVC_",strip++),x,0.10,2.270,w,2.00,0.012,mat_pvc,tag_ceiling);
  }
  box("12_Tapa_inspeccion_cielo",0.14,1.64,2.257,0.38,0.38,0.014,mat_pvc,tag_ceiling);
  box("12_Marco_tapa_cielo_frontal",0.13,1.63,2.252,0.40,0.015,0.020,mat_black,tag_ceiling);
  box("12_Marco_tapa_cielo_tras",0.13,2.015,2.252,0.40,0.015,0.020,mat_black,tag_ceiling);
  box("12_Marco_tapa_cielo_izq",0.13,1.645,2.252,0.015,0.37,0.020,mat_black,tag_ceiling);
  box("12_Marco_tapa_cielo_der",0.515,1.645,2.252,0.015,0.37,0.020,mat_black,tag_ceiling);
  box("12_Moldura_cielo_izq",0.095,0.095,2.255,0.020,2.01,0.025,mat_pvc,tag_ceiling);
  box("12_Moldura_cielo_der",1.085,0.095,2.255,0.020,2.01,0.025,mat_pvc,tag_ceiling);
  box("12_Moldura_cielo_tras",0.10,2.085,2.255,1.00,0.020,0.025,mat_pvc,tag_ceiling);

  if (!ok) {
    SUModelRelease(&model);
    SUTerminate();
    return 1;
  }

  const SUResult saved = SUModelSaveToFileWithVersion(model, output.c_str(), SUModelVersion_SU2021);
  if (!Check(saved, "SUModelSaveToFileWithVersion")) {
    SUModelRelease(&model);
    SUTerminate();
    return 1;
  }
  SUModelRelease(&model);
  SUTerminate();
  std::cout << "Modelo detallado generado: " << output << "\n";
  return 0;
}
