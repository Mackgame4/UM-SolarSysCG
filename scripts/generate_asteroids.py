import random

def append_asteroids_to_xml(num_asteroids=1200, file_path="solar_system.xml"):
    with open(file_path, "r") as file:
        lines = file.readlines()
 
    # Finding the correct index to insert the asteroids
    insert_index = -1
    for i, line in enumerate(lines):
        if '<group name="Asteroids Belt">' in line:
            insert_index = i + 1  # Insert after the line
    
    if insert_index == -1:
        print("[Error]: The group 'Asteroids Belt' could not be found in the XML file.")
        return
    
    asteroid_data = []
    for i in range(1, num_asteroids + 1):
        angle = random.uniform(0, 360)  
        distance = random.uniform(11, 12)  
        scale = random.uniform(0.0001, 0.03) 

        asteroid_data.append(f'    <group name="Asteroid{i}">\n')
        asteroid_data.append(f'        <models>\n')
        asteroid_data.append(f'            <model file="planet.3d"></model>\n')
        asteroid_data.append(f'        </models>\n')
        asteroid_data.append(f'        <transform>\n')
        asteroid_data.append(f'            <rotate angle="{angle}" x="0" y="1" z="0" />\n')
        asteroid_data.append(f'            <translate x="{distance}" y="0" z="0" />\n')
        asteroid_data.append(f'            <scale x="{scale}" y="{scale}" z="{scale}" />\n')
        asteroid_data.append(f'        </transform>\n')
        asteroid_data.append(f'    </group>\n')

    updated_lines = lines[:insert_index] + asteroid_data + lines[insert_index:]

    with open(file_path, "w") as file:
        file.writelines(updated_lines)

    print(f"Asteroids added successfully to the XML file ({num_asteroids} asteroids).")

append_asteroids_to_xml()
