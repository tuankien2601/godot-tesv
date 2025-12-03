import json
import os

types = [
    "void",
    "bool",
    "char",
    "int8",
    "uint8",
    "int16",
    "uint16",
    "int32",
    "uint32",
    "int64",
    "uint64",
    "float",
    "hkVector4",
    "hkQuaternion",
    "hkMatrix3",
    "hkRotation",
    "hkQsTransform",
    "hkMatrix4",
    "hkTransform",
    "Zero",
    "{subtype}{class_name}*",
    "FunctionPointer",
    "hkArray<{subtype}>",
    "hkInPlaceArray<{subtype}>",
    "{subtype}",
    "struct {class_name}",
    "hkSimpleArray<{subtype}>",
    "hkHomogeneousArray<{subtype}>",
    "hkVariant",
    "char*",
    "uint64",
    "hkFlags",
    "hkHalf",
    "hkStringPtr",
]

root_dir = os.path.dirname(__file__)

layout = open(f"{root_dir}/hk_2010.2.0-r1_1-0-1-4.json", "r")
data = json.load(layout)

classes: dict = {}

for clazz in data["classes"]:
    classes[clazz["name"]] = clazz

for type_info in data["typeinfo"]:
    class_name = type_info["name"]
    classes[class_name]["vtable"] = type_info["vtable"]


def get_dependencies(clazz, classes):
    """Extract all class dependencies from a class definition."""
    deps = set()
    
    # Add parent class as dependency
    if clazz.get("parent"):
        deps.add(clazz["parent"])
    
    # Add member type dependencies
    for member in clazz.get("members", []):
        member_type = types[member.get("type", 0)]
        subtype_idx = member.get("subtype", 0)
        
        # Check if the member references another class
        if "class" in member:
            class_ref = member["class"]
            if class_ref in classes:
                deps.add(class_ref)
        
        # Handle parameterized types
        if "{subtype}" in member_type and subtype_idx < len(types):
            subtype = types[subtype_idx]
            if subtype in classes:
                deps.add(subtype)
    
    return deps


def topological_sort(classes):
    """Sort classes in dependency order using topological sort."""
    visited = set()
    sorted_classes = []
    visiting = set()
    
    def visit(class_name):
        if class_name in visited:
            return
        if class_name in visiting:
            # Circular dependency - skip to avoid infinite loop
            return
        
        if class_name not in classes:
            # External class, mark as visited
            visited.add(class_name)
            return
        
        visiting.add(class_name)
        
        # Visit dependencies first
        deps = get_dependencies(classes[class_name], classes)
        for dep in deps:
            visit(dep)
        
        visiting.remove(class_name)
        visited.add(class_name)
        sorted_classes.append(classes[class_name])
    
    # Visit all classes
    for class_name in classes:
        visit(class_name)
    
    return sorted_classes


def write_members(members, header_file, indent="    "):
    for member in members:
        print(member)
        member_type = types[member.get("type", 0)]
        subtype = types[member.get("subtype", 0)].format(subtype="", class_name=member.get("class", "void"))
        member_type = member_type.format(subtype=subtype, class_name=member.get("class", ""))
        member_name = member["name"]
        header_file.write(f"{indent}{member_type} m_{member_name};\n")


header_file = open(f"{root_dir}/havok.h", "w")
header_file.write("#include \"hkBaseTypes.h\"\n\n")

# Sort classes in dependency order
sorted_classes = topological_sort(classes)

for clazz in sorted_classes:
    has_vtable = clazz.get("vtable", 0) != 0
    if clazz["parent"]:
        header_file.write(f"class {clazz['name']}: public {clazz['parent']} \n")
    else:
        header_file.write(f"class {clazz['name']} \n")
    header_file.write("{\n")
    header_file.write("public:\n")
    if has_vtable:
        header_file.write("\n    virtual ~" + clazz['name'] + "();\n")
    
    write_members(clazz["members"], header_file)
    header_file.write("};\n")
    header_file.write("\n")

header_file.write("\n")
header_file.write("std::map<std::string, typeinfo> typeinfos = {\n")
for clazz in sorted_classes:
    vtable = clazz.get("vtable", 0)
    size = clazz.get("objectSize", 0)
    header_file.write('    {\n')
    header_file.write(f'        "{clazz["name"]}", \n')
    header_file.write('        {\n')
    header_file.write(f'            {vtable},\n')
    header_file.write(f'            {size},\n')
    header_file.write(f'            [](void *mem) -> void* {{ return new (mem) {clazz["name"]}(); }}\n')
    header_file.write('        }\n')
    header_file.write('    },\n')
header_file.write("};\n")
header_file.close()
