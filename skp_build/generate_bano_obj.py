from pathlib import Path
import math

OUT = Path(__file__).with_name("bano_1x2.obj")
vertices = []
faces = []
objects = []


def add_box(name, x, y, z, w, d, h):
    start_v = len(vertices) + 1
    local = [
        (x, y, z), (x+w, y, z), (x+w, y+d, z), (x, y+d, z),
        (x, y, z+h), (x+w, y, z+h), (x+w, y+d, z+h), (x, y+d, z+h),
    ]
    vertices.extend(local)
    local_faces = [
        (1,3,2),(1,4,3), (5,6,7),(5,7,8),
        (1,2,6),(1,6,5), (2,3,7),(2,7,6),
        (3,4,8),(3,8,7), (4,1,5),(4,5,8),
    ]
    start_f = len(faces)
    for f in local_faces:
        faces.append(tuple(start_v + i - 1 for i in f))
    objects.append((name, start_f, len(faces)))


def add_cylinder(name, cx, cy, z, radius, height, segments=20):
    start_v = len(vertices) + 1
    local = []
    for zz in (z, z + height):
        for i in range(segments):
            a = 2 * math.pi * i / segments
            local.append((cx + radius * math.cos(a), cy + radius * math.sin(a), zz))
    local.append((cx, cy, z))
    local.append((cx, cy, z + height))
    vertices.extend(local)
    bc = start_v + 2 * segments
    tc = bc + 1
    start_f = len(faces)
    for i in range(segments):
        j = (i + 1) % segments
        b1, b2 = start_v + i, start_v + j
        t1, t2 = start_v + segments + i, start_v + segments + j
        faces.extend([(b1,b2,t2),(b1,t2,t1),(bc,b2,b1),(tc,t1,t2)])
    objects.append((name, start_f, len(faces)))


# Fundaciones de referencia: 50 x 50 x 60 cm
for name, x, y in [
    ("Fundacion_1", -0.20, -0.20), ("Fundacion_2", 0.90, -0.20),
    ("Fundacion_3", -0.20, 1.90), ("Fundacion_4", 0.90, 1.90),
]:
    add_box(name, x, y, -0.60, 0.50, 0.50, 0.60)

# Vigas de riostra y radier
add_box("Riostra_frontal", 0.00, 0.00, -0.30, 1.20, 0.20, 0.30)
add_box("Riostra_trasera", 0.00, 2.00, -0.30, 1.20, 0.20, 0.30)
add_box("Riostra_izquierda", 0.00, 0.20, -0.30, 0.20, 1.80, 0.30)
add_box("Riostra_derecha", 1.00, 0.20, -0.30, 0.20, 1.80, 0.30)
add_box("Radier_10cm", 0.00, 0.00, 0.00, 1.20, 2.20, 0.10)
add_box("Ceramica_piso", 0.08, 0.08, 0.10, 1.04, 2.04, 0.012)

# Columnas 100 x 100 mm
for name, x, y in [
    ("Columna_1_100x100", 0.00, 0.00), ("Columna_2_100x100", 1.10, 0.00),
    ("Columna_3_100x100", 0.00, 2.10), ("Columna_4_100x100", 1.10, 2.10),
]:
    add_box(name, x, y, 0.10, 0.10, 0.10, 2.40)

# Marco y viguetas del entrepiso
add_box("Viga_superior_frontal", 0.00, 0.00, 2.40, 1.20, 0.10, 0.10)
add_box("Viga_superior_trasera", 0.00, 2.10, 2.40, 1.20, 0.10, 0.10)
add_box("Viga_superior_izquierda", 0.00, 0.10, 2.40, 0.10, 2.00, 0.10)
add_box("Viga_superior_derecha", 1.10, 0.10, 2.40, 0.10, 2.00, 0.10)
for n, yy in enumerate([0.20, 0.60, 1.00, 1.40, 1.80, 2.05], 1):
    add_box(f"Vigueta_superior_{n}", 0.10, yy, 2.40, 1.00, 0.05, 0.10)
add_box("Terciado_estructural_21mm", 0.00, 0.00, 2.50, 1.20, 2.20, 0.021)
add_box("Placa_cementicia_superior", 0.00, 0.00, 2.521, 1.20, 2.20, 0.010)

# Muros exteriores de 80 mm y vano de puerta de 70 cm
add_box("Muro_izquierdo", 0.00, 0.08, 0.10, 0.08, 2.04, 2.30)
add_box("Muro_derecho", 1.12, 0.08, 0.10, 0.08, 2.04, 2.30)
add_box("Muro_trasero", 0.08, 2.12, 0.10, 1.04, 0.08, 2.30)
add_box("Muro_frontal_izquierdo", 0.08, 0.00, 0.10, 0.17, 0.08, 2.00)
add_box("Muro_frontal_derecho", 0.95, 0.00, 0.10, 0.17, 0.08, 2.00)
add_box("Dintel_puerta", 0.25, 0.00, 2.10, 0.70, 0.08, 0.30)

# Revestimientos interiores esquematicos
add_box("PVC_interior_izquierdo", 0.08, 0.10, 0.10, 0.008, 2.00, 2.20)
add_box("PVC_interior_derecho", 1.112, 0.10, 0.10, 0.008, 2.00, 2.20)
add_box("PVC_interior_trasero", 0.10, 2.112, 0.10, 1.00, 0.008, 2.20)
add_box("Puerta_70x200", 0.25, -0.04, 0.10, 0.70, 0.04, 2.00)

# Columna falsa y tuberias verticales
add_box("Columna_falsa_instalaciones", 0.08, 1.82, 0.10, 0.28, 0.30, 2.20)
add_cylinder("Bajante_110mm", 0.22, 1.97, 0.10, 0.055, 2.75)
add_cylinder("Agua_fria", 0.12, 1.88, 0.10, 0.010, 2.75, 12)
add_cylinder("Agua_caliente", 0.16, 1.88, 0.10, 0.010, 2.75, 12)

# Ducha, WC y lavamanos simplificados
add_box("Receptaculo_ducha", 0.10, 1.22, 0.112, 1.00, 0.85, 0.07)
add_box("Mampara_vidrio", 0.58, 1.20, 0.18, 0.52, 0.012, 1.85)
add_box("WC_base", 0.14, 0.63, 0.112, 0.38, 0.58, 0.38)
add_box("WC_estanque", 0.16, 1.02, 0.42, 0.34, 0.18, 0.55)
add_box("Lavamanos_compacto", 0.68, 0.28, 0.82, 0.40, 0.32, 0.12)
add_box("Pedestal_lavamanos", 0.83, 0.37, 0.112, 0.12, 0.14, 0.708)

# Cielo y extractor
add_box("Cielo_falso_PVC", 0.08, 0.08, 2.24, 1.04, 2.04, 0.012)
add_cylinder("Extractor_100mm", 1.118, 0.72, 1.86, 0.05, 0.04, 20)

# Pasadas hacia el segundo baño
add_cylinder("Pasada_WC_superior", 0.22, 1.97, 2.531, 0.055, 0.25)
add_cylinder("Pasada_agua_fria_superior", 0.12, 1.88, 2.531, 0.010, 0.25, 12)
add_cylinder("Pasada_agua_caliente_superior", 0.16, 1.88, 2.531, 0.010, 0.25, 12)

# Escritura OBJ; cada pieza se mantiene como objeto independiente.
with OUT.open("w", encoding="ascii", newline="\n") as f:
    f.write("# Bano 1x2 m preliminar para conversion a SketchUp\n")
    f.write("# Unidades: metros\n")
    for x, y, z in vertices:
        f.write(f"v {x:.6f} {y:.6f} {z:.6f}\n")
    for name, first, last in objects:
        f.write(f"o {name}\n")
        f.write(f"g {name}\n")
        for a, b, c in faces[first:last]:
            f.write(f"f {a} {b} {c}\n")

print(f"OBJ creado: {OUT} | vertices={len(vertices)} | caras={len(faces)}")
