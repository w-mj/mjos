
import os

for root, dirs, files in os.walk(".", topdown=False):
    for name in files:
        if name[0] != '.' and name[-2:] == '.h' and "@" not in name:
            print(os.path.join(root, name))
            path = os.path.join(root, name)
            new_f = os.path.join(root, name + "h")
            with open(new_f, "w") as nf:
                with open(path) as f:
                    for line in f:
                        if line == "#pragma once\n":
                            nf.write(line)
                            nf.write("#ifdef __cplusplus\n")
                            nf.write("extern \"C\" {\n")
                            nf.write("#endif\n")
                            nf.write("\n")
                        else:
                            nf.write(line)
                    nf.write("\n")
                    nf.write("#ifdef __cplusplus\n")
                    nf.write("}\n")
                    nf.write("#endif\n")
            os.remove(path)
            os.rename(new_f, path)
