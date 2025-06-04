import math
import random
import xml.etree.ElementTree as ET

def generate_ellipse_points(a, b, num_points):
    points = []
    for i in range(num_points):
        angle = 2 * math.pi * i / num_points
        x = a * math.cos(angle)
        z = b * math.sin(angle)
        points.append((x, 0, z))
    return points

def create_asteroid_group(index):
    a = random.uniform(10, 20)
    b = random.uniform(10, 20)
    duration = random.uniform(30, 90)
    rotation = random.uniform(0, 360)
    scale = random.uniform(0.015, 0.025)

    group = ET.Element("group", {"name": f"Asteroid {index}"})

    models = ET.SubElement(group, "models")
    model = ET.SubElement(models, "model", {"file": "models/sphere_low_res.3d"})
    color = ET.SubElement(model, "color")
    ET.SubElement(color, "diffuse", {"R": "200", "G": "200", "B": "200"})
    ET.SubElement(color, "ambient", {"R": "50", "G": "50", "B": "50"})
    ET.SubElement(color, "specular", {"R": "0", "G": "0", "B": "0"})
    ET.SubElement(color, "emissive", {"R": "0", "G": "0", "B": "0"})
    ET.SubElement(color, "shininess", {"value": "0"})

    transform = ET.SubElement(group, "transform")
    ET.SubElement(transform, "rotate", {
        "angle": str(rotation), "x": "0", "y": "1", "z": "0"
    })
    translate = ET.SubElement(transform, "translate", {
        "time": f"{duration:.6f}", "align": "False"
    })

    for x, y, z in generate_ellipse_points(a, b, 40):
        ET.SubElement(translate, "point", {
            "x": str(x), "y": str(y), "z": str(z)
        })

    ET.SubElement(transform, "scale", {
        "x": str(scale), "y": str(scale), "z": str(scale)
    })

    return group

def main():
    tree = ET.parse("./scenes/solar.xml")
    root = tree.getroot()

    # Encontrar o grupo "Asteroids Belt"
    for group in root.findall(".//group[@name='Asteroids Belt']"):
        # Remover asteroides antigos
        group.clear()
        group.attrib["name"] = "Asteroids Belt"  # reatribuir o nome apagado pelo clear()

        for i in range(1, 1501):
            asteroid = create_asteroid_group(i)
            group.append(asteroid)

    tree.write("./scenes/scene_with_asteroids.xml", encoding="utf-8", xml_declaration=True)

if __name__ == "__main__":
    main()
