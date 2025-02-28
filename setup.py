from setuptools import setup, Extension, find_packages
import sys


sha256_ext = Extension(
    'resumablesha256._sha256_ext',
    sources=['resumablesha256/_sha256_ext.c', 'resumablesha256/bcon_sha256.c'],
    extra_compile_args=(['/O2'] if sys.platform == 'win32' else ['-O3']))

setup(
    name='resumablesha256',
    version='1.0',
    description=
        "A resumable SHA-256 hasher using B-Con's sha256 C implementation",
    author='Luke Moore',
    packages=find_packages(),
    ext_modules=[sha256_ext],
    package_data={'resumablesha256': ['bcon_sha256.h']},
    classifiers=[
        'Programming Language :: Python :: 3',
        'Operating System :: OS Independent',
        'License :: Public Domain',
    ],
    license_files = ['LICENSE.txt'],
    python_requires='>=3.6',
)
