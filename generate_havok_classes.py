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


def write_members(members, header_file, indent="    "):
    for member in members:
        print(member)
        member_type = types[member.get("type", 0)]
        subtype = types[member.get("subtype", 0)].format(subtype="", class_name=member.get("class", ""))
        member_type = member_type.format(subtype=subtype, class_name=member.get("class", ""))
        member_name = member["name"]
        header_file.write(f"{indent}{member_type} {member_name};\n")


header_file = open(f"{root_dir}/havok.h", "w")
header_file.write("#include \"hkBaseTypes.h\"\n\n")

for clazz in classes.values():
    has_vtable = clazz.get("vtable", 0) != 0
    if has_vtable:
        if clazz["parent"]:
            header_file.write(f"class {clazz['name']}: public {
                clazz['parent']} \n")
        else:
            header_file.write(
                f"class {clazz['name']} \n")
        header_file.write("{\n")
        header_file.write("public:\n")
        header_file.write("\n    virtual ~" + clazz['name'] + "();\n")
    else:
        header_file.write(f"struct {clazz['name']} \n")
        header_file.write("{\n")
        header_file.write("public:\n")
    write_members(clazz["members"], header_file)
    header_file.write("};\n")


header_file.close()
