import os
from pathlib import *

export_dir = Path("SquareLine/export")
install_inc_dir = Path("Core/Inc/sl_ui")
install_src_dir = Path("Core/Src/sl_ui")


def substitute(content):
    content = content.replace('#include "ui', '#include "sl_ui/ui')
    content = content.replace('#include "lvgl/lvgl.h"', '#include "lvgl.h"')
    content = "// clang-format off\n\n" + content
    return content


if __name__ == "__main__":
    # Ensure directories exist
    if not install_inc_dir.exists():
        install_inc_dir.mkdir()
    if not install_src_dir.exists():
        install_src_dir.mkdir()
    # Delete all old files
    for file in install_inc_dir.glob("*"):
        os.remove(file)
    for file in install_src_dir.glob("*"):
        os.remove(file)
    # Find all headers and sources
    headers = export_dir.glob("*.h")
    sources = export_dir.glob("*.c")
    install_log = []

    for header in headers:
        with open(header, "r") as f:
            content = f.read()
        target_file = install_inc_dir / header.name
        with open(target_file, "w") as f:
            f.write(substitute(content))
            install_log.append(f"\tInstalled {header.name} to {target_file}")

    for source in sources:
        with open(source, "r") as f:
            content = f.read()
        target_file = install_src_dir / source.name
        with open(target_file, "w") as f:
            f.write(substitute(content))
            install_log.append(f"\tInstalled {header.name} to {target_file}")

    print(f"Installed {len(install_log)} files:")
    print("\n".join(install_log))
