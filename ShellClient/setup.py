#!/usr/bin/python3

from setuptools import setup

setup(
    name='AxiomShell-client',
    version='0.1.0',
    description='OK-tier remote shell re-implementation',
    url='http://localhost',
    author='mallo-m',
    author_email='none',
    license='BSD 2-clause',
    packages=[
        'AxiomShellSource'
    ],
    package_data = {'': [
    ]},
    include_package_data=True,
    install_requires=[
        'impacket',
        'setuptools',
        'rich',
        'prompt_toolkit'
    ],
    classifiers=[
        'Development Status :: 1 - Planning',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: BSD License',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
    ],
    entry_points={
        'console_scripts': ['AxiomShell=AxiomShellSource.__main__:main'],
    }
)

