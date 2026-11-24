Import("env")
import subprocess
from pathlib import Path

root = Path(env.subst("$PROJECT_DIR"))
sha = subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=root, text=True).strip()
dirty = subprocess.check_output(["git", "status", "--porcelain", "--", "."], cwd=root, text=True).strip()
if dirty:
    raise RuntimeError("Commit scoped source changes before target build; keep local_config.h ignored")
env.Append(CPPDEFINES=[("LINK_BUILD_SHA", env.StringifyMacro(sha))])
