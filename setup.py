from setuptools import setup, find_packages, Extension


setup(
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    include_package_data=False,
    ext_modules=[Extension(
        "resumablesha256._sha256_ext",
        [
            "src_ext/_sha256_ext.c",
            "src_ext/bcon_sha256.c"
        ],
        include_dirs=["src_ext"],
        py_limited_api=True)
    ]
)
