import os
from pathlib import *

export_dir = Path("SquareLine/export")
install_inc_dir = Path("Core/Inc/sl_ui")
install_src_dir = Path("Core/Src/sl_ui")


if __name__ == '__main__':
    # Ensure directories exist
    if not install_inc_dir.exists():
        install_inc_dir.mkdir()
    if not install_src_dir.exists():
        install_src_dir.mkdir()
    # Find all headers and sources
    headers = export_dir.glob("*.h")
    sources = export_dir.glob("*.c")

    for header in headers:
        with open(header, 'r') as f:
            content = f.read()
        content = content.replace('#include "ui', '#include "sl_ui/ui')
        with open(install_inc_dir / header.name, 'w') as f:
            f.write(content)

    for source in sources:
        with open(source, 'r') as f:
            content = f.read()
        content = content.replace('#include "ui', '#include "sl_ui/ui')
        with open(install_src_dir / source.name, 'w') as f:
            f.write(content)
