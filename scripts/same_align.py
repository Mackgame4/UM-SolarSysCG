def update_rotation_except_saturn_ring(file_path="solar_system_align.xml", new_angle=125.32185250821448):
    with open(file_path, "r") as file:
        lines = file.readlines()

    inside_saturn_ring = False
    updated_lines = []

    for line in lines:
        if '<group name="Saturn-Ring">' in line:
            inside_saturn_ring = True  

        if inside_saturn_ring and "</group>" in line:
            inside_saturn_ring = False  

        if not inside_saturn_ring and '<rotate angle="' in line:
            updated_line = f'            <rotate angle="{new_angle}" x="0" y="1" z="0" />\n'
            updated_lines.append(updated_line)
        else:
            updated_lines.append(line)

    with open(file_path, "w") as file:
        file.writelines(updated_lines)

update_rotation_except_saturn_ring("solar_system_align.xml")
