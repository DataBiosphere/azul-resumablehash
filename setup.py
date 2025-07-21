from setuptools import (
    Extension,
    find_packages,
    setup,
)
# Note that for Python 3.6, which only supports setuptools<=59.6.0, we need
# to specify the name and version, pyproject.toml is only supported in >=61.0.
setup(
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    package_data={"": ["*.pyi", "py.typed"]},
    ext_modules=[
        Extension(
            "resumablehash._sha256_ext",
            [
                "src_ext/_sha256_ext.c",
                "src_ext/bcon/sha256.c"
            ],
            include_dirs=["src_ext"],
            py_limited_api=True
        ),
        Extension(
            "resumablehash._md5_ext",
            [
                "src_ext/_md5_ext.c",
                "src_ext/bcon/md5.c"
            ],
            include_dirs=["src_ext"],
            py_limited_api=True
        )
    ]
)
